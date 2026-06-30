# Tester Report — SMRIV-04

**Status:** PASS

## Task Summary

SMRIV-04: Phase-change height reset in `FBattleDisplay` lower panel.
`ensureLowerPanelLayoutState()` now stores `lastBattleState` and `lastBattlePhase`
(initialized to -1). When the state/phase changes (`phaseChanged == true`), the
max-preserve of `requestedDisplayHeight` is skipped so the panel can shrink back to
base content height for the new phase rather than ratcheting up permanently.

## Branch

`sf2-SMRIV-04-tester-20260629`

## Test Commit Hash

`27c71a8`

## Test Files Modified

- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/TacticalGuiLiveTest.h`

## Commands Run

```
make -C tests/gui
DISPLAY=:0 tests/gui/GuiTests
```

## Final Test Outcomes

GuiTests: 53 run, 9 failures (all pre-existing), 0 errors.

New test `testLowerPanelHeightShrinksBackAfterPhaseChange` PASSED.

The 9 pre-existing failures are source-contract and geometry tests unrelated to this
change. They were present on the branch before the SMRIV-04 tester work began.

## Acceptance Criteria Validated

- **AC1**: `requestedDisplayHeight` decreases after transitioning from BS_PlaceMines
  (expanded by 6-Minelayer source list, height = 126) to BS_Battle/PH_MOVE (height
  resets to 120 floor). Verified by behavioral assertion `movePhaseHeight < expandedMineHeight`.
- **AC3**: `requestedDisplayHeight` remains >= 120 after the transition (120-px floor
  preserved). Verified by `movePhaseHeight >= 120`.
- **AC4**: Test fails against pre-SMRIV-04 ratchet-only code (max-preserve always runs,
  so `movePhaseHeight` stays at `expandedMineHeight`; `movePhaseHeight < expandedMineHeight`
  is false) and passes with the SMRIV-04 fix.

## Unmet Acceptance Criteria

None.

## Key Implementation Notes

`GetParent()->SendSizeEvent()` fires synchronously (uses `HandleWindowEvent`, not
`wxPostEvent`). This causes `reflowLowerPanelLayout()` to run inside the draw call.
Two offscreen draws in BS_PlaceMines are required: the first primes `lastBattleState`
(on that draw, `phaseChanged == true` because `lastBattleState == -1`; the height is
reset to the floor but `lastBattleState` is updated). The second draw runs with
`phaseChanged == false`, so same-phase max-preserve keeps the expanded height (126 > 120).

## Artifacts Written

- `artifacts/seeker-missile-fixes-round4/SMRIV-04/tester_report.md`
- `artifacts/seeker-missile-fixes-round4/SMRIV-04/tester_result.json`
- `artifacts/seeker-missile-fixes-round4/SMRIV-04/documenter_prompt.txt`
