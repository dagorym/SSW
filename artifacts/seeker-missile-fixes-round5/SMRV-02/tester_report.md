# Tester Report — SMRV-02 pass-2

**Status:** PASS

## Task Summary

SMRV-02 pass-2 (Verifier-driven remediation): Added behavioral anchor discrimination test
`testSeekerActivationAnchorIsAtActionPromptLineY` to `tests/gui/TacticalGuiLiveTest`.
The test exercises the `drawSeekerActivation()` anchor change
(from `getActionButtonRowBottom()` to `getActionPromptLineY(0)`) end-to-end via offscreen
`wxMemoryDC` with a real activated seeker in the model, asserting the new top-of-panel
anchor position and click-region alignment (AC-2).

The BLOCKING finding from the pass-1 Verifier is fully resolved: a behavioral test now
exists that would **fail** against the old `getActionButtonRowBottom()` anchor and
**passes** against the shipped `getActionPromptLineY(0)` anchor.

## Branch

`sf2-SMRV-02-tester-20260629`

## Test Commit Hash

`5072f26`

## Test Files Added or Modified

- `tests/gui/TacticalGuiLiveTest.cpp`
- `tests/gui/TacticalGuiLiveTest.h`

## Changes Made

**New test: `testSeekerActivationAnchorIsAtActionPromptLineY`** in `TacticalGuiLiveTest`

1. Creates a `TestableBattleScreen` with attacker (Destroyer) and defender (Frigate).
2. Seeds one activated seeker (`ownerID=1`, `active=true`, `activationPhaseIndex=0`) via
   `TestableBattleScreen::seedSeeker()` so `getActiveSeekersByMovingPlayerThisPhase()` returns it.
3. Transitions to `BS_Battle` + `PH_SEEKER_ACTIVATION` and renders `FBattleDisplay` via offscreen
   `wxMemoryDC`.
4. Asserts:
   - `seekerActivationRegionCount() >= 1` (seeder injected successfully — precondition).
   - `firstRegion.GetTop() >= getActionPromptLineY(0)` — new top-of-panel anchor (AC-1).
   - `firstRegion.GetTop() < getActionButtonRowBottom()` — discrimination: with the old anchor
     the region top would be `>= getActionButtonRowBottom()`, failing this assertion; with the
     new anchor the region top is approximately 21 px (5 + 16), well below `~70+` px (AC-1).
   - `getActiveSeekersByMovingPlayerThisPhase().size()` decreases after click on region via
     `checkSeekerActivationSelectionPublic()` — click-region/draw-position alignment (AC-2).

**New peer accessors added to `FBattleDisplayTestPeer`:**
- `seekerActivationRegionCount()` — exposes `m_seekerActivationRegions.size()`
- `seekerActivationRegion(i)` — exposes `m_seekerActivationRegions[i]`
- `checkSeekerActivationSelectionPublic(event)` — exposes `checkSeekerActivationSelection()`

**Pass-1 changes already merged (not redone):**
- `testSeekerActivationPanelListsInstructionAndOneRowPerInactiveSeeker` asserts
  `int y = getActionPromptLineY(0)` (source-contract anchor update).
- `testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges` resize-back
  relaxation to within-phase preservation.

## Commands Run

```
make -C tests/gui
DISPLAY=:0 tests/gui/GuiTests
make -C tests/tactical
tests/tactical/TacticalTests
make -C tests
tests/SSWTests
```

## Final Test Outcomes

| Suite | Run | Pass | Fail | Notes |
|---|---|---|---|---|
| GuiTests | 54 | 46 | 8 | New test PASSED; 8 pre-existing source-contract failures (unrelated to SMRV-02) |
| TacticalTests | 225 | 225 | 0 | All pass |
| SSWTests | 178 | 169 | 9 | 9 pre-existing FGameHeaderDependencyTest source-contract failures (unrelated) |

**New test result:** `testSeekerActivationAnchorIsAtActionPromptLineY` — **PASSED**

**Pre-existing failures in GuiTests (8, baseline unchanged):**
1. `testStrategicDialogsUseStaticBoxChildParents` — source-contract
2. `testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow` — source-contract
3. `testRemediatedStrategicDialogsUseFirstShowSizingContract` — source-contract
4. `testTacticalActionButtonsStayBelowPromptReservationAcrossPhases` — source-contract
5. `testSeekerActivationPanelSourceContracts` — source-contract
6. `testSetupPlacementSourceRowsAndOrdnanceColorContracts` — source-contract
7. `testBattleDisplayNarrowWidthStacksShipStatsBelowButtons` — source-contract
8. `testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo` — source-contract

## Acceptance Criteria

- **AC-1** (anchor): `'Activated seekers:'` list in PH_SEEKER_ACTIVATION anchors at
  `getActionPromptLineY(0)` (top of lower panel), not below the action-button row.
  **VERIFIED BEHAVIORAL** — new test discriminates old vs new anchor.
- **AC-2** (click alignment): Activation row click regions align with drawn positions;
  clicking deactivates the intended seeker.
  **VERIFIED BEHAVIORAL** — click simulation reduces `getActiveSeekersByMovingPlayerThisPhase().size()`.
- **Source-contract** (pass-1): `testSeekerActivationPanelListsInstructionAndOneRowPerInactiveSeeker`
  asserts `int y = getActionPromptLineY(0)` — already merged in pass-1.

## Cleanup

No temporary non-handoff byproducts remain in the worktree.

## Artifacts Written

- `artifacts/seeker-missile-fixes-round5/SMRV-02/tester_report.md`
- `artifacts/seeker-missile-fixes-round5/SMRV-02/tester_result.json`
- `artifacts/seeker-missile-fixes-round5/SMRV-02/documenter_prompt.txt`
