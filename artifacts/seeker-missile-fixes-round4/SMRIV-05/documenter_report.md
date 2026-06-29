# Documenter Report

Status:
- pass

Task summary:
- SMRIV-05: Fix activation-phase seeker missiles not removed from m_seekerMissiles after impact. The fix replaces inline if (m_ui != NULL) { requestRedraw(); resolvePendingSeekerDetonationDamage(); } blocks in beginSeekerActivationPhase() and completeSeekerActivationPhase() with a single call to applyMovementSeekerDamage(), which handles redraw, resolution, and seeker erasure in one step. FTacticalGame.h docblocks for both methods were updated by the Implementer. Documenter added SMRIV-05 entry to doc/test-contracts.md covering the fix, the updated testSeekerActivationPhaseResolvesPendingDamageWhenModelHasUI source-contract, the updated testCompleteSeekerActivationResolvesMovementContactAndSurvivorProgressionRuntime behavioral expectations, the new testActivationPhaseImpactingSeekerRemovedAndDoesNotAttackOnNextPhase AC1/AC4 behavioral test, and the incidental PREAGAME->PREGAME include-guard typo fix.

Branch name:
- sf2-SMRIV-05-documenter-20260629

Documentation commit hash:
- 92e1eb0

Documentation files added or modified:
- doc/test-contracts.md

Commands run:
- make -C tests tactical-tests
- tests/tactical/TacticalTests
- make -C tests
- tests/SSWTests

Final test outcomes:
- TacticalTests: 224/224 pass (223 prior + 1 new test: testActivationPhaseImpactingSeekerRemovedAndDoesNotAttackOnNextPhase).
- testSeekerActivationPhaseResolvesPendingDamageWhenModelHasUI UPDATED: verifies applyMovementSeekerDamage() in both activation-phase functions.
- testCompleteSeekerActivationResolvesMovementContactAndSurvivorProgressionRuntime UPDATED: outcomes.size()==0, seekers.size()==1.
- SSWTests: 9 pre-existing failures in FGameHeaderDependencyTest, unrelated to SMRIV-05.

Assumptions:
- None

Artifacts written:
- artifacts/seeker-missile-fixes-round4/SMRIV-05/documenter_report.md
- artifacts/seeker-missile-fixes-round4/SMRIV-05/documenter_result.json
- artifacts/seeker-missile-fixes-round4/SMRIV-05/verifier_prompt.txt
