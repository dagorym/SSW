## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated contributor guidance so direct startup, tactical, and GUI image loads now explicitly route through `FGameConfig::resolveAssetPath(...)`, and removed the stale `WXIconCache` `../` fallback note.
- **`doc/DesignNotes.md`** — Documented the AP-02 asset-resolution follow-up across `WXStartupLaunch`, `FBattleDisplay`, `FBattleBoard`, `FBattleScreen`, and `WXIconCache`, plus the source-contract tests that lock the policy down.

### Summary
Documented the tested AP-02 asset-path resolution behavior so contributor guidance and design notes now reflect the shared `FGameConfig::resolveAssetPath(...)` seam for startup, tactical, and icon image loading, including the updated regression coverage expectations.

### Commit Message
`Document AP-02 asset path resolution`
