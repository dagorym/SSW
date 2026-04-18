## Documenter Report

### Files Updated
- **`AGENTS.md`** — Added contributor guidance that both wx app startup paths must keep delegating to `createStartupSplashAndFrame(...)` so the shared splash/frame seam and its startup ordering remain regression-testable.
- **`artifacts/startup-splash-frame-centering/subtask-1/documenter_report.md`** — Recorded the documentation-stage outcome and rationale.
- **`artifacts/startup-splash-frame-centering/subtask-1/documenter_result.json`** — Captured the machine-readable documentation result using the documentation commit hash.
- **`artifacts/startup-splash-frame-centering/subtask-1/verifier_prompt.txt`** — Prepared the Verifier handoff covering implementation, tests, and documentation.

### Summary
Updated contributor documentation in `AGENTS.md` to record the new shared wx startup launch seam for `FApp` and `FBattleSimApp`. No product/user-guide update was warranted because subtask 1 changed startup structure and testability, not user-visible startup behavior.

### Commit Message
`docs: note shared startup launch seam`
