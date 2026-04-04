## Documenter Report

### Files Updated
- **`AGENTS.md`** — Expanded repository guidance for deterministic offscreen strategic GUI coverage, headless `xvfb-run` usage, `FGamePanel` smoke coverage, and the `WXIconCache` fallback/invalid-image expectations that now match the implemented GUI changes.
- **`.github/copilot-instructions.md`** — Kept the shared Copilot-facing contributor guidance aligned with the same strategic GUI rendering coverage and headless icon-loading behavior documented in `AGENTS.md`.

### Summary
Updated the repository guidance to reflect the implemented strategic GUI live-test expansion: deterministic offscreen rendering checks now cover map/player/game displays, headless GUI runs rely on `xvfb-run`, `FGamePanel` keeps smoke-level live-parent paint coverage, and GUI draw paths now depend on `WXIconCache`'s `../` fallback plus invalid-image guards.

### Commit Message
`Document strategic GUI headless test coverage`
