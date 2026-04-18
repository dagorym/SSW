## Documenter Report

### Files Updated
- **`AGENTS.md`** — Documented the `WXStrategicUI` runtime-guard contract and noted the new strategic adapter regression coverage in the core and GUI test summaries.

### Summary
Updated repository guidance so contributor-facing docs now describe the `WXStrategicUI` precondition that modal strategic dialogs require a usable wx runtime, the no-runtime fallbacks (`wxID_CANCEL`, early return, and message-output fallback), and the test coverage that locks those semantics while preserving parentless live-wx behavior.

### Commit Message
`Document WXStrategicUI runtime guards`
