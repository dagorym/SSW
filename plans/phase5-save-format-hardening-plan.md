# Phase 5 — Save-Format Hardening (Plan)

**Feature:** Save-format hardening (roadmap `doc/synthesized-roadmap.md` Part III
"Next" item #5; Cross-Cutting Initiative #2).
**Planner:** claude-opus-4-8[1m] (planner role)
**Status:** Ready for Coordinator execution.
**Coordination base branch:** dedicated per-plan branch (never `master`).
**Subtask artifact root:** `artifacts/phase5-save-format-hardening/` (per-subtask
`P5-<n>/` subdirectories, matching prior phases).

---

## 1. Feature Summary (engineering terms)

A single coordinated rebuild of the SSW binary serialization format so saves are
**versioned, portable, and robust against corrupt/malicious input**. This bundles
confirmed roadmap findings **H1** (no magic/version/error-detection, non-portable
`size_t` widths), **H2** (active defense not persisted), **H3** (ID counters not
restored → post-load ID collisions), **H4** (route-0 destroyed by the legacy
`NO_ROUTE` normalization), the `writeString` embedded-NUL wire bug, the
`FJumpRoute`/`FMap` 16-bit pointer-smuggling ID mask, factory null-deref on
unknown type, and the deferred item **F2-serialization** (persist
`FPlayer::m_destroyed`).

The format compatibility break happens exactly once here; there is no legacy read
path.

## 2. Resolved Design Decisions (binding for all subtasks)

1. **Clean break.** The loader requires a new save magic + format version. Any
   file lacking the correct magic, or carrying an unsupported version, is
   rejected — there is **no** legacy-format read path. The legacy
   `m_jumpRouteID == 0 → NO_ROUTE` normalization and the route-0 problem (H4) are
   **removed**, not preserved.
2. **Fixed-width, little-endian wire ints.** All counts/sizes and all object IDs
   are written as fixed **`uint32_t`, explicit little-endian** byte emission (not
   raw native-order struct copies). Single-byte scalars (`bool`, `char`) are
   already portable and unchanged; other multi-byte scalar fields keep the generic
   helper (portable across the two supported little-endian platforms).
3. **Robust helpers + boundary checks.** `read`/`readString`/`write`/`writeString`
   check stream state and signal failure; `readString` is capped at **64 KB**
   (named constant) and never allocates before validating the length; every
   factory call (`createShip`/`createWeapon`/`createDefense`) is null-checked
   before use; failure aborts the load at the container/`FGame::load` boundary.
   No field-by-field propagation through all ~40 loaders; no stream exceptions.
4. **Scope (IN):** H1 format core, H2 active-defense persistence, H3 ID-counter
   restore, H4/route-0 removal, `FJumpRoute`/`FMap` ID pointer-mask fix,
   `writeString` NUL fix, and F2-serialization (`m_destroyed`).
   **Scope (DEFERRED, recorded in `doc/deferred-tasks.md`):** weapon/defense enum
   stable codes (`SF-enum-codes`) and `FHexMap::save/load` stub implementation
   (`SF-fhexmap`).
5. **Loader error UX.** On a rejected/corrupt load the operation aborts, the
   partially-constructed game is torn down (returning to the no-game state — the
   prior game was already closed by `onOpen`'s `onClose` step), and the user is
   informed via the `IStrategicUI::showMessage(...)` seam (console fallback when
   no UI/runtime exists). A rejected load never leaves a half-built `FGame`
   reachable.

## 3. Confirmed Repository Facts

- Wire helpers: `include/core/FPObject.h` (`write<T>`/`read<T>` inline templates)
  and `src/core/FPObject.cpp` (`writeString`/`readString`). All four
  unconditionally `return 0`. `writeString` uses `os << s.c_str()`
  (NUL-truncates); `readString` reads an unchecked `size_t` then allocates it.
- Format head: `FGame::save`/`load` (`src/strategic/FGame.cpp:599`, `:623`) — no
  magic, no version; writes `m_players.size()` as raw `size_t`.
- `m_currentDefense` is already re-pointed to `m_defenses[0]` on load (CRIT-3,
  `src/ships/FVehicle.cpp:249-255`); the *active* selection is not persisted (H2).
- Static `m_nextID` on `FVehicle`, `FFleet`, `FPlayer`, `FSystem`, `FJumpRoute`;
  none advanced in `load()`. Increment conventions differ (`FSystem` uses
  `++m_nextID`; `FPlayer` starts at 1).
- Route-0 hack: `FFleet::load` (`src/strategic/FFleet.cpp:156`); `FJumpRoute` IDs
  start at 0 (`src/strategic/FJumpRoute.cpp:43`).
- ID pointer-smuggling: `FJumpRoute::load` stores start/end system IDs into
  `FSystem*` fields (`FJumpRoute.cpp:128,130`); `FMap::load` unpacks them via
  `& 0x0000FFFFL` (`src/strategic/FMap.cpp:347-348`).
- Factory null-deref sites: `FVehicle::load` (`createWeapon`/`createDefense`),
  `FFleet::load` and `FPlayer::load` (`createShip` on `m_unattached`),
  `createShip`/`createWeapon`/`createDefense` return `NULL` on unknown type.
- `FPlayer::m_destroyed` is owned/freed in memory (P2-5) but not serialized (F2).
- Load orchestration: `FMainFrame::onOpen` (`src/FMainFrame.cpp:147-185`) closes
  any current game, then `FGame::create(m_strategicUI)` + `m_game->load(is)` with
  no return check; `onSave` (`:128-145`) is already Cancel/`GetPath()`-hardened.
- `IStrategicUI::showMessage(title, body)` exists
  (`include/strategic/IStrategicUI.h:55`) for user-facing messages; mock
  `IStrategicUI` test seam exists (`FGameMockStrategicUITest`).

## 4. Subtask Decomposition

Subtask IDs use the established `P5-<n>` convention (artifacts land under
`artifacts/phase5-save-format-hardening/P5-<n>/`).

---

### P5-1 — Core wire-format primitives & format constants

**Security review: required**

**What:** Rebuild the shared serialization substrate in `FPObject` and add the
format constants. This is the foundation every serializable class depends on.

- Add explicit fixed-width little-endian count/ID helpers (suggested
  `writeU32(std::ostream&, uint32_t)` / `readU32(std::istream&, uint32_t&)`) that
  emit/consume exactly 4 bytes in little-endian order (byte shift/mask, not a raw
  struct copy) and return `0` on success / nonzero on stream failure.
- Make the generic `write<T>`/`read<T>` templates check stream state after the
  I/O and return nonzero on failure (single-byte and other scalars keep native
  representation; behavior otherwise unchanged).
- Fix `writeString` to write the length via the new fixed-width helper and then
  `os.write(s.data(), s.size())` (no `c_str()`); reject/oversize-guard strings
  longer than the cap; return nonzero on failure.
- Fix `readString` to read the length via the fixed-width helper, **validate it
  against a 64 KB cap (named constant, suggested `kMaxSerializedStringBytes =
  65536`) before allocating anything**, check stream state, and return nonzero on
  cap violation or stream failure without allocating on the bad path.
- Define the save magic and format-version constants (suggested `kSaveMagic`
  32-bit tag and `kSaveFormatVersion` `uint32_t`) plus the string cap, in a
  natural core home (extend `FPObject.h` or add a small
  `include/core/FSaveFormat.h`; keep it wx-free/core-pure).

**Allowed files:**
- `include/core/FPObject.h`
- `src/core/FPObject.cpp`
- (optional new) `include/core/FSaveFormat.h`

**Acceptance criteria (implementation outcomes):**
- `readString` returns nonzero and performs no allocation when the encoded length
  exceeds 64 KB or the stream is in a failed state; a valid round-trip of a string
  containing an embedded `'\0'` reproduces the exact original bytes and length.
- `writeString`/`readString` and the fixed-width `uint32_t` helpers round-trip
  counts/lengths losslessly with explicit little-endian byte order (verifiable by
  inspecting the emitted bytes).
- `write`/`read`/`writeString`/`readString` return nonzero when the underlying
  stream is in a failed state, and `0` on success.
- The 64 KB string cap and the magic/version values are named constants, not
  inline literals.
- Core module remains wx-free; `make` (core) builds clean.

**Documentation Impact:** Update the serialization/`FPObject` Doxygen headers to
describe the new error-code contract, the little-endian fixed-width helpers, the
64 KB string cap, and the magic/version constants. No `AGENTS.md`/`CLAUDE.md`
change expected unless a new `FSaveFormat.h` seam warrants a one-line pointer.

**Dependencies:** none (foundation).

---

### P5-2 — Vehicle / weapon / defense save-load hardening (H2, H3, factory null-checks)

**Security review: required**

**What:** Harden the ship/weapon/defense wire layer against the new format and
untrusted input.

- Convert every count/size and object ID written in `FVehicle::save/load` (and in
  the weapon/defense `save/load` paths where counts/IDs appear) to the P5-1
  fixed-width LE helpers, reading into a local `uint32_t` and assigning back.
- Null-check `createWeapon(...)` and `createDefense(...)` results in
  `FVehicle::load`; on an unknown/NULL type, abort the load by returning nonzero
  (do not dereference).
- **H2:** Persist the active defense selection in `FVehicle::save` (e.g. the index
  of `m_currentDefense` within `m_defenses`, or its defense-type identity) and
  restore it in `FVehicle::load` after the defense list is rebuilt, so a ship
  saved with a raised screen (e.g. Masking Screen) reloads with that same defense
  active and its associated state consistent (no base-defense/MS-count mismatch).
  Preserve the existing CRIT-3 safety fallback when the list is empty/invalid.
- **H3:** After reading `m_ID` in `FVehicle::load`, advance the static `m_nextID`
  so it is strictly greater than any loaded ID (guard form
  `if (m_ID >= m_nextID) m_nextID = m_ID + 1;`), preventing post-load ID reuse.

**Allowed files:**
- `include/ships/FVehicle.h`, `src/ships/FVehicle.cpp`
- `include/weapons/FWeapon.h`, `src/weapons/FWeapon.cpp` (only if count/ID wire
  fields need conversion)
- `include/defenses/FDefense.h`, `src/defenses/FDefense.cpp` (only if count/ID
  wire fields need conversion)

**Acceptance criteria (implementation outcomes):**
- A vehicle carrying weapons and defenses round-trips through `save`→`load` with
  all weapons, defenses, HP/MR/ADF/DCR/heading/speed/flags preserved.
- A vehicle saved with a non-default active defense (e.g. Masking Screen raised)
  reloads with the **same** defense reported by `getCurrentDefense()`, and its
  masking-screen/turn-count state is internally consistent with that selection.
- `FVehicle::load` returns nonzero (and does not dereference NULL) when the stream
  encodes an unknown weapon or defense type.
- After loading a vehicle with a given `m_ID`, a newly constructed vehicle
  receives an ID strictly greater than every loaded ID (no collision).
- Counts/IDs in the vehicle wire format are fixed-width little-endian `uint32_t`.
- `make` builds clean; non-GUI ship/weapon/defense modules remain wx-free.

**Documentation Impact:** Update `FVehicle::save`/`load` (and any touched
weapon/defense save/load) Doxygen headers for the active-defense persistence, the
ID-counter restore, factory null-check behavior, and fixed-width fields. When H2
is delivered, mark the `H2` entry in `doc/deferred-tasks.md` resolved/removed.

**Dependencies:** P5-1.

---

### P5-3 — Player / fleet save-load hardening (F2 `m_destroyed`, H3, H4/route-0 removal, createShip null-check)

**Security review: required**

**What:** Harden the player/fleet wire layer.

- Convert all counts/sizes and IDs in `FPlayer::save/load` and `FFleet::save/load`
  to the P5-1 fixed-width LE helpers.
- Null-check `createShip(...)` in `FPlayer::load` (`m_unattached`) and
  `FFleet::load`; on unknown/NULL type, abort by returning nonzero (no deref).
- **F2-serialization:** Serialize and restore `FPlayer::m_destroyed` (type-tag +
  `createShip` + `v->load`, mirroring the existing `m_unattached`/fleet pattern,
  with the same null-check), preserving `FPlayer`'s sole-ownership contract. The
  list is persisted-but-not-yet-consumed (the Replacements rule that reads it does
  not exist yet — this is the intended state).
- **H4/route-0 removal:** Remove the `if (m_jumpRouteID == 0) m_jumpRouteID =
  NO_ROUTE;` normalization in `FFleet::load` so a fleet saved mid-transit on jump
  route ID 0 retains its route after load.
- **H3:** Advance `FPlayer::m_nextID` and `FFleet::m_nextID` past any loaded ID
  (non-colliding guard), respecting each class's increment convention.

**Allowed files:**
- `include/strategic/FPlayer.h`, `src/strategic/FPlayer.cpp`
- `include/strategic/FFleet.h`, `src/strategic/FFleet.cpp`

**Acceptance criteria (implementation outcomes):**
- A player with unattached ships, fleets (each holding ships), and a non-empty
  `m_destroyed` list round-trips through `save`→`load` with all three collections
  restored (counts, IDs, names, and per-ship state), and no double-free/leak of
  `m_destroyed` ships.
- A fleet saved while in transit on jump route ID 0 reloads with
  `getJumpRoute() == 0` (route preserved, not normalized to `NO_ROUTE`).
- `FPlayer::load`/`FFleet::load` return nonzero (no NULL deref) on an unknown ship
  type in the stream.
- After loading players/fleets, newly constructed players/fleets receive IDs
  strictly greater than every loaded ID (no collision).
- Player/fleet wire counts/IDs are fixed-width little-endian `uint32_t`.
- `make` builds clean; strategic module remains wx-free.

**Documentation Impact:** Update `FPlayer::save`/`load` and `FFleet::save`/`load`
Doxygen headers (destroyed-list persistence, route-0 fix, ID-counter restore,
factory null-checks, fixed-width fields). When F2 ships, mark the
`F2-serialization` entry in `doc/deferred-tasks.md` resolved/removed.

**Dependencies:** P5-1; P5-2 (a player/fleet round-trip exercises the hardened
`FVehicle::load`).

---

### P5-4 — Map / system / jump-route save-load hardening & ID pointer-mask fix

**Security review: required**

**What:** Harden the map/system/jump-route wire layer and eliminate the 16-bit
pointer-smuggling ID mask.

- Convert all counts/sizes and IDs in `FMap::save/load`, `FSystem::save/load`, and
  `FJumpRoute::save/load` to the P5-1 fixed-width LE helpers.
- **ID pointer-mask fix:** Serialize the jump route's start/end **system IDs as
  real fixed-width `uint32_t` fields**; in `FMap::load`, resolve them via
  `getSystem(id)` from those fields (remove the `(FSystem*)`-cast smuggling and the
  `& 0x0000FFFFL` mask), so IDs > 65535 and LLP64 hosts resolve correctly.
- **H3:** Advance `FSystem::m_nextID` and `FJumpRoute::m_nextID` past any loaded
  ID (non-colliding guard), respecting `FSystem`'s pre-increment convention.
- Null-guard the system lookup in `FMap::load` so an out-of-range/unknown system
  ID in a jump route aborts the load (nonzero) rather than storing a bad pointer.

**Allowed files:**
- `include/strategic/FMap.h`, `src/strategic/FMap.cpp`
- `include/strategic/FSystem.h`, `src/strategic/FSystem.cpp`
- `include/strategic/FJumpRoute.h`, `src/strategic/FJumpRoute.cpp`

**Acceptance criteria (implementation outcomes):**
- A map with multiple systems and jump routes round-trips through `save`→`load`
  with every system restored and every jump route's start/end system pointers
  correctly re-resolved to the matching `FSystem`.
- A jump route whose start/end system ID exceeds 65535 resolves to the correct
  systems after load (the 16-bit truncation no longer occurs).
- `FMap::load` returns nonzero (and stores no invalid pointer) when a jump route
  references a system ID that is not present.
- After loading systems/jump-routes, newly constructed ones receive IDs strictly
  greater than every loaded ID (no collision).
- Map/system/jump-route wire counts/IDs are fixed-width little-endian `uint32_t`.
- `make` builds clean; strategic module remains wx-free.

**Documentation Impact:** Update `FMap::load`, `FSystem::save`/`load`, and
`FJumpRoute::save`/`load` Doxygen headers for the real system-ID fields (replacing
the pointer-mask), the ID-counter restore, and fixed-width fields.

**Dependencies:** P5-1. (Independent of P5-2/P5-3 — disjoint files.)

---

### P5-5 — Save-file header, load-failure orchestration & error UX

**Security review: required**

**What:** Add the versioned file header, aggregate error propagation, and the
abort/preserve/report loader UX.

- Write the save magic + format version at the head of `FGame::save`; convert
  `FGame`'s own counts/IDs (e.g. `m_players.size()`, `m_currentPlayer`) to the
  P5-1 fixed-width LE helpers.
- In `FGame::load`: read and validate the magic first (mismatch → nonzero) then
  the version (unsupported → nonzero); check the return of each sub-load
  (`m_universe->load`, each `player->load`) and every header read; on any failure
  report via `m_ui->showMessage(...)` when `m_ui != NULL` (console fallback
  otherwise) and return nonzero, leaving no half-built game committed.
- In `FMainFrame::onOpen`: check `is.is_open()` and the `FGame::load` return code;
  on failure, tear down the freshly-created failed game (return to the `m_game ==
  NULL` no-game state via the existing reset path) and do **not** enable the
  turn/menu items; on success, keep the current post-load menu-enable behavior
  unchanged.

**Allowed files:**
- `include/strategic/FGame.h`, `src/strategic/FGame.cpp`
- `src/FMainFrame.cpp`, `include/FMainFrame.h`

**Acceptance criteria (implementation outcomes):**
- `FGame::save` emits the magic + version header before any game data; a
  full-game `save`→`load` round-trip on a valid current-version file restores
  round, current player, retreat condition, loss counters, the map, and all
  players (with fleets/ships) intact.
- `FGame::load` returns nonzero and reports via the installed `IStrategicUI`
  (verifiable through a mock `IStrategicUI`) when given: (a) a stream with wrong
  magic, (b) an unsupported version, (c) a truncated/short stream, or (d) a stream
  whose contained factory type is unknown — and in each case leaves no
  partially-constructed game as the live singleton.
- `FMainFrame::onOpen` does not enable turn/menu items and returns to the no-game
  state when `FGame::load` fails or the file cannot be opened; the success path's
  menu-enable/turn-state logic is unchanged.
- `FGame` header counts/IDs are fixed-width little-endian `uint32_t`.
- `make check` is green across all three suites.

**Documentation Impact:** Update `FGame::save`/`load` Doxygen headers for the
magic/version header and error contract; update `FMainFrame::onOpen` header for
the load-failure handling. Update `AGENTS.md`/`CLAUDE.md` only if the save format
is described there (add a brief note that saves are magic+version-tagged,
fixed-width little-endian, and reject unknown/legacy files). If the roadmap or
`doc/deferred-tasks.md` needs a "resolved" mark for H1/H2/H3/H4/F2, apply it.

**Dependencies:** P5-1, P5-2, P5-3, P5-4 (aggregates the whole hardened format and
its error returns).

## 5. Dependency Ordering & Parallelization

```
P5-1  (core primitives + constants)      [foundation]
   |
   +--> P5-2 (vehicle/weapon/defense) --> P5-3 (player/fleet)
   |
   +--> P5-4 (map/system/jump-route)                 [parallel branch]
                                   \
                                    +--> P5-5 (header + orchestration + UX)
```

- **P5-1 first**, always — it defines the wire-helper API, the little-endian
  fixed-width helpers, the 64 KB cap, and the magic/version constants that every
  other subtask consumes.
- **P5-2 → P5-3** are serial: a player/fleet round-trip exercises the hardened
  `FVehicle::load`, and P5-3's fleet loading depends on P5-2's vehicle contract.
- **P5-4 may run in parallel** with the P5-2→P5-3 branch: it touches a disjoint
  file set (`FMap`/`FSystem`/`FJumpRoute`) and depends only on P5-1. (Conservative
  note: if the Coordinator prefers strictly serial execution, run P5-4 after P5-3;
  there is no shared file or shared decision beyond the P5-1 API.)
- **P5-5 last** — it joins the header/orchestration/UX onto the fully hardened
  format and requires P5-2, P5-3, and P5-4 complete so the aggregate error
  propagation and full-game round-trip are real.

## 6. Overall Documentation Impact

- **Doxygen headers** on every touched `save`/`load` and on the `FPObject` helper
  surface (mandatory per repo convention; update `@date Last Modified` and add the
  implementing model to `@author`).
- **`doc/deferred-tasks.md`:** mark `H2` and `F2-serialization` resolved/removed as
  their subtasks (P5-2, P5-3) land; the two newly-recorded deferrals
  (`SF-enum-codes`, `SF-fhexmap`) remain.
- **`doc/synthesized-roadmap.md`:** optionally annotate H1/H2/H3/H4 and Part III
  item #5 as resolved once the phase completes (consistent with prior phases'
  `***RESOLVED***` annotations).
- **`AGENTS.md`/`CLAUDE.md`:** only if the save format is described there — add a
  brief note that saves are magic+version-tagged, fixed-width little-endian, and
  reject unknown/legacy files. Do not restate low-level format details in
  entry-point files (single-source-of-truth rule).
- **User-facing docs** (`doc/UsersGuide.md`): note that saves from prior builds are
  no longer loadable (one-time compatibility break) if save/load is documented
  there.

## 7. Security Notes

Loading a save file is an **untrusted-input path**. Every subtask that parses the
stream is marked `Security review: required` because the hardening *is* the
security fix: the 64 KB `readString` cap (huge-alloc DoS), factory null-checks
(null-deref crash on unknown type), stream-state checks and bounded reads
(out-of-bounds / garbage reads), the removed 16-bit ID truncation (wrong-object
resolution), and the abort-without-committing-a-half-built-game loader contract.
The Security stage should focus on malformed/adversarial input: oversized string
lengths, truncated streams, unknown type tags, out-of-range system IDs, and
integer-boundary counts.

---

## 7.5 Output Artifact Path

All paths are repository-root-relative.

- **This plan artifact:** `plans/phase5-save-format-hardening-plan.md`
- **Subtask artifact root:** `artifacts/phase5-save-format-hardening/`
- **Per-subtask artifact directories:** `artifacts/phase5-save-format-hardening/P5-<n>/`
  (e.g. `artifacts/phase5-save-format-hardening/P5-1/`) — each Implementer/Tester/
  Documenter/Security/Verifier stage writes its reports and results there, matching
  the layout of prior phases; the plan-level Reviewer report lands at
  `artifacts/phase5-save-format-hardening/reviewer_report.md`.

## 8. Implementer Prompts (Coordinator-ready Implementer Agent prompts)

### P5-1 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Rebuild the core serialization primitives and add the save-format constants for the
save-format hardening phase (P5-1). This is the shared foundation for all later
subtasks.

Allowed files:
- include/core/FPObject.h
- src/core/FPObject.cpp
- (optional new) include/core/FSaveFormat.h

Implement:
- Add explicit fixed-width little-endian count/ID helpers (suggested
  writeU32(std::ostream&, uint32_t) / readU32(std::istream&, uint32_t&)) that emit
  and consume exactly 4 bytes in little-endian order via byte shift/mask (NOT a raw
  struct copy), returning 0 on success and nonzero on stream failure.
- Make the generic write<T>/read<T> templates check stream state after the I/O and
  return nonzero on failure; otherwise behavior unchanged (single-byte and other
  scalars keep their native representation).
- Fix writeString: write the length via the fixed-width helper, then
  os.write(s.data(), s.size()) (no c_str()); guard against strings longer than the
  cap; return nonzero on failure.
- Fix readString: read the length via the fixed-width helper, VALIDATE it against a
  64 KB cap (named constant, suggested kMaxSerializedStringBytes = 65536) BEFORE
  allocating anything, check stream state, and return nonzero on cap violation or
  stream failure without allocating on the bad path.
- Define save magic and format-version constants (suggested kSaveMagic 32-bit tag,
  kSaveFormatVersion uint32_t) plus the string cap, in a natural core home (extend
  FPObject.h or add include/core/FSaveFormat.h). Keep everything wx-free / core-pure.

Acceptance criteria (implementation outcomes):
- readString returns nonzero and performs no allocation when the encoded length
  exceeds 64 KB or the stream has failed; a round-trip of a string containing an
  embedded '\0' reproduces the exact original bytes and length.
- writeString/readString and the fixed-width uint32_t helpers round-trip
  counts/lengths losslessly with explicit little-endian byte order.
- write/read/writeString/readString return nonzero on a failed stream and 0 on
  success.
- The 64 KB cap and magic/version values are named constants, not inline literals.
- Core module remains wx-free; the core module builds clean.

Validation guidance: build the core module and the top-level build (`make` from repo
root); the five pure non-GUI module Makefiles (including core) must stay wx-free (no
wx-config, no WX_CXXFLAGS). Follow the mandatory Doxygen header convention: update
@date Last Modified to the current date and add your model id to @author on every
function you change.

Tester guidance: behavioral coverage belongs in tests/core (a new FPObject/
serialization round-trip fixture registered in tests/SSWTests.cpp), covering the
embedded-NUL round-trip, the 64 KB readString cap (no-allocation rejection),
little-endian byte layout, and error-code return on a failed stream. Do not rely on
source-string matching as the sole verification.

Artifact guidance: write your implementer report and result to
artifacts/phase5-save-format-hardening/P5-1/ (repository-root-relative).

If no blocking issue is present after preflight, continue and complete the work in
this same run.

Security review: required

Do not report success unless all required artifacts exist and all changes are committed.
```

### P5-2 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Harden the vehicle/weapon/defense save-load wire layer for the save-format hardening
phase (P5-2), including active-defense persistence (H2), ID-counter restore (H3), and
factory null-checks. Depends on P5-1's helpers/constants.

Allowed files:
- include/ships/FVehicle.h, src/ships/FVehicle.cpp
- include/weapons/FWeapon.h, src/weapons/FWeapon.cpp (only if count/ID wire fields
  need conversion)
- include/defenses/FDefense.h, src/defenses/FDefense.cpp (only if count/ID wire
  fields need conversion)

Implement:
- Convert every count/size and object ID written in FVehicle::save/load (and in the
  weapon/defense save/load paths where counts/IDs appear) to the P5-1 fixed-width
  little-endian helpers, reading into a local uint32_t and assigning back.
- Null-check createWeapon(...) and createDefense(...) in FVehicle::load; on an
  unknown/NULL type, return nonzero and do not dereference.
- H2: Persist the active defense selection in FVehicle::save (e.g. the index of
  m_currentDefense within m_defenses, or its defense-type identity) and restore it in
  FVehicle::load after the defense list is rebuilt, so a ship saved with a raised
  screen (e.g. Masking Screen) reloads with that same defense active and consistent
  associated state. Preserve the existing CRIT-3 empty/invalid-list fallback.
- H3: After reading m_ID in FVehicle::load, advance the static m_nextID to be strictly
  greater than any loaded ID (guard: if (m_ID >= m_nextID) m_nextID = m_ID + 1;).

Acceptance criteria (implementation outcomes):
- A vehicle with weapons and defenses round-trips through save->load with all weapons,
  defenses, and HP/MR/ADF/DCR/heading/speed/flags preserved.
- A vehicle saved with a non-default active defense (e.g. Masking Screen raised)
  reloads with the same defense reported by getCurrentDefense(), with consistent
  masking-screen/turn-count state.
- FVehicle::load returns nonzero (no NULL deref) on an unknown weapon or defense type.
- After loading a vehicle with a given m_ID, a newly constructed vehicle receives an
  ID strictly greater than every loaded ID.
- Vehicle wire counts/IDs are fixed-width little-endian uint32_t.
- The build is clean; ship/weapon/defense modules remain wx-free.

Validation guidance: build via `make`; keep the pure modules wx-free. Update Doxygen
@author/@date on every changed function. When H2 lands, mark the H2 entry in
doc/deferred-tasks.md resolved/removed.

Tester guidance: behavioral coverage in tests/ships (extend/add a FVehicle save/load
fixture, and the concrete-ship fixtures as needed) plus tests/weapons and
tests/defenses if their wire fields changed; assert the active-defense round-trip, the
ID-counter restore (no collision), and the unknown-type nonzero return through
constructed objects and streams, not source-string matching.

Artifact guidance: write reports to
artifacts/phase5-save-format-hardening/P5-2/ (repository-root-relative).

If no blocking issue is present after preflight, continue and complete the work in
this same run.

Security review: required

Do not report success unless all required artifacts exist and all changes are committed.
```

### P5-3 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Harden the player/fleet save-load wire layer for the save-format hardening phase
(P5-3): serialize FPlayer::m_destroyed (F2), restore ID counters (H3), remove the
route-0 normalization (H4), and null-check ship construction. Depends on P5-1 and P5-2.

Allowed files:
- include/strategic/FPlayer.h, src/strategic/FPlayer.cpp
- include/strategic/FFleet.h, src/strategic/FFleet.cpp

Implement:
- Convert all counts/sizes and IDs in FPlayer::save/load and FFleet::save/load to the
  P5-1 fixed-width little-endian helpers.
- Null-check createShip(...) in FPlayer::load (m_unattached) and FFleet::load; on
  unknown/NULL type, return nonzero without dereferencing.
- F2-serialization: serialize and restore FPlayer::m_destroyed using the same
  type-tag + createShip + v->load pattern as m_unattached/fleet ships (with the same
  null-check), preserving FPlayer's sole-ownership contract. The list is persisted but
  not yet consumed by any rule — that is the intended state.
- H4: remove the `if (m_jumpRouteID == 0) m_jumpRouteID = NO_ROUTE;` normalization in
  FFleet::load so a fleet saved mid-transit on jump route ID 0 keeps its route.
- H3: advance FPlayer::m_nextID and FFleet::m_nextID past any loaded ID
  (non-colliding guard), respecting each class's increment convention.

Acceptance criteria (implementation outcomes):
- A player with unattached ships, fleets (holding ships), and a non-empty m_destroyed
  list round-trips through save->load with all three collections restored and no
  double-free/leak of m_destroyed ships.
- A fleet saved while in transit on jump route ID 0 reloads with getJumpRoute() == 0.
- FPlayer::load/FFleet::load return nonzero (no NULL deref) on an unknown ship type.
- After loading players/fleets, newly constructed players/fleets receive IDs strictly
  greater than every loaded ID.
- Player/fleet wire counts/IDs are fixed-width little-endian uint32_t.
- The build is clean; strategic module remains wx-free.

Validation guidance: build via `make`; keep strategic wx-free. Update Doxygen
@author/@date on changed functions. When F2 lands, mark the F2-serialization entry in
doc/deferred-tasks.md resolved/removed.

Tester guidance: behavioral coverage in tests/strategic (extend FPlayerTest and
FFleetTest) asserting the m_destroyed round-trip, the route-0 preservation, the
ID-counter restore, and the unknown-type nonzero return via constructed objects and
streams, not source-string matching.

Artifact guidance: write reports to
artifacts/phase5-save-format-hardening/P5-3/ (repository-root-relative).

If no blocking issue is present after preflight, continue and complete the work in
this same run.

Security review: required

Do not report success unless all required artifacts exist and all changes are committed.
```

### P5-4 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Harden the map/system/jump-route save-load wire layer for the save-format hardening
phase (P5-4) and eliminate the 16-bit pointer-smuggling system-ID mask. Depends on
P5-1; independent of P5-2/P5-3 (disjoint files).

Allowed files:
- include/strategic/FMap.h, src/strategic/FMap.cpp
- include/strategic/FSystem.h, src/strategic/FSystem.cpp
- include/strategic/FJumpRoute.h, src/strategic/FJumpRoute.cpp

Implement:
- Convert all counts/sizes and IDs in FMap::save/load, FSystem::save/load, and
  FJumpRoute::save/load to the P5-1 fixed-width little-endian helpers.
- ID pointer-mask fix: serialize the jump route's start/end system IDs as real
  fixed-width uint32_t fields; in FMap::load resolve them via getSystem(id) from those
  fields, removing the (FSystem*) cast smuggling and the & 0x0000FFFFL mask so IDs
  > 65535 and LLP64 hosts resolve correctly.
- H3: advance FSystem::m_nextID and FJumpRoute::m_nextID past any loaded ID
  (non-colliding guard), respecting FSystem's pre-increment convention.
- Null-guard the system lookup in FMap::load so an out-of-range/unknown jump-route
  system ID aborts the load (nonzero) instead of storing a bad pointer.

Acceptance criteria (implementation outcomes):
- A map with multiple systems and jump routes round-trips through save->load with
  every system restored and every jump route's start/end pointers correctly
  re-resolved to the matching FSystem.
- A jump route whose start/end system ID exceeds 65535 resolves to the correct systems
  after load (no 16-bit truncation).
- FMap::load returns nonzero (stores no invalid pointer) when a jump route references
  an absent system ID.
- After loading systems/jump-routes, newly constructed ones receive IDs strictly
  greater than every loaded ID.
- Map/system/jump-route wire counts/IDs are fixed-width little-endian uint32_t.
- The build is clean; strategic module remains wx-free.

Validation guidance: build via `make`; keep strategic wx-free. Update Doxygen
@author/@date on changed functions.

Tester guidance: behavioral coverage in tests/strategic (extend FMapTest, FSystemTest,
FJumpRouteTest) asserting the start/end re-resolution, a >65535 system-ID round-trip,
the absent-system nonzero return, and the ID-counter restore via constructed objects
and streams, not source-string matching.

Artifact guidance: write reports to
artifacts/phase5-save-format-hardening/P5-4/ (repository-root-relative).

If no blocking issue is present after preflight, continue and complete the work in
this same run.

Security review: required

Do not report success unless all required artifacts exist and all changes are committed.
```

### P5-5 Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Add the versioned save-file header, aggregate load-error propagation, and the
abort/preserve/report loader UX for the save-format hardening phase (P5-5). Depends on
P5-1, P5-2, P5-3, and P5-4.

Allowed files:
- include/strategic/FGame.h, src/strategic/FGame.cpp
- src/FMainFrame.cpp, include/FMainFrame.h

Implement:
- Write the save magic + format version at the head of FGame::save; convert FGame's
  own counts/IDs (e.g. m_players.size(), m_currentPlayer) to the P5-1 fixed-width
  little-endian helpers.
- In FGame::load: read and validate the magic first (mismatch -> nonzero), then the
  version (unsupported -> nonzero); check the return of each sub-load
  (m_universe->load, each player->load) and every header read; on any failure report
  via m_ui->showMessage(...) when m_ui != NULL (console fallback otherwise) and return
  nonzero, leaving no half-built game committed.
- In FMainFrame::onOpen: check is.is_open() and the FGame::load return code; on
  failure tear down the freshly-created failed game (return to the m_game == NULL
  no-game state via the existing reset path) and do NOT enable the turn/menu items; on
  success keep the current post-load menu-enable/turn-state behavior unchanged.

Acceptance criteria (implementation outcomes):
- FGame::save emits the magic + version header before any game data; a valid
  current-version full-game save->load round-trip restores round, current player,
  retreat condition, loss counters, the map, and all players (with fleets/ships).
- FGame::load returns nonzero and reports via the installed IStrategicUI (verifiable
  with a mock IStrategicUI) for each of: wrong magic, unsupported version, truncated
  stream, and unknown factory type — leaving no partially-constructed game as the live
  singleton.
- FMainFrame::onOpen does not enable turn/menu items and returns to the no-game state
  when load fails or the file cannot be opened; the success path is unchanged.
- FGame header counts/IDs are fixed-width little-endian uint32_t.
- `make check` is green across all three suites (SSWTests, TacticalTests, GuiTests).

Validation guidance: build and run `make check` from the repo root (SSWTests,
TacticalTests, and GuiTests under xvfb-run). Route strategic user-facing messages
through IStrategicUI (console fallback), not direct gui dependencies in strategic code.
Update Doxygen @author/@date on changed functions. Optionally annotate resolved
findings in doc/synthesized-roadmap.md and doc/deferred-tasks.md.

Tester guidance: behavioral coverage in tests/strategic using a mock IStrategicUI
(mirroring FGameMockStrategicUITest) for the full-game round-trip and each malformed-
input rejection path (wrong magic, bad version, truncation, unknown type), asserting
the nonzero return and the showMessage report; GUI-side onOpen no-game/menu behavior is
covered under the GUI suite. Do not rely on source-string matching as the sole
verification.

Artifact guidance: write reports to
artifacts/phase5-save-format-hardening/P5-5/ (repository-root-relative).

If no blocking issue is present after preflight, continue and complete the work in
this same run.

Security review: required

Do not report success unless all required artifacts exist and all changes are committed.
```
