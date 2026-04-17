## Documenter Report

### Files Updated
- **`AGENTS.md`** — Expanded contributor guidance for the shared wx startup seam so future changes preserve centered splash/frame startup behavior, the restored splash style contract, and the removal of legacy fixed SSW startup coordinates.
- **`doc/UsersGuide.md`** — Updated the SSW startup description to match the user-visible behavior now shipped: centered splash, centered main window, and brief splash-over-frame overlap during initialization.
- **`doc/DesignNotes.md`** — Documented the implementation/tested startup placement policy for both SSW and BattleSim, including the shared helper ordering, splash style semantics, deterministic frame centering, and regression coverage surface.
- **`artifacts/startup-splash-frame-centering/subtask-2/documenter_report.md`** — Recorded the documentation-stage outcome and rationale.
- **`artifacts/startup-splash-frame-centering/subtask-2/documenter_result.json`** — Captured the machine-readable documentation result using the documentation commit hash.
- **`artifacts/startup-splash-frame-centering/subtask-2/verifier_prompt.txt`** — Prepared the Verifier handoff covering implementation, tests, and documentation.

### Summary
Updated existing documentation only. Assumed `plans/startup-splash-frame-centering-plan.md` is the governing plan context from repository evidence and used the implementer commit `a5b065e`, tester commit `4e7ebf4`, and current tester/documenter branch history as the source of truth. The docs now reflect deterministic startup centering for SSW and BattleSim, the restored splash style/ordering contract, and the removal of the old fixed SSW startup position.

### Commit Message
`docs: note startup centering behavior`
