## Documenter Report

### Files Updated
- **`AGENTS.md`** — Clarified that strategic adapter regression coverage now distinguishes the guarded no-runtime `WXStrategicUI` path from the parentless-but-live-wx retreat-modal path.

### Summary
Updated the repository testing guidance in `AGENTS.md` so future maintainers can see that `WXStrategicUITest` locks the safe no-runtime guard behavior even with a non-null parent pointer, while `StrategicGuiLiveTest` separately preserves the valid parentless live-wx retreat dialog flow. Assumption used for diff review: compare this subtask branch against the `test-fixes` merge-base (`a441f16dd57dbac098c47161371b3cc347086856`).

### Commit Message
`Document strategic adapter runtime guard split`
