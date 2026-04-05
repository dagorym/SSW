## Documenter Report

### Files Updated
- **`AGENTS.md`** — Updated the GUI test guidance to note that `StrategicGuiLiveTest` now asserts `SelectCombatGUI` list-box parenting under the `Attacking Fleets` and `Defending Fleets and Stations` static boxes before the attack-launch path proceeds.
- **`artifacts/WXWIDGETS_UPGRADE_CHANGES.md`** — Recorded the Subtask 2 wxGTK static-box ownership rule, the `SelectCombatGUI` parenting fix it now documents, and the tester validation commands/outcomes showing the covered `("Combat")` warning is gone.

### Summary
Updated repository documentation to match the implemented and tester-validated `SelectCombatGUI` remediation. The docs now capture the wxGTK static-box parenting rule for the combat dialog, the strategic live GUI coverage that asserts the corrected parentage, and the passing headless validation record for Subtask 2. Documentation commit: `05c53006d6b954a46500af4f4ffca24e831232a0`.

### Commit Message
`Document SelectCombat wxGTK remediation`
