## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated the repository testing guidance to reflect that live `WXTacticalUI` modal ICM/message/winner verification now runs in `TacticalGuiLiveTest`, while `WXTacticalUIAdapterTest` keeps only stable behavioral guard checks.
- **`doc/DesignNotes.md`** — Documented that TACTICAL-FAIL-003 removes stale tactical adapter source-token and runtime non-rewire assertions, moves the remaining modal verification into `GuiTests`, and records the passing GUI/tactical validation commands.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-003/documenter_report.md`** — Recorded the documentation outcome in the required archival report format.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-003/documenter_result.json`** — Captured the machine-readable documenter result using the documentation commit hash.
- **`artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-003/verifier_prompt.txt`** — Prepared the Verifier handoff with implementation, test, and documentation review scope.

### Summary
Updated `AGENTS.md` and `doc/DesignNotes.md` so repository guidance matches the shipped TACTICAL-FAIL-003 remediation: `WXTacticalUIAdapterTest` now relies on stable behavioral checks instead of stale source-token and runtime non-rewire assertions, while `TacticalGuiLiveTest` owns the live `WXTacticalUI` modal ICM-selection, message-dialog, and winner verification executed by `GuiTests`. Then wrote the required Documenter artifacts and Verifier handoff under `artifacts/tactical-runner-failing-tests-remediation/TACTICAL-FAIL-003`.

### Commit Message
- `Document tactical adapter coverage shift`
- `Add documenter handoff artifacts for TACTICAL-FAIL-003`
