# Milestone 2 Planning Context

## Scope

This artifact captures the repo inspection used to prepare
`plans/milestone2-istrategicui-plan.md`.

## Verified findings

- `include/strategic/FGame.h` currently forward-declares wx classes but still exposes
  wx-dependent strategic APIs such as `create(wxWindow *)`, `init(wxWindow *)`,
  `draw(wxDC &)`, and mouse-event handlers.
- `src/strategic/FGame.cpp` directly includes GUI dialog headers and calls wx UI APIs
  such as `wxMessageBox` and `wxMessageDialog`.
- No `IStrategicUI` or `WXStrategicUI` class exists in the repo yet.
- `include/gui/WXGameDisplay.h` is still a placeholder and does not implement the
  strategic UI boundary.
- `FleetList` and `PlayerList` are currently typedefs owned by `FFleet.h` and
  `FPlayer.h`, which makes a lightweight `IStrategicUI.h` awkward unless those aliases
  are centralized first.

## Planning implication

Milestone 2 should stay header-only and focus on:
- introducing a canonical strategic forward/types header
- defining `IStrategicUI` without wx types
- avoiding behavior changes in `FGame.cpp`
- leaving the real `FGame` and wx migration for Milestone 3
