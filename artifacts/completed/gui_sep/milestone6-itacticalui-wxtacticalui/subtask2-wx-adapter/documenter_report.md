## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Documented that Milestone 6 adds the additive `ITacticalUI` / `WXTacticalUI` tactical UI boundary while leaving the live tactical wx runtime on `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`.
- **`AGENTS.md`** — Updated contributor guidance to treat `ITacticalUI` as the non-wx tactical UI boundary and `WXTacticalUI` as additive wx-backed infrastructure that is not the runtime path yet.
- **`.github/copilot-instructions.md`** — Kept repository AI guidance aligned with the same Milestone 6 tactical adapter and test coverage expectations.

### Summary
Documented Subtask 2 as completed additive infrastructure: `WXTacticalUI` now exists as the wx-backed `ITacticalUI` adapter, tactical tests cover the adapter boundary, and runtime tactical delegation remains intentionally unre-wired until a later milestone.

### Commit Message
`docs(tactical): document WXTacticalUI adapter`
