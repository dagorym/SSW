## Documenter Report

### Files Updated
- **`AGENTS.md`** — Added contributor guidance that the Milestone 5 `FTacticalHexData`/`FTacticalTurnData` duplication with `FBattleBoard` is an intentional compatibility boundary to keep model-only includes wx-free until later cleanup.
- **`.github/copilot-instructions.md`** — Kept repository-wide Copilot guidance aligned with `AGENTS.md` on the temporary tactical type duplication and later cleanup expectation.
- **`doc/DesignNotes.md`** — Recorded that `FTacticalGame` intentionally mirrors `FBattleBoard`'s legacy tactical state structs during Milestone 5 isolation so later migration work removes the correct legacy copies.

### Summary
Documented the Subtask 3 isolation decision as a temporary compatibility boundary, not a behavior change. Repository guidance and internal design notes now explain that `FTacticalGame` duplicates `FBattleBoard`'s tactical state structs on purpose so the additive model stays wx-free while the legacy runtime path remains active until later cleanup milestones.

### Commit Message
`docs: document tactical isolation boundary`
