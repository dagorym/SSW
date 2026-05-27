## Documenter Report

### Files Updated
- **`include/tactical/FTacticalCombatReport.h`** — Added in-code Doxygen for the shared tactical report-type surface so the seeker-specific `TRT_SeekerDamage` label is documented alongside the existing immediate-report contexts.
- **`include/tactical/FTacticalGame.h`** — Refreshed seeker-activation and pending-detonation comment blocks to describe immediate seeker-damage summary resolution and the summary-before-cleanup ordering.
- **`include/tactical/FBattleScreen.h`** — Updated the seeker-activation forwarding comment to document wx-side destroyed-ship cleanup consumption and early winner exit after immediate seeker summaries.
- **`doc/DesignNotes.md`** — Documented the shipped TSM-009 architecture flow: parentless temporary `FWeapon::SM` attacks, shared ICM selection, `TRT_SeekerDamage` batching, and deferred cleanup consumption through `FBattleScreen`.
- **`doc/UsersGuide.md`** — Added the player-visible seeker detonation notes covering largest-ship target selection, the normal ICM interception dialog, and the seeker-specific damage-summary label before ship removal.

### Summary
Reviewed the tested TSM-009 diff against `seekers` and updated the existing in-code and repository docs to match the shipped seeker detonation flow. Documentation now covers seeker-specific summary labeling, highest-max-HP same-hex targeting, reuse of the normal ICM dialog, and the immediate-summary-before-cleanup ordering already validated upstream. Additional validation for this stage was limited to `git --no-pager diff --check`; implementation and test behavior remained unchanged from the passing tester stage.

### Commit Message
`Document seeker detonation reporting flow`
