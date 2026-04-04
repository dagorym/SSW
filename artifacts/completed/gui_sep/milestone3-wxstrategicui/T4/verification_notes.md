# T4 Verification Notes — Milestone 3 WXStrategicUI

## Subtask
- Identifier: T4
- Worktree: `/home/tstephen/worktrees/SSW/gui_sep-t4-implementer-20260328`
- Branch: `gui_sep-t4-implementer-20260328`

## Commands Run
1. `cd /home/tstephen/worktrees/SSW/gui_sep-t4-implementer-20260328/src/gui && make`
2. `cd /home/tstephen/worktrees/SSW/gui_sep-t4-implementer-20260328/src/strategic && make`
3. `rg '#include\\s+"gui/(SystemDialogGUI|UPFUnattachedGUI|SatharFleetsGUI|ViewFleetGUI|SatharRetreatGUI|SelectCombatGUI)\\.h"' src/strategic/FGame.cpp`
4. `rg '#include\\s+"gui/.*"' src/strategic/FGame.cpp`
5. `rg 'm_ui->' src/strategic/FGame.cpp`
6. `rg 'wx[A-Za-z0-9_]*' include/strategic/FGame.h src/strategic/FGame.cpp`

## Outcomes
- `src/gui` build: **PASS** (`libgui.a` built successfully).
- `src/strategic` build: **PASS** (`libstrategic.a` built successfully).
- Build warning observed in `FJumpRoute.cpp` (`-Wint-to-pointer-cast`), pre-existing and unrelated to Milestone 3/T4 scope.

## Strategic Dialog Decoupling Check (`FGame`)
- Confirmed no concrete strategic dialog header includes remain in `src/strategic/FGame.cpp`:
  - `SystemDialogGUI.h`
  - `UPFUnattachedGUI.h`
  - `SatharFleetsGUI.h`
  - `ViewFleetGUI.h`
  - `SatharRetreatGUI.h`
  - `SelectCombatGUI.h`
- Confirmed strategic dialog/notification flows are routed through `IStrategicUI` calls (`m_ui->...`) for setup, dialogs, notifications, retreat conditions, and combat selection.

## Residual wx Coupling Boundary (Deferred to Milestone 4)
Remaining wx usage in `FGame` is concentrated in rendering/event-window integration paths and compatibility shims, including:
- Header/API wx types: `wxDC`, `wxMouseEvent`, `wxWindow` (`draw(wxDC&)`, `onLeftDClick(wxMouseEvent&)`, `onLeftUp(wxMouseEvent&)`, compatibility signatures `create(wxWindow*)` and `init(wxWindow*)`).
- Source rendering/event usage: `wxClientDC`, `wxCoord`, `wxColour`, brushes/pens/fonts, and direct drawing methods.
- GUI display helpers used for map/player rendering: `WXMapDisplay`, `WXPlayerDisplay`, `WXIconCache` (and include of `WXGameDisplay.h`).
- Transitional global strategic window pointer: `g_strategicWindow`.

This matches the intended Milestone 3 boundary: strategic dialog decoupling is complete while rendering/event extraction remains for Milestone 4.

## Follow-on Items Identified
1. Milestone 4 should remove remaining wx type exposure from `FGame` public API and isolate draw/event handling behind a dedicated rendering/input boundary.
2. Milestone 4 should retire `create(wxWindow*)`, `init(wxWindow*)`, and `g_strategicWindow` compatibility path once rendering/event extraction is complete.
3. Optional cleanup after milestone work: remove now-unneeded `WXGameDisplay.h` include from `FGame.cpp` if confirmed unused.
