## Documenter Report

### Files Updated
- **`doc/UsersGuide.md`** — Added the user-facing resize note that the tactical lower panel reflows immediately while the window is open so the movement reminder, selected-ship status block, and completion-button band stay separated as width changes.
- **`doc/DesignNotes.md`** — Documented the resize/reflow implementation follow-up: `FBattleScreen::onSize(...)` now forces `FBattleDisplay::reflowLowerPanelLayout()` before screen-level layout policy, `FBattleDisplay` recalculates move-prompt reservation through the new helper seams, and the tester-validated tactical/gui commands both passed without introducing tactical-rules or fire-resolution changes.

### Summary
I reviewed the plan hints, the implemented tactical layout diff, the tester's acceptance results, and the existing tactical documentation. The only user-facing documentation gap was that the guide did not say the lower panel reflows immediately during an open-window resize, and the design notes did not capture the new deterministic resize/reflow seam or its validation results. I updated those two docs only; `doc/rules/tactical_operations_manual.md` remained untouched, and no repository-wide agent guidance changes were needed because this task stayed within tactical layout behavior.

### Documentation Commit
- **Commit:** `6dd9605e4d2f1a35972db79732ccb2182a6e76f7`
- **Message:** `docs: capture tactical resize reflow validation`

### Validation Context
- Reused the tester-validated results for this layout-only follow-up:
  - `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (150 tests)` baseline, `OK (152 tests)` post-change
  - `cd tests/gui && make && xvfb-run -a ./GuiTests` → `OK (35 tests)` baseline and post-change
- No documentation-specific build or lint step exists in this repository.
