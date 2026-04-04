# Milestone 3 Planning Context

## Scope

This artifact captures the repository inspection used to prepare
`plans/milestone3-wxstrategicui-plan.md`.

## Verified findings

- `IStrategicUI` already exists in `include/strategic/IStrategicUI.h`.
- No `WXStrategicUI` implementation exists yet in either `include/gui/` or `src/gui/`.
- `FGame` still directly includes and constructs the strategic dialog classes:
  `SystemDialogGUI`, `UPFUnattachedGUI`, `SatharFleetsGUI`, `ViewFleetGUI`,
  `SatharRetreatGUI`, and `SelectCombatGUI`.
- `FGame` still directly uses `wxMessageBox`, `wxMessageDialog`, and `Refresh()` for
  strategic-layer UI behavior.
- `FGame` still stores `wxWindow * m_parent` and exposes `create(wxWindow*)` and
  `init(wxWindow*)`.
- `FMainFrame` creates `FGame` with `m_drawingPanel` in both new/open flows.
- `src/gui/Makefile` does not yet compile a `WXStrategicUI.o` object.
- `BattleResultsGUI.cpp` uses `FGame::create()` with no argument, so the Milestone 3
  constructor/creation refactor must preserve that call path.

## Planning implication

Milestone 3 should focus on:
- adding a concrete `WXStrategicUI` adapter in the gui module
- injecting `IStrategicUI` into `FGame`
- replacing direct strategic dialog/message-box usage in `FGame` with interface calls
- wiring `FMainFrame` to create and own the adapter
- preserving non-frame and no-argument `FGame::create()` compatibility

## Boundary note

The design document says Milestone 3 removes wx and gui includes from `FGame`, but the
same document defers `draw(wxDC&)`, `onLeftDClick(wxMouseEvent&)`, and `onLeftUp(...)`
extraction to Milestone 4. The Milestone 3 plan therefore treats dialog/message-box
separation as the required scope and leaves rendering/event wx exposure as an explicit
Milestone 4 follow-up unless implementation reveals a safe way to reduce it earlier.
