# Phase 2 — Rules-Correctness Fixes + Phase-1 Reviewer Follow-ups — Implementation Plan

**Author:** Planner (Claude Opus 4.8, high)
**Date:** 2026-07-11
**Source roadmap:** `doc/synthesized-roadmap.md` Part III "Now" item #2 (lines 676–677: T1, T2/T3, S2/S3, retreat-condition validation), plus §6.1 (T1–T3), §5.4 (S2/S3).
**Source follow-ups:** `artifacts/phase1-critical-defects/reviewer_report.md` → "Follow-up feature requests for planning" (F1, F2, F3/H2, F4/H15, F5).
**Scope chosen by user:** the roadmap "Now #2" rules-correctness cluster **and** the reviewer follow-on items. All material design decisions were resolved with the user during planning (see §2b).

**Output Artifact Path:** `plans/phase2-rules-correctness-followups-plan.md` (this file, repository-root-relative).

---

## 1. Feature Restatement (engineering terms)

Two related bodies of work, all correctness/robustness (no new game mechanics, no refactors, no *intentional* save-format break — format-affecting items are explicitly deferred):

**A. Rules-correctness (roadmap "Now" #2).** The tactical rules manual is non-volatile truth (repo policy); where code contradicts it, the code is wrong.

| ID | Fix | Function / file (confirmed) |
|----|-----|------------------------------|
| T1 | Disruptor Cannon range `12 → 9` | `FDisruptorCannon::FDisruptorCannon()` — `src/weapons/FDisruptorCannon.cpp:16` |
| T2/T3 | To-hit must resolve against the target's **most effective operating defense** (RH + the one raised screen), with the **attracting-screen override**; the masking-screen "fired out of the screen" effect applies to **lasers only** | `FWeapon::fire()` — `src/weapons/FWeapon.cpp:133-171` (+ a target-side helper on `FVehicle`) |
| S2 | Retreat condition 3 counter (`m_lostSatharShips`) must **include fighters** | `FGame::cleanUpShips()` — `src/strategic/FGame.cpp` (loss-counter block ~`:1214-1224`) |
| S3 | Retreat condition 5 tenday counters (`m_lostTendaySathar`/`m_lostTendayUPF`) must **exclude fighters and militia** | `FGame::cleanUpShips()` — `src/strategic/FGame.cpp` (~`:1205-1213`) |
| RV | Sathar retreat-condition selection must be **validated** (re-prompt on invalid/cancel) instead of storing a bogus value that silently disables UPF victory | `FGame` setup path calling `selectRetreatCondition()` — `src/strategic/FGame.cpp:114` |

**B. Phase-1 reviewer follow-ups.**

| ID | Fix | Function / file (confirmed) |
|----|-----|------------------------------|
| F2 | `FPlayer::m_destroyed` is never freed → the ships `cleanUpShips()` now hands off leak. Establish `FPlayer` as sole owner and free them in the destructor | `FPlayer::~FPlayer()` — `src/strategic/FPlayer.cpp:27-54` (+ `include/strategic/FPlayer.h`) |
| F4 | `onSave`/`onOpen` ignore dialog Cancel and use `GetFilename()` (cwd-dependent) instead of `GetPath()` | `FMainFrame::onSave`/`onOpen` — `src/FMainFrame.cpp:128-159` |
| F1 | Flaky `FFleetTest::testDecTransitTime` (RNG via `irand(100)` in `FFleet::decTransitTime()`). Add a minimal deterministic **RNG seed seam** so tests can force reproducible rolls | `include/Frontier.h:47` (`irand`) + `FGame` ctor seeding — `src/strategic/FGame.cpp` |
| F5 | **DONE during planning** — added `@date Last Modified` to `FGameConfig::computeSafeTerminatorIndex()` Doxygen block (`include/core/FGameConfig.h:128`). Commit with this plan. |

**Deferred (recorded, not implemented here):** H2 (persist active defense across save/load) and F2-serialization (serialize `m_destroyed`) both change the save wire format and are deferred to the coordinated **save-format hardening** phase (roadmap "Next" #5). This plan creates `doc/deferred-tasks.md` to track them (see §2b decisions 3 & 4 and P2-5).

---

## 2. Confirmed Repository Facts

All verified directly against the working tree during planning:

- **Enums.** `FDefense::Defense = {UNDEF,NONE,RH,MS,ICM,SS,PS,ES}` (`include/defenses/FDefense.h:32`); `FWeapon::Weapon = {NONE,LB,LC,DC,EB,PB,RB,AR,T,SM,M}` (`include/weapons/FWeapon.h:33`).
- **T1.** `FDisruptorCannon` sets `m_range=12` (`src/weapons/FDisruptorCannon.cpp:16`); the manual (weapon text l.951, Weapon Restrictions Table l.1145) says RA **9**.
- **T2 current behavior.** `FWeapon::fire()` computes the defense modifier from a **single** defense: the attacker's current defense if it is `MS`, else the target's `getCurrentDefense()->getAttackModifier(m_type)` (`FWeapon.cpp:135-140`). It never considers the target's other operating defenses, so a raised screen that is *worse* than the hull against the incoming weapon makes the ship easier to hit.
- **T2 rule.** Manual l.235-238 / 411-416: "If the target ship has more than one defensive system operating, the defense that is most effective against the attacking weapon is used." Manual l.1038-1041 (NOTE): some screens **attract** a weapon type, and then "the percentage chance to hit is calculated against the defense that is attracting it, not the defense that is most effective." Attract relations: **Proton Screen attracts Electron Beams**; **Electron Screen attracts Proton Beams**; **Stasis Screen attracts Torpedoes, Seeker Missiles, and Mines** (l.1018-1032).
- **Attract-modifier data (from the tables).** `FProtonScreen::getAttackModifier(EB) = +10`; `FElectronScreen::getAttackModifier(PB) = +10`; `FStasisScreen::getAttackModifier(T) = +25`, `(SM)=+15`, `(M)=+20`. Reflective hull returns `0` for everything (`src/defenses/FReflectiveHull.cpp:26`). These positive (attract) modifiers are *larger* than RH's 0, so the attract override makes the target **easier** to hit — the observable that distinguishes correct from most-effective-only behavior.
- **T3 current behavior.** Damage halving is already laser-only (`FWeapon.cpp:167` gates on `LC||LB`). The **to-hit** branch is the bug: when the *attacker* has `MS` up, **every** weapon type uses the attacker's MS modifier and ignores the target's defense (`FWeapon.cpp:135-137`). Manual l.433-436: the "fired out of the screen" effect applies **only to lasers**.
- **T3 rule.** A masking screen affects a laser fired *out of* it exactly as one fired *into* it; resolve as if the defending ship were masked. Non-laser weapons fired from inside an MS are unaffected by the attacker's MS.
- **S2/S3 current behavior.** In `cleanUpShips()`, Sathar losses increment `m_lostSatharShips` only when `ship->getType() != "Fighter"` (excludes fighters — wrong for condition 3), while `m_lostTendaySathar`/`m_lostTendayUPF` increment for **every** destroyed ship (should exclude fighters **and** militia for condition 5).
- **Militia is identifiable.** Militia is a **fleet-level** flag: `FFleet::isMilitia()` (`include/strategic/FFleet.h:326`); the militia fleets are created via `setMilitia(true, home)` (`src/strategic/FGame.cpp:650…866`). The `cleanUpShips()` loop already iterates fleets (`fleet` in scope) before ships, so `fleet->isMilitia()` is directly available at the counting site. Fighters are `ship->getType() == "Fighter"`.
- **S2/S3 counters are independent.** `m_lostSatharShips` feeds only condition 3; `m_lostTendaySathar`/`m_lostTendayUPF` feed only condition 5. The two fixes do not interact.
- **RV current behavior.** `m_satharRetreat = m_ui->selectRetreatCondition();` (`FGame.cpp:114`) stores the raw return unvalidated. A dialog X-close returns `wxID_CANCEL` (5101), which falls through the `switch` defaults in `showRetreatConditions()`/`checkForVictory()` and silently disables UPF victory for the whole game. Valid values are `1..5`. `IStrategicUI::selectRetreatCondition()` is a re-invocable seam.
- **F2.** `FPlayer::~FPlayer()` deletes `m_fleets` and `m_unattached` but never touches `m_destroyed` (`src/strategic/FPlayer.cpp:27-54`). `addDestroyedShip()` just `push_back`s (`:75-78`). Post-C1, `cleanUpShips()` removes a destroyed ship from its fleet via `FFleet::removeShip(id)` (which erases from the fleet's `m_ships` and **returns without deleting**) and hands the pointer to `addDestroyedShip()` — so after cleanup a destroyed ship is referenced **only** by `m_destroyed`, making `FPlayer` the safe sole owner.
- **F4.** `onSave` calls `d->ShowModal()` with no result check and uses `d->GetFilename()`; `onOpen` calls `FGame::create()` and `load()` regardless of Cancel, also on `GetFilename()`. Both use `wxFD_CHANGE_DIR`, so files only land correctly by mutating the process cwd. (`onClose` was already fixed in C5 and is out of scope.)
- **F1.** `irand(unsigned)` is a free inline over the process-global `rand()` (`include/Frontier.h:47-49`); `FFleet::decTransitTime()` rolls `irand(100) > getRJChance()` for the risk-jump (`src/strategic/FFleet.cpp:98`). `FGame`'s constructor seeds the global RNG with `srand(time(NULL))` (roadmap §3.7).
- **Build/test entry points** (`AGENTS.md`): repo-root `make`; top-level suite `cd tests && make && ./SSWTests`; GUI suite `cd tests/gui && make && xvfb-run -a ./GuiTests`; per-module runners `cd tests/<module> && make && ./<Module>Tests` (weapons → `WeaponsTests`, ships → `ShipsTests`, tactical → `TacticalTests`). **Strategic tests have no standalone runner** — `tests/strategic` builds `libstrategicTests.a` linked into `SSWTests`, so strategic coverage (`FGameTest`, `FPlayerTest`, `FFleetTest`) runs via `./SSWTests`.

## 2b. Design Decisions Resolved With User (binding)

1. **T2/T3 modeling — "Operating set: RH + raised screen."** Compute the to-hit defense modifier over the target's *operating* defenses only: the reflective hull (always operating if the ship owns one) **plus** whichever single screen is currently raised (`getCurrentDefense()`), deduplicated. Choose the **most effective** (minimum to-hit modifier) among them, **except** when one of those operating defenses *attracts* the attacking weapon type, in which case that attracting defense's modifier is used. T3 folds into the same function: the masking-screen laser effect (to-hit modifier + half-damage) applies to lasers only, using the **attacker's** MS when the attacker fired from inside one.
2. **Retreat-condition validation — "Re-prompt until valid."** Validate in the model (`FGame`): if `selectRetreatCondition()` returns a value outside `1..5`, re-invoke it via `IStrategicUI` until a valid `1..5` selection is returned, then store that. (Model-side; no UI-adapter change required.)
3. **H2 — Deferred.** Do **not** persist the active defense in this plan (it breaks the save format). Record it in `doc/deferred-tasks.md` for the save-format hardening phase.
4. **F2 — Fix leak + ownership contract now, defer serialization.** `FPlayer` becomes the documented sole owner of `m_destroyed` and frees them in its destructor. Do **not** serialize `m_destroyed` here; record that deferral in `doc/deferred-tasks.md`.
5. **F1 — Minimal production seed seam.** Add a small deterministic seeding seam the strategic RNG path honors so tests can force reproducible rolls; not the full injectable-RNG refactor (roadmap "Next" #6).
6. **F5 — Done directly during planning** (not a subtask).

## 2c. Assumptions (labeled; not repository-confirmed)

- **(T2/T3 helper location)** The most-effective/attract resolution is expected to live in a new target-side helper on `FVehicle` (e.g. `resolveToHitModifier(FWeapon::Weapon)`), keeping `FWeapon::fire()` thin. The attract relation is a small type-based check inside that helper; **no per-defense interface change is required** (avoids editing all seven defense classes). If the Implementer prefers a per-defense `attracts()` predicate it must justify the wider file surface in its artifact — the default is the single `FVehicle` helper.
- **(F2 double-delete safety)** After C1, destroyed ships are referenced only by `m_destroyed`. The Implementer must confirm no surviving fleet/unattached list still references a ship it is about to free; if any aliasing is found it must be resolved (not left as a latent double-delete).
- **(F1 seam shape)** A minimal seam is two inline helpers in `include/Frontier.h` — a clock-based seed for normal play and an explicit-seed function for deterministic tests/replay — with `FGame`'s constructor switching from raw `srand(time(NULL))` to the clock seam. The exact names are the Implementer's choice; the observable contract (below) is fixed.

---

## 3. Subtasks

Stable IDs `P2-1`…`P2-7`. None require the specialist Security stage (all are local correctness/memory-safety/robustness changes with no attacker-controlled trust boundary; file dialogs read local user-chosen paths only).

### P2-1 — Disruptor Cannon range 12 → 9 (T1)
- **Allowed files:** `src/weapons/FDisruptorCannon.cpp`
- **Task:** Change `m_range` from `12` to `9` in the `FDisruptorCannon` constructor to match the manual (RA 9). Update the constructor's Doxygen `@date Last Modified`/`@author` per repo convention.
- **Acceptance criteria (observable):**
  - A freshly constructed `FDisruptorCannon` reports range `9` (via its range accessor).
  - `setTarget(target, r, …)` with `r == 9` accepts the target; with `r > 9` (e.g. 10–12) the target is rejected (range clamp now excludes 10–12, which the old code accepted).
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Constructor Doxygen header only. No user-facing docs.
- **Security review:** not required.
- **Dependencies:** none. Parallelizable with all other subtasks.

### P2-2 — Most-effective / attracting defense resolution + masking-screen laser-only fix (T2/T3)
- **Allowed files:** `src/weapons/FWeapon.cpp`, `src/ships/FVehicle.cpp`, `include/ships/FVehicle.h`
- **Task:** Replace the single-defense to-hit modifier logic in `FWeapon::fire()` with the manual-correct resolution:
  1. Add a target-side helper on `FVehicle` (recommended `resolveToHitModifier(FWeapon::Weapon w)`) that computes the to-hit modifier over the target's **operating** defenses — the reflective hull (if the ship owns one) plus the currently-raised defense (`getCurrentDefense()`), deduplicated. Enumerate via `getDefense(i)`/`getDefenseCount()` (do not rely on the ambiguous `hasDefense()` return). Return the **minimum** (most effective) modifier, **unless** one operating defense **attracts** `w` — Proton Screen ↔ Electron Beam, Electron Screen ↔ Proton Beam, Stasis Screen ↔ Torpedo/Seeker Missile/Mine — in which case return that attracting defense's modifier instead.
  2. In `fire()`, set the defense modifier as: **if** the weapon is a laser (`LC`/`LB`) **and** the attacker (`m_parent`) has `MS` raised → use the attacker's MS modifier (laser fired out of the screen); **else** → use `m_target->resolveToHitModifier(m_type)`. Keep the existing 5%-floor, ICM, head-on, and CCS-damage adjustments unchanged.
  3. Leave the laser-only half-damage block as-is (it already gates on `LC||LB` and target-or-attacker MS). Guard the `m_parent` MS check against a null parent (mines/seekers fire with no parent).
- **Acceptance criteria (observable):**
  - **Most-effective (no attract):** target owning a reflective hull with a **Proton Screen raised**, fired at by a Laser Battery, resolves to-hit against the **hull** (modifier 0), not the raised Proton Screen (+15) — i.e. the ship is not made *easier* to hit by raising a worse screen.
  - **Attract override (screen worse-but-attracting is forced):** target with a **raised Electron Screen** fired at by a **Proton Beam** resolves against the Electron Screen (attract modifier `+10`), **not** the reflective hull (0), even though the hull is "more effective" — demonstrating the attract override. Analogous behavior for Proton Screen vs Electron Beam and Stasis Screen vs Torpedo/Seeker/Mine.
  - **Masking laser-out (T3):** attacker with `MS` raised firing a **laser** resolves to-hit using the attacker's MS laser modifier and halves damage (rounded up).
  - **Masking non-laser-out (T3 regression):** attacker with `MS` raised firing a **Torpedo** resolves to-hit against the **target's** defense (its `resolveToHitModifier`), **not** the attacker's MS, and does **not** halve damage. (This assertion fails against the pre-fix code.)
  - **Masking laser-in:** attacker without MS firing a laser at a target with `MS` raised resolves against the target's MS (chosen as most effective) and halves damage.
  - No null-deref when a parentless weapon (mine/seeker) fires.
  - Repo builds; existing suites still pass (exact table percentages the Tester derives from the manual's Combat Table).
- **Documentation Impact:** Doxygen headers on `FWeapon::fire()` and the new `FVehicle` helper. No user-facing docs (rules manual is non-editable and already correct).
- **Security review:** not required.
- **Dependencies:** none. Parallelizable (disjoint files from all other subtasks).

### P2-3 — Loss-counter filters: include fighters in condition 3; exclude fighters + militia in condition 5 (S2/S3)
- **Allowed files:** `src/strategic/FGame.cpp`, `include/strategic/FGame.h`
- **Task:** In `FGame::cleanUpShips()`'s destroyed-ship counting block:
  - **S2:** count fighters toward `m_lostSatharShips` (remove the `ship->getType() != "Fighter"` exclusion for that counter) so condition 3 ("40 ships, including fighters") is honored.
  - **S3:** increment the tenday counters `m_lostTendaySathar`/`m_lostTendayUPF` **only** for ships that are neither fighters (`ship->getType() == "Fighter"`) nor members of a militia fleet (`fleet->isMilitia()`), for **both** sides, so condition 5 ("fighters and militia not counted") is honored. Use the `fleet` already in scope in the loop.
  - Do not change conditions 1/2 (`m_lostAC`/`m_lostHC`) or any removal/ownership logic (that is C1/P2-5).
- **Acceptance criteria (observable):**
  - Destroying Sathar fighters increases `m_lostSatharShips` (condition-3 counter) by the fighter count.
  - Destroying fighters (either side) does **not** change `m_lostTendaySathar`/`m_lostTendayUPF`; destroying **militia** ships does **not** change them either; destroying non-fighter, non-militia ships increments the correct side's tenday counter by exactly that count.
  - Condition 5 evaluation (via `checkForVictory` at a tenday boundary) reflects the filtered tenday counts.
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Doxygen header on `cleanUpShips()` (`@date Last Modified`, `@author`, description noting the fighter/militia counting rules). No user-facing docs.
- **Security review:** not required.
- **Dependencies:** **Runs first in the `FGame.cpp` serial lane** (before P2-4 and P2-7).

### P2-4 — Validate the Sathar retreat-condition selection (RV)
- **Allowed files:** `src/strategic/FGame.cpp`, `include/strategic/FGame.h`
- **Task:** At the setup site that assigns `m_satharRetreat` from `m_ui->selectRetreatCondition()` (`FGame.cpp:114`), validate the returned value: if it is outside `1..5`, re-invoke `selectRetreatCondition()` until a valid `1..5` value is returned, then store it. Guard against a null `m_ui` (fall back to leaving the current behavior/console path rather than looping forever with no UI). Do not change the victory switch bodies (that was C2).
- **Acceptance criteria (observable):**
  - Given a mock `IStrategicUI` that first returns an invalid value (e.g. `wxID_CANCEL`/0/6) and then a valid one, `m_satharRetreat` ends up holding the **valid** value, and setup does not proceed with an out-of-range condition.
  - Given a mock that returns a valid `1..5` immediately, it is stored unchanged with no extra prompts.
  - `checkForVictory()` therefore always evaluates a real `1..5` retreat condition after setup (UPF victory is never silently disabled by a bogus stored value).
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Doxygen header on the affected setup function in `include/strategic/FGame.h`. No user-facing docs.
- **Security review:** not required.
- **Dependencies:** **Runs after P2-3** (shared `FGame.cpp`/`FGame.h`); **before P2-7** in the same lane.

### P2-5 — Free `FPlayer::m_destroyed` + document ownership contract; create the deferred-tasks doc (F2)
- **Allowed files:** `src/strategic/FPlayer.cpp`, `include/strategic/FPlayer.h`
- **Task:** In `FPlayer::~FPlayer()`, `delete` every `FVehicle*` in `m_destroyed` and clear the vector (mirroring the `m_fleets`/`m_unattached` cleanup), so the ships transferred by `cleanUpShips()` are no longer leaked. Document the ownership contract on `m_destroyed`/`addDestroyedShip()` in `include/strategic/FPlayer.h`: `FPlayer` is the **sole owner** of destroyed ships once `addDestroyedShip()` is called (they have already been removed — not deleted — from their fleet by `FFleet::removeShip`), and it frees them in its destructor. Before freeing, ensure no destroyed ship is still referenced by `m_fleets`/`m_unattached` (per §2c assumption). **Do not** serialize `m_destroyed` (deferred).
- **Acceptance criteria (observable):**
  - An `FPlayer` that owns ships in `m_destroyed` (e.g. added via `addDestroyedShip()`, or produced by a `cleanUpShips()` run) frees them on destruction — each destroyed ship destructed exactly once, with no double-delete of ships still held by surviving fleets. (Behaviorally demonstrable via an instrumented/counting test vehicle or an object-lifetime hook chosen by the Tester.)
  - Surviving (non-destroyed) ships and fleets are unaffected — still owned and freed via the existing `m_fleets`/`m_unattached` paths.
  - Repo builds; existing suites still pass.
- **Documentation Impact:**
  - Doxygen header on `~FPlayer()` and the `m_destroyed`/`addDestroyedShip()` ownership comment.
  - **Create `doc/deferred-tasks.md`** (new standing list of intentionally-deferred technical items) with entries for: **H2** — persist which defense is active across save/load (from C3 + this plan's decision), target: save-format hardening phase (roadmap "Next" #5); and **F2-serialization** — serialize `FPlayer::m_destroyed` (needed by the future Replacements rule), target: save-format hardening phase. Each entry: short description, origin, why deferred (save-format break), and target phase.
- **Security review:** not required.
- **Dependencies:** none. Parallelizable (disjoint files from all other subtasks). *(Conceptually related to P2-3's counting but touches only `FPlayer`, not `FGame.cpp`.)*

### P2-6 — Harden save/open dialog paths and cancel handling (F4 / H15)
- **Allowed files:** `src/FMainFrame.cpp`, `include/FMainFrame.h`
- **Task:** In `FMainFrame::onSave()` and `onOpen()`: check the `wxFileDialog::ShowModal()` result and **do nothing on Cancel** (no stream opened, no truncation, no `FGame` created/loaded); on OK, use `d->GetPath()` (full path) instead of `d->GetFilename()`. In `onOpen()`, only create the `FGame` and `load()` after a confirmed OK so a cancelled open cannot construct+load from a bogus path. Preserve the existing post-load menu-enable / turn-state logic on the success path. (`onClose`'s save branch was already fixed in C5 — do not re-touch it beyond what these two handlers need.)
- **Acceptance criteria (observable):**
  - Cancelling the Save dialog leaves the filesystem untouched (no file created/truncated) and does not call `FGame::save`.
  - Confirming Save writes the game to the dialog's full `GetPath()` location (not merely a bare filename resolved against the cwd).
  - Cancelling the Open dialog does not create an `FGame` or attempt a load, and leaves the frame in a consistent state.
  - Confirming Open loads from the dialog's full `GetPath()` and restores the post-load menu/turn state as before.
  - Repo builds; the GUI suite passes.
- **Documentation Impact:** Doxygen headers on `onSave()`/`onOpen()`. No user-facing docs.
- **Security review:** not required.
- **Dependencies:** none. Parallelizable (disjoint files from all other subtasks).

### P2-7 — Minimal deterministic RNG seed seam (F1)
- **Allowed files:** `include/Frontier.h`, `src/strategic/FGame.cpp`
- **Task:** Add a minimal seeding seam the shared game RNG honors: an explicit-seed entry point for deterministic tests/replay and a clock-based entry point for normal play (both wrapping the process RNG used by `irand()`). Switch `FGame`'s constructor from raw `srand(time(NULL))` to the clock-based seam so normal play still varies per session. `irand()` itself is unchanged. Ensure required includes (`<ctime>`/`<cstdlib>`) are present where the seam is defined.
- **Acceptance criteria (observable):**
  - Calling the explicit-seed function with a fixed value makes subsequent `irand()` results reproducible: the same seed yields the same sequence of `irand()` outputs across two runs in-process.
  - With a fixed seed set, `FFleet::decTransitTime()`'s risk-jump outcome is reproducible (the same seed → the same transit-time progression), enabling a deterministic `testDecTransitTime`.
  - A newly constructed `FGame` seeds via the clock-based seam (i.e. normal play is not pinned to a constant seed).
  - Repo builds; existing suites still pass.
- **Tester note:** the primary behavioral payoff is making `tests/strategic/FFleetTest.cpp::testDecTransitTime` deterministic by seeding via the new seam in the fixture and asserting the exact expected transit-time sequence.
- **Documentation Impact:** Doxygen headers on the new seam functions and the updated `FGame` constructor. No user-facing docs.
- **Security review:** not required.
- **Dependencies:** **Runs last in the `FGame.cpp` serial lane** (after P2-3 and P2-4). Also touches `include/Frontier.h` (touched by no other subtask).

---

## 4. Dependency Ordering & Parallelization

**Serial lane α — shared `src/strategic/FGame.cpp` / `include/strategic/FGame.h`:**
`P2-3` → `P2-4` → `P2-7` (executed in this order; each edits a distinct region but the shared file forces sequential merge, matching the C1→C2 precedent from Phase 1).

**Independent subtasks — parallelizable with lane α and with each other (all touch disjoint files):**
- `P2-1` — `src/weapons/FDisruptorCannon.cpp`
- `P2-2` — `src/weapons/FWeapon.cpp`, `src/ships/FVehicle.{cpp,h}`
- `P2-5` — `src/strategic/FPlayer.{cpp,h}`
- `P2-6` — `src/FMainFrame.{cpp,h}`

No cross-subtask design agreement is required; the only ordering constraint is the shared-file lane. `P2-2` and `P2-3` both concern combat/victory rules but edit disjoint files (`FWeapon`/`FVehicle` vs `FGame`) and are safe to run concurrently.

---

## 5. Documentation Impact (overall)

- **`doc/synthesized-roadmap.md`** — annotate the addressed items as RESOLVED (mirroring the C1–C6 tags): T1 (§6.1), T2, T3, S2, S3 (§5.4), the retreat-condition validation Medium (§2 "Notable Medium"), and the reviewer follow-ups F1, F2, F4. Each item's owning subtask's Documenter adds its own annotation; since these are distinct rows/lines, the Coordinator union-merges them at integration (as in Phase 1). H2 and F2-serialization remain **open** and move to `doc/deferred-tasks.md`.
- **`doc/deferred-tasks.md`** — **created by P2-5** as the standing deferred-items list (H2, F2-serialization). Future planners append here.
- **Per-subtask Doxygen headers** — mandatory on every touched function/class per `CLAUDE.md` (`@date Last Modified`, add current agent model to `@author`, refresh description/params).
- **`doc/test-contracts.md`** — only if a subtask adds/edits a tactical *source-contract* test; these fixes are behavioral, so no change is expected. Do not expand source-string matching.
- **Non-editable:** `doc/rules/tactical_operations_manual.md` and `doc/rules/second-sathar-war-rules.md` must not be touched (they are the correctness oracle these fixes conform to).

## 6. Untouched Files / Constraints

- **Do not edit** `doc/rules/tactical_operations_manual.md`, `doc/rules/second-sathar-war-rules.md`, `AGENTS.md`, `CLAUDE.md`, `doc/DesignNotes.md`.
- **Do not** change the save wire format anywhere (H2 and `m_destroyed` serialization are deferred). P2-5 must not add serialization; P2-3/P2-4 must not touch `save()`/`load()`.
- **Do not** edit generated wxFormBuilder headers.
- Keep the six non-GUI module Makefiles free of active `wx-config` usage (unchanged by this plan).

---

## 7. Implementer Prompts (Coordinator-ready)

Each subtask's artifacts go under `artifacts/phase2-rules-correctness/<ID>/` (repository-root-relative). None require the Security stage.

### P2-1 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Fix tactical defect T1: the Disruptor Cannon range is 12 but the tactical operations manual (RA 9; weapon text l.951 and the Weapon Restrictions Table l.1145) requires 9.

Allowed files (edit only these):
- src/weapons/FDisruptorCannon.cpp

Task:
- In the FDisruptorCannon constructor, change m_range from 12 to 9.
- Update the constructor's Doxygen header per CLAUDE.md: set @date Last Modified to today and add your model (e.g. "Claude Opus 4.8 (high)") to @author.

Acceptance criteria (implementation outcomes):
- A freshly constructed FDisruptorCannon reports range 9.
- setTarget(target, 9, ...) accepts the target; setTarget(target, 10..12, ...) rejects it (range clamp now excludes 10-12).
- Repo builds clean; existing suites still pass.

Validation:
- Build from repo root: `make`.
- Module tests: `cd tests/weapons && make && ./WeaponsTests`.
- Full suite: `cd tests && make && ./SSWTests`.

Tester handoff:
- Behavioral tests belong in tests/weapons/FDisruptorCannonTest.{h,cpp} (and/or the existing range-clamp coverage in tests/tactical/FTacticalBatteryRangeClampTest). A behavioral test must construct a real FDisruptorCannon and assert range 9 and the 9-accept / 10-reject setTarget behavior; it must fail against the old value 12.

Artifacts:
- Write your report/results under artifacts/phase2-rules-correctness/P2-1/ (repository-root-relative).

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P2-2 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Fix tactical defects T2 and T3 in the weapon to-hit resolution. Per the manual, a target's to-hit is resolved against the MOST EFFECTIVE of its OPERATING defenses, EXCEPT an attracting screen overrides; and the masking-screen "fired out of the screen" effect applies to LASERS ONLY. The current code resolves against a single defense and wrongly applies the attacker's masking screen to every weapon type.

Allowed files (edit only these):
- src/weapons/FWeapon.cpp
- src/ships/FVehicle.cpp
- include/ships/FVehicle.h

Task:
1. Add a target-side helper on FVehicle (recommended signature: int resolveToHitModifier(FWeapon::Weapon w)) that:
   - Considers only the target's OPERATING defenses: the reflective hull (if the ship owns one) plus the currently-raised defense from getCurrentDefense(), deduplicated. Enumerate via getDefense(i)/getDefenseCount(); do NOT use the ambiguous hasDefense() return value.
   - Returns the MINIMUM (most effective) getAttackModifier(w) among those operating defenses, EXCEPT: if an operating defense ATTRACTS w, return that defense's modifier instead. Attract relations: Proton Screen (PS) attracts Electron Beam (EB); Electron Screen (ES) attracts Proton Beam (PB); Stasis Screen (SS) attracts Torpedo (T), Seeker Missile (SM), and Mine (M).
2. In FWeapon::fire(), replace the current single-defense modifier logic (the m_parent MS vs m_target->getCurrentDefense() branch) with:
   - if the weapon is a laser (LC or LB) AND m_parent != NULL AND m_parent has MS raised: use m_parent's MS getAttackModifier(m_type) (laser fired out of the screen);
   - else: use m_target->resolveToHitModifier(m_type).
   Keep the existing 5%-floor, ICM, head-on, and combat-control-damage adjustments unchanged. Guard the m_parent MS check against a null parent (mines/seekers have none).
3. Leave the laser-only half-damage block unchanged (it already gates on (LC||LB) and target-or-attacker MS), but ensure it does not deref a null m_parent.

Acceptance criteria (implementation outcomes):
- Target with reflective hull + a RAISED Proton Screen, hit by a Laser Battery: to-hit resolves against the hull (modifier 0), not the Proton Screen (+15).
- Target with a RAISED Electron Screen, hit by a Proton Beam: to-hit resolves against the Electron Screen (attract modifier +10), NOT the hull (0) — the attract override. (Analogous for PS vs EB and SS vs T/SM/M.)
- Attacker with MS raised firing a LASER: to-hit uses the attacker's MS laser modifier and damage is halved (rounded up).
- Attacker with MS raised firing a TORPEDO: to-hit resolves against the TARGET's defense (resolveToHitModifier), NOT the attacker's MS, and damage is NOT halved. (This fails against the pre-fix code.)
- Attacker without MS firing a laser at a target with MS raised: resolves against the target's MS and halves damage.
- A parentless weapon (mine/seeker) fires with no null-deref.
- Repo builds clean; existing suites still pass.

Validation:
- Build from repo root: `make`.
- Module tests: `cd tests/weapons && make && ./WeaponsTests` and `cd tests/ships && make && ./ShipsTests`.
- Full suite: `cd tests && make && ./SSWTests`.

Tester handoff:
- Behavioral tests belong in tests/weapons/ (FWeaponTest / weapon-specific fixtures) and/or tests/ships/FTacticalAttackIntegrationTest. Tests must construct real FVehicle targets/attackers with the relevant defenses raised and assert observed to-hit probability and damage outcomes (deriving exact Combat-Table percentages from the manual). The torpedo-out-of-MS test must fail against the unfixed code.

Artifacts:
- Write your report/results under artifacts/phase2-rules-correctness/P2-2/ (repository-root-relative).

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P2-3 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Fix strategic rules deviations S2 and S3 in the post-combat loss counting. Retreat condition 3 must count fighters ("40 ships, including fighters"); retreat condition 5 must NOT count fighters or militia ("Fighters and Militia ships are not counted").

Allowed files (edit only these):
- src/strategic/FGame.cpp
- include/strategic/FGame.h

Task:
- In FGame::cleanUpShips(), in the destroyed-ship counting block:
  - S2: include fighters in m_lostSatharShips (remove the getType() != "Fighter" exclusion guarding that counter).
  - S3: increment m_lostTendaySathar / m_lostTendayUPF ONLY when the destroyed ship is neither a fighter (getType() == "Fighter") nor a member of a militia fleet (use the in-scope fleet->isMilitia()), for BOTH sides.
- Do NOT change conditions 1/2 (m_lostAC/m_lostHC), the removal/ownership logic, or save()/load().
- Update the cleanUpShips() Doxygen header (@date Last Modified, add your model to @author, note the fighter/militia counting rules).

Acceptance criteria (implementation outcomes):
- Destroying Sathar fighters increases m_lostSatharShips by the fighter count.
- Destroying fighters (either side) does NOT change m_lostTendaySathar/m_lostTendayUPF; destroying militia ships does NOT change them; destroying non-fighter non-militia ships increments the correct side's tenday counter by exactly that count.
- checkForVictory() at a tenday boundary evaluates condition 5 against the filtered tenday counts.
- Repo builds clean; existing suites still pass.

Validation:
- Build from repo root: `make`.
- Full suite (strategic tests link here): `cd tests && make && ./SSWTests`.

Tester handoff:
- Behavioral tests belong in tests/strategic/FGameTest.{h,cpp}. Construct real fleets containing fighters, militia (fleet setMilitia(true, ...)), and regular ships at HP<=0, run cleanUpShips(), and assert the exact counter deltas above. Tests must fail against the unfixed counting.

Artifacts:
- Write your report/results under artifacts/phase2-rules-correctness/P2-3/ (repository-root-relative).

Dependency: this is the FIRST subtask in the shared-FGame.cpp lane; it runs before P2-4 and P2-7.

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P2-4 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Validate the Sathar retreat-condition selection. Currently m_satharRetreat is assigned the raw return of m_ui->selectRetreatCondition() (FGame.cpp:114); an X-close/cancel returns wxID_CANCEL and silently disables UPF victory for the whole game. Valid values are 1..5.

Allowed files (edit only these):
- src/strategic/FGame.cpp
- include/strategic/FGame.h

Task:
- At the setup site assigning m_satharRetreat, validate the value: if selectRetreatCondition() returns outside 1..5, re-invoke it via m_ui until a valid 1..5 value is returned, then store it. Guard against a null m_ui (do not loop forever with no UI; fall back to the existing no-UI behavior/console path).
- Do NOT change the checkForVictory() switch bodies (that was C2) or the retreat-condition display text.
- Update the affected function's Doxygen header (@date Last Modified, @author, description).

Acceptance criteria (implementation outcomes):
- With a mock IStrategicUI returning an invalid value first (e.g. wxID_CANCEL / 0 / 6) then a valid one, m_satharRetreat ends up holding the VALID value; setup does not proceed with an out-of-range condition.
- With a mock returning a valid 1..5 immediately, it is stored unchanged with no extra prompts.
- After setup, checkForVictory() always evaluates a real 1..5 condition (UPF victory is never silently disabled by a stored bogus value).
- Repo builds clean; existing suites still pass.

Validation:
- Build from repo root: `make`.
- Full suite: `cd tests && make && ./SSWTests`.

Tester handoff:
- Behavioral tests belong in tests/strategic/ using a mock IStrategicUI (see FGameMockStrategicUITest patterns). Assert the re-prompt-until-valid behavior and the stored value. A test must fail against the unvalidated assignment.

Artifacts:
- Write your report/results under artifacts/phase2-rules-correctness/P2-4/ (repository-root-relative).

Dependency: runs AFTER P2-3 and BEFORE P2-7 in the shared-FGame.cpp lane.

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P2-5 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Fix the FPlayer::m_destroyed memory leak (reviewer follow-up F2). After C1, cleanUpShips() removes destroyed ships from their fleets (via FFleet::removeShip, which does NOT delete) and hands them to FPlayer::addDestroyedShip(); but FPlayer::~FPlayer() never frees m_destroyed, so those ships leak. Establish FPlayer as the sole owner and free them. DO NOT serialize m_destroyed (that is deferred to the save-format phase).

Allowed files (edit only these):
- src/strategic/FPlayer.cpp
- include/strategic/FPlayer.h

Task:
- In FPlayer::~FPlayer(), delete every FVehicle* in m_destroyed and clear the vector (mirror the m_fleets/m_unattached cleanup).
- Before freeing, confirm no destroyed ship is still referenced by m_fleets/m_unattached (post-C1 it is referenced only by m_destroyed). If you find aliasing, resolve it; do not leave a latent double-delete. Record what you verified in your artifact.
- Document the ownership contract in include/strategic/FPlayer.h on m_destroyed and addDestroyedShip(): FPlayer is the sole owner of destroyed ships once addDestroyedShip() is called, and frees them in the destructor.
- Update the ~FPlayer() Doxygen header (@date Last Modified, @author, description).
- Do NOT touch save()/load().

Acceptance criteria (implementation outcomes):
- An FPlayer owning ships in m_destroyed frees them on destruction — each destructed exactly once, with no double-delete of ships still held by surviving fleets.
- Surviving (non-destroyed) ships/fleets are unaffected (still freed via the existing m_fleets/m_unattached paths).
- Repo builds clean; existing suites still pass.

Documentation deliverable (in addition to code):
- CREATE doc/deferred-tasks.md as the standing list of intentionally-deferred technical items, with entries for:
  (1) H2 — persist which defense is active across save/load (origin: C3 + Phase-2 decision); deferred because it breaks the save wire format; target: save-format hardening phase (roadmap "Next" #5).
  (2) F2-serialization — serialize FPlayer::m_destroyed (needed by the future Replacements rule); deferred for the same save-format reason; same target phase.
  Each entry: short description, origin, why deferred, target phase.

Validation:
- Build from repo root: `make`.
- Full suite: `cd tests && make && ./SSWTests`.

Tester handoff:
- Behavioral tests belong in tests/strategic/FPlayerTest.{h,cpp}. Use an instrumented/counting test vehicle (or an object-lifetime hook) to assert destroyed ships are freed exactly once on FPlayer destruction and that surviving-fleet ships are not double-freed. The test must fail (leak/absence of delete) against the unfixed destructor.

Artifacts:
- Write your report/results under artifacts/phase2-rules-correctness/P2-5/ (repository-root-relative).

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P2-6 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Harden the strategic Save/Open file-dialog paths (reviewer follow-up F4 / roadmap H15). onSave()/onOpen() ignore dialog Cancel and use GetFilename() (cwd-dependent) instead of GetPath(); a cancelled save can truncate a stale filename in the cwd, and a cancelled open constructs+loads from a bogus path.

Allowed files (edit only these):
- src/FMainFrame.cpp
- include/FMainFrame.h

Task:
- In onSave(): check wxFileDialog::ShowModal(). On wxID_CANCEL (or non-OK), do nothing (no ofstream opened, no FGame::save). On OK, save to d->GetPath() (full path), not GetFilename().
- In onOpen(): check ShowModal() first. On Cancel/non-OK, do NOT create an FGame or load. On OK, create the FGame and load() from d->GetPath(), then run the existing post-load menu-enable / turn-state logic unchanged.
- Do NOT re-touch onClose()'s save branch (already fixed in C5) beyond what these handlers require.
- Update the onSave()/onOpen() Doxygen headers (@date Last Modified, @author, description).

Acceptance criteria (implementation outcomes):
- Cancelling Save leaves the filesystem untouched (no file created/truncated) and does not call FGame::save.
- Confirming Save writes to the dialog's full GetPath() location (not a bare cwd-relative filename).
- Cancelling Open creates no FGame and attempts no load; the frame stays consistent.
- Confirming Open loads from GetPath() and restores the post-load menu/turn state as before.
- Repo builds clean; the GUI suite passes.

Validation:
- Build from repo root: `make`.
- GUI suite (needs a display): `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester handoff:
- Behavioral tests belong in tests/gui/StrategicGuiLiveTest.{h,cpp} using the WXGuiTestHarness helpers (drive the dialog via showModalWithAction / runModalFunctionWithAction). Assert the cancel-no-op and GetPath() behaviors. Tests must fail against the pre-fix handlers.

Artifacts:
- Write your report/results under artifacts/phase2-rules-correctness/P2-6/ (repository-root-relative).

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P2-7 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Add a minimal deterministic RNG seed seam (reviewer follow-up F1) so the flaky FFleetTest::testDecTransitTime can be made deterministic. The flakiness comes from irand(100) (global rand()) in FFleet::decTransitTime(). This is the minimal seam, NOT the full injectable-RNG refactor.

Allowed files (edit only these):
- include/Frontier.h
- src/strategic/FGame.cpp

Task:
- In include/Frontier.h, add a minimal seeding seam the shared RNG (used by irand()) honors: an explicit-seed entry point for deterministic tests/replay and a clock-based entry point for normal play (both wrapping the process RNG seed). Add required includes (<ctime>/<cstdlib>) as needed. Leave irand() itself unchanged.
- In src/strategic/FGame.cpp, switch the constructor from raw srand(time(NULL)) to the new clock-based seam so normal play still varies per session.
- Update the Doxygen headers on the new seam functions and the FGame constructor.

Acceptance criteria (implementation outcomes):
- Calling the explicit-seed function with a fixed value makes subsequent irand() outputs reproducible (same seed -> same sequence in-process).
- With a fixed seed set, FFleet::decTransitTime()'s risk-jump outcome is reproducible (same seed -> same transit-time progression).
- A newly constructed FGame seeds via the clock-based seam (normal play is not pinned to a constant seed).
- Repo builds clean; existing suites still pass.

Validation:
- Build from repo root: `make`.
- Full suite: `cd tests && make && ./SSWTests`.

Tester handoff:
- The behavioral payoff is deterministic strategic tests: update tests/strategic/FFleetTest.cpp::testDecTransitTime to seed via the new seam in the fixture and assert the exact expected transit-time sequence, removing the ~9% intermittent failure. Optionally add a small Frontier-level test asserting seed -> reproducible irand() sequence.

Artifacts:
- Write your report/results under artifacts/phase2-rules-correctness/P2-7/ (repository-root-relative).

Dependency: runs LAST in the shared-FGame.cpp lane (after P2-3 and P2-4).

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

---

## 8. Completion Definition

This plan is complete when P2-1…P2-7 are implemented, tested (behaviorally), documented, verified, and merged back through the Coordinator workflow; `doc/deferred-tasks.md` exists with the H2 and F2-serialization entries; the roadmap is annotated with the resolved items; and the F5 cosmetic edit (already applied) is committed. No save-format change is introduced. The final Reviewer pass runs after all subtasks merge.
