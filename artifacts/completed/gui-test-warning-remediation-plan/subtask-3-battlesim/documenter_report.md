## Documenter Report

### Files Updated
- **`doc/DesignNotes.md`** — Clarified that the BattleSim live tests now prove concrete `LocalGameDialog`, `ScenarioDialog`, and `ScenarioEditorGUI` launch identities plus parent linkage, while preserving `FBattleScreen` lifecycle and zero-residual shown-window cleanup notes.
- **`AGENTS.md`** — Tightened the GUI testing guidance so BattleSim launch-path coverage explicitly calls out concrete dialog identity checks, parent hide/destroy cleanup, and the zero-residual shown-window requirement beyond the pre-launch baseline.

### Summary
Updated the existing BattleSim live-GUI documentation surfaces to match the remediated and tester-validated behavior: the tests now prove concrete dialog identities and parent linkage for `BattleSimFrame -> LocalGameDialog` and `LocalGameDialog -> ScenarioDialog` / `ScenarioEditorGUI`, while the scenario launch paths still preserve `FBattleScreen` lifecycle assertions and explicitly hide, destroy, stabilize, and clean up shown top-level windows before confirming teardown returned to baseline.

### Commit Message
`Document remediated BattleSim GUI assertions`
