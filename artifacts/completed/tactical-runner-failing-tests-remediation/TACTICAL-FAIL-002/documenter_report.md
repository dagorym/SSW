## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated the tactical test guidance to note the behavioral `FTacticalGame` report lifecycle/normalization coverage and the hit-detail rule that only per-attack internal events surface in tactical hit details.
- **`doc/DesignNotes.md`** — Documented that TACTICAL-FAIL-002 replaces tactical report lifecycle/normalization source inspection with behavioral FTacticalGame API coverage, aligns hit-detail semantics to per-attack internal events, and removes architecture-coupled source-inspection checks from the touched tactical runner fixtures.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-002/documenter_report.md`** — Recorded the documentation outcome in the required archival report format.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-002/documenter_result.json`** — Captured the machine-readable documenter result using the documentation commit hash.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-002/verifier_prompt.txt`** — Prepared the Verifier handoff with implementation, test, and documentation review scope.

### Summary
Updated `doc/DesignNotes.md` and `AGENTS.md` so the repository documentation matches the shipped TACTICAL-FAIL-002 test remediation: tactical report lifecycle and normalization semantics are now validated behaviorally through `FTacticalGame` APIs rather than source-token inspection, tactical hit details only surface per-attack internal-event details, and the touched tactical runner fixtures no longer claim architecture-coupled source-inspection coverage that was removed. Then wrote the required Documenter artifacts and Verifier handoff under `artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-002`.

### Commit Message
- `Document tactical report test behavior`
- `Add documenter handoff artifacts for TACTICAL-FAIL-002`
