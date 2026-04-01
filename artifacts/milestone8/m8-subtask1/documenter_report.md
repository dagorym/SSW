## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Added Milestone 8 Subtask 1 notes describing the newly exposed `FTacticalGame` interaction/render-state API surface, the `FBattleScreen` forwarding behavior, and the explicit caveat that `FBattleBoard`/`FBattleDisplay` are not yet rewired in this subtask.
- **`AGENTS.md`** — Updated contributor guidance so Milestone 8 documentation accurately describes `FTacticalGame` as the additive non-wx tactical model surface through Milestone 8 and clarifies that Subtask 1 expands forwarding without yet converting the legacy wx runtime classes.

### Summary
Documented the validated Milestone 8 Subtask 1 tactical model/API expansion by recording the new model-owned interaction and renderer-facing accessors on `FTacticalGame`, the redraw-oriented forwarding now performed by `FBattleScreen`, and the important boundary that the live wx rendering path still remains in `FBattleBoard`/`FBattleDisplay` until later Milestone 8 work.

### Commit Message
`docs: document milestone 8 tactical model api`
