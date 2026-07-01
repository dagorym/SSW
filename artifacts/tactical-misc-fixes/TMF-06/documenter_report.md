# Documenter Report — TMF-06 (pass-2)

## Task
TMF-06 — Skip post-fire damage-summary dialog when no weapon actually fired.
This is pass-2 remediation. The pass-1 Verifier raised a WARNING (behavioral test
gap for AC2/AC3 dialog suppression) and a NOTE (comment typo in FBattleDisplay.h).
Pass-2 tester addressed the WARNING with direct behavioral GUI tests.

## Documentation Changes

### doc/test-contracts.md
Extended the existing TMF-06 entry to describe the two new behavioral GUI tests added
by the pass-2 tester:
- `testDefensiveFireDoneSkipsDialogWhenNoWeaponsFired` (TacticalGuiLiveTest / GuiTests)
- `testOffensiveFireDoneSkipsDialogWhenNoWeaponsFired` (TacticalGuiLiveTest / GuiTests)

Both tests install `CountingMockTacticalUI` on the `ITacticalUI` seam via a
`FireDoneObserverBattleScreen`, fire the respective Done button event with an empty game
(`weaponsFired == 0`), and assert the mock's `showDamageSummary` counter did not
increase. The description explains how the tests directly observe dialog suppression
through the counting mock. All pass-1 entries (weaponsFired behavioral tests and
source-contract supplements) were retained unchanged.

### AGENTS.md
Added a new sub-bullet in the GUI tests section (alongside the existing TMF-05
sub-bullet) describing the two TMF-06 behavioral dialog-suppression tests and how
they exercise the `ITacticalUI` seam to confirm the guard in `onDefensiveFireDone`
and `onOffensiveFireDone` suppresses the dialog when `weaponsFired == 0`.
The existing pass-1 fire-phase note at the ownership bullet (line ~401) was not
duplicated; the new sub-bullet is additive.

### include/tactical/FBattleDisplay.h
The pass-1 NOTE (inline comment typo: `onDefensiveFireDone` comment saying "offensive"
instead of "defensive") was already corrected in pass-1. Confirmed the comment now
reads:
  `/// event handler for defensive fire complete button; shows damage summary only when weaponsFired > 0`
And `onOffensiveFireDone` correctly reads "offensive". No further change needed.

### doc/rules/tactical_operations_manual.md
Not modified (non-volatile external artifact, per project policy).

## Documentation Commit
Commit hash: `7a4e00c`
Branch: `tmf-TMF-06-documenter-20260630`

## Pass Determination
PASS — all required documentation targets updated with minimal, localized edits;
the pass-1 WARNING (behavioral test gap) is fully addressed by the tester's new
GUI tests, which are now documented in both doc/test-contracts.md and AGENTS.md;
the pass-1 NOTE (comment typo) was already resolved in pass-1 and confirmed correct.
