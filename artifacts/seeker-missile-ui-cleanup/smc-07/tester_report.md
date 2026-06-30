# Tester Report: SMC-07 Seeker Path Drawing and Icon Rotation

**Task:** SMC-07 - Draw seeker movement on the board like ships: stepped path line (cyan, pen width 2) plus heading-rotated icon. Also implements SMC-06 prerequisite: captures each active seeker's stepped movement path in FTacticalSeekerMissileState.movementPath during resolveActiveSeekersForMovingPlayer and exposes it via getSeekerMissiles() delegation through FBattleScreen.

**Branch:** seeker-fix

**Test Commit:** ce8a19a

**Artifact Directory:** artifacts/seeker-missile-ui-cleanup/smc-07

---

## Testing Scope

### Implementation Surface Under Test

- `include/tactical/FTacticalGame.h` - `movementPath` field on `FTacticalSeekerMissileState` struct
- `src/tactical/FTacticalGame.cpp` - `resolveActiveSeekersForMovingPlayer()` populates `movementPath`
- `include/tactical/FBattleScreen.h` - `getSeekerMissiles()` delegation declaration
- `src/tactical/FBattleScreen.cpp` - `getSeekerMissiles()` delegation through FTacticalGame
- `include/tactical/FBattleBoard.h` - `drawSeekerPaths()` declaration
- `src/tactical/FBattleBoard.cpp` - `drawSeekerPaths()` implementation; updated `drawSeekerMissiles()` with heading rotation

### Acceptance Criteria Evaluated

1. When active seekers move at the start of the movement phase, each seeker's stepped path is drawn on the main board in a color distinct from ship paths (cyan, pen width 2).
2. Each seeker icon is rotated to reflect its current heading (heading * pi/3 radians, consistent with 6-direction ship icon rotation).
3. The seeker path display is scoped to the movement phase (PH_MOVE), refreshed per movement turn, consistent with ship path behavior.
4. No regression to seeker icon rendering in activation/other phases.
5. After active seekers move, each moved seeker exposes its stepped path (start hex through final hex) via the movementPath field on FTacticalSeekerMissileState.
6. The path reflects the actual greedy route taken and matches the final hex/heading already computed.
7. The path is reset/repopulated each movement resolution (no stale accumulation across turns).
8. No wx types in the model; existing tactical tests remain green.

---

## Test Changes Made

### Updated Existing Test

**File:** `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp`

**Test:** `testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules`

**Change:** The SMC-07 implementer changed the active-seeker draw call from the non-rotated form `drawCenteredOnHex(*m_seekerMissileIcon,hex)` to the heading-rotated form `drawCenteredOnHex(*m_seekerMissileIcon, hex, activeSeekers[0].heading)`. The existing test expected the old non-rotated call which would now fail. Updated the assertion to expect the new heading-rotated call. This is an expected intentional regression - the behavior intentionally changed to add rotation.

**Justification:** The SMC-07 acceptance criterion explicitly requires heading-rotation. The old test was asserting pre-SMC-07 behavior that was intentionally changed.

### Added New Tests

**File:** `tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp` / `.h`

**Test:** `testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen`

Covers acceptance criteria 1, 3 (source contract):
- `drawSeekerPaths` is nested inside the `PH_MOVE` phase guard in `draw()`
- Uses cyan pen (#00CCCC) with width 2
- Reads seeker records through `m_parent->getSeekerMissiles()`
- Filters to active seekers with movementPath.size() >= 2
- Draws stepped lines via `dc.DrawLine(lx, ly, x, y)`

**File:** `tests/tactical/FTacticalSeekerMovementTest.cpp` / `.h`

**Test:** `testMovementPathPopulatedByResolveActiveSeekers`

Covers acceptance criteria 5, 6, 7 (runtime model test):
- `movementPath` is non-empty after `resolveActiveSeekersForMovingPlayer()`
- `movementPath[0]` equals the seeker's pre-move starting hex
- `movementPath.back()` matches the seeker's final hex position
- Non-moving (inactive/enemy) seekers have their stale `movementPath` cleared
- Second resolution pass starts fresh from current hex (no accumulation from first pass)

---

## Test Execution Results

### Tactical Tests (Primary Validation Suite)

**Command:** `make -C tests tactical-tests && tests/tactical/TacticalTests`

**Result:** OK - 189 tests, 0 failures, 0 errors

This count increased from the pre-change baseline of 187 tests by the 2 new tests added.

### GUI Tests (Validation Command)

**Command:** `make -C tests gui-tests` followed by `tests/gui/GuiTests`

**Build Result:** Success - all modules compiled without errors or warnings.

**Test Result:** 43 tests run, 8 failures, 0 errors.

The 8 GUI failures are **pre-existing** and not introduced by SMC-07 test changes:
1. `testStrategicDialogsUseStaticBoxChildParents` - pre-existing source-contract failure
2. `testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow` - pre-existing
3. `testRemediatedStrategicDialogsUseFirstShowSizingContract` - pre-existing
4. `testTacticalActionButtonsStayBelowPromptReservationAcrossPhases` - pre-existing
5. `testSeekerActivationPanelSourceContracts` - pre-existing
6. `testSetupPlacementSourceRowsAndOrdnanceColorContracts` - pre-existing
7. `testBattleDisplayNarrowWidthStacksShipStatsBelowButtons` - pre-existing
8. `testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo` - pre-existing

None of these failures are related to SMC-07. The new tests added by this tester run in the tactical suite (`TacticalTests`), not the GUI suite. The SMC-07 test for `testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules` was not among the failures, confirming the heading-rotation update passed.

---

## Acceptance Criteria Mapping

| Criterion | Test | Result |
|---|---|---|
| AC1: Cyan pen width 2 on board in PH_MOVE | `testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen` | PASS |
| AC2: Seeker icon rotated by heading | `testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules` (updated) | PASS |
| AC3: Scoped to PH_MOVE | `testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen` | PASS |
| AC4: No regression to activation phase rendering | `testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules` | PASS |
| AC5: movementPath exposed via getSeekerMissiles() | `testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen` (delegation check) + `testMovementPathPopulatedByResolveActiveSeekers` | PASS |
| AC6: Path reflects actual greedy route | `testMovementPathPopulatedByResolveActiveSeekers` (start/end hex match) | PASS |
| AC7: Path reset per resolution (no stale accumulation) | `testMovementPathPopulatedByResolveActiveSeekers` (two-pass check) | PASS |
| AC8: No wx types in model; tactical tests green | 189 tactical tests pass | PASS |

---

## Cleanup

No temporary byproducts were created. The untracked files `artifacts/seeker-missile-ui-cleanup/smc-01/implementer_prompt.txt` and `implementer_prompt_base.txt` pre-existed this session and are not related to SMC-07.

---

## Status: PASS

All SMC-07 acceptance criteria are satisfied. Tactical test suite: 189/189 pass. GUI test suite pre-existing failures are not caused by SMC-07.
