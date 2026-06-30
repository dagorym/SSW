# Implementer Report

Status:
- success

Task summary:
- PGS-01: Restore pre-game mine board placement. Fixed FBattleDisplay::onSetSpeed() to not null m_curShip/m_curWeapon when BS_PlaceMines is entered via beginMinePlacement(). Also hardened FTacticalGame::beginOrdnancePlacement() to only enter BS_PlaceMines when selectPlacementSourceByIndex() succeeds.

Changed files:
- src/tactical/FBattleDisplay.cpp
- src/tactical/FTacticalGame.cpp

Validation commands run:
- make -C /home/tstephen/repos/worktrees/sf2-pgs-01-implementer-20260622/tests tactical-tests
- /home/tstephen/repos/worktrees/sf2-pgs-01-implementer-20260622/tests/tactical/TacticalTests
- make -C /home/tstephen/repos/worktrees/sf2-pgs-01-implementer-20260622/tests
- /home/tstephen/repos/worktrees/sf2-pgs-01-implementer-20260622/tests/SSWTests

Validation outcome:
- PARTIAL-EXPECTED: Tactical tests 213/214 pass. The single failure (FTacticalGameMechanicsTest::testOrdnancePlacementSourceTrackingAndCompatibilityFlows line 586) is a source-inspection test that expected bare 'selectPlacementSourceByIndex(selectedIndex);' but the approved fix wraps this in an if-guard. This is an expected consequence of the approved behavior change. SSWTests: 169/178 pass with 9 pre-existing FGameHeaderDependencyTest failures unchanged from baseline.

Implementation/code commit hash:
- 6f9241d

Artifacts written:
- artifacts/pregame-ordnance-placement/PGS-01/implementer_report.md
- artifacts/pregame-ordnance-placement/PGS-01/tester_prompt.txt
- artifacts/pregame-ordnance-placement/PGS-01/implementer_result.json

Implementation context:
- Root cause: FBattleDisplay::onSetSpeed() (src/tactical/FBattleDisplay.cpp) called m_parent->setShip(NULL) unconditionally after beginMinePlacement(), nulling m_curShip before the first board click. FTacticalGame::placeMineAtHex() returns false when m_curShip == NULL, so every mine click was silently dropped.
- Fix 1 (FBattleDisplay.cpp): Track enteredMinePlacement flag. If beginMinePlacement() succeeds (returns true), skip the setShip(NULL) call so m_curShip/m_curWeapon remain set for the first board click.
- Fix 2 (FTacticalGame.cpp): In beginOrdnancePlacement(), the call selectPlacementSourceByIndex(selectedIndex) at the end now checks the return value: if (!selectPlacementSourceByIndex(selectedIndex)) { return false; } before entering BS_PlaceMines.
- Seeker placement is unaffected: beginSeekerPlacement() has its own path with no setShip(NULL) racing the first click.
- The mine placement done path (completeMinePlacement) correctly calls setShip(NULL)/setWeapon(NULL) after the phase ends.
- The Tester must write a behavioral test that exercises the real entry flow and asserts mine placement succeeds.

Expected validation failures carried forward:
- FrontierTests::FTacticalGameMechanicsTest::testOrdnancePlacementSourceTrackingAndCompatibilityFlows (line 586): assertContains for 'selectPlacementSourceByIndex(selectedIndex);' fails because the approved fix wraps this in 'if (!selectPlacementSourceByIndex(selectedIndex)) {'. Tester must update this assertion to match the new guarded form and add the required behavioral regression test.
