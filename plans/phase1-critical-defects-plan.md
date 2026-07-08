# Phase 1 — Critical Defects (C1–C6) Implementation Plan

**Author:** Planner (Claude Opus 4.8, high)
**Date:** 2026-07-08
**Source roadmap:** `doc/synthesized-roadmap.md` §2 "Confirmed Defects → Critical", and Part III "Now".
**Scope chosen by user:** the six Critical, game-corrupting/crashing defects C1–C6. Nothing else from the roadmap is in scope for this plan.

---

## 1. Feature Restatement (engineering terms)

Fix the six confirmed Critical defects that corrupt game outcomes or crash the process. Each is a small, surgical correctness fix in one function, in one source file (plus a mandatory Doxygen header update in that function's declaring header). No new mechanics, no refactors, no format changes.

| ID | Defect | Function / file (confirmed) |
|----|--------|------------------------------|
| C1 | `cleanUpShips()` mutates **copies** of the fleet/ship vectors, so destroyed ships are never removed from real fleets, loss counters inflate every pass, ships are re-recorded, and empty fleets are never removed | `FGame::cleanUpShips()` — `src/strategic/FGame.cpp:1185-1221` |
| C2 | Sathar retreat conditions 4 and 5 are evaluated **swapped** relative to the displayed condition text | `FGame::checkForVictory()` — `src/strategic/FGame.cpp:1046-1064` (display text at `:980-993`) |
| C3 | `FVehicle::load()` rebuilds `m_defenses` but never reassigns `m_currentDefense`, leaving it dangling at the freed constructor-time defense (use-after-free in combat after any load) | `FVehicle::load()` — `src/ships/FVehicle.cpp:217-229` |
| C4 | `TwoPlanetsGUI` returns `EndModal(1)`/`EndModal(2)`; caller uses the raw return as a 0-based index into a 2-element planet list → button 1 targets the wrong planet, button 2 indexes `[2]` (out of bounds) | `SelectCombatGUI::onAttack()` — `src/gui/SelectCombatGUI.cpp:212-273` |
| C5 | Close dialog is `wxYES_NO\|wxCANCEL` but the "save?" branch tests `result == wxID_OK`; a Yes press returns `wxID_YES`, so "Yes, save" silently discards the game | `FMainFrame::onClose()` — `src/FMainFrame.cpp:298-309` |
| C6 | `readlink("/proc/self/exe", …)` return stored in a `size_t`; on failure (−1 → huge) or a ≥1000-char path, `buf[size] = 0` is an out-of-bounds write, in a singleton constructor with no error path | `FGameConfig::FGameConfig()` — `src/core/FGameConfig.cpp:105-117` |

---

## 2. Confirmed Repository Facts

All verified directly against the working tree during planning:

- **C1 removal APIs exist and are usable.** `FFleet::removeShip(unsigned int id)` (`src/strategic/FFleet.cpp:71`) finds by ID, erases from `m_ships`, and **returns the removed pointer without deleting it**. `FPlayer::removeFleet(unsigned int id)` (`src/strategic/FPlayer.cpp:162`) does the same for `m_fleets`. `FSystem::removeFleet(id)` is already called by the current cleanup code (`FGame.cpp:1216`). So C1 needs no new API — only correct use of existing ones on the real (non-copied) containers.
- **C1 accessors return copies vs references.** `FPlayer::getFleetList()` returns `FleetList&` (a reference), but the buggy code assigns it to a **by-value** local `FleetList fList = …`; likewise `FFleet::getShipList()` returns `const VehicleList&` and is copied into `VehicleList sList = …`. The mutation-on-copy is the root cause.
- **C2 fix direction is certain.** Display text: condition **4 = "a tenday passes without the Sathar destroying at least two stations…"** (`FGame.cpp:980-985`, status shows `m_stationsDestroyed`); condition **5 = "the Sathar have lost more ships … than the UPF"** (`:987-993`, status shows `m_lostTendaySathar`/`m_lostTendayUPF`). `checkForVictory()` implements case 4 as the **ship-loss** test and case 5 as the **stations** test — inverted. Fix = swap the two case bodies **including their counter-reset lines** (`m_stationsDestroyed = 0` belongs with the stations condition; `m_lostTendaySathar/UPF = 0` belongs with the ship-loss condition).
- **C3.** `FVehicle::load()` deletes/clears `m_defenses`, reloads defense objects, and returns without touching `m_currentDefense`. Ships always serialize at least the base `FNone` defense (constructor puts it at `m_defenses[0]`), so index 0 is a safe post-load default.
- **C4.** `TwoPlanetsGUI::OnButton1/OnButton2` are defined inline in the **wxFormBuilder-generated header** `include/gui/TwoPlanetsGUI.h:40-41` (banner: "PLEASE DO 'NOT' EDIT THIS FILE"). `SelectCombatGUI::onAttack` uses the class directly (no subclass) and does `planet = d.ShowModal();` then `m_system->getPlanetList()[planet]` at lines 227, 258, 273.
- **C5.** `onClose` uses `wxYES_NO|wxCANCEL|wxYES_DEFAULT|wxICON_QUESTION`; it correctly handles `wxID_CANCEL` but the save branch tests `wxID_OK` instead of `wxID_YES`.
- **C6.** Linux branch: `size_t size = readlink("/proc/self/exe", buf, bufsize);` with `char buf[1000]; int bufsize = 1000;` then `buf[size] = 0;`.
- **Build/test entry points** (from `AGENTS.md`): repo-root `make`; top-level suite `cd tests && make && ./SSWTests`; GUI suite `cd tests/gui && make && xvfb-run -a ./GuiTests`; per-module runners `cd tests/<module> && make && ./<Module>Tests`.

## 2a. Assumptions (labeled; not repository-confirmed)

- **(Scope, roadmap-backed)** C1 is limited to the copy-mutation correctness fix. The deeper **destroyed-ship ownership contract** (whether `m_destroyed` owns/frees ships; roadmap finding #8 / the Medium "`m_destroyed` never freed" item) is **Phase 2** and explicitly out of scope here. Removed destroyed-ship pointers remain in the player's destroyed list as today (no new `delete` of ships).
- **(Scope, roadmap-backed)** C3 is limited to eliminating the dangling pointer. **Persisting which defense was active** across save/load (roadmap H2) is a separate item and out of scope; after load the active defense resets to the base `FNone` at `m_defenses[0]`.
- **(C1 fleet lifetime)** An emptied fleet, once removed from both the player (`FPlayer::removeFleet`) and its system (`FSystem::removeFleet`), has no other known owner and may be `delete`d to avoid a leak. If the Implementer finds another live reference (e.g., in-transit tracking) it must remove that reference too or skip the delete and note it in the artifact — but it must **not** leave the destroyed ship in the fleet.
- **(C4 mechanism)** The generated `TwoPlanetsGUI.h` will **not** be edited; the fix is caller-side in `SelectCombatGUI::onAttack` — translate the dialog's `1`/`2` return to a 0-based index and clamp any other/cancel value into range.

---

## 3. Subtasks

Stable IDs `CRIT-1`…`CRIT-6` map 1:1 to roadmap defects C1…C6.

### CRIT-1 — Make `cleanUpShips()` mutate the real fleets/players
- **Allowed files:** `src/strategic/FGame.cpp`, `include/strategic/FGame.h`
- **Task:** Rewrite the post-combat cleanup loop in `FGame::cleanUpShips()` so it operates on the **live** containers, not by-value copies. Use existing APIs: iterate players by reference; for each fleet, identify destroyed ships (HP ≤ 0); for each, record it once via `addDestroyedShip`, apply the loss-counter increments exactly once, and remove it from the **real** fleet with `FFleet::removeShip(id)`. Avoid the erase-while-iterating skip bug (e.g., collect target IDs first, then mutate). For any fleet left empty, remove it from the real player via `FPlayer::removeFleet(id)` **and** from its system via `FSystem::removeFleet(id)`, then delete the emptied fleet object (subject to the fleet-lifetime assumption in §2a). Preserve the existing surviving-ship `reload()` behavior and the `@todo` supply note.
- **Acceptance criteria (observable):**
  - After cleanup, a ship whose HP ≤ 0 is **absent** from its owning `FFleet::getShipList()` and present in the owning `FPlayer` destroyed record exactly once.
  - Loss counters (`m_lostTendayUPF`/`m_lostTendaySathar`/`m_lostSatharShips`/`m_lostHC`/`m_lostAC`) increase by exactly the number of matching destroyed ships across one `cleanUpShips()` call — no inflation from re-scanning.
  - A fleet emptied by cleanup is absent from both its `FPlayer` fleet list and its `FSystem` fleet list afterward.
  - Surviving ships remain in their fleet and have been `reload()`ed.
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Update the Doxygen header on `cleanUpShips()` (`@date Last Modified`, add current model to `@author`, refresh description). No user-facing docs expected.
- **Security review:** not required.
- **Dependencies:** none. **Must run before CRIT-2** (same file/header).

### CRIT-2 — Un-swap retreat conditions 4 and 5 in `checkForVictory()`
- **Allowed files:** `src/strategic/FGame.cpp`, `include/strategic/FGame.h`
- **Task:** In `FGame::checkForVictory()`, make `case 4` evaluate the **stations** condition (`m_stationsDestroyed < 2` at tenday end) with its `m_stationsDestroyed = 0` reset, and `case 5` evaluate the **ship-loss** condition (`m_lostTendaySathar > m_lostTendayUPF` at tenday end) with its `m_lostTendaySathar = 0; m_lostTendayUPF = 0;` resets, matching the displayed condition text at `FGame.cpp:980-993`. Do **not** alter the fighter/militia counting (S2/S3 are out of scope). Keep the tenday gate (`m_round % 10 == 0 && m_round > 0`).
- **Acceptance criteria (observable):**
  - With retreat condition 4 selected, `checkForVictory()` reports UPF victory at a tenday boundary iff fewer than two stations were destroyed that tenday; the station counter resets at the boundary.
  - With retreat condition 5 selected, it reports UPF victory at a tenday boundary iff the Sathar lost more tenday ships than the UPF; the tenday loss counters reset at the boundary.
  - The evaluated condition now matches the condition text shown for the same case number.
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Update the Doxygen header on `checkForVictory()` (`@date Last Modified`, `@author`, description). No user-facing docs expected.
- **Security review:** not required.
- **Dependencies:** **Runs after CRIT-1** (shared `FGame.cpp`/`FGame.h`).

### CRIT-3 — Reset `m_currentDefense` after `FVehicle::load()`
- **Allowed files:** `src/ships/FVehicle.cpp`, `include/ships/FVehicle.h`
- **Task:** After `FVehicle::load()` rebuilds `m_defenses`, set `m_currentDefense` to a valid live defense: `m_defenses[0]` when the vector is non-empty; otherwise a safe fallback (e.g., recreate a default `FNone` and point at it) so `getCurrentDefense()` never returns a dangling/null pointer. Do not add persistence of the previously-active defense (H2, out of scope).
- **Acceptance criteria (observable):**
  - After `save()` then `load()` on a vehicle, `getCurrentDefense()` returns a valid, live defense object (calling `getType()` on it is safe) rather than a freed pointer.
  - A loaded vehicle can be fired upon / enter the combat damage path without a use-after-free.
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Update the Doxygen header on `FVehicle::load()` in `include/ships/FVehicle.h`. No user-facing docs expected.
- **Security review:** not required.
- **Dependencies:** none. Parallelizable with C1/C2/C4/C5/C6.

### CRIT-4 — Fix planet-index selection in `SelectCombatGUI::onAttack()`
- **Allowed files:** `src/gui/SelectCombatGUI.cpp`, `include/gui/SelectCombatGUI.h`
- **Task:** In `onAttack()`, translate the `TwoPlanetsGUI` return value into a **valid 0-based index** into `m_system->getPlanetList()`: button 1 → index 0, button 2 → index 1, and any other/cancel value → a safe in-range default (index 0). Use the translated index consistently at all three `getPlanetList()[planet]` sites (attack target, station lookup, `destroyStation`). **Do not edit** the generated `include/gui/TwoPlanetsGUI.h`.
- **Acceptance criteria (observable):**
  - Choosing the first planet resolves combat against `getPlanetList()[0]`; choosing the second resolves against `getPlanetList()[1]`.
  - No code path indexes `getPlanetList()` out of range for a 2-planet system, regardless of dialog outcome (including cancel/close).
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Update the Doxygen header on `SelectCombatGUI::onAttack()`. No user-facing docs expected.
- **Security review:** not required (memory-safety/correctness fix; no trust boundary or untrusted external input).
- **Dependencies:** none. Parallelizable.

### CRIT-5 — Save on "Yes" when closing (`FMainFrame::onClose()`)
- **Allowed files:** `src/FMainFrame.cpp`, `include/FMainFrame.h`
- **Task:** In `onClose()`, change the save-branch test from `result == wxID_OK` to `result == wxID_YES` so a Yes response saves before reset. Keep the existing `wxID_CANCEL` early-return and the subsequent `resetGame()`. (Out of scope: the broader save/open dialog path/cancel hardening — roadmap H15.)
- **Acceptance criteria (observable):**
  - Answering **Yes** to the close prompt invokes the save path before the game is reset.
  - Answering **No** resets without saving; **Cancel** aborts the close without resetting or saving.
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Update the Doxygen header on `FMainFrame::onClose()`. Documenter should verify whether `doc/UsersGuide.md` describes close/save behavior and needs a wording tweak now that Yes actually saves (low likelihood).
- **Security review:** not required.
- **Dependencies:** none. Parallelizable.

### CRIT-6 — Guard the `readlink` return in `FGameConfig`
- **Allowed files:** `src/core/FGameConfig.cpp`, `include/core/FGameConfig.h`
- **Task:** In the Linux branch of the `FGameConfig` constructor, capture `readlink`'s return in a signed `ssize_t`, pass a length that reserves room for the terminator (`sizeof(buf) - 1`), and only write the null terminator at a clamped, in-bounds index. On failure (return ≤ 0) fall back safely (e.g., empty/known default path) instead of writing `buf[(size_t)-1]`. No out-of-bounds write may occur for any return value or path length.
- **Acceptance criteria (observable):**
  - For a normal executable path, `m_basePath`/`m_executablePath` resolve as before.
  - A `readlink` failure (return −1) does not write out of bounds and leaves the config in a safe, defined state.
  - A path at/above the buffer capacity does not write past `buf`.
  - Repo builds; existing suites still pass.
- **Documentation Impact:** Update the Doxygen header on the `FGameConfig` constructor. No user-facing docs expected.
- **Security review:** not required. Rationale: this hardens an unchecked syscall return (memory safety) but reads the process's own `/proc/self/exe`, not attacker-controlled input crossing a trust boundary; it does not meet the specialist-security threshold. Acceptance criteria already require no OOB write.
- **Dependencies:** none. Parallelizable.

---

## 4. Dependency Ordering & Parallelization

- **Serial lane (shared `src/strategic/FGame.cpp` + `include/strategic/FGame.h`):** `CRIT-1` → `CRIT-2`. These edit the same file/header and must not run concurrently.
- **Independent lanes (disjoint files, safely parallel with the serial lane and with each other):** `CRIT-3`, `CRIT-4`, `CRIT-5`, `CRIT-6`.
- Recommended Coordinator execution: run `CRIT-1` first; once it merges back, run `CRIT-2`; run `CRIT-3`/`CRIT-4`/`CRIT-5`/`CRIT-6` in parallel at any time. Use a dedicated per-plan coordination branch (never `master`).

---

## 5. Overall Documentation Impact

- **Mandatory (Implementer, per `CLAUDE.md`):** each modified function's Doxygen header gets an updated `@date Last Modified`, an added `@author` entry for the implementing model, and a refreshed description.
- **Optional (Documenter):** annotate `doc/synthesized-roadmap.md` to mark C1–C6 as resolved once merged (traceability only; not required for feature correctness).
- **Verify (Documenter):** `doc/UsersGuide.md` close/save wording for C5 (low likelihood).
- **Untouched:** `AGENTS.md`, `CLAUDE.md`, `doc/DesignNotes.md`, and — non-negotiably — `doc/rules/tactical_operations_manual.md`.
- Artifacts are repository-root-relative under `artifacts/phase1-critical-defects/<subtask-id>/`.

---

## 6. Implementer Prompts (Coordinator-ready)

### CRIT-1
```
Your role is 'implementer'. Your task is as follows:

Fix Critical defect C1: FGame::cleanUpShips() mutates by-value copies of the fleet and ship
vectors, so destroyed ships are never removed from the real fleets, loss counters inflate on
re-scan, ships are re-recorded, and empty fleets are never removed.

Allowed files:
- src/strategic/FGame.cpp
- include/strategic/FGame.h

Implement:
- Rewrite the cleanup loop in FGame::cleanUpShips() (around src/strategic/FGame.cpp:1185-1221)
  to operate on the LIVE containers, not by-value copies.
- Iterate players by reference. For each fleet, find ships with getHP() <= 0; for each such
  ship, call addDestroyedShip once and apply the loss-counter increments once, then remove it
  from the real fleet using FFleet::removeShip(id) (defined at src/strategic/FFleet.cpp:71,
  returns the pointer without deleting).
- Avoid the erase-while-iterating skip bug (e.g., collect the target ship IDs first, then
  mutate).
- For any fleet left empty, remove it from the real player via FPlayer::removeFleet(id)
  (src/strategic/FPlayer.cpp:162) AND from its system via FSystem::removeFleet(id), then delete
  the emptied, now-orphaned fleet object. If you find another live reference to that fleet
  (e.g., in-transit tracking), remove that reference too or skip the delete and note it in your
  artifact — but never leave a destroyed ship in a fleet.
- Preserve the existing surviving-ship reload() behavior and the supply-check @todo.
- Scope limit: do NOT redefine destroyed-ship ownership or add deletion of destroyed ships
  (that is Phase 2). Removed destroyed-ship pointers stay in the player's destroyed list as today.
- Update the Doxygen header on cleanUpShips() (@date Last Modified to today, add your model to
  @author, refresh the description).

Acceptance criteria:
- A ship with HP <= 0 is absent from its owning FFleet::getShipList() after cleanup and appears
  exactly once in the owning FPlayer's destroyed record.
- Loss counters increase by exactly the count of matching destroyed ships per cleanUpShips()
  call (no inflation).
- A fleet emptied by cleanup is absent from both its FPlayer fleet list and its FSystem fleet list.
- Surviving ships remain and are reload()ed.

Validation:
- Build from repo root: `make`.
- Run the top-level suite: `cd tests && make && ./SSWTests`.

Tester guidance: behavioral strategic coverage belongs in tests/strategic/ (e.g.
FGameTest.cpp/.h): construct a real player+fleet with a zero-HP ship, call the cleanup path,
and assert fleet/destroyed-list membership, counter deltas, and empty-fleet removal.

Artifacts: write your implementer artifact under artifacts/phase1-critical-defects/CRIT-1/
(repository-root-relative).

Continue past preflight if no blocker is present.

Do not report success unless all required artifacts exist and all changes are committed.
```

### CRIT-2
```
Your role is 'implementer'. Your task is as follows:

Fix Critical defect C2: in FGame::checkForVictory() the Sathar retreat conditions 4 and 5 are
evaluated swapped relative to the displayed condition text.

Allowed files:
- src/strategic/FGame.cpp
- include/strategic/FGame.h

Context: the displayed text (src/strategic/FGame.cpp:980-993) defines condition 4 = "a tenday
passes without the Sathar destroying at least two stations" (uses m_stationsDestroyed) and
condition 5 = "the Sathar have lost more ships than the UPF" (uses m_lostTendaySathar /
m_lostTendayUPF). checkForVictory() (:1046-1064) currently implements them inverted.

Implement:
- Make case 4 evaluate the STATIONS condition (m_stationsDestroyed < 2 at tenday end) and carry
  its m_stationsDestroyed = 0 reset.
- Make case 5 evaluate the SHIP-LOSS condition (m_lostTendaySathar > m_lostTendayUPF at tenday
  end) and carry its m_lostTendaySathar = 0; m_lostTendayUPF = 0; resets.
- Keep the tenday gate (m_round % 10 == 0 && m_round > 0). Do NOT change fighter/militia
  counting (S2/S3 are out of scope).
- Update the Doxygen header on checkForVictory() (@date Last Modified, @author, description).

Acceptance criteria:
- Condition 4 selected: UPF victory reported at a tenday boundary iff fewer than two stations
  were destroyed that tenday; station counter resets at the boundary.
- Condition 5 selected: UPF victory reported at a tenday boundary iff Sathar tenday ship losses
  exceed UPF; tenday loss counters reset at the boundary.
- The evaluated condition matches the condition text for the same case number.

Validation:
- Build from repo root: `make`.
- Run the top-level suite: `cd tests && make && ./SSWTests`.

Tester guidance: behavioral strategic coverage in tests/strategic/ (FGameTest.cpp/.h): set
controlled tenday/station counters, invoke the victory check at a tenday boundary for each
selected condition, and assert the exact outcome and counter resets.

Artifacts: write your implementer artifact under artifacts/phase1-critical-defects/CRIT-2/.

This subtask edits the same file as CRIT-1; ensure CRIT-1 is merged first.

Continue past preflight if no blocker is present.

Do not report success unless all required artifacts exist and all changes are committed.
```

### CRIT-3
```
Your role is 'implementer'. Your task is as follows:

Fix Critical defect C3: FVehicle::load() rebuilds m_defenses but never reassigns
m_currentDefense, leaving it dangling at the freed constructor-time defense (use-after-free
whenever a loaded game enters combat).

Allowed files:
- src/ships/FVehicle.cpp
- include/ships/FVehicle.h

Implement:
- In FVehicle::load() (src/ships/FVehicle.cpp:217-229), after m_defenses is rebuilt, set
  m_currentDefense to m_defenses[0] when the vector is non-empty; otherwise set a safe fallback
  (e.g., create a default FNone and point at it) so getCurrentDefense() is never dangling or null.
- Scope limit: do NOT add persistence of which defense was previously active (that is roadmap
  H2). Resetting to the base defense at index 0 is sufficient here.
- Update the Doxygen header on FVehicle::load() in include/ships/FVehicle.h.

Acceptance criteria:
- After save() then load(), getCurrentDefense() returns a valid live defense (getType() is safe),
  not a freed pointer.
- A loaded vehicle can enter the take-damage / fire path without a use-after-free.

Validation:
- Build from repo root: `make`.
- Run the top-level suite: `cd tests && make && ./SSWTests`.

Tester guidance: behavioral serialization coverage in tests/ships/ (FVehicleTest.cpp/.h): save a
ship, load it, then call getCurrentDefense()->getType() and exercise a damage/fire path.

Artifacts: write your implementer artifact under artifacts/phase1-critical-defects/CRIT-3/.

Continue past preflight if no blocker is present.

Do not report success unless all required artifacts exist and all changes are committed.
```

### CRIT-4
```
Your role is 'implementer'. Your task is as follows:

Fix Critical defect C4: TwoPlanetsGUI returns EndModal(1)/EndModal(2), and
SelectCombatGUI::onAttack() uses that raw return as a 0-based index into a 2-element planet list,
so button 1 targets the wrong planet and button 2 indexes [2] (out of bounds / crash).

Allowed files:
- src/gui/SelectCombatGUI.cpp
- include/gui/SelectCombatGUI.h

Implement:
- In onAttack() (src/gui/SelectCombatGUI.cpp:212-273), translate the TwoPlanetsGUI ShowModal()
  return into a valid 0-based index into m_system->getPlanetList(): button 1 -> 0, button 2 -> 1,
  any other/cancel value -> 0 (safe in-range default).
- Use the translated index consistently at all three getPlanetList()[planet] sites (attack
  target at :227, station lookup at :258, destroyStation at :273).
- DO NOT edit the generated header include/gui/TwoPlanetsGUI.h (it carries a "do not edit" banner).
- Update the Doxygen header on SelectCombatGUI::onAttack().

Acceptance criteria:
- Selecting the first planet resolves against getPlanetList()[0]; the second against
  getPlanetList()[1].
- No path indexes getPlanetList() out of range for a 2-planet system, including on cancel/close.

Validation:
- Build from repo root: `make`.
- Build and run the GUI suite: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester guidance: GUI-facing coverage lives in tests/gui/ (StrategicGuiLiveTest). If the wx modal
path is hard to drive directly, keep the index-translation observable/testable and assert the
selected index maps to the correct planet without out-of-range access.

Artifacts: write your implementer artifact under artifacts/phase1-critical-defects/CRIT-4/.

Continue past preflight if no blocker is present.

Do not report success unless all required artifacts exist and all changes are committed.
```

### CRIT-5
```
Your role is 'implementer'. Your task is as follows:

Fix Critical defect C5: FMainFrame::onClose() shows a wxYES_NO|wxCANCEL dialog but tests
result == wxID_OK before saving, so answering "Yes, save" (which returns wxID_YES) silently
discards the game.

Allowed files:
- src/FMainFrame.cpp
- include/FMainFrame.h

Implement:
- In onClose() (src/FMainFrame.cpp:298-309), change the save-branch test from
  result == wxID_OK to result == wxID_YES, so Yes saves before resetGame().
- Keep the existing wxID_CANCEL early return and the subsequent resetGame() call.
- Scope limit: do NOT expand into the save/open dialog path/cancel hardening (roadmap H15).
- Update the Doxygen header on FMainFrame::onClose().

Acceptance criteria:
- Yes invokes the save path before reset; No resets without saving; Cancel aborts the close
  (no save, no reset).

Validation:
- Build from repo root: `make`.
- Build and run the GUI suite: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

Tester guidance: GUI-facing coverage in tests/gui/ (StrategicGuiLiveTest). Drive the close
decision paths (Yes/No/Cancel) and assert the save path is taken only on Yes and reset occurs
except on Cancel.

Artifacts: write your implementer artifact under artifacts/phase1-critical-defects/CRIT-5/.

Continue past preflight if no blocker is present.

Do not report success unless all required artifacts exist and all changes are committed.
```

### CRIT-6
```
Your role is 'implementer'. Your task is as follows:

Fix Critical defect C6: in the FGameConfig constructor, readlink("/proc/self/exe", ...) stores
its return in a size_t; on failure (-1 becomes huge) or a path >= the buffer size, buf[size] = 0
is an out-of-bounds write, in a singleton constructor with no error path.

Allowed files:
- src/core/FGameConfig.cpp
- include/core/FGameConfig.h

Implement:
- In the LINUX branch of FGameConfig::FGameConfig() (src/core/FGameConfig.cpp:105-117), capture
  readlink's return in a signed ssize_t, pass a length that reserves room for the terminator
  (sizeof(buf) - 1), and write the null terminator only at a clamped, in-bounds index.
- On failure (return <= 0), fall back to a safe/defined path state instead of writing
  buf[(size_t)-1].
- No out-of-bounds write may occur for any return value or path length.
- Update the Doxygen header on the FGameConfig constructor.

Acceptance criteria:
- Normal executable path: m_basePath / m_executablePath resolve as before.
- readlink failure (-1) causes no OOB write and leaves config in a safe, defined state.
- A path at/above buffer capacity writes no byte past buf.

Validation:
- Build from repo root: `make`.
- Run the top-level suite: `cd tests && make && ./SSWTests`.

Tester guidance: behavioral core coverage in tests/core/ (FGameConfigTest.cpp/.h): assert normal
base-path resolution is unchanged and that the terminator index stays in-bounds for boundary and
failure returns (exercise via the seam available without spawning a real process where possible).

Artifacts: write your implementer artifact under artifacts/phase1-critical-defects/CRIT-6/.

Continue past preflight if no blocker is present.

Do not report success unless all required artifacts exist and all changes are committed.
```

---

## Output Artifact Path

This plan artifact is written (repository-root-relative) to:

- `plans/phase1-critical-defects-plan.md`

Per-subtask implementer/tester/documenter/verifier artifacts are written under
`artifacts/phase1-critical-defects/CRIT-<n>/` (one directory per subtask, `n` = 1…6).

---

## 7. Handoff Summary for the Coordinator

- 6 implementation subtasks, all Critical bug fixes; none require the specialist Security stage.
- Ordering: `CRIT-1` → `CRIT-2` (serial, shared `FGame.cpp`); `CRIT-3`, `CRIT-4`, `CRIT-5`, `CRIT-6` parallel.
- Each subtask flows through the default Implementer → Tester → Documenter → Verifier chain; the Reviewer pass runs after all six merge back.
- Use a dedicated per-plan coordination branch; commits are authorized within the coordinator workflow.
