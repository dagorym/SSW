## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated the testing guidance to record the new focused `FGameConfig::resolveAssetPath(...)` regression coverage, the shared-resolver `WXIconCacheTest` contract, and the tactical zoom-image resolver guard without implying any new broad harness.

### Summary
Updated contributor-facing test coverage documentation so the repository guidance now reflects the shipped focused asset-path regression tests: core coverage documents repo-asset lookup, normalized relative paths, executable-parent fallback behavior, and `WXIconCache` shared-resolver enforcement; tactical coverage now explicitly records the `resolveAssetPath("data/zoom.png")` guard against raw base-path concatenation. Assumption: `image-path` remained the correct comparison base and `AGENTS.md` was the right documentation target because no existing feature doc covered this test-only regression work.

### Commit Message
`Document asset-path resolver regression coverage`
