# Documenter Report

Status:
- pass

Task summary:
- SMRIV-03: Repositioned the PH_ATTACK_FIRE offensive-seeker recall list in FBattleDisplay::draw() to anchor at getActionPromptLineY(0) in the column to the right of the Done button (lMargin=310), instead of below the action-button row at leftOffset/getActionButtonRowBottom()+BORDER. Consistent with pre-game placement treatment used by drawPlaceMines and drawPlaceSeekers. Updated docblock in FBattleDisplay.h.

Branch name:
- sf2-SMRIV-03-documenter-20260629

Documentation commit hash:
- 738f644

Documentation files added or modified:
- doc/test-contracts.md

Commands run:
- make -C tests tactical-tests
- ./tests/tactical/TacticalTests
- make -C tests/gui
- DISPLAY=:0 ./tests/gui/GuiTests

Final test outcomes:
- Tactical: 223/223 pass.
- GUI: 43/52 pass, 9 pre-existing failures (unrelated to SMRIV-03).
- testOffensiveSeekerPendingListRegionVisibilityAndRecall PASSED — AC2 position assertions verify recall regions at x >= 310 / y >= getActionPromptLineY(0); AC3 click-recall via checkOffensiveSeekerPendingSelectionPublic confirmed ammo restored.
- testActionPromptSpacingContractAppliedAcrossActionPhases PASSED — updated source-contract assertion checks drawOffensiveSeekerPendingRows(dc, 310, getActionPromptLineY(0), 10).

Assumptions:
- None

Artifacts written:
- artifacts/seeker-missile-fixes-round4/SMRIV-03/documenter_report.md
- artifacts/seeker-missile-fixes-round4/SMRIV-03/documenter_result.json
- artifacts/seeker-missile-fixes-round4/SMRIV-03/verifier_prompt.txt
