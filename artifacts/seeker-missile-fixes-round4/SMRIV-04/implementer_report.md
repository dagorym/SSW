# Implementer Report — SMRIV-04

## Status

success

## Task Summary

Change the lower-panel height lifecycle in FBattleDisplay so it no longer ratchets up
permanently. The panel keeps expanded height for the duration of the current tactical
phase but resets when the tactical phase changes, allowing it to shrink back when the
next phase needs less height.

## Changed Files

- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

## Implementation/Code Commit Hash

`c1d5627`

## What Was Changed

Two `int` fields, `lastBattleState` and `lastBattlePhase` (both initialised to -1),
were added to the `LowerPanelLayoutState` struct in `FBattleDisplay.h`.

In `ensureLowerPanelLayoutState()` in `FBattleDisplay.cpp`, the function now compares
`m_parent->getState()` and `m_parent->getPhase()` (cast to int) against the stored
last-seen values.  If either differs (`phaseChanged == true`), it updates the stored
values and **skips** the old max-preserve block:

```cpp
if (m_lowerPanelLayoutState.requestedDisplayHeight > requestedHeight)
    requestedHeight = m_lowerPanelLayoutState.requestedDisplayHeight;
```

so `requestedDisplayHeight` is recomputed from the current content instead of being
kept at a higher value carried over from a previous phase.

When `phaseChanged == false` (same phase), max-preserve is applied as before,
preventing the row-clipping regression that the original guard was introduced to fix.

The 120 px minimum-height floor is enforced in all code paths.  No other logic was
modified.

## Validation Commands Run

```
make -C tests tactical-tests
./tests/tactical/TacticalTests
make -C tests gui-tests
```

## Validation Outcome

PASS — 223 tactical tests pass; GUI test module builds clean with no errors or
warnings in the changed files.

## Expected Validation Failures Carried Forward

None.

## Artifacts Written

- `artifacts/seeker-missile-fixes-round4/SMRIV-04/implementer_report.md`
- `artifacts/seeker-missile-fixes-round4/SMRIV-04/implementer_result.json`
- `artifacts/seeker-missile-fixes-round4/SMRIV-04/tester_prompt.txt`
