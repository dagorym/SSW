## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Documented that tactical ship rollups now keep attack-level hull damage canonical for the attacked ship, suppress duplicate standalone `TDET_HullDamage` additions when `damagingAttacksReceived > 0`, preserve raw event/internal-event capture, and reflect the corrected `4 hull damage from 1 attack` expectation.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-001/documenter_report.md`** — Recorded the documentation outcome in the required archival report format.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-001/documenter_result.json`** — Captured the machine-readable documenter result using the documentation commit hash.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-001/verifier_prompt.txt`** — Prepared the Verifier handoff with implementation, test, and documentation review scope.

### Summary
Updated `doc/DesignNotes.md` to match the shipped TACTICAL-FAIL-001 behavior: attack-level hull damage remains the player-facing canonical total, standalone report-level `TDET_HullDamage` events no longer inflate that total for ships that already received damaging attacks, non-hull standalone effects still roll up normally, and raw-event/internal-event capture remains intact for debugging/detail use. Then wrote the required Documenter artifacts and Verifier handoff under `artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-001`.

### Commit Message
- `Document tactical hull summary rule`
- `Add documenter handoff artifacts for TACTICAL-FAIL-001`
