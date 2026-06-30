# Implementer Report — SMRIV-05

**Status:** success

**Task:** SMRIV-05 — Fix activation-phase seeker missiles not removed from `m_seekerMissiles` after impact, causing them to attack again on later phases/turns.

**Branch:** `sf2-SMRIV-05-implementer-20260629`

**Code commit hash:** `0e6bfe3`

**Changed files:**
- `src/tactical/FTacticalGame.cpp`
- `include/tactical/FTacticalGame.h`

**Validation commands run:**
- `cd tests && make tactical-tests && ./tactical/TacticalTests`
- `cd tests/gui && make`
- `cd tests/gui && xvfb-run -a ./GuiTests`

**Validation outcome:** Build succeeded. 223 tests run, 2 failures — both are expected consequences of the approved behavior change (see below). No unintended regressions.

**Artifacts written:**
- `artifacts/seeker-missile-fixes-round4/SMRIV-05/implementer_report.md`
- `artifacts/seeker-missile-fixes-round4/SMRIV-05/tester_prompt.txt`
- `artifacts/seeker-missile-fixes-round4/SMRIV-05/implementer_result.json`

## Implementation Context

In both `completeSeekerActivationPhase()` and the no-inactive-hexes early path in `beginSeekerActivationPhase()`, replaced the inline `if (m_ui != NULL) { requestRedraw(); resolvePendingSeekerDetonationDamage(); }` block with a single call to `applyMovementSeekerDamage()`.

`applyMovementSeekerDamage()` already implements the full sequence:
1. Collect detonated seeker IDs from `m_pendingSeekerContactOutcomes`
2. If UI installed: `requestRedraw()` + `resolvePendingSeekerDetonationDamage()`; otherwise: `clearPendingSeekerContactOutcomes()`
3. Erase each detonated seeker from `m_seekerMissiles` by ID

The activation-phase paths were missing step 3 entirely, leaving impacting seekers alive in `m_seekerMissiles`.

**SMF-06 preserved:** `applyMovementSeekerDamage()` redraws before showing dialogs so the impacting seeker remains visible at its final hex during ICM/damage dialogs, and erases it only after resolution returns.

**Non-impacting seekers unaffected:** only seekers whose IDs appear in `m_pendingSeekerContactOutcomes` are erased.

Updated Doxygen headers for both functions to reflect the new delegation.

## Expected Validation Failures (Approved Behavior Change)

1. **`FTacticalMineDamageFlowTest::testSeekerActivationPhaseResolvesPendingDamageWhenModelHasUI`** — source-contract test checking for `if (m_ui != NULL) {` and `resolvePendingSeekerDetonationDamage();` directly in both activation functions. The approved fix routes both through `applyMovementSeekerDamage()` instead. Tester must update assertions to check for `applyMovementSeekerDamage();` in both function bodies.

2. **`FTacticalSeekerMovementTest::testCompleteSeekerActivationResolvesMovementContactAndSurvivorProgressionRuntime`** — behavioral test whose expected values document the old buggy behavior: `outcomes.size()==1` and `seekers.size()==2` after no-UI activation. After fix: `outcomes.size()==0` (cleared) and `seekers.size()==1` (impacting seeker 9101 erased, survivor 9102 remains). Tester must update expected values and verify survivor progression still holds.

## Test Location Guidance (for Tester)

New behavioral test: `tests/tactical/FTacticalSeekerMovementTest.cpp` or `tests/tactical/FTacticalGameMechanicsTest.cpp`.

Carry-C item: The Tester stage may also correct the include-guard typo in `tests/tactical/FTacticalPreGameOrdnanceTest.h:15` (PREAGAME → PREGAME).

