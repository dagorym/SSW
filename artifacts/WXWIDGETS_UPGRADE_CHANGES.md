# wxWidgets 3.0.3 to 3.3.1 Upgrade Changes

This document summarizes all changes made to upgrade the SSW project from wxWidgets 3.0.3 to 3.3.1.

## Summary

All necessary code changes have been completed. The project should now compile against wxWidgets 3.3.1 located at ~/repos/wxWidgets-3.3.1.

## Changes Made

### 1. Makefile Updates (Build Configuration)

**Changed:** All references to `wx-config-3.0` updated to `wx-config`

**Files Modified:**
- `src/Makefile`
- `src/core/Makefile`
- `src/gui/Makefile`
- `src/ships/Makefile`
- `src/strategic/Makefile`
- `src/tactical/Makefile`
- `src/weapons/Makefile`
- `src/defenses/Makefile`

**Note:** Test Makefiles already used `wx-config` without version suffix and required no changes.

### 2. Visual Studio Project Updates

**Changed:** All wxWidgets paths and library names updated for version 3.3.1

**Files Modified:**
- `SSW.vcxproj` - Updated all configurations (Debug/Release, Win32/x64)
- `BattleSim/BattleSim.vcxproj` - Updated all configurations

**Changes Made:**
- Include paths: `wxWidgets-3.0.3` → `wxWidgets-3.3.1`
- Library paths: `wxWidgets-3.0.3\lib\vc_lib` → `wxWidgets-3.3.1\lib\vc_lib`
- Library paths (x64): `wxWidgets-3.0.3\lib\vc_x64_lib` → `wxWidgets-3.3.1\lib\vc_x64_lib`
- Library names: `wxmsw30` → `wxmsw33`, `wxbase30` → `wxbase33`

**Example library changes:**
- Debug: `wxmsw30ud_core.lib` → `wxmsw33ud_core.lib`
- Release: `wxmsw30u_core.lib` → `wxmsw33u_core.lib`

### 3. Deprecated API Removals

#### wxInitAllImageHandlers()
**Status:** Removed (automatically called in wxWidgets 3.1+)

**Files Modified:**
- `src/FApp.cpp` - Removed call and added comment explaining removal
- `src/FBattleSimApp.cpp` - Removed call and added comment explaining removal

### 4. Boolean Literal Updates

**Changed:** `TRUE` → `true`, `FALSE` → `false`

**Files Modified:**
- `src/FApp.cpp`:
  - Line 38: `frame->Show(TRUE)` → `frame->Show(true)`
  - Line 42: `return TRUE` → `return true`

- `src/FMainFrame.cpp`:
  - Line 87: `Close(TRUE)` → `Close(true)`

- `src/FBattleSimApp.cpp`:
  - Line 37: `frame->Show(TRUE)` → `frame->Show(true)`
  - Line 41: `return TRUE` → `return true`

### 5. Constructor Parameter Updates

**Changed:** Modern C++ null pointer and ID constants

**Files Modified:**
- `src/FApp.cpp`:
  - `wxSplashScreen` constructor: `NULL, -1` → `nullptr, wxID_ANY`

- `src/FMainFrame.cpp`:
  - Line 26: `wxFrame((wxFrame *)NULL, -1, ...)` → `wxFrame(nullptr, wxID_ANY, ...)`

- `src/FBattleSimApp.cpp`:
  - `wxSplashScreen` constructor: `NULL, -1` → `nullptr, wxID_ANY`

### 6. String Macro Updates

**Changed:** `_T(...)` → plain string literals (Unicode is default in modern wxWidgets)

**Files Modified:**
- `src/FMainFrame.cpp`:
  - Line 174: Removed `_T()` from error message strings

- `src/strategic/FGame.cpp`:
  - Lines 140-141, 147-148: Removed `_T()` from commented-out wxGetTextFromUser calls
  - Line 183: Removed `_T()` from wxMessageBox title

## Building the Project

### Windows (Visual Studio)

1. **Ensure wxWidgets 3.3.1 is built:**
   - Navigate to `C:\Users\steph26\repos\wxWidgets-3.3.1\build\msw`
   - Open the appropriate solution file (e.g., `wx_vc17.sln` for VS 2022)
   - Build both Debug and Release configurations for Win32 and x64
   - This will create the libraries in:
     - `C:\Users\steph26\repos\wxWidgets-3.3.1\lib\vc_lib` (Win32)
     - `C:\Users\steph26\repos\wxWidgets-3.3.1\lib\vc_x64_lib` (x64)

2. **Build the SSW project:**
   - Open `C:\Users\steph26\repos\SSW\SSW.sln` in Visual Studio
   - Select your desired configuration (Debug/Release) and platform (Win32/x64)
   - Build Solution (F7)

### Linux (Make)

### Setting up wx-config

Before building, ensure that `wx-config` points to your wxWidgets 3.3.1 installation. You can:

1. **Create a symbolic link:**
   ```bash
   cd ~/repos/wxWidgets-3.3.1
   sudo ln -s $(pwd)/wx-config /usr/local/bin/wx-config-3.3
   sudo ln -s /usr/local/bin/wx-config-3.3 /usr/local/bin/wx-config
   ```

