# Documenter Report

Status:
- success

Task summary:
- SMRIV-01: Repositioned BS_PlaceMines source-selection rows to anchor at the top of the lower panel (right column at lMargin=310, starting at getActionPromptLineY(0)) and wrapped mine instruction text onto two lines via drawWrappedActionPrompt() in FBattleDisplay.cpp.

Branch name:
- sf2-SMRIV-01-documenter-20260623

Documentation commit hash:
- a119cee

Documentation files added or modified:
- AGENTS.md
- include/tactical/FBattleDisplay.h

Commands run:
- git diff 529b615 HEAD -- src/tactical/FBattleDisplay.cpp (reviewed implementation diff)
- git diff 529b615 HEAD -- tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp (reviewed test diff)
- git diff 529b615 HEAD -- tests/gui/TacticalGuiLiveTest.cpp (reviewed GUI live test diff)

Final test outcomes:
- TacticalTests: 222/222 PASS (all tactical tests pass)
- GuiTests: 42/51 pass, 9 pre-existing failures unrelated to SMRIV-01
- testPlaceMinesSourceListStartsAtActionButtonRowBottom: PASS (updated for new getActionPromptLineY(0) anchor)
- testPlacementSourceRowsArePopulatedAndClickSelectionUpdatesSources: PASS (AC3 updated for new layout)

Assumptions:
- None

Artifacts written:
- artifacts/seeker-missile-fixes-round4/SMRIV-01/documenter_report.md
- artifacts/seeker-missile-fixes-round4/SMRIV-01/documenter_result.json
- artifacts/seeker-missile-fixes-round4/SMRIV-01/verifier_prompt.txt
