# Implementer Report — tactical-gui-regressions subtask-1

## Plan Step Status
1. Preflight scope check — completed.
2. Implement incremental relayout fix — completed.
3. Add regression coverage in allowed tactical/gui test files — completed.
4. Run required validations — completed (blocked by missing CppUnit headers in environment).
5. Commit code changes, then artifacts — completed.

## Files Changed
- src/tactical/FBattleDisplay.cpp
- tests/tactical/FTacticalBattleDisplayFireFlowTest.h
- tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp
- tests/gui/TacticalGuiLiveTest.h
- tests/gui/TacticalGuiLiveTest.cpp

## Implementation Summary
- Added `Layout()` immediately after showing tactical completion buttons in:
  - `drawMoveShip(...)`
  - `drawDefensiveFire(...)`
  - `drawAttackFire(...)`
  - `drawPlaceMines(...)`
- Added `Layout()` immediately after hiding completion buttons in:
  - `onMoveDone(...)`
  - `onDefensiveFireDone(...)`
  - `onOffensiveFireDone(...)`
  - `onMinePlacementDone(...)`
- Added tactical source-regression tests that assert show/hide paths include post-visibility `Layout()` calls.
- Added live GUI regression test to verify movement/defensive/offensive/mine completion buttons appear visible with nonzero size and positioned away from origin/zoom area after phase activation.

## Validation Commands Run
1. `cd tests/tactical && make && ./TacticalTests`
2. `cd tests/gui && make && xvfb-run -a ./GuiTests`

## Validation Outcomes
- Both commands failed in this environment before and after changes due missing CppUnit headers:
  - `fatal error: cppunit/extensions/HelperMacros.h: No such file or directory`
- This is an environment dependency issue (unable to install via `sudo` due password prompt in non-interactive run), not a code regression introduced by this change.

## Commits
- Implementation/code commit: `417109347f3771234424cdd51cc6812b57d3f836`