2. **Or modify your PATH:**
   ```bash
   export PATH=~/repos/wxWidgets-3.3.1:$PATH
   ```

3. **Or edit each Makefile** to use `wx-config-3.3` if you create that symlink.

### Compiling

```bash
cd ~/repos/SSW
make clean
make all
```

## Testing Checklist

After compilation, test the following functionality:

1. **Application Startup**
   - [ ] Splash screen displays correctly
   - [ ] Main window opens properly

2. **Menu Functionality**
   - [ ] File menu operations (New, Open, Save, Close, Exit)
   - [ ] Show menu options
   - [ ] Turn menu options

3. **String Handling**
   - [ ] All dialog boxes display text correctly
   - [ ] Error messages are properly formatted

4. **Image Loading**
   - [ ] Splash screen image loads
   - [ ] Game icons and graphics load correctly

## Potential Issues

### If Compilation Fails

1. **Check wx-config:**
   ```bash
   wx-config --version  # Should show 3.3.1
   wx-config --cxxflags  # Verify paths are correct
   wx-config --libs      # Verify library paths
   ```

2. **FindItemByPosition() deprecation:**
   If you see warnings or errors about `FindItemByPosition()`, the method exists in 3.3.1 but may behave differently. Consider refactoring to use menu item IDs directly with `FindItem(id)` instead.

3. **NULL vs nullptr:**
   If you encounter any remaining NULL comparisons or assignments, replace them with `nullptr`.

## Additional Notes

- wxGTK layout-sensitive controls should prefer sizer-managed placement over free-positioned creation when they appear during tactical screen setup; `FBattleDisplay` now creates its speed `wxSpinCtrl` and related action buttons with `wxDefaultPosition`, installs them under a root sizer via `SetSizer(...)`, and refreshes layout after showing or hiding the speed-selection path.
- Subtask 1 of the GUI console warning remediation plan validated this tactical layout rule with:
  - `make -s`
  - `cd tests/tactical && make -s && ./TacticalTests`
  - `cd tests/gui && set -o pipefail && make -s && xvfb-run -a ./GuiTests`
- Validation outcome for that remediation pass: `TacticalTests` OK (88 tests), `GuiTests` OK (24 tests), combined 112 passing tests, and no `GtkSpinButton` assertion observed in the covered tactical flow.
- Subtask 2 of the GUI console warning remediation plan validated the wxGTK static-box ownership rule for strategic combat selection: controls placed inside a `wxStaticBoxSizer` must be created as children of that sizer's static box rather than the dialog. `SelectCombatGUI` now creates the combat list boxes with `sbSizer1->GetStaticBox()` and `sbSizer2->GetStaticBox()`, and `StrategicGuiLiveTest` asserts those `Attacking Fleets` and `Defending Fleets and Stations` parents before the attack-launch flow continues.
- Validation outcome for that remediation pass: `cd tests/gui && make && xvfb-run -a ./GuiTests`, `cd tests/gui && xvfb-run -a ./GuiTests`, and `cd tests/gui && xvfb-run -a ./GuiTests 2>&1 | grep -F '("Combat")' || true` all passed for the covered flow, with `GuiTests` reporting `OK (24 tests)` and no `("Combat")` wxStaticBox warning lines observed.
- Subtask 4 of the GUI console warning remediation plan extended the same wxGTK static-box ownership rule to BattleSim's `ScenarioEditorGUI`: the defender and attacker team selectors, ship-type lists, assigned-ship lists, and related controls now use the owning static boxes as parents instead of the dialog.
- `tests/gui/BattleSimGuiLiveTest.cpp` now asserts that `ScenarioEditorGUI` uses static-box-correct parenting through `ScenarioEditorGUITestPeer::usesStaticBoxParenting()` before continuing the existing battle-launch flow, so the live test still verifies `finalizeStartBattle()` and `FBattleScreen` lifecycle behavior on the corrected path.
- Validation outcome for that remediation pass: `cd tests/gui && make && xvfb-run -a ./GuiTests`, `cd tests/gui && xvfb-run -a ./GuiTests`, and `cd tests/gui && out=$(xvfb-run -a ./GuiTests 2>&1); status=$?; printf '%s\n' "$out" | grep -n "wxStaticBoxSizer should be created" | head -n 20; printf '%s\n' "$out" | grep -n "Scenario Editor"; printf '%s\n' "$out" | tail -n 5; exit $status` all passed, with `GuiTests` reporting `OK (24 tests)` and no static-box warning lines referencing `Scenario Editor`.
- The `.svn` directories contain old versions with the previous code and were not modified
- All main source files have been updated; test directories were already compatible
- The changes maintain backward compatibility with existing game save files
- No changes to the game logic or behavior were made—only API updates

## Version Info

- **From:** wxWidgets 3.0.3 at ~/repos/wxWidgets-3.0.3
- **To:** wxWidgets 3.3.1 at ~/repos/wxWidgets-3.3.1
- **Date:** November 10, 2025
