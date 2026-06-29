# Implementer Report

Status:
- success

Task summary:
Repositioned the PH_ATTACK_FIRE offensive-seeker recall list in FBattleDisplay::draw() to anchor at getActionPromptLineY(0) in the column to the right of the Done button (lMargin=310), instead of below the action-button row at leftOffset/getActionButtonRowBottom()+BORDER. Consistent with pre-game placement treatment used by drawPlaceMines and drawPlaceSeekers. Updated docblock in FBattleDisplay.h.

Changed files:
- src/tactical/FBattleDisplay.cpp
- include/tactical/FBattleDisplay.h

Validation commands run:
- make -C tests tactical-tests
- ./tests/tactical/TacticalTests

Validation outcome:
Build: PASS (clean compile). Tests: 222/223 pass. 1 expected failure: FTacticalBattleDisplayFireFlowTest::testActionPromptSpacingContractAppliedAcrossActionPhases — source-contract assertion checking old call pattern 'drawOffensiveSeekerPendingRows(dc, leftOffset, pendingRegionTop, 10)' which was the exact pattern the approved task replaced. This is an expected consequence of the approved behavior change, not a regression.

Implementation/code commit hash:
- 10e9b82

Artifacts written:
- artifacts/seeker-missile-fixes-round4/SMRIV-03/implementer_report.md
- artifacts/seeker-missile-fixes-round4/SMRIV-03/tester_prompt.txt
- artifacts/seeker-missile-fixes-round4/SMRIV-03/implementer_result.json

Implementation context:
Single call site change in draw() at the BS_Battle/PH_ATTACK_FIRE block (src/tactical/FBattleDisplay.cpp ~line 690):

  Old: drawOffensiveSeekerPendingRows(dc, leftOffset, getActionButtonRowBottom() + BORDER, 10)
  New: drawOffensiveSeekerPendingRows(dc, 310, getActionPromptLineY(0), 10)

lMargin=310 is the same right-column margin used by drawPlaceMines (SMRIV-01). drawOffensiveSeekerPendingRows itself is unchanged; click regions (m_pendingSeekerRecallRegions/m_pendingSeekerRecallHexes) are rebuilt each draw call from the new startY and lMargin, so checkOffensiveSeekerPendingSelection() automatically uses the new positions.

Expected validation failures carried forward:
- FTacticalBattleDisplayFireFlowTest::testActionPromptSpacingContractAppliedAcrossActionPhases: source-contract test checked old call string. Tester must update it to check for 'drawOffensiveSeekerPendingRows(dc, 310, getActionPromptLineY(0), 10)'.
