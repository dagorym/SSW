# Final Review Report — tactical-battery-range-clamp-bug

## Overall Verdict: CHANGES REQUESTED

## Review Summary

### 1. Correctness
The production fix in `src/tactical/FBattleBoard.cpp:1008-1015` is correct: `xMax` and `yMax` are now clamped to `m_nCol - 1` and `m_nRow - 1`, preventing the inclusive loops from visiting off-map coordinates on both axes.

### 2. Completeness
The code change satisfies the clamp fix itself, but the feature is not fully complete against the plan because the regression tests do not exercise center, edge, and corner positions or validate the returned point set behavior as the plan explicitly requires.

### 3. Test Coverage
The 5 new tests guard against textual reintroduction of the old clamp bug (including the remediated `if (yMax < m_nRow)` negative assertion), but are source-inspection only and do not verify runtime behavior of `computeBatteryRange` or the actual contents of `tList`.

### 4. Missed Functionality
Plan acceptance criteria requiring:
- Tests that *fail before the fix and pass after* for boundary scenarios
- Tests that verify the returned point set contains only valid board coordinates
- Explicit center, edge, and corner scenario coverage
...were not met by the delivered test suite.

### 5. Edge Cases
The implementation appears sound for right/bottom edge and corner clamping. However, tests do not behaviorally cover exact edge/corner inputs, do not confirm center-board results are unchanged as an actual returned set, and do not prove no off-map coordinates are ever emitted at runtime.

## Blocking Issues

**Plan acceptance gap in regression testing:** `FTacticalBatteryRangeClampTest.cpp` verifies source structure rather than behavior, so it does **not** satisfy the plan requirement to exercise center/edge/corner positions and verify the returned point set contains only in-bounds coordinates with fail-before-fix / pass-after-fix boundary behavior.

*Root cause:* `FBattleBoard` inherits from `wxScrolledWindow` and cannot be instantiated without a running `wxApp`. The test runner has no `wxApp`, making direct behavioral testing impossible without architectural changes.

## Follow-up Feature Requests for Planner

1. **Extract clamp logic into a GUI-independent helper** — Refactor the scan-window clamping (or the full `computeBatteryRange` computation) into a free function or non-GUI class so it can be unit tested without a `wxApp`. Then add behavioral regression tests for center, edge, and corner positions that assert every returned hex satisfies `0 <= x < m_nCol` and `0 <= y < m_nRow`.

2. **Fail-before-fix regression baseline** — Once a testable seam exists, add tests that would have caught this bug: assert that a corner-position battery range contains no off-map hexes, and verify the pre-fix logic would fail this assertion.
