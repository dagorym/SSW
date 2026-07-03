Verifier Report

Scope reviewed:
- Implementer commit 725915c: fixed canUseEndOfMoveTurn() in src/tactical/FTacticalGame.cpp to replace the per-hex end-of-path MR_TURN flag test with the whole-path budget rule countFlags(MR_TURN) >= (unsigned int)ship->getMR(), preserving the pendingEndOfMoveFacing and minimum-move guards.
- Tester commit 2ad1e08: updated tests/tactical/FTacticalEndOfMoveTurnTest.{cpp,h} -- replaced the obsolete testCannotApplyTurnWhenPathEndHexHasMRTurnFlag with testCanApplyTurnWhenMRTurnBudgetRemainsAfterOneTurnUsed, added testCannotApplyTurnWhenMRTurnBudgetExhausted, and added two real-movement repro tests driven through handleHexClick() (testMinelayerWithMRTwoCanStillTurnAfterUsingOneTurnViaRealMovement, testMinelayerWithMRTwoBlockedAfterUsingBothTurnsViaRealMovement).
- Documenter commit 3394ef8: updated doc/test-contracts.md TMF-05 entry to describe the whole-path budget rule and the current 20-test FTacticalEndOfMoveTurnTest roster.
- Reviewed diff range 72498ab (TMFR-01 verifier PASS) .. 401333d (TMFR-02 documenter head) covering src/tactical/FTacticalGame.cpp, tests/tactical/FTacticalEndOfMoveTurnTest.{cpp,h}, doc/test-contracts.md, and the TMFR-02 artifact files.

Acceptance criteria / plan reference:
- plans/tactical-misc-fixes-round2-plan.md, subtask TMFR-02 (Fix Turn Left/Right enable rule): acceptance criteria requiring the whole-path countFlags(MR_TURN) < getMR() budget rule, minelayer MR=2 one-turn-used scenario allowed, reverse/undo/clamp/commit unchanged, no regression to in-route turning or Movement Done gating.

Convention files considered:
- AGENTS.md (Behavioral Verification Is Mandatory policy; Doxygen header conventions; module boundary rules; doc/rules/tactical_operations_manual.md non-edit constraint)
- CLAUDE.md (pointer to AGENTS.md)
- doc/test-contracts.md (canonical tactical test-source-contract catalog)

Findings

BLOCKING
- None

WARNING
- None

NOTE
- None

Test sufficiency assessment:
- All four new/renamed tests are behavioral, constructing a real FTacticalGame/FVehicle and asserting on canApplyEndOfMoveTurnLeft()/Right() runtime results -- no source-text inspection is used for the fixed behavior, satisfying the repository's Behavioral Verification policy.
- testCanApplyTurnWhenMRTurnBudgetRemainsAfterOneTurnUsed and testCannotApplyTurnWhenMRTurnBudgetExhausted cover the budget boundary (1<3 allowed, 2>=2 blocked) via directly-flagged path state, matching the corrected gate formula.
- testMinelayerWithMRTwoCanStillTurnAfterUsingOneTurnViaRealMovement and testMinelayerWithMRTwoBlockedAfterUsingBothTurnsViaRealMovement drive the exact plan repro (minelayer, MR=2) end-to-end through handleHexClick() and getLeftTurnHexes()/getRightTurnHexes(), which is the highest-value coverage since it exercises the real MR_TURN-flagging code path (handleMoveHexSelection) rather than manually injected flags.
- Pending-turn and minimum-move gating already had dedicated tests (testSecondSameDirectionTurnIsBlockedWhenPendingExists, testCannotApplyTurnBeforeMinimumMoveIsSatisfied) that remain unmodified and passing, and reverse/undo/clamp/commit tests (testApplyingReverseWhenLeftPendingRestoresOriginHeading, testApplyingReverseWhenRightPendingRestoresOriginHeading, testReverseClearsPendingFacingFields, testFinalizeMovementStateCommitsPendingFacing, testFinalizeWithNoPendingUsesCurrentHeading) are confirmed present and byte-for-byte unchanged in the diff, directly verifying the 'unchanged' acceptance criterion.
- Full suite counts independently reproduced in this review: tests/tactical/TacticalTests rebuilt clean via 'make tactical-tests' and run -- OK (254 tests); tests/SSWTests run from the tests/ working directory (matching its relative-path test conventions) -- OK (198 tests). No new failures beyond the intentionally replaced test.
- No test-sufficiency gaps identified for this subtask's acceptance criteria.

Documentation accuracy assessment:
- doc/test-contracts.md's TMF-05 entry was checked against the shipped code and test file: the entry now states canApplyEndOfMoveTurnLeft/Right() gate on 'turnData->path.countFlags(MR_TURN) < ship->getMR()', which matches src/tactical/FTacticalGame.cpp:855 exactly (countFlags(MR_TURN) >= getMR() blocks, i.e. < allows).
- The entry's stated test roster ('20 behavioral tests') matches the actual CPPUNIT_TEST count in tests/tactical/FTacticalEndOfMoveTurnTest.h (verified via grep: 20 CPPUNIT_TEST(...) registrations), and each newly named test in the doc text (testCanApplyTurnWhenMRTurnBudgetRemainsAfterOneTurnUsed, testCannotApplyTurnWhenMRTurnBudgetExhausted, testMinelayerWithMRTwoCanStillTurnAfterUsingOneTurnViaRealMovement, testMinelayerWithMRTwoBlockedAfterUsingBothTurnsViaRealMovement) exists in the test file with matching intent.
- The doc explicitly notes the retirement of testCannotApplyTurnWhenPathEndHexHasMRTurnFlag and explains why its premise no longer holds, avoiding a stale/contradictory reference.
- No duplication or contradiction with other doc/test-contracts.md entries was found; doc/rules/tactical_operations_manual.md was confirmed unedited, consistent with the non-negotiable constraint.

Artifacts written:
- artifacts/tactical-misc-fixes-round2/TMFR-02/verifier_report.md
- artifacts/tactical-misc-fixes-round2/TMFR-02/verifier_result.json

Verdict:
- PASS
