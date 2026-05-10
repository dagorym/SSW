## Documenter Report

### Files Updated
- `doc/DesignNotes.md` — documented the stopped-ship move-phase UI integration, including temporary facing preview, prompt branching, preserved non-stopped route overlays, and the tester coverage that locks those seams.
- `doc/UsersGuide.md` — replaced the outdated stopped-ship warning with current move-phase instructions for choosing a facing, rotating in place, and using `Movement Done` after zero-displacement turns.

### Summary
Reviewed `plans/stopped-ship-free-rotation-plan.md`, the implementer/tester diffs, `doc/DesignNotes.md`, and `doc/UsersGuide.md`. Using comparison-base assumptions `stopped-ship-free-rotation-plan-implementer-20260509..stopped-ship-free-rotation-plan-tester-20260509` for tester-stage review and `stopped-ship-free-rotation-plan..HEAD` for the full shipped story, I updated the existing tactical docs instead of creating a new document. The design notes now describe how the existing battle board and move prompt surface stopped-ship facing selection, while the user guide now explains that stopped ships can choose a facing, optionally rotate in place, and still finish the move phase through the normal `Movement Done` flow.

### Validation
- `cd tests && make tactical-tests && ./tactical/TacticalTests`
- Result: `OK (138 tests)`

### Assumptions
- Existing tactical movement documentation belongs in `doc/DesignNotes.md` and `doc/UsersGuide.md`; no new documentation file was justified.
- No `AGENTS.md` or `.myteam` guidance update was required because the shipped change only affects tactical move UI behavior and the corresponding docs.

### Commit Message
`docs: describe stopped-ship move UI flow`
