# Documenter Report — TMF-02

## Story
Remove `SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG)` from
`FBattleScreen` constructor to restore the minimize button on GTK, while
preserving the custom modal event loop and `gtk_window_set_modal` grab.

## Documentation Changes

### doc/test-contracts.md
Added **TMF-02** bullet documenting the three new `TacticalGuiLiveTest`
contracts introduced by the Tester:
- `testBattleScreenExtraStyleExcludesTopLevelExDialog` — behavioral: constructs
  a fresh `FBattleScreen`, asserts `(GetExtraStyle() & wxTOPLEVEL_EX_DIALOG) == 0`
  after construction; FAILS against old code, PASSES after removal.
- `testBattleScreenDefaultStyleIncludesMinimizeBox` — behavioral: asserts
  `(GetWindowStyleFlag() & wxMINIMIZE_BOX) != 0`; confirms the minimize button
  is enabled at the wxWidgets level after the extra-style fix.
- `testBattleScreenShowModalContainsGtkWindowSetModal` — source-contract
  supplement: asserts `gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE)` still
  present in `src/tactical/FBattleScreen.cpp`; locks the modal-grab invariant
  so it cannot be accidentally removed along with the dialog extra-style line.

### doc/UsersGuide.md
Added a sentence to the tactical window description noting that the battle screen
window supports minimize and maximize using standard title bar buttons, reflecting
the restored minimize functionality.

### AGENTS.md
No update required. No existing note about `wxTOPLEVEL_EX_DIALOG` or
`FBattleScreen` extra-style behavior was present; the change is a single-line
bug fix with no impact on bootstrap guidance or repository-wide runtime guidance.

### In-code documentation (FBattleScreen.h and FBattleScreen.cpp)
Verified consistent. Implementer and Tester already updated:
- `include/tactical/FBattleScreen.h` constructor doxygen: author list extended
  with `claude-sonnet-4-6 (medium)`, Last Modified updated to Jun 30, 2026.
- `src/tactical/FBattleScreen.cpp` file header: author list extended with
  `claude-sonnet-4-6 (medium)`, Last Modified updated to Jun 30, 2026.

## Documentation Commit
`a20237b`

## Result
PASS
