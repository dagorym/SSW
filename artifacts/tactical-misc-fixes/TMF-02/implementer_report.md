# TMF-02 Implementer Report

## Task

Restore a working minimize button on the FBattleScreen (BattleBoard) window by removing the `wxTOPLEVEL_EX_DIALOG` extra-style applied in the constructor. The custom modal event loop and `gtk_window_set_modal` grab must be preserved.

## Change Summary

**Single line removed** from `src/tactical/FBattleScreen.cpp` constructor:

```cpp
// REMOVED:
SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);
```

On GTK, `wxTOPLEVEL_EX_DIALOG` causes wxWidgets to call `gtk_window_set_type_hint(GTK_WINDOW_TYPE_HINT_DIALOG)`, which instructs the window manager to suppress standard frame decorations including the minimize button. Removing this line allows the frame to present as `GTK_WINDOW_TYPE_HINT_NORMAL`, restoring the minimize/maximize controls.

**Preserved (unchanged):**
- `gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE)` call before `Show()` in `ShowModal()` (line 253)
- `gtk_window_set_modal(GTK_WINDOW(m_widget), FALSE)` call in the cleanup path (line 265)
- Custom `wxEventLoop` modal event loop in `ShowModal()`/`EndModal()`
- `Centre()` call and `applyLayoutPolicy()` — centering and sizing unchanged

**Doxygen headers updated:**
- `src/tactical/FBattleScreen.cpp` file header: added `claude-sonnet-4-6 (medium)` to `@author`, updated `Last Modified` to Jun 30, 2026
- `include/tactical/FBattleScreen.h` constructor header: added `claude-sonnet-4-6 (medium)` to `@author`, updated `Last Modified` to Jun 30, 2026

## Files Changed

- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleScreen.h`

## Validation

| Command | Result |
|---|---|
| `make` (repo root) | PASS |
| GUI tests (DISPLAY=:0) | 56 run, 9 failures — all pre-existing source-contract failures in unrelated files; no regressions from this change |

### Pre-existing GUI test failures (unrelated to this change)

1. `StrategicGuiLiveTest::testStrategicDialogsUseStaticBoxChildParents` — source-contract in ViewFleetGUI.cpp
2. `StrategicGuiLiveTest::testWXStrategicUISourceGuardsRuntimeAndPreservesParentlessFlow` — source not found
3. `StrategicGuiLiveTest::testRemediatedStrategicDialogsUseFirstShowSizingContract` — source not found
4. `TacticalGuiLiveTest::testBattleDisplayActionPromptWithDynamicMarginRendersPositionText` — source-contract
5. `TacticalGuiLiveTest::testBattleDisplayNarrowWidthStacksShipStatsBelowButtons` — source-contract in FBattleDisplay.cpp
6. `TacticalGuiLiveTest::testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo` — source-contract in ICMSelectionGUI.cpp
7. `BattleSimGuiLiveTest::testLocalGameDialogLaunchesPredefinedAndCustomModalChains` — centering assertion
8-9. Two additional unrelated source-contract failures

## Acceptance Criteria Status

| Criterion | Status |
|---|---|
| Minimize button present | PASS — wxTOPLEVEL_EX_DIALOG removed |
| Battle remains modal | PASS — gtk_window_set_modal calls preserved |
| Centering/first-show sizing unchanged | PASS — Centre() and applyLayoutPolicy() unchanged |
| Doxygen headers updated | PASS |

## Commit

`b938d8f` — TMF-02: remove wxTOPLEVEL_EX_DIALOG to restore minimize button on BattleBoard

## Tester Handoff

Test file: `tests/gui/TacticalGuiLiveTest.cpp` / `.h`

Add behavioral coverage:
- Construct `FBattleScreen` and assert `GetExtraStyle()` does NOT include `wxTOPLEVEL_EX_DIALOG`
- Assert `GetWindowStyleFlag() & wxMINIMIZE_BOX` is non-zero (minimize box style preserved in constructor default)
- Source-contract supplement: verify `ShowModal()` source still contains `gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE)`
