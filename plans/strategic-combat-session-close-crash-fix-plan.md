# Strategic Combat Session Close Crash Fix Plan

## Problem

Ending a tactical combat session from the strategic game crashes with the wx assertion recorded in `BBCloseError`:

- `wxDialog::EndModal()` asserts that it was either called twice or called after the dialog had already stopped being modal.

The backtrace shows the failing path:

- `FBattleDisplay::fireAllWeapons()`
- `FBattleScreen::clearDestroyedShips()`
- `FBattleScreen::declareWinner()`
- `wxDialog::EndModal()`

## Diagnosis

The immediate crash source is in `src/tactical/FBattleScreen.cpp`:

- `FBattleScreen::declareWinner()` currently does:
  - `wxMessageBox(...)`
  - `Destroy()`
  - `EndModal(0)`

`FBattleScreen` is a `wxDialog` and is entered via `ShowModal()`. Calling `Destroy()` before `EndModal()` tears down the dialog/modal state first, so the subsequent `EndModal(0)` trips the exact assertion seen in `BBCloseError`.

There is also an ownership problem behind the assertion:

- `SelectCombatGUI`, `ScenarioDialog`, and `ScenarioEditorGUI` allocate `FBattleScreen` with `new`.
- Those callers call `ShowModal()` but do not `delete` the dialog afterward.
- That means the current `Destroy()` call in `declareWinner()` was likely acting as an implicit cleanup mechanism for heap-allocated modal dialogs.

So the fix should not be just “swap the two calls” or “remove `Destroy()`”; doing only that would likely stop the assertion but leave the modal battle screen leaked or inconsistently owned.

## Proposed Fix

### 1. Give `FBattleScreen` one authoritative close path

Refactor battle-screen shutdown so it closes exactly once and uses modal semantics correctly.

Planned shape:

- Add a dedicated helper in `FBattleScreen` for ending the battle session.
- Guard it against double-close/re-entrancy with a small state flag (for example `m_isClosing`).
- In modal use, close with `EndModal(...)` only.
- Only use `Destroy()` for non-modal fallback paths, if any remain.

This removes the invalid `Destroy(); EndModal(...);` sequence that is causing the assertion.

### 2. Move lifetime ownership back to the callers

Update every modal caller that creates `FBattleScreen` so ownership is explicit and compatible with `EndModal()`:

- `src/gui/SelectCombatGUI.cpp`
- `src/battleSim/ScenarioDialog.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`

Preferred direction:

- create `FBattleScreen` as a stack dialog where feasible, then call `ShowModal()`, or
- otherwise keep heap allocation but delete the dialog after `ShowModal()` returns.

This preserves cleanup after removing the current self-destruction behavior from `declareWinner()`.

### 3. Add regression coverage for the dialog-close contract

Extend tactical source-inspection tests to lock in the corrected lifecycle behavior.

Coverage should assert that:

- `declareWinner()` does not call `Destroy()` before `EndModal()`
- battle-screen close logic uses a single close path
- modal caller sites do not rely on battle-screen self-destruction anymore

Most natural starting point:

- extend `tests/tactical/FTacticalBattleScreenElectricalFireTest.*`

If the assertions fit better in a dedicated test file, add a new tactical test and wire it into:

- `tests/tactical/Makefile`
- `tests/tactical/TacticalTests.cpp`
- `tests/SSWTests.cpp`

### 4. Validate with tactical and full-suite runs

After the fix:

- build tactical tests
- run tactical tests
- run the full `SSWTests` suite

If there is a deterministic way to exercise the strategic combat close path manually, include that as an additional smoke check.

## Expected Files To Change

- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `src/gui/SelectCombatGUI.cpp`
- `src/battleSim/ScenarioDialog.cpp`
- `src/battleSim/ScenarioEditorGUI.cpp`
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.h`
- `tests/tactical/FTacticalBattleScreenElectricalFireTest.cpp`
- possibly `tests/tactical/Makefile`
- possibly `tests/tactical/TacticalTests.cpp`
- possibly `tests/SSWTests.cpp`

## Key Decision

Treat this as a combined modal-lifecycle and ownership bug, not just a one-line wx assertion fix. The safest correction is:

- `FBattleScreen` closes modally with `EndModal()` exactly once
- callers own destruction/cleanup after `ShowModal()` returns
- regression tests lock in both behaviors
