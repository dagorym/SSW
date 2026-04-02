# Implementer Report - Milestone 8 Remediation Subtask 5

## Summary
Rewired the live wx move-done callback path so `FBattleDisplay::onMoveDone()` now delegates through the canonical screen/model seam (`FBattleScreen::completeMovePhase()`), which calls `FTacticalGame::completeMovePhase()`.

## Plan Steps Completed
1. Confirmed worktree/branch context and implementer-agent requirements.
2. Traced existing move-done runtime path in `FBattleDisplay` and `FBattleScreen`.
3. Applied minimal code change to route callback through canonical seam.
4. Ran tactical build and tactical test suite validations.

## Files Changed
- `src/tactical/FBattleDisplay.cpp`

## Validation Commands Run
- `make -C src/tactical`
- `cd tests/tactical && make && ./TacticalTests`

## Validation Outcomes
- Tactical module build: **PASS**
- Tactical tests: **PASS** (`OK (65 tests)`)

## Acceptance Criteria Mapping
- Runtime move-done callback reaches canonical seam: **Met** (`onMoveDone()` now calls `m_parent->completeMovePhase()`, which forwards to `FTacticalGame::completeMovePhase()`).
- Expected phase progression/redraw behavior preserved: **Met** (`FBattleScreen::completeMovePhase()` calls model completion then `reDraw()`).
- Fix applies to live wx UI callback path: **Met** (`FBattleDisplay::onMoveDone()` is the wired button callback path).

## Implementation Commit
- `03258db0886d1ad3ecf6565773a48329b14e5c52`
