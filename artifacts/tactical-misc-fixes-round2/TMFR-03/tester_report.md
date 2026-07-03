# TMFR-03 Tester Report

## Scope

Validate the implementer's fix to the Turn Left/Turn Right panel layout in
`FBattleDisplay::drawMoveShip()` (`src/tactical/FBattleDisplay.cpp`,
`include/tactical/FBattleDisplay.h`): the panel's placement is now computed from
wrapped instruction line widths (via the new `measureWrappedActionPromptWidth()`
helper) instead of the unwrapped prompt extent, the long move-phase instruction
line is split across `detailPromptTwo`/`detailPromptThree`, and a 4-line caption
is drawn above the Turn Left/Turn Right buttons with the lower panel growing to
avoid clipping.

## Test-directory / command assumptions

- Test locations: `tests/gui/TacticalGuiLiveTest.{cpp,h}` (new behavioral coverage,
  per explicit Tester handoff note) and `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp/.h`
  (update stale source-contract tests locked to pre-fix signatures/text) — both
  explicitly named in the task.
- Test commands (assumption, matches repository conventions in AGENTS.md/README):
  - `make` (top-level build)
  - `cd tests && make` (build all suites) then `cd tests/tactical && ./TacticalTests`
  - `cd tests/gui && ./GuiTests` (DISPLAY=:0 was available; xvfb-run not required)
  - `cd tests && ./SSWTests` (full top-level regression)

## Acceptance criteria validated

1. **Instruction text wrapped; panel X from wrapped line widths, not unwrapped extent.**
   Confirmed via source-contract update in
   `testMovePromptConstrainedWidthSelectionPathUsesDeterministicHelpers`
   (asserts `drawMoveShip()` calls `measureWrappedActionPromptWidth()` for all four
   prompt strings) plus the existing/updated `testLowerPanelReflowPathRecomputesMovePromptReservationFromCurrentGeometry`.
   Behaviorally exercised as a precondition of AC2 below (the panel is only shown
   when the fit-guard math based on wrapped widths succeeds).

