# Implementer Report

Status:
- PASS

Task summary:
- Fix end-of-move Turn Left/Right enable logic in canUseEndOfMoveTurn() (src/tactical/FTacticalGame.cpp) so a ship with remaining MR can still turn: replaced the buggy per-hex MR_TURN flag test at the path end hex with the whole-path budget rule countFlags(MR_TURN) < getMR() already used by computeRemainingMoves(), so a minelayer with MR=2 that used one turn can still turn in the hex right after that turn.

Changed files:
- src/tactical/FTacticalGame.cpp

Validation commands run:
- make
- cd tests && make tactical-tests && ./tactical/TacticalTests

Validation outcome:
- make succeeded with no warnings/errors introduced by the change. tests/tactical/TacticalTests: 251 run, 1 failure, 0 errors. The single failure (FTacticalEndOfMoveTurnTest::testCannotApplyTurnWhenPathEndHexHasMRTurnFlag) is an expected consequence of the approved behavior change: that test manually flags the end-of-path hex with MR_TURN while MR=3 (only 1 of 3 MR turns used) and asserts the turn should be blocked, which is exactly the buggy per-hex-flag behavior the task instructed us to remove. Under the corrected whole-path-budget rule (countFlags(MR_TURN)=1 < getMR()=3) the turn is correctly allowed, so this existing test now encodes the old bug and needs to be updated by the Tester stage per the task's Tester handoff note (add a turn-budget enable test with MR=2, one turn used).

Implementation/code commit hash:
- 725915cd02be927aaf7317a1c7e2c8d191387130

Artifacts written:
- artifacts/tactical-misc-fixes-round2/TMFR-02/implementer_report.md
- artifacts/tactical-misc-fixes-round2/TMFR-02/tester_prompt.txt
- artifacts/tactical-misc-fixes-round2/TMFR-02/implementer_result.json

Implementation context:
- Fixed function: static bool canUseEndOfMoveTurn(FVehicle*, FTacticalTurnData*) in src/tactical/FTacticalGame.cpp (~line 843).
- Old (buggy) rule: tested turnData->path.getFlag(turnData->path.endPoint()) & MR_TURN, which is wrong because MR_TURN is recorded on the hex a ship turns INTO (see handleMoveHexSelection ~line 2631-2632), not the hex it turns from; a turn-then-advance on the final leg flags the ship's current (destination) hex even though MR remains.
- New rule: turnData->path.countFlags(MR_TURN) >= (unsigned int)ship->getMR() blocks the turn; this matches the whole-path budget check already used by computeRemainingMoves() at src/tactical/FTacticalGame.cpp:2376 (turnData->path.countFlags(MR_TURN) < turnLimit).
- Preserved unchanged: the ship->getMR() == 0 short-circuit, the nMoved >= (speed - ADF) minimum-move guard, and the turnData->pendingEndOfMoveFacing != -1 guard (a pending turn always blocks a second one regardless of remaining MR budget).
- Not touched: canApplyEndOfMoveTurnLeft()/canApplyEndOfMoveTurnRight() reverse/undo logic, applyEndOfMoveTurn(), the one-hexside clamp in turnShip(), and finalizeMovementState() commit-at-Movement-Done behavior.
- Repro scenario from the task: minelayer with MR=2, one turn already used via normal movement (1 MR_TURN flag on the path), buttons should now be enabled in the hex right after the turn (countFlags==1 < MR==2); blocked only once a second turn brings countFlags to 2.

Expected validation failures carried forward:
- tests/tactical/FTacticalEndOfMoveTurnTest.cpp::testCannotApplyTurnWhenPathEndHexHasMRTurnFlag fails against the corrected code because it asserts the old buggy per-hex-flag behavior (MR=3, 1 turn used via manual flag on the end-of-path hex, asserts turn blocked). Under the whole-path MR budget rule this should be allowed (1 < 3), so the assertion is now backwards. The Tester must update or replace this test's expectation to assert the turn is allowed here, and add/keep coverage for the case where countFlags(MR_TURN) == getMR() (turns exhausted) still blocking the turn.
