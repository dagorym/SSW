## Documenter Report

### Files Updated
- **`AGENTS.md`** — Added contributor guidance that model objects now keep icon filenames while GUI and tactical render paths resolve ship and fleet images through `WXIconCache`.
- **`artifacts/milestone1-wximage-removal/T7/documenter_result.json`** — Recorded the documentation outcome for T7.
- **`artifacts/milestone1-wximage-removal/T7/verifier_prompt.txt`** — Wrote the Verifier handoff with implementation, test, and documentation scope.

### Summary
Updated the developer guidance in `AGENTS.md` to match the implemented WXIconCache migration now used by both GUI and tactical display code. The new note documents the actual post-T7 boundary: model classes keep icon names, and rendering code resolves `wxImage` assets through `WXIconCache`.

### Commit Message
`docs: note WXIconCache usage for GUI and tactical displays` *(not committed in this session because no git execution tool was available)*
