# Implementer Report

Status:
- success

Task summary:
- Reposition the BS_PlaceMines source-selection rows in drawPlaceMines() to anchor to the top of the lower panel (right column at lMargin=310, starting at getActionPromptLineY(0)) rather than below the action-button row. Wrap the mine instruction text onto two lines in the left column using drawWrappedActionPrompt().

Changed files:
- src/tactical/FBattleDisplay.cpp

Validation commands run:
- make -C tests tactical-tests
- ./tests/tactical/TacticalTests

Validation outcome:
- Tactical tests: 221 pass, 1 expected failure (testPlaceMinesSourceListStartsAtActionButtonRowBottom). GUI tests: build succeeded; xvfb-run unavailable so execution was skipped.

Implementation/code commit hash:
- 304d6a2

Artifacts written:
- artifacts/seeker-missile-fixes-round4/SMRIV-01/implementer_report.md
- artifacts/seeker-missile-fixes-round4/SMRIV-01/tester_prompt.txt
- artifacts/seeker-missile-fixes-round4/SMRIV-01/implementer_result.json

Implementation context:
- drawPlaceMines() in src/tactical/FBattleDisplay.cpp was changed.
- The instruction text is now rendered via drawWrappedActionPrompt() with maxWidth = lMargin - leftOffset - BORDER (310 - 70 - 5 = 295), which wraps the string onto two lines.
- Source rows now start at int y = getActionPromptLineY(0) = ACTION_PROMPT_TOP_MARGIN = 5 at x = lMargin = 310.
- m_shipNameRegions entries still use wxRect(lMargin, y, ...) matching drawn positions.
- The mineListBottom height-expansion logic and Done button lifecycle are unchanged.
- Two existing source-contract tests are expected failures (see tester_instructions).

Expected validation failures carried forward:
- FTacticalBattleDisplayFireFlowTest::testPlaceMinesSourceListStartsAtActionButtonRowBottom (source-contract asserts old 'int y = getActionButtonRowBottom();' which is intentionally replaced by 'int y = getActionPromptLineY(0);')
- TacticalGuiLiveTest::testPlacementSourceRowsArePopulatedAndClickSelectionUpdatesSources AC3 (asserts rows start at or below getActionButtonRowBottom(); new layout starts rows at getActionPromptLineY(0) which is above that)
