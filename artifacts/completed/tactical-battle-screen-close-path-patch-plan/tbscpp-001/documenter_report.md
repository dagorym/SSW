## Documenter Report

### Files Updated
- **`include/tactical/FBattleScreen.h`** — refreshed the class and close-lifecycle Doxygen comments to describe the shipped frame-backed behavior: `File -> Quit` and title-bar close share one screen-owned close path, non-modal screens hide before `Destroy()`, and modal callers unwind through `EndModal(...)` without default frame destruction touching stack-owned instances.
- **`doc/DesignNotes.md`** — removed the stale claim that `FTacticalGame` owns close-in-progress state and corrected the menu/title-bar lifecycle narrative to match the delivered `Close(true)` → `onClose(...)` → `closeBattleScreen(...)` flow.
- **`AGENTS.md`** — aligned the tactical regression-coverage guidance with the actual accepted close path so repository guidance no longer describes the old direct menu-to-helper binding.

### Summary
Reviewed TBSCPP-001 against the approved plan, the implemented diff, and the tester artifacts. Updated only the documentation affected by the shipped close-lifecycle repair so repository docs now match the tested behavior: the close guard is screen-owned, non-modal frames hide before pending-delete destruction, modal `ShowModal()` callers unwind by return code, and accepted close events do not continue into wx default frame close handling. Validation used `git diff --check` after the documentation-only edits.

### Commit Message
`docs: document TBSCPP-001 close lifecycle`
