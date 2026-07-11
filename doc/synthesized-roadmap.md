# SSW Deep Code Review and Rules-Coverage Gap Analysis

**Date:** July 3, 2026
**Baseline:** commit `a4e80cb` (master), ~31,300 lines of C++ across 8 modules
**Scope:** Full read-only review of architecture, structure, patterns, and code quality across
all modules, the build system, and the test suite; plus a rules-coverage gap analysis against
`doc/rules/second-sathar-war-rules.md` and `doc/rules/tactical_operations_manual.md`.
**Method:** Nine parallel deep-review passes (one per architectural area plus two rules-gap
analyses), followed by direct verification of every Critical/High defect claim against the
source. All file:line references are to the baseline commit. No code was modified.

**Synthesis note (2026-07-08):** This document began as the 2026-07-03 review and has
been merged with the parallel 2026-07-04 codex deep review. Findings that originated in
the codex pass and were independently re-verified against the source during the merge are
tagged **_(codex)_**. Everything untagged is from the original review.

---

## Executive Summary

The codebase is in substantially better architectural shape than most legacy wx projects of its
age. The recent model-extraction work (`FTacticalGame` + `ITacticalUI`, `IStrategicUI`) achieved
its goal: the tactical and strategic *models* are genuinely wx-free, testable through mock
seams, and the tactical rules engine is remarkably faithful to the Knight Hawks manual — the
full Advanced Combat Table, the complete 19-band damage table, all weapon/defense stat lines,
and the notoriously fiddly seeker-missile lifecycle all verify correct against the manual.

The three big findings:

1. **A cluster of real, confirmed defects** — most seriously: `FGame::cleanUpShips()` operates
   on *copies* of the fleet/ship lists, so destroyed ships are never removed at the strategic
   layer and every victory counter inflates each round; the Sathar retreat conditions 4 and 5
   are evaluated swapped; and `FVehicle::load()` leaves `m_currentDefense` dangling
   (use-after-free on any save that proceeds into combat). These are bugs, not style issues,
   and several directly corrupt game outcomes.

2. **The logistics half of the strategic game is missing.** Setup, movement, risk jumps,
   combat initiation, and victory detection are substantially implemented — but the entire
   back-half of the rules (the **Repair system**, supply/rearming, replacements, shatter
   drones, break-off, fighter attrition, militia deputization) does not exist. On the tactical
   side, the **Repair Turn**, docking/basing, evasive maneuvers, and planet line-of-sight
   blocking are the main gaps, plus four engine behaviors that *contradict* the manual
   (which, per repo policy, means the code is wrong).

3. **Structural debt is concentrated and known.** One god class (`FTacticalGame`, ~5,200
   lines), one paint-driven UI layer that generates recurring layout regressions
   (`FBattleDisplay`), ~1,100 lines of pure-data ship/weapon boilerplate, an unversioned and
   non-portable save format, and a tactical test suite still dominated by source-string
   matching (1,329 `assertContains` sites). Each has a clear, incremental remediation path
   described below.

---

# Part I — Architecture & Code Quality Review

## 1. What Is Working Well

These are genuine strengths worth preserving through any refactor:

- **The model/UI seam architecture.** `ITacticalUI` (106 lines) and `IStrategicUI` are small,
  wx-free, pure-virtual boundaries. Every UI call from the models is null-guarded with console
  fallbacks, so headless operation works. `WXStrategicUI` centralizes `wxTheApp` runtime guards
  exactly as documented (`src/gui/WXStrategicUI.cpp:22-30`). The delegation refactor is
  *complete*: `FBattleScreen` holds zero tactical game state — all 123 delegation call sites
  forward to `FTacticalGame`, and `FBattleBoard` reads occupancy/overlays exclusively through
  model-backed accessors.
- **Rules-engine fidelity.** The Advanced Combat Table, all 19 damage-table bands (including
  the weapon/defense hit priority lists and hull-hit fallbacks, `src/ships/FVehicle.cpp:404-613`),
  all ten weapon stat lines, all seven defenses, MR/ADF movement, mines, and the complete
  seeker lifecycle (acceleration 2/4/…/12, ≤3-hexside pre-turn, closest-target/largest-target
  tie-breaking, 12-hex expiry, station immunity) verify line-by-line against the manual.
- **Centralized combat math.** All to-hit/damage resolution lives once in `FWeapon::fire()`
  and `FVehicle::advancedDamage()`; the tactical module *reuses* them (seeker detonation and
  mine damage build transient weapons via `createWeapon(...)`) rather than duplicating.
- **Deliberate testability seams in new code.** Seeker targeting is split into pure, const,
  side-effect-free helpers with randomness funneled through one narrow chokepoint
  (`chooseRandomSeekerIndex`, `src/tactical/FTacticalGame.cpp:1552`). The ordnance provenance
  model (`FTacticalOrdnanceSource`: ship ID + weapon slot + weapon ID) supports exact
  ammo-restoring undo, with transactional rollback on failed undo.
- **Serialization shape.** Polymorphic ship reconstruction by type-name tag + factory
  (`FFleet::load` → `createShip(type)` → `v->load(is)`) is the right pattern, and the
  template `read`/`write`/`readString`/`writeString` helpers centralize the wire format so a
  fix lands everywhere at once.
- **`FGameConfig::resolveAssetPath(...)`** is a good abstraction (normalization + 4-candidate
  fallback), has regression tests, and is consistently adopted across GUI/tactical callers.
- **Hex math.** `FHexMap::computeHexDistance` uses the correct O(1) closed-form offset→axial
  metric with negative-safe `Floor2`/`Ceil2`; `FHexPath::addPoint` enforces the adjacency
  invariant at insertion.
- **Build hygiene (recently modernized).** Real header dependency tracking (`-MMD -MP` +
  `-include $(DEPS)`) in every Makefile; coverage flags correctly opt-in behind `COVERAGE=1`;
  VS projects use wildcards + a shared props file, minimizing double maintenance.
- **The GUI test harness** (`tests/gui/WXGuiTestHarness`) is genuinely good: bounded-wait
  modal helpers, orphan-top-level cleanup, and fallback auto-dismiss so a hung dialog cannot
  wedge the suite.
- **Doxygen discipline is real**, including the agent-attribution convention; platform hacks
  (GTK modal-grab, double-EndModal) are documented at the exact point they matter with root
  causes.

## 2. Confirmed Defects (fix-first list)

All items in this section were verified directly against the source during this review.
Ordered by impact.

### Critical — corrupts game outcomes or crashes

