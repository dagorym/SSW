## Documenter Report

### Files Updated
- **`AGENTS.md`** — Clarified that `GuiTests` can run directly in an existing display session and that headless automation must provide `xvfb-run` or an equivalent virtual-display wrapper.
- **`.github/copilot-instructions.md`** — Matched the contributor guidance to the shipped GUI-test workflow and made the headless display-tool requirement explicit.
- **`artifacts/gui-automated-tests-plan/1-prompt/documenter_report.md`** — Archived the documenter-stage report for the shared handoff directory.
- **`artifacts/gui-automated-tests-plan/1-prompt/documenter_result.json`** — Recorded the machine-readable documenter outcome, documentation commit hash, and artifact paths.
- **`artifacts/gui-automated-tests-plan/1-prompt/verifier_prompt.txt`** — Prepared the verifier handoff covering implementation, test, and documentation review scope.

### Summary
Updated the contributor-facing GUI test guidance to preserve the build-versus-run split (`make gui-tests` builds only; `./GuiTests` still needs a display) and to state that headless environments must have `xvfb-run` or an equivalent virtual-display wrapper available. Added the required documenter artifacts so verification can review the implementation, tests, and updated documentation together.

### Commit Message
`Record documenter artifact commit hash`
