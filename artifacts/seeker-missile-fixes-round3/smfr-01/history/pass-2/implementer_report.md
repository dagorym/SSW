# Implementer Report

Status:
- success

Task summary:
- SMFR-01 pass-2 remediation: Fix drawSeekerActivation() height expansion being silently overwritten by ensureLowerPanelLayoutState() in the BS_Battle draw() pass. Added a max-preservation guard so any previously-expanded requestedDisplayHeight survives the stats-based layout pass. This fixes the one remaining failing acceptance criterion (drawSeekerActivation() must reserve vertical space) without regressing the two already-passing criteria (drawPlaceMines and drawPlaceSeekers expansion).

Changed files:
- src/tactical/FBattleDisplay.cpp

Validation commands run:
- make -C tests tactical-tests
- ./tests/tactical/TacticalTests
- make -C tests gui-tests
- DISPLAY=:0 ./tests/gui/GuiTests

Validation outcome:
- PASS: 201 tactical tests pass (OK). GUI: 46 tests, 9 failures — all pre-existing before this fix (verified stash baseline: 10 failures without fix, 9 with fix). The target test testOrdnancePlacementAndActivationPanelHeightAutoExpands seeker-activation assertion now passes. Zero regressions.

Implementation/code commit hash:
- d3ee240

Artifacts written:
- artifacts/seeker-missile-fixes-round3/smfr-01/implementer_report.md
- artifacts/seeker-missile-fixes-round3/smfr-01/tester_prompt.txt
- artifacts/seeker-missile-fixes-round3/smfr-01/implementer_result.json

Implementation context:
- Root cause: in draw() for BS_Battle, drawSeekerActivation(dc) raised requestedDisplayHeight, then drawCurrentShipStats(dc) called ensureLowerPanelLayoutState() which unconditionally overwrote requestedDisplayHeight with max(buttonRowHeight, statsBottom).
- Fix: added a max-preservation guard at end of ensureLowerPanelLayoutState() (~line 586): if m_lowerPanelLayoutState.requestedDisplayHeight > newly computed requestedHeight, use the larger value. This ensures list-widget expansions from drawSeekerActivation(), drawPlaceMines(), or drawPlaceSeekers() survive the stats layout pass.
- Approach chosen: preserve the max in ensureLowerPanelLayoutState (not reorder draw calls) — least invasive, no draw-order changes, generalizes for future draw helpers.
- drawPlaceMines() and drawPlaceSeekers() run in BS_PlaceMines/BS_PlaceSeekers states where drawCurrentShipStats() is not called, so they were unaffected and remain passing.
- Pre-existing GUI failures (9 of 46): StrategicGuiLiveTest tests, testTacticalActionButtonsStayBelowPromptReservationAcrossPhases, testSeekerActivationPanelSourceContracts, testSetupPlacementSourceRowsAndOrdnanceColorContracts, testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges, testBattleDisplayNarrowWidthStacksShipStatsBelowButtons, testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo, BattleSimGuiLiveTest::testLocalGameDialogLaunchesPredefinedAndCustomModalChains — all pre-existed before this fix (verified by stash baseline).

Expected validation failures carried forward:
- testStrategicDialogsUseStaticBoxChildParents (pre-existing, out of scope)
- testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow (pre-existing, out of scope)
- testRemediatedStrategicDialogsUseFirstShowSizingContract (pre-existing, out of scope)
- testTacticalActionButtonsStayBelowPromptReservationAcrossPhases (pre-existing, out of scope)
- testSeekerActivationPanelSourceContracts (pre-existing, out of scope)
- testSetupPlacementSourceRowsAndOrdnanceColorContracts (pre-existing, out of scope)
- testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges (pre-existing, out of scope)
- testBattleDisplayNarrowWidthStacksShipStatsBelowButtons (pre-existing, out of scope)
- testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo (pre-existing, out of scope)
- testLocalGameDialogLaunchesPredefinedAndCustomModalChains (pre-existing, out of scope)
