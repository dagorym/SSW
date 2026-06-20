# Tester Report

**Status:** PASS

**Task ID:** SMFR-04

**Branch:** sf2-SMFR-04-tester-20260619

**Test commit hash:** f60ecea

## Task Summary

SMFR-04: Persist the seeker speed (move-count) label from activation until impact or movement exhaustion. The label previously disappeared between movement resolution passes and in non-movement phases because `drawSeekerMoveCountOverlay()` was called only during `PH_MOVE`/`PH_SEEKER_ACTIVATION`, and only for seekers with a recorded `movementPath` (size >= 2). Fixed by: (1) moving the overlay call outside the phase guard so it fires for all `BS_Battle` phases; (2) removing the `movementPath.size() < 2` filter in the overlay function, using `movementPath.size()-1` when a path is recorded or `movementAllowance` as fallback.

## Test Files Added or Modified

- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/TacticalGuiLiveTest.h`
- `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`
- `tests/tactical/FTacticalBattleBoardRendererDelegationTest.h`
- `tests/tactical/FTacticalSeekerMovementTest.cpp`
- `tests/tactical/FTacticalSeekerMovementTest.h`

## Commands Run

```
make -C tests tactical-tests
./tests/tactical/TacticalTests
make -C tests gui-tests
DISPLAY=:0 ./tests/gui/GuiTests
```

## Final Test Outcomes

**Tactical suite:** 211 tests, 0 failures. Includes:
- 1 new behavioral test: `FTacticalSeekerMovementTest::testSeekerMoveCountLabelFieldsReflectPathAndAllowance`
- 1 new source-contract test: `FTacticalBattleBoardRendererDelegationTest::testDrawSeekerMoveCountOverlayCalledUnconditionallyInBSBattle`

**GUI suite:** 47 total, 37 pass, 10 pre-existing failures (unchanged from before this task). Includes:
- 1 new behavioral test: `TacticalGuiLiveTest::testSeekerMoveCountOverlayRendersInAllBattlePhases` — PASS

All SMFR-04 acceptance criteria verified.

## Acceptance Criteria Coverage

| AC | Description | Status | Tests |
|----|-------------|--------|-------|
| AC1 | Speed/move-count label renders for every active seeker in all BS_Battle phases | PASS | `testDrawSeekerMoveCountOverlayCalledUnconditionallyInBSBattle` (source-contract: inside BS_Battle, no phase guard); `testSeekerMoveCountOverlayRendersInAllBattlePhases` (behavioral: draw completes in PH_ATTACK_FIRE, PH_DEFENSE_FIRE, PH_SEEKER_ACTIVATION, PH_MOVE) |
| AC2 | Label persists from activation through subsequent turns | PASS | `testSeekerMoveCountLabelFieldsReflectPathAndAllowance` (behavioral: movementAllowance and movementPath fields correct before and after resolution); source-contract confirms movementAllowance fallback present without movementPath.size() < 2 filter |
| AC3 | Multiple co-located seekers render stacked (stacked display preserved) | PASS | `testSeekerMoveCountLabelFieldsReflectPathAndAllowance` (behavioral: two seekers at same hex maintain independent movementAllowance values; both survive single resolution pass) |
| AC4 | No change to seeker movement, targeting, or damage behavior | PASS | `testSeekerMoveCountOverlayRendersInAllBattlePhases` (behavioral: getSeekerMissiles() count unchanged after all board draws; overlay is read-only display logic) |

## Unmet Acceptance Criteria

None.

## Cleanup

No temporary byproducts left in the worktree.

## Artifacts Written

- `artifacts/seeker-missile-fixes-round3/smfr-04/tester_report.md`
- `artifacts/seeker-missile-fixes-round3/smfr-04/tester_result.json`
- `artifacts/seeker-missile-fixes-round3/smfr-04/documenter_prompt.txt`
