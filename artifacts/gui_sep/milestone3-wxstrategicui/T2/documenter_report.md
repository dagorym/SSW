## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated repository guidance to reflect the shipped `FGame::create()` overload set and the `IStrategicUI` strategic UI boundary.
- **`.github/copilot-instructions.md`** — Kept Copilot guidance in sync with `AGENTS.md` for the same `FGame` singleton and strategic UI routing changes.
- **`artifacts/gui_sep/milestone3-wxstrategicui/T2/documenter_report.md`** — Records the T2 documentation review outcome.
- **`artifacts/gui_sep/milestone3-wxstrategicui/T2/documenter_result.json`** — Stores the machine-readable T2 documentation result.
- **`artifacts/gui_sep/milestone3-wxstrategicui/T2/verifier_prompt.txt`** — Hands off implementation, tests, and documentation scope to the Verifier.

### Summary
Reviewed T2 against `plans/milestone3-wxstrategicui-plan.md`, the tested diff from `gui_sep` through the tester branch, and the existing repository guidance files. A documentation update was warranted because both `AGENTS.md` and `.github/copilot-instructions.md` still described the old `FGame::create(wxWindow*)` entry point and did not tell contributors to keep strategic dialogs and notifications behind `IStrategicUI`. Updated both files in sync so repository-wide guidance now matches the implemented and tested behavior: `FGame` is created via `create()` or `create(IStrategicUI*)`, `create(wxWindow*)` is only a compatibility shim, and strategic UI interactions should stay routed through `IStrategicUI`/gui adapters such as `WXStrategicUI`.

### Commit Message
`T2: document FGame strategic UI boundary`
