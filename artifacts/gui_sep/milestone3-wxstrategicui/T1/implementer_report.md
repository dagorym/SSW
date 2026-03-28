Subtask: T1
Branch: gui_sep-t1-implementer-20260328
Implementation commit: 6160ac7

Summary
- Added concrete `Frontier::WXStrategicUI` implementing `Frontier::IStrategicUI`.
- Added bridge comments in header/source stating this class bridges strategic logic to wx dialogs.
- Kept wx/dialog includes in gui layer (`include/gui/WXStrategicUI.h`, `src/gui/WXStrategicUI.cpp`).
- Implemented wrappers for:
  - generic message display
  - failed jump notification
  - victory notification
  - retreat-condition selection
  - UPF unattached setup
  - Sathar fleet setup
  - system dialog
  - fleet dialog
  - retreat-condition display
  - combat selection
  - redraw request
- Made `requestRedraw()` null-safe.

Files changed
- include/gui/WXStrategicUI.h
- src/gui/WXStrategicUI.cpp

Validation
- Command: `cd /home/tstephen/worktrees/SSW/gui_sep-t1-implementer-20260328/src/gui && make`
- Result: PASS

Notes for Tester
- No new tests were added (per instructions).
- Validate gui module build and inspect integration surface expectations for strategic/ui boundary.
