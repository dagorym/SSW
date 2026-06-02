# Implementer Report

Status:
- success

Task summary:
- SMF-03: relocate offensive-fire pending-seeker list to a dedicated bounded region left of the ship-status widget. drawOffensiveSeekerPendingRows moved from drawCurrentShipStats into draw() at (leftOffset, getActionButtonRowBottom()+BORDER) during PH_ATTACK_FIRE. Rows remain click-deletable. Panel auto-expands height when pending rows extend beyond requestedDisplayHeight.

Changed files:
- include/tactical/FBattleDisplay.h
- src/tactical/FBattleDisplay.cpp
- tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp

Validation commands run:
- make -C tests tactical-tests && ./tests/tactical/TacticalTests
- make -C tests gui-tests && DISPLAY=:99 ./tests/gui/GuiTests

Validation outcome:
- PASS. tactical-tests: 196/196 PASS. gui-tests: 8 pre-existing failures unchanged from baseline before this change; 0 new failures introduced.

Implementation/code commit hash:
- 7c57b64

Artifacts written:
- artifacts/seeker-missile-fixes-round2/SMF-03/implementer_report.md
- artifacts/seeker-missile-fixes-round2/SMF-03/tester_prompt.txt
- artifacts/seeker-missile-fixes-round2/SMF-03/implementer_result.json

Implementation context:
- draw() now calls drawOffensiveSeekerPendingRows(dc, leftOffset, getActionButtonRowBottom()+BORDER, 10) inside a PH_ATTACK_FIRE guard, after drawCurrentShipStats
- drawCurrentShipStats no longer calls drawOffensiveSeekerPendingRows at all
- drawOffensiveSeekerPendingRows expands requestedDisplayHeight and calls applyRequestedDisplayHeight() when rendered rows extend below current panel height
- m_pendingSeekerRecallRegions / m_pendingSeekerRecallHexes cleared and rebuilt on each draw call by drawOffensiveSeekerPendingRows
- checkOffensiveSeekerPendingSelection in onLeftUp is unchanged — still dispatches recall via recallSelectedOffensivePendingSeekerAtHex
- FTacticalBattleDisplayFireFlowTest::testActionPromptSpacingContractAppliedAcrossActionPhases updated: assertNotContains(statsBody, 'drawOffensiveSeekerPendingRows('); assertContains(drawBody, 'drawOffensiveSeekerPendingRows(dc, leftOffset, pendingRegionTop, 10);')
- Note for Documenter: getAllPendingOffensiveFireSeekerHexes (FTacticalGame/FBattleScreen) returns all pending hexes across all launchers; getSelectedOffensivePendingSeekerHexGroups returns groups for the selected launcher — these are distinct methods and any AGENTS.md prose referencing them should distinguish them clearly

Expected validation failures carried forward:
- gui-tests: 8 pre-existing failures (StrategicGuiLiveTest x3, TacticalGuiLiveTest x5) — all present in the baseline before SMF-03 and unchanged by this implementation