| # | Defect | Location | Detail |
|---|--------|----------|--------|
| C1 | **`cleanUpShips()` mutates vector copies** — ***RESOLVED*** (CRIT-1, commit `43fc466f`) | `src/strategic/FGame.cpp:1187,1189` | `FleetList fList = (*pItr)->getFleetList();` and `VehicleList sList = (*fItr)->getShipList();` copy the vectors; all `erase()` calls edit throwaways. Destroyed ships are never removed from fleets, are re-counted by every subsequent pass (inflating `m_lostHC`/`m_lostAC`/`m_lostSatharShips`/tenday counters and corrupting every retreat-based victory condition), the same pointer is pushed into `m_destroyed` repeatedly (latent double-delete), and empty fleets are never removed or freed. Even with references, the in-loop erase pattern (`sItr = sList.erase(sItr)` followed by the loop's `sItr++`) skips elements. _Resolved: `cleanUpShips()` now operates on the live `FleetList&`/`VehicleList&` containers, collects destroyed-ship IDs before mutating, removes each exactly once via `FFleet::removeShip`, and deletes emptied fleets after removing them from both the owning player and system; see `artifacts/phase1-critical-defects/CRIT-1`._ |
| C2 | **Retreat conditions 4 and 5 evaluated swapped** | `src/strategic/FGame.cpp:1046-1064` vs `:980-994` | The display (and `SatharRetreatGUI` in doc order) says 4 = "tenday without destroying two stations", 5 = "Sathar lost more ships". `checkForVictory()` implements case 4 as the ship-loss comparison and case 5 as the station check. Whichever condition the player picks, the other is enforced. |
| C3 | **Dangling `m_currentDefense` after `FVehicle::load()`** | `src/ships/FVehicle.cpp:218-228` | `load()` deletes every defense and rebuilds the list but never reassigns `m_currentDefense`, which still points at the deleted constructor-time `m_defenses[0]`. Every post-load consumer (`FWeapon::fire()`, `FBattleBoard.cpp:276`, `FBattleDisplay.cpp:1535`, `FTacticalGame.cpp:3360`) dereferences freed memory — bites whenever a loaded game enters tactical combat. Related: the active defense selection is not persisted at all (H2 below). |
| C4 | **Out-of-bounds planet index from `TwoPlanetsGUI`** | `include/gui/TwoPlanetsGUI.h:40-41` + `src/gui/SelectCombatGUI.cpp:217,227,258,273` | Dialog returns `EndModal(1)`/`EndModal(2)`; caller does `planet = d.ShowModal()` then indexes `getPlanetList()[planet]` on a 2-element (0/1) list. Button 1 attacks the *wrong* planet; button 2 indexes `[2]` — UB/crash. |
| C5 | **"Yes, save" on close never saves** | `src/FMainFrame.cpp:298-308` | Dialog is `wxYES_NO\|wxCANCEL` but the code tests `result == wxID_OK`; `ShowModal()` returns `wxID_YES`. Answering Yes silently discards the game. |
| C6 | **Unchecked `readlink` — latent out-of-bounds write** | `src/core/FGameConfig.cpp:111-113` | `readlink` returns `ssize_t`; on failure −1 stored into `size_t` makes `buf[size] = 0` a wild write; a ≥1000-char path writes one past the array. Runs in a singleton constructor with no error path. |

### High

| # | Defect | Location | Detail |
|---|--------|----------|--------|
| H1 | **Save format: no magic, no version, no error detection, non-portable widths** | `include/core/FPObject.h:93-132`, `src/core/FPObject.cpp:28-40`, `src/strategic/FGame.cpp:569-591` | Every `read`/`write` unconditionally returns 0 regardless of stream state (the documented error-code contract is implemented nowhere); `readString` allocates an unchecked size (corrupt file → multi-GB alloc or garbage); an unknown ship type makes `createShip` return NULL and `FFleet::load` immediately calls `v->load(is)` — null-deref on a bad save. Counts are written as `size_t` (8 bytes Linux x64, 4 bytes Win32) — saves are not portable between the two supported platforms. |
| H2 | **Active defense not persisted; MS state inconsistent after load** | `src/ships/FVehicle.cpp:154-180` | `save()` writes `m_maskingScreenTurnCount` but not which defense is current; a ship saved with a masking screen up reloads with RH active and a nonzero MS count. |
| H3 | **ID counters not restored on load — post-load collisions** | `FVehicle.cpp:188` (`m_nextID` never advanced), same pattern in `FFleet`, `FPlayer`, `FSystem`, `FJumpRoute` | Ships/fleets created after a load can duplicate loaded IDs. Tactical bookkeeping is keyed by ship ID; `removeFleet(id)` can remove the wrong fleet. Fix: `if (m_ID >= m_nextID) m_nextID = m_ID + 1;` in each `load()`. |
| H4 | **`NO_ROUTE` legacy normalization destroys jump route 0** | `src/strategic/FFleet.cpp:158-159`, `FJumpRoute.cpp:15,43` | Route IDs start at 0; the first route created ("Sathar Start Circle 2" → Prenglar) has ID 0, so any fleet saved mid-transit on it loses its route on load and is stuck forever. |
| H5 | **Per-ship leak of the base `FNone` defense** | `src/ships/FVehicle.cpp:130-132` + all 12 ship ctors | Base ctor allocates an `FNone`; every ship constructor calls `m_defenses.clear()` without deleting it. One leak per ship ever constructed (hundreds per strategic game). |
| H6 | **`createShip(type,name)` null-deref on unknown type** | `src/ships/FVehicle.cpp:317-329` | One-arg overload returns NULL for unknown names; two-arg overload calls `v->setName(name)` unchecked. Dozens of string-literal call sites; any typo is a crash. |
| H7 | **`wxClientDC` drawing inside the paint path** | `src/tactical/FBattleBoard.cpp:249` (called from `onPaint` via `drawShips`/`drawSeekerMissiles`); also `src/FGamePanel.cpp:20-24` | Half the tactical scene renders through the `wxPaintDC`, half through per-image `wxClientDC`s in the same pass. Unsupported/ignored on wxGTK3-Wayland and wxOSX, undefined z-order, bypasses the update-region clip. The single biggest portability landmine in the GUI. |
| H8 | **Winner announcement hard-codes attacker = Sathar** | `src/gui/WXTacticalUI.cpp:87-91` | `attackerWins ? "Sathar" : "UPF"` — any battle where UPF attacks announces the wrong winner name. |
| H9 | **`m_first` flag drives connect/show lifecycle of seven buttons** | `include/tactical/FBattleDisplay.h:111` + 7 sites in `FBattleDisplay.cpp` | Any phase transition that bypasses the matching Done handler (model-driven transitions exist) leaves the next phase's button never connected/shown — a soft-lock with no crash. Fix: Bind all handlers once in the ctor; drive only Show/Enable per phase. |
| H10 | **Fleet-rollback erase-while-iterating** | `src/gui/SatharFleetsGUI.cpp:518-522` | `list->erase(itr)` inside `for(...; itr++)` skips elements; consecutive new fleets survive cancellation half-deleted with a dangling pointer in the player's list. |
| H11 | **Scenario editor Start Battle has no validation** | `src/battleSim/ScenarioEditorGUI.cpp:388-449` | With "-- Select Team --" still selected, `setOwner(defenderID-1)` underflows an `unsigned int` and a zero-ship battle can launch. |
| H12 | **`FHexPath` UB on empty/short paths and off-path points** | `src/core/FHexPath.cpp:55-97` | `getPointHeading` dereferences before the end check and reads before `begin()`; `getLastHeading` wraps an unsigned index on paths < 2; `removeTrailingPoints` on an off-path point pops to empty then calls `back()`. Exactly the "unexpected click during move phase" inputs a GUI produces. |
| H13 | **Mine ownership is a single global** | `src/tactical/FTacticalGame.cpp:2180,3162` | `m_mineOwner` is overwritten on every placement; if both sides ever have live mines, the last placer's mines become friendly to the wrong side. The per-record `ownerID` needed to fix it already exists in `m_placedOrdnance`. |
| H14 | **Victory is advisory; game never ends** | `src/strategic/FGame.cpp:521-524` (`@todo`), `FMainFrame.cpp:202-226` | `endUPFTurn()` computes victory and drops it; menus keep enabling turns after the victory dialog. Also `checkForVictory` has a hole: `UPFVictory && fortressCount==0 && !SatharVictory` falls through with no result. |
| H15 _(codex)_ | **Save/Open dialogs ignore Cancel and use `GetFilename()`, not `GetPath()`** | `src/FMainFrame.cpp:128-159` | `onSave()`/`onOpen()` call `ShowModal()` without checking the result, so pressing Cancel still proceeds to open a stream on `d->GetFilename()`; and because they use `GetFilename()` (not `GetPath()`) with `wxFD_CHANGE_DIR`, the file only lands correctly by mutating the process working directory. Cancelling a save can truncate/overwrite a stale filename in the cwd; save/open location is cwd-dependent and brittle. This is the read/write sibling of the close-save bug C5 — fix all three together with `wxID_*` checks and `GetPath()`. |

### Notable Medium (selection — full detail in section 3)

- Retreat-condition selection stored unvalidated (X-close yields `wxID_CANCEL`=5101 in
  `m_satharRetreat`, silently disabling UPF victory forever) — `FGame.cpp:114`,
  `WXStrategicUI.cpp:78-89`.
- `FMap::load` appends without clearing; `FJumpRoute::load` smuggles system IDs through
  `FSystem*` pointers unpacked with a 16-bit mask (`FMap.cpp:350-351`) — breaks for IDs > 65535
  and on LLP64. _(codex)_ Also: `FMap::getMap()` returns `*m_map` with no null guard
  (`FMap.cpp:31-33`), unlike `FMap::create()` which null-checks first — any `getMap()` before
  a `create()` (e.g. a partial/bad load) is a null deref.
- `FPlayer::m_destroyed` never freed, never serialized (it is the data the Replacements rule
  needs).
- `cancelJump()` leaves `m_dx/m_dy` pointing at the cancelled destination — the returning
  fleet's map position keeps advancing the wrong way (`FFleet.cpp:190-198`).
- ICM rule data duplicated in two places that can double-apply (`m_ICMMod` in weapon ctors vs
  `FICM::getAttackModifier`).
- `hasWeapon`/`hasDefense` return 0 for both "not found" and "index 0"; consumers call
  `getDefense(hasDefense(ICM))` unconditionally and silently read slot 0 on ships without ICMs.
- Board-bounds constants disagree: `isHexInBounds` accepts 0..99 while the real board is
  55×39 (`FTacticalGame.cpp:976-978` vs `:2697,3345,2749`) — seekers can be chased off the
  visible board and never expire.
- `reset()` silently detaches the installed `ITacticalUI` (`FTacticalGame.cpp:283`).
- Modal-heavy dialogs leaked (`new` + `ShowModal` + abandon) in `SystemDialogGUI.cpp:222,237,292`;
  `BattleResultsGUI::onAllDestroyed` leaks the removed fleet; `ICMSelectionGUI` leaks
  `AssignedICMData` and mis-indexes spinners against the unfiltered vehicle list.
- `exit(0)` in BattleSim Quit skips all teardown (`include/battleSim/BattleSimFrame.h:39`).
- `TransferShipsGUI` recovers a fleet's icon by serializing the fleet and hand-parsing 11
  binary fields (`TransferShipsGUI.cpp:39-62`) — replace with a `getIconName()` accessor.
- `writeString` uses `os << s.c_str()` after writing the length — an embedded NUL misaligns
  the entire rest of the save file; `os.write(s.data(), s.size())` is the one-line fix.
- `wxClientDC`+`SetClientSize` inside `FGamePanel::onPaint`; `static unsigned int count` in
  `makeShipChoice` shared across all battles (`FBattleDisplay.cpp:995`).

## 3. Architecture & Design Findings by Area

### 3.1 core

Solid math and config foundations with three systemic weaknesses:

- **Serialization robustness** (H1 above) — the module's helpers are the right place to fix
  the whole format at once. Recommended incremental path: (a) magic + `uint32_t` version at
  the head of `FGame::save`; (b) make `read`/`readString` check stream state and return
  nonzero, cap `readString` (e.g. 1 MB), propagate at call sites; (c) fixed-width wire types
  for all counts/IDs; (d) null-check factory results in `FFleet::load`/`FPlayer::load`.
  Alternative: stream exceptions (`is.exceptions(failbit|badbit)`) — less boilerplate across
  ~40 load functions but foreign to the codebase's error style.
- **Layering:** `include/core/FHexMap.h` includes `ships/FVehicle.h` — the foundation module
  depends upward. A forward declaration + local `typedef std::vector<FVehicle*> VehicleList;`
  fixes it with no API change. Similarly, `FPoint` carries a pointless virtual destructor
  (vptr doubles the size of every element in every point container) and a misdocumented
  `(-9999,-9999)` default; `operator==`/`getDistance` are non-const, which blocks const usage
  downstream.
- **`FHexMap` save/load are inline `{return 0;}` stubs** — a caller persisting a battle
  silently loses the occupancy map and is told "success".
- Test gap: **no `FHexMapTest`** — the most correctness-sensitive math in the module
  (`computeHexDistance`/`computeHeading`/`findNextHex`) has no direct core-level coverage; and
  no round-trip test for `writeString`/`readString` (which would have caught the NUL issue).
- Minor: `findNextHex` brute-forces a 3×3 window through float trig where a 2×6 offset table
  answers exactly; `sign()` returning `bool` is a misuse trap; `codecvt_utf8` (deprecated,
  removed in C++26) on the Windows path.

### 3.2 strategic

The turn sequencer works but is *implicit*: state = `m_currentPlayer` + `m_round` + menu-enable
side effects duplicated across three `FMainFrame` handlers. There is no phase enum and no guard
preventing out-of-order calls; `checkForCombat()` always returns 0 and its result is dead.

- **~60% of `FGame.cpp` is inline scenario data** — the full order of battle, nine
  near-identical `create*Militia()` functions, stations, and the entire Frontier map in
  `FMap::setUpFrontier` (~760 + ~180 lines). A single data table collapses the militia
  functions to one loop; a text/JSON file under `data/` would make maps and scenarios
  moddable without recompiling (the `createShip(string)` type names are already the schema).
- **Player identity is resolved three inconsistent ways** — index (`m_players[0/1]`), ID
  (`m_currentPlayer`), and name string (`getName()=="UPF"` in loss accounting). A serialized
  faction enum on `FPlayer` plus `upfPlayer()`/`satharPlayer()` accessors removes the silent
  breakage class.
- **Combat sequencing leaks past the UI seam:** `resolveCombat` hands full control to
  `SelectCombatGUI::onAttack`, which owns attack/decline flow, planet targeting, and the
  tactical launch. Model-authoritative sequencing (who attacks, where, outcome application)
  belongs behind `IStrategicUI` as data in/out — mirroring what was already done for tactical.
- `FGame::create(ui)` silently ignores the new UI when an instance exists; an `installUI()`
  setter (as `FTacticalGame` has) would prevent stale-adapter bugs.
- Victory/end-of-game latching (H14), retreat-condition validation, Nova idempotence +
  persistence, tenday off-by-one (`checkForVictory` runs before `m_round++`), and the
  `m_destroyed` leak round out the list.

### 3.3 ships / weapons / defenses

The headline: **zero behavioral polymorphism**. No ship overrides anything; no weapon has
custom behavior (`fire()` handles all ten via flags); the defenses' only virtual is
`getAttackModifier`, which is seven switch statements encoding a 7×11 integer table. The
entire layer is ~60 rows of data expressed as ~1,100 lines across 29 files, and 6 of 12 ships
forgot to set `m_iconName` (silently inheriting `ufo.png`) — exactly the omission a table
makes impossible.

Options for the boilerplate (in descending recommendation order):

- **A. In-code data table + single `FShip` class** *(recommended)*. `createShip` becomes a
  table lookup. Pros: deletes ~1,100 lines/29 files; adding a ship = one row; fixes the
  unknown-type NULL path, the icon omissions, and the max/current duplication in one stroke.
  Cons: largest one-time churn (per-class tests reference concrete types; the game code
  already goes through `createShip(string)` and needs no change). An external data *file*
  variant adds asset/versioning complexity for little gain; the in-code table gets 90% of the
  benefit.
- **B. Keep subclasses, centralize data** via a protected `FVehicle::init(const ShipSpec&)`.
  Pros: minimal diff, no test churn. Cons: still 7 touch points per new ship; the factory
  chain and enum fragility remain.
- **C. CRTP/templates** — not a fit; there is no per-type behavior to specialize.

Independently worthwhile regardless of option: (1) map-based factory registries so unknown
types are loud; (2) flatten the seven `getAttackModifier` switches into one
`static const int table[DEFENSE_COUNT][WEAPON_COUNT]`; (3) serialize weapon/defense enums as
explicit stable codes — today inserting mid-enum silently corrupts old saves; (4) an
`isStation()` helper on `FVehicle` to replace the string-triple comparison that already
appears twice.

Also: `FVehicle`/`FWeapon`/`FDefense` are copyable with owning raw pointers (shallow copy →
double delete; `FFleet` was already made non-copyable for this exact reason); `setMR`/`setDCR`
take `unsigned` and check `< 0` (dead), while an underflowing caller would clamp to *max*;
`setTarget` silently drops out-of-range targets and leaves stale `m_isHeadOn`.

_(codex)_ Two related weapon invariants are also weak: (1) **negative ammo can fire.**
`setCurrentAmmo(int)` clamps only to `m_maxAmmo`, never to zero
(`include/weapons/FWeapon.h:77`, `include/defenses/FDefense.h:58`), and `FWeapon::fire()`
treats *only* exact zero as "no ammo" (`src/weapons/FWeapon.cpp:112`) then decrements any
nonzero value (`:185`) — so a negative count fires and grows more negative each shot. Clamp
to `[0, maxAmmo]`. (2) **Null-parent deref in the laser/masking-screen damage branch.**
`src/weapons/FWeapon.cpp:170` reads `m_parent->getCurrentDefense()` with no null check, even
though the sibling to-hit branches at `:136` and `:149` guard `m_parent != NULL`. Current
mine/seeker transient weapons dodge it by type, but the invariant is fragile — guard it or
require a parent for all normal ship-mounted fire.

### 3.4 tactical model (`FTacticalGame`)

The extraction succeeded, and the newest code in it (seeker engine, ordnance provenance,
report pipeline) is the best-designed code in the repository. The structural risks, in order:

1. **The phase machine is implicit, side-effectful, and unguarded.** `m_state`/`m_phase` are
   plain ints backed by anonymous enums; `setPhase(PH_MOVE)` never sets PH_MOVE — it reroutes
   into seeker activation, which may resolve damage (modal dialogs) and land in
   `beginMovePhase()`, which applies fire damage and toggles the active player. Calling
   `setPhase(PH_FINALIZE_MOVE)` twice double-finalizes movement and double-toggles the player.
   Only one completion function guards its precondition. Invalid transitions fail silently by
   corrupting whose turn it is — the least debuggable bug class in a turn-based game.
   *Cheap fix:* rename transition entry points (`requestMovePhase()`, `finalizeMove()`), make
   `setPhase` a private true setter, add a legal-transition table. *Durable fix:* a small
   `FTacticalPhaseController` owning state/phase/players with single toggle authority.
2. **`toggleActivePlayer` as a temporary-context hack.** `clearDestroyedShips()` toggles,
   scans the other side, toggles back; `applyFireDamage` wraps it in *another* toggle pair.
   Any early return between a pair leaves global turn state inverted. Also: the winner check
   inside `clearDestroyedShips` examines only one side (mutual destruction declares the wrong
   winner) and disagrees with `isCombatOver()`, which checks both. Pass an explicit side
   parameter; compute the winner from both sides in one place.
3. **The destroyed-ship consume/clear protocol is a hand-run two-phase commit** spread across
   two classes and three clear styles — and the model itself bypasses the consumed-flag in two
   places (`applyFireDamage`, `applyMineDamage` call `.clear()` directly), silently discarding
   unconsumed IDs. Preferred fix: return destroyed IDs by value inside
   `FTacticalCombatReportSummary` (which already travels to the wx layer at exactly the right
   moment) and delete the flag/accessors.
4. **God-class trajectory** (~5,240 lines, ~95 public methods, 33-member protected data blob,
   8+ responsibilities). Recommended sequence: extract the phase controller (above), then the
   two most separable subsystems — `FTacticalSeekerEngine` (~700 lines, mostly pure helpers)
   and `FTacticalOrdnancePlacement` (~900 lines) — with `FTacticalGame` keeping ownership and
   forwarding, which preserves the `FBattleScreen` delegation surface and most test contracts.
   A full six-way domain split is defensible but highest-churn; defer unless growth continues.
5. **Modal dialogs run deep inside model state transitions** (`applyMineDamage` mutates,
   raises modal UI mid-function, keeps mutating). Works for synchronous wx; undocumented and
   re-entrancy-unsafe. At minimum, document the synchronous/non-reentrant contract on
   `ITacticalUI`; longer term split resolution into compute / present / commit.
6. **Duplicated state that must be manually synchronized:** `m_moved` ↔ `turnData->nMoved`;
   `m_gravityTurns` ↔ per-ship `gravityTurns`; ship `speed`/`heading` on `FVehicle` *and* in
   `FTacticalTurnData`, reconciled by two commit paths that must stay identical
   (`finalizeMovementState` and `completeMovePhase` re-doing each other's work).
7. **Dead code retained to satisfy source-inspection tests** (`placeMineAtHex` has an
   if/else with identical branches plus a comment saying the tokens exist for tests) — the
   exact anti-pattern the repo's own testing policy warns about.
8. Performance: `findShipHex`/`removeShipFromHexOccupancy` sweep the full 100×100 grid;
   seeker target selection is O(ships × 10,000). A `map<shipID, FPoint>` index maintained at
   the three mutation points fixes it and removes the bounds-constant coupling. The
   ~0.5 MB inline `m_hexData[100][100]` (vs the design note's planned sparse map) is the
   long-known outstanding refactor.
9. _(codex)_ **Seeker detonation damage is UI-gated — a headless-correctness hole.**
   `applyMovementSeekerDamage()` only resolves pending seeker damage when a UI is installed:
   `if (m_ui != NULL) { requestRedraw(); resolvePendingSeekerDetonationDamage(); } else {
   clearPendingSeekerContactOutcomes(); }` (`src/tactical/FTacticalGame.cpp:3222-3229`). With
   no UI, the seeker is still removed from the board but its damage is silently discarded, so
   model-only/headless callers and behavioral tests that don't install an `ITacticalUI` mock
   see seekers vanish without effect. Mechanics should resolve regardless of UI presence, with
   the UI as an optional observer of already-computed results (return an event/summary bundle
   so tests can assert damage without a UI). This is the same "compute vs present vs commit"
   split flagged in point 5 above.
10. _(codex)_ **`FTacticalCombatReport.h` is a heavy header — compile-coupling and layering.**
    The report header is 816 lines, `#include`s `ships/FVehicle.h` and `weapons/FWeapon.h`, and
    carries substantial inline formatting/summary-building implementation (~60+ inline-impl
    sites). Every translation unit that touches a combat report pulls in the vehicle/weapon
    headers and pays the rebuild cost on any change. Keep the structs/enums in the header and
    move non-trivial formatters and summary builders into a new
    `src/tactical/FTacticalCombatReport.cpp`.

### 3.5 tactical GUI (`FBattleScreen` / `FBattleBoard` / `FBattleDisplay`)

The delegation refactor left a clean board (genuinely narrowed to geometry/rendering/
hit-testing) and a screen that is ~90% forwarding boilerplate (~60 pure pass-throughs plus
~30 "delegate + `if (changed) reDraw()`" wrappers). Options for the facade:

- **(a) Expose the model** to the children (`m_parent->tacticalGame()`), keep only
  wx-lifecycle/orchestration on the screen, and move the redraw-on-change policy to the
  observer that already exists (`ITacticalUI::requestRedraw`, emitted by the model). Deletes
  ~600 lines and ends the copy-paste tax. Recommended.
- **(b) Facade for mutations, `const FTacticalGame&` for queries** — halves the surface.
- **(c) Status quo** — single choke point, but growth is unbounded and mechanical (the seeker
  feature alone added ~20 forwarders).

`FBattleDisplay` is the highest-regression-rate file in the repo, and the reason is
architectural: **layout and UI state mutation happen inside paint handlers** (hit regions,
Connect/Disconnect, Show/Hide, `Layout()`, panel positioning, deferred `SendSizeEvent`), with
guards layered on guards (`m_inResizeReflow`, the ratchet/reset mechanism, a measurement loop
run twice to reach a fixed point). The 40-line changelog of layout-fix tickets at the top of
the file is the evidence. Options:

- **A. Keep manual DC layout** — pixel-exact, current tests assert against it; but every new
  row list re-implements wrap/measure/hit-rects, no DPI awareness (zero `FromDIP`; on 2×
  HiDPI the panel layout and hit-testing will misalign), and the record shows each addition
  costs 1–3 follow-up fixes.
- **B. Real controls per phase** (`wxSimplebook` of phase panels, list controls for row
  lists, wrapped `wxStaticText` prompts, permanently-bound buttons) — deletes the wrap/
  measure/region/ratchet machinery (~600–800 lines) and eliminates the `m_first` and
  paint-mutation bug classes structurally; but dark styling of native controls is
  platform-dependent and the live-test suite must migrate from pixel assertions to
  control-state assertions. A real project, not a patch.
- **C. Hybrid** *(recommended near-term)*: (i) bind buttons once (H9), (ii) move all
  Show/Enable/position decisions into a non-paint `updateForPhase()` called from
  `setState`/`setPhase`/resize, (iii) extract one `drawClickableRowList(...)` helper shared by
  the four hand-rolled row lists, (iv) fix the paint-DC mixing (H7) and add
  `wxAutoBufferedPaintDC`. Captures most of the robustness win without disturbing test
  contracts.

Residual game-flow logic still lives in the HUD: `onSetSpeed` decides the setup state machine
and mutates the model directly (`getShip()->setSpeed(...)`), bypassing the `FTacticalGame`
seam every other mutation uses; `setStationRotation` advances state. Push
`advanceAfterSpeedSet()`/`setStationRotation(cw)` into the model — these are exactly the
transitions mock-UI regression tests currently can't reach.

Module placement: the "non-wx tactical module" invariant is currently *nominal* —
`FBattleScreen` is-a `wxFrame`, includes GTK headers, and `src/tactical/Makefile:19-20`
complies with the no-wx-config rule by hardcoding a machine-specific
`/usr/local/lib/wx/include/gtk3-unicode-static-3.3` default that breaks on any other host.
Long-term the right structure is moving Screen/Board/Display to `src/gui` (they are wx
widgets, full stop); the immediate cheap win is deriving `WX_CXXFLAGS` from a shared fragment
instead of a hardcoded path. Also: unbounded zoom (`m_size` → 0 → `Scale(0,0)` asserts),
per-paint 2,145-hex grid scans returning vectors by value, per-paint `Scale()`+`wxBitmap`
construction with no scaled-bitmap cache, reserved `__FBattleScreen__`-style include guards.

### 3.6 strategic GUI, BattleSim, app layer

`TacticalDamageSummaryGUI` is the model dialog (Bind + lambda, `SetSizerAndFit`, centering
fallback); the other ~16 dialogs are wxFormBuilder-2008 era: 111 `Connect(` calls with
mirrored `Disconnect` destructors, "DO NOT EDIT" banners on hand-maintained files, generator
names (`m_button5`, `m_textCtrl9`), three colliding `EndModal` conventions (0=OK/1=cancel vs
payload-as-return-code vs raw `ShowModal` codes — none using `wxID_OK`/`wxID_CANCEL`, which is
the root cause of C4 and the retreat-condition validation bug).

- **SatharFleetsGUI vs UPFUnattachedGUI are ~88% identical** (only 118 of ~1,000 lines differ
  after name normalization) *and have drifted*: Sathar's cancel rolls back committed fleets
  (with a `@todo` saying UPF needs it too); UPF's cancel loses ships in uncommitted fleets.
  Recommended: merge into one parameterized `FleetSetupGUI` (team, presets, done-condition,
  rollback policy) — ~994 → ~550 lines, and UPF gets the missing rollback for free. Pair with
  a thin `FDialogBase` (layout epilogue + OK/Cancel row + wx-id return convention) applied
  opportunistically. A full dialog-layer rewrite is not worth the risk absent automated GUI
  playback.
- Magic numbers that will break silently: ship-ID threshold `getID()>47` for fleet-clearing
  (both fleet dialogs), `26*count` row heights (breaks under HiDPI/font scaling), hard-coded
  task-force names.
- BattleSim's four predefined scenarios are 240 lines of imperative construction, and the
  editor's roster/display-name/icon tables are three parallel hard-coded maps
  (`ScenarioEditorGUI.cpp:451-529`) — the commented-out "Pirates" entries show the extension
  pressure. A data-driven scenario/ship-catalog file collapses all four tables and makes the
  scenario editor's output savable/loadable.
- Positional menu lookups (`GetMenu(2)->FindItemByPosition(3)` in 5 handlers) — one inserted
  item breaks every handler; use command IDs.
- _(codex)_ **Station Status dialog is an unfinished stub.** The "Station Status" button in
  `SystemDialogGUI` is created disabled (`src/gui/SystemDialogGUI.cpp:65-66`) and the User's
  Guide records it as not implemented (`doc/UsersGuide.md:180`). A `ViewFleetGUI`-style status
  view for station `FVehicle`s (HP, ADF/MR/DCR, weapons/ammo, defenses, active defense, damage
  flags) is the natural fill-in.
- Dead: `src/FMainBattleSimFrame.cpp` (0 bytes, no header, not in the Makefile) plus its
  ghost comment block in `BattleSim.cpp`. ~120 commented-out `std::cerr` lines and two live
  ones. User-visible typos: "destoryed", "Statisitics", "woud", "captial".
- Per-paint `wxImage::Scale` for every fleet icon (`WXPlayerDisplay.cpp:38`) — a
  scaled-bitmap cache keyed on (filename, size) in `WXIconCache` fixes both strategic and
  tactical layers.

### 3.7 Build system and tests

- **The single most consequential test finding:** the tactical suite is still dominated by
  source-contract tests — **1,329 `assertContains` sites across 12 files**
  (`FTacticalGameMechanicsTest.cpp` alone: 498 vs 129 behavioral asserts), including a
  home-grown brace-counting C++ "parser" and 22 literal source lines of `reset()`'s body
  asserted verbatim. AGENTS.md itself records that this pattern shipped real regressions, and
  production code is now being shaped to keep the string-matchers green (the `placeMineAtHex`
  dead branch). Newer work (TMF-*, PGS-03) correctly pairs behavioral + supplement; the debt
  is the pre-policy corpus. Recommended: an explicit burn-down — for each source-only cluster,
  write the behavioral equivalent and delete the strings (the `reset()` contract is one
  behavioral test asserting getters after `reset()`); `doc/test-contracts.md` shrinks
  proportionally.
- **"Run all tests" is a fiction:** `SSWTests` registers only 4 of 25 tactical fixtures; the
  rest run only in `TacticalTests`; GUI fixtures only in `GuiTests`; root `make tests` builds
  but never executes; there is no CI. Fix: a `make check` target chaining all three runners
  (GUI under `xvfb-run`), plus a minimal GitHub Actions workflow — the build is already
  headless-capable. Also: the defenses module has **zero** dedicated tests.
- **No C++ standard pinned anywhere**: Linux builds at GCC's default (gnu++17) while MSVC
  defaults to C++14 with `ConformanceMode=false`. One line per Makefile (or one after
  common.mk) + `<LanguageStandard>stdcpp17</LanguageStandard>`.
- **16 near-identical Makefiles** (core vs strategic differ by exactly one line) with visible
  drift: two use `.RECIPEPREFIX`, `src/defenses/Makefile:64` has `rm -f*.gcov` (a no-op typo),
  a stale out-of-repo include path in `tests/core/Makefile`. A repo-root `common.mk` reduces
  each module to ~3 lines and makes cross-cutting flag changes one edit.
- **No `-O` flag on Linux** (debug-grade binaries) while Windows Release uses MaxSpeed;
  `-O2 -g` default with a `DEBUG=1` switch. `-Wall -Woverloaded-virtual` only; add `-Wextra`.
- **CMake question:** current double-maintenance cost is lower than typical (wildcard
  vcxprojs + shared props). CMake would give one definition of 8 libs/2 apps/3 runners,
  native VS support, CTest (fixing the run-all gap), and Windows test execution — but is a
  real one-time effort (the `--start-group` circular gui↔tactical link is debt CMake will
  force you to confront) and disrupts an agent workflow tuned to the Makefiles. Verdict: do
  common.mk + `make check` + CI now; revisit CMake only if Windows testing/CI becomes a goal.
- **Randomness:** `srand(time(NULL))` inside the `FGame` constructor reseeds the process-wide
  RNG shared with all combat code; BattleSim paths that never construct `FGame` run at seed 1
  (identical rolls every session). Tests defend themselves by reseeding and depend on glibc's
  `rand()` sequence — non-portable to Windows CRT. Recommended: a small injectable dice seam
  (`std::mt19937` owned per game, settable seed) — this simultaneously enables deterministic
  behavioral tests (repo policy), battle replay/debugging, and Windows test parity. The
  seeker path already funnels through one chokepoint, making it a ~20-line change there.
- Include-guard reality: three styles plus typos ("DISPALY") across 93 headers; the
  documented `_FOO_H_` convention is also formally reserved-identifier territory.
  `#pragma once` is the cheapest durable fix.
- AGENTS.md contradicts the build on coverage flags (says always-on; they are opt-in) — a
  doc-only fix, but it is the repo's source of truth for agents.

## 4. Cross-Cutting Improvement Initiatives (summary)

In rough order of leverage per unit risk:

1. **Fix the confirmed defect list (section 2).** C1–C6 first; nearly all are small,
   surgical fixes with obvious behavioral tests.
2. **Save-format hardening** (magic/version, fixed-width counts, error propagation, factory
   null-checks, persist current defense + ID counters). Unlocks: cross-platform saves, safe
   format evolution, and removal of the route-0 hack.
3. **Phase-machine hardening** — tactical first (rename transition entry points, legal-
   transition table, single toggle authority), then the same treatment for the strategic
   turn sequencer (explicit state enum + latched game-over).
4. **Deterministic RNG seam** — enables the behavioral tests the repo policy demands and
   battle replay.
5. **Data-driven stats** (ships/weapons/defenses table + factory registries + stable
   serialization codes), then the same approach for BattleSim scenarios and the strategic
   order of battle.
6. **Tactical GUI hybrid cleanup** (bind-once buttons, `updateForPhase()`, paint-DC fix,
   shared row-list renderer, DPI pass), with the facade slimming (expose model or
   const-queries) as a follow-on.
7. **Test-suite burn-down** (source-contract → behavioral) + `make check` + CI + pinned
   `-std=c++17` + common.mk.
8. **`FTacticalGame` decomposition** (seeker engine, ordnance placement) once 3 and 7 have
   de-risked it.

---

# Part II — Rules-Coverage Gap Analysis

Per repo policy, `doc/rules/tactical_operations_manual.md` is non-volatile truth: where the
implementation contradicts it, the code is wrong. The strategic rules doc is treated the same
way here, with deviations flagged for discussion.

## 5. Strategic game (`second-sathar-war-rules.md`)

### 5.1 What is substantially implemented

Setup and orders of battle (task forces, all 10 militias, fortresses/fortified/armed stations
with correct stats), the Frontier map with Sathar-only start-circle routes, transit-box
movement (abstracted to per-route day counts), risk jumps with the exact Misjump Probability
Table, misjump loss, militia movement tethering, the strategic turn sequence, Strike Force
Nova's placement table, combat initiation with Sathar-first option and the counter-attack
restriction, pre-battle minelayer ordnance placement, retreat-condition selection/tracking,
and win/draw detection (modulo the bugs above).

### 5.2 Missing features (the "what's left to build" list)

1. **The Repair system — entirely absent** (rules l.316–348). No post-battle DCR repair, no
   hull-repair-failure rules, no SCC registry or Repair Capacity Table (9 systems, 10–40
   HP/day), no dock-for-a-full-day state, no Sathar Void round-trip (leave at an SSC,
   re-enter repaired 6 days later), no SCC capture (all stations destroyed + no UPF ships)
   with half-rate Sathar repair. DCR is stored and displayed but never used. Natural home:
   `endUPFTurn`/`endSatharTurn` plus an SCC attribute on `FPlanet` and a repair-state on
   `FFleet`/`FVehicle`.
2. **Supply and rearming** (l.304–314). No supply tracing (path to fortress/SSC avoiding
   enemy systems), no one-day rearm action, no fighter rearm-twice-out-of-supply counter.
   Currently every surviving ship is rearmed unconditionally after every battle
   (`FGame.cpp:1209-1212`, acknowledged with `@todo`).
3. **Replacements** (optional, l.350–362). 20-day cycle; Sathar d100≤35 per lost ship; UPF
   slots per surviving fortress/fortified station; cap at original OOB; no militia
   replacement. `FPlayer::m_destroyed` already collects the required data (but must be fixed
   to be reachable — see C1 — and serialized).
4. **Shatter drones** (optional, l.364–374). Frigate conversion, end-of-movement detonation
   = torpedo hit on every ship in the hex, excluded from victory counting.
5. **Breaking off combat** (l.298–302). No tactical escape to an adjacent transit box, no
   victor stay-or-move choice, no militia must-fire-once rule. Tactical battles currently run
   to annihilation or manual quit.
6. **Fighter attrition/basing** (l.61, 304–306). No station basing; no destruction of
   fighters caught without a carrier/station at day advance (only the 8-per-carrier
   composition check exists).
7. **Militia special behaviors** (l.260–262). No forced return home under imminent threat; no
   deputization when all home stations are destroyed.
8. **Retreat execution** (l.400). Meeting the retreat condition shows a dialog; the Sathar
   ships never actually retreat to the nearest SSC.
9. **Basic vs Advanced rules modes** and the ship-count rules-suggestion table (manual result
   entry is the de-facto stand-in).
10. **Multi-player/referee side division** (2–6 players) — presumably out of scope by design.

### 5.3 Partial implementations

- Transit boxes: day counts are derived from euclidean map distance (the code itself notes
  "should be ceil instead of round", `FJumpRoute.cpp:42`) rather than FDM box counts; no
  stop-at-jump-box turn; "decelerating >1 box also risk-jumps" absent.
- Risk-jump constraints: constant-rate-until-jump-point not enforced (hold/resume can change
  speed mid-route via `SystemDialogGUI`); SSC red-box deceleration cap only enforced at fleet
  creation; **unrepaired-hull-damage → max 1 box/turn is absent** (no hull/speed coupling).
- Interlocked computers: fleet-wide single roll is the only (mandatory) mode; no per-ship or
  sub-group option.
- _(codex)_ Attack-declaration ordering is implicit, not modeled as phases. The Sathar-first
  option and the "UPF may not attack Sathar ships that already attacked" restriction exist at
  the point of combat, but there is no explicit strategic phase for Sathar attack declarations
  followed by UPF counter-declarations: `endUPFTurn()` calls `checkForCombat()`, which
  auto-detects contested systems, calls `resolveCombat()`, and always returns 0 (its result is
  dead — see §3.2). To enforce the declaration restriction reliably, model the day as explicit
  phases and track which fleets/systems have already attacked, and make `checkForCombat()`
  return a meaningful result feeding a declaration UI.
- Sathar setup: ≥20 ships and ≤half checked, but per-*fleet* instead of per-*start-circle*.
- Battle setup: defender-first and planet/station on board work; "defender on/adjacent to
  planet" and "attacker enters from a map edge" are not enforced.
- Victory edges: retreat met with 0 fortresses remaining falls through with no result.

### 5.4 Contradictions / deviations (confirmed against source)

| # | Deviation | Rules say | Code does |
|---|-----------|-----------|-----------|
| S1 | Retreat conditions 4/5 swapped in evaluation (= defect C2) | 4 = tenday stations; 5 = tenday losses | Evaluated inverted (`FGame.cpp:1046-1064`) |
| S2 | Condition 3 fighter counting | "lost 40 ships, **including** fighters" | Fighters excluded (`FGame.cpp:1198-1200`) |
| S3 | Condition 5 fighter/militia counting | Fighters and militia **not** counted | All ships counted (`FGame.cpp:1194-1197`) |
| S4 | Sathar OOB light cruisers | 2 | 7 (`FGame.cpp:220`) — verified |
| S5 | UPF non-attached | 1 Minelayer, no carrier | 1 Assault Carrier, no Minelayer (`FGame.cpp:245-278`) |
| S6 | Armed stations | 10 (18 stations total) | 11 — extra at Dramune/Outer Reach (`FGame.cpp:929-930`); the Sathar victory threshold (`stationCount <= 7`) is calibrated to 19, so the two deviations only cancel if both stay |
| S7 | Rearming | In-supply + full day at planet | Unconditional auto-reload after every battle |
| S8 | Mid-game reinforcements | 20-ship minimum applies to initial setup only | `SatharFleetsGUI` demands ≥20 per dialog session even for reinforcements |
| S9 | Jump speed | Constant rate from first box | Hold-then-resume at RJ2/RJ3 allowed mid-route |
| S10 | Naming | White Light planet "Gollywog" | "Clarion" throughout (probably intentional; note only) |
| S11 | Sequencing | Sathar choose resolution order; victory "immediately" | Map-iteration order; victory checked at end of UPF turn |

## 6. Tactical game (`tactical_operations_manual.md`)

### 6.1 Contradictions with the manual (high priority — code is wrong by policy)

| # | Contradiction | Manual | Code |
|---|---------------|--------|------|
| T1 | **Disruptor Beam Cannon range** | RA **9** (weapon text l.951 and Weapon Restrictions Table l.1145) | `m_range=12` (`src/weapons/FDisruptorCannon.cpp:16`) — verified |
| T2 | **"Most effective defense" rule** (l.235-238, 411-416) | To-hit resolves against the most effective defense the target owns; only an attracting screen overrides | Resolves against the *currently selected* defense only (`FWeapon.cpp:136-140`). A ship with RH that raises a Proton Screen becomes *easier* to hit with lasers (75% vs the correct 60%) |
| T3 | **Masking-screen fired-out-of effect** (l.433-436) | Applies **only to lasers** | If the attacker has MS active, *every* weapon type uses the attacker's MS modifier and ignores the target's defense entirely — e.g. a torpedo fired from inside an MS at a stasis-screened target resolves at 50% instead of 75% |
| T4 | **Head-on gravity crash** (l.887-896) | Cannot be avoided "even if it turns in hex 2" | Ship is only destroyed if its *finalized* position ends adjacent to and facing the planet; the movement UI still offers MR turns from the gravity hex, letting it escape |
| T5 | Automatic misses (l.305-307) | d100 ≥ 96 always misses | No upper clamp in `fire()` (nearly unreachable with current stats, but not coded) |
| T6 | Navigation-hit direction (l.1183) | 1d10 rolled **before the ship moves** each move | Direction fixed once at damage time by `irand(2)` |
| T7 | Scenario fidelity | Scenario-specific reduced loadouts, 100-HP Fortress Kdikit, ADF-1 Megasaurus freighter with MS×1 | All scenario ships carry full default class armaments; Kdikit is the 300-HP `FFortress`; Megasaurus is an ADF-0 crippled Assault Carrier |

### 6.2 Missing tactical features

1. **The Repair Turn — entirely absent** (l.1215–1281, the manual's largest section). No
   game-turn counter exists at all, so nothing can trigger the every-3-turns repair turn; no
   DCR point allocation, repair rolls, ADF/MR/system/hull/fire repair, 90-00 auto-fail, or
   99-00 permanent damage. DCR is damaged by the table and displayed, but is write-only.
   Consequence: power shorts, nav errors, CCS hits, and fires are all effectively permanent,
   which also breaks the balance assumptions of the damage table. Shape: turn counter in
   `FTacticalGame`, a repair-allocation dialog via `ITacticalUI`, and a `repairSystem()` API
   on `FVehicle` clearing the (already granular) damage flags.
2. **Docking / basing / rearming** (l.508–633, 932–936, 1374–1376). No docking at stations or
   carriers, no fighter/scout rearm (`FVehicle::reload()` exists, unused in tactical), no
   docked immunity/fire restrictions, no half-HP splash when a station dies, no docked-ICM
   contribution, no carrier fighter capacity.
3. **Evasive maneuvers** (l.274–288). Fighters/assault scouts cannot dodge torpedoes
   (−5%×MR + forced full-MR turning next move).
4. **Planet line-of-sight blocking** (l.181). `computeFFRange`/`computeBatteryRange` have no
   planet exclusion — targets behind the planet are fully attackable. Small fix: hex-line
   trace in the range/target-validation functions.
5. **Ship orbits** (l.183–191). Only the station auto-orbits; regular ships cannot declare
   orbit (speed 0 + 1 hex/turn + chosen direction + free facing).
6. **Severe-damage breakup rolls** (l.1151–1161). Nothing tracks ADF+MR points used against a
   >50%-damaged hull.
7. **Advanced off-map handling** (l.898–904). Off-map is always fatal (Basic rule); no
   wrap/extension option.
8. **Scenario special rules & victory conditions.** The only end condition is annihilation.
   Missing: Doliin Bay evacuation/escape victory, Close Escort forced freighter movement and
   escape/draw logic, fighters-only draw (two scenarios), map-edge entry and setup
   constraints, initiative roll, civilian/freighter ship class.
9. **Basic Game mode.** The Basic combat table and hull-only damage path
   (`takeDamage(basic=true)`) exist but are unreachable — acceptable as a design choice,
   worth a deliberate decision.
10. _(codex)_ **Drifting is not a first-class state** (manual l.1171). A ship that loses all
    of both its ADF and MR must *drift* — continue at the same speed and heading, unable to
    turn/accelerate/decelerate until repaired. Damage can zero ADF/MR, and some movement
    constraints fall out of those values, but there is no named drift state (zero `drift`
    references anywhere in `src/`/`include/`) and no enforcement that a drifting ship keeps its
    prior vector with all voluntary movement locked out. Needs an explicit drift state derived
    from ADF=0/MR=0 (or a damage flag) plus movement enforcement and reporting.
11. _(codex)_ **P/E/S screen visual overlays are unimplemented** (`doc/DesignNotes.md:232-239`).
    Only the Masking Screen has a visual (it swaps the ship icon for `MaskingScreen.png`); the
    Proton/Electron/Stasis screens have no on-board indication. Add letter-in-circle overlays
    (blue P / green E / yellow S) rendered over the ship in `FBattleBoard`, backed by offscreen
    pixel tests like the existing tactical rendering coverage.
12. _(codex)_ **Tactical Load/Save, Damage Details, and Help/About menus are disabled stubs**
    (`src/tactical/FBattleScreen.cpp:153-169`; `doc/UsersGuide.md:323`). Tactical save/load
    should wait until the tactical model is decomposed/serializable (ties into save-format
    hardening and the `FTacticalGame` split); Damage Details can be backed by
    `FTacticalCombatReport` history; Help/About are low-risk once asset/document path handling
    is settled.

### 6.3 Partial tactical implementations

- **Fire-damage timing:** applied once per side-turn but at movement-phase start rather than
  combat-phase start (l.869–871) — fire ADF/MR losses land before the side's move instead of
  after.
- **Defensive FF approach-hex restriction** (l.215): a FF weapon may engage a ship entering
  its hex regardless of approach hex.
- **Power short / DC hit / nav error:** effects correct, but permanent (no repair system).
- **Stations:** orbit and fight correctly; all docking-adjacent rules absent.
- **Scenarios 1–4:** correct opposing forces by class/name, planet+station setup, and
  (Scenario 4) full pre-game mine/seeker placement; loadouts, constraints, and victory rules
  absent (see T7).

---

# Part III — Suggested Priority Order

A pragmatic sequencing that front-loads correctness and de-risks the bigger refactors:

**Now (small, high-payoff, mostly independent):**
1. Defects C1–C6 (each with a behavioral regression test).
2. T1 (disruptor range 9), T2/T3 (defense-selection logic in `FWeapon::fire()` — one
   function), S2/S3 (loss-counter filters), retreat-condition validation.
3. `make check` target + minimal CI; pin `-std=c++17` both platforms; common.mk.
4. Bind-once buttons + `updateForPhase()` in `FBattleDisplay`; paint-DC fix in
   `FBattleBoard`; delete dead files/commented-out code.

**Next (structural, sequenced):**

5. Save-format hardening (version header, fixed-width, error propagation, persist current
   defense + ID counters) — one coordinated change, since it breaks save compatibility once.
6. Deterministic RNG seam; then start the source-contract → behavioral burn-down (the two
   reinforce each other).
7. Tactical phase-controller extraction + destroyed-ship protocol simplification (IDs in the
   report summary).
8. Data-driven ship/weapon/defense stat tables + factory registries; then data-driven
   BattleSim scenarios and strategic OOB.

**Then (feature work — the missing game):**

9. Tactical Repair Turn (unlocks the damage table's intended balance; needs the turn counter
   anyway) → strategic Repair/SCC system → supply & rearming → break-off → docking/basing →
   evasive maneuvers, planet LOS, ship orbits, breakup rolls → scenario victory conditions →
   Replacements and Shatter Drones (optional rules).

**Deliberate decisions to make (no action until decided):**
- Whether S4/S5/S6 (OOB deviations, extra station) are intentional balance choices or drift —
  if intentional, document them as house rules; if not, fix and recalibrate the station
  victory threshold.
- Basic Game mode: implement or formally declare out of scope.
- CMake: revisit only if Windows testing/CI becomes a goal.
- Moving `FBattleScreen`/`FBattleBoard`/`FBattleDisplay` to the gui module (right long-term
  structure; churn-heavy).

---

*Review conducted by Claude Fable 5. Findings marked "verified" were checked directly against
the source during this review; all other findings carry file:line evidence gathered by
area-focused review passes over the full files named. Line numbers reference commit `a4e80cb`.*