2. **At default window size the panel is visible, right of Movement Done, top of
   lower panel, left of ship-info column; lower panel expands as needed.**
   New behavioral tests (drive the real `FBattleScreen`/`FBattleDisplay` at
   panelWidth=1200, matching `FBattleScreen`'s default `wxSize(1200,900)`):
   - `testTurnPanelShownAtDefaultWindowSize` — asserts `isTurnButtonPanelShown()`.
   - `testTurnPanelPlacedRightOfMovementDoneAndLeftOfShipInfoColumn` — asserts
     `panelRect.GetX() >= moveDoneRect.GetRight()`,
     `panelRect.GetRight() <= shipStatsLeftMargin`, and
     `panelRect.GetY() >= actionPromptLineY(0)`.
   - `testTurnPanelHeightExpandsToContainCaptionAndButtonsWithoutClipping` — asserts
     `requestedDisplayHeightPublic() >= panelRect.GetBottom()` (no clipping) and
     no overlap with the ship-info column.

3. **2-4 line caption drawn above the buttons, roughly the width of the two
   buttons side by side.**
   `testTurnPanelCaptionRendersAboveButtons` — behavioral: renders to an offscreen
   `wxBitmap`, converts to `wxImage`, and scans the region between the first
   action-prompt line and the button panel's top edge for white text pixels
   (the caption is drawn in white on the black-cleared background). Supplemented
   with a source-contract check locking the four caption lines' exact wording
   (static `DrawText` calls have no other externally observable text surface;
   per AGENTS.md this supplements, not replaces, the pixel-level behavioral
   assertion).

4. **Panel height grows to contain caption + buttons without clipping; no
   overlap with instruction or ship-info columns.**
   `testTurnPanelHeightExpandsToContainCaptionAndButtonsWithoutClipping` (see AC2).

5. **No regression to seeker/mine/other move-phase draw paths.**
   Full `TacticalTests` (254 tests) and `GuiTests` (71 tests) suites pass,
   including all pre-existing seeker/mine/fire-phase coverage
   (`FTacticalSeekerMovementTest`, `FTacticalMineDamageFlowTest`,
   `testOrdnancePlacementAndActivationPanelHeightAutoExpands`,
   `testSeekerMoveCountOverlayRendersInAllBattlePhases`, etc.) and the pre-existing
   TMF-05 turn-button tests (`testTurnButtonPanelHiddenInNonMovePhase`,
   `testTurnButtonPanelShownAndEnableStateReflectsModelInMovePhase`,
   `testTurnButtonClickAppliesEndOfMoveTurnToModel`).

6. **Caption wording exact.**
   Locked via source-contract assertions in `testTurnPanelCaptionRendersAboveButtons`
   against all four literal caption lines in `src/tactical/FBattleDisplay.cpp`.

## Existing tests updated (justification)

Per the task's `expected_validation_failures`, three tests in
`FTacticalBattleDisplayFireFlowTest.cpp` and one assertion in
`TacticalGuiLiveTest.cpp` were locked to pre-fix source shape (the old 3-arg
`buildMovePromptText()` signature and the old single-line "Press the 'Movement
Done' button..." instruction). These are source-contract tests over exactly the
behavior the approved task changed, not runtime regressions:

- `testMovePromptDifferentiatesStoppedFacingNormalAndNoShipCases` — updated to
  extract `buildMovePromptText()`'s body (where the stopped-facing/normal/no-ship
  branching logic now lives, since `drawMoveShip()` calls it directly instead of
  duplicating it) and to assert the new `detailPromptTwo`/`detailPromptThree` split.
- `testLowerPanelReflowPathRecomputesMovePromptReservationFromCurrentGeometry` —
  updated the asserted `buildMovePromptText(...)` call to its 4-arg form.
- `testMovePromptConstrainedWidthSelectionPathUsesDeterministicHelpers` — updated
  `extractFunctionBody(...)` signature lookups to the 4-arg `buildMovePromptText()`
  form, updated `drawBody` assertions to check for the `measureWrappedActionPromptWidth()`
  calls and `detailPromptThree` variable, and updated `builderBody`/header assertions
  for the new signature.
- `TacticalGuiLiveTest::testTacticalActionButtonsStayBelowPromptReservationAcrossPhases` —
  the single source-line assertion for the old long instruction line was split into
  two assertions checking both halves of the now-two-line instruction
  (`detailPromptTwo`/`detailPromptThree`); all other assertions in this test were
  already passing and remain unchanged.

## Test execution results

- `make` (top-level, from repo root): succeeds, no warnings/errors introduced.
- `cd tests && make`: succeeds; builds `SSWTests`, `tests/tactical/TacticalTests`,
  and `tests/gui/GuiTests`.
- `cd tests/tactical && ./TacticalTests`: **254 tests, OK** (0 failures).
- `cd tests/gui && ./GuiTests` (DISPLAY=:0, no xvfb-run needed): **71 tests, OK**
  (0 failures), including the 4 new TMFR-03 tests
  (`TMFR03-shown-default`, `TMFR03-placement`, `TMFR03-caption`,
  `TMFR03-height-expand` all printed and passed).
- `cd tests && ./SSWTests`: **198 tests, OK** (0 failures).

No implementation defects found; all acceptance criteria are behaviorally
validated and all pre-existing coverage continues to pass.

## Cleanup

No temporary non-handoff byproducts were created outside the committed test
files and this artifact directory. Build outputs (`.o`, `.a`, test binaries)
were left in place under `tests/` as these are standard, gitignored build
artifacts produced by the repository's own Makefiles and are not tracked or
committed.

## Commit state

- Test changes committed: **yes** — commit `4e0d9c8ccef48ddcaba8e2838d095c6ee34ede37`
  ("TMFR-03: add Turn-panel behavioral GUI coverage, update stale prompt
  source-contracts") on branch `fixes-tmfr-03-tester-20260703`.
- Required output artifacts (`tester_report.md`, `tester_result.json`,
  `documenter_prompt.txt`) are committed separately in a second commit, per the
  two-commit artifact flow.
