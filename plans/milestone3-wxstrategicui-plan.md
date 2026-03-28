# Milestone 3 — Implement `WXStrategicUI` and Wire to `FGame`: Implementation Plan

## Problem Statement

Milestone 3 is the first behavioral step in the strategic-layer UI separation. The
goal is to introduce a concrete `WXStrategicUI` adapter, route `FGame`'s strategic
dialogs and notifications through `IStrategicUI`, and remove direct strategic-to-GUI
dialog coupling from `FGame`.

This milestone should preserve current gameplay behavior while making the strategic
module depend on a pure interface instead of concrete wx dialog classes.

---

## Verified Context (Codebase Inspection Results)

The following facts were confirmed by direct file inspection.

### Confirmed current starting point

- [`include/strategic/IStrategicUI.h`](/home/tstephen/repos/SSW/include/strategic/IStrategicUI.h) already exists and defines the Milestone 2 interface.
- [`include/gui/WXStrategicUI.h`](/home/tstephen/repos/SSW/include/gui/WXStrategicUI.h) and [`src/gui/WXStrategicUI.cpp`](/home/tstephen/repos/SSW/src/gui/WXStrategicUI.cpp) do not exist yet.
- [`src/strategic/FGame.cpp`](/home/tstephen/repos/SSW/src/strategic/FGame.cpp) still directly includes:
  `SystemDialogGUI.h`, `UPFUnattachedGUI.h`, `SatharFleetsGUI.h`,
  `ViewFleetGUI.h`, `SatharRetreatGUI.h`, `SelectCombatGUI.h`,
  `WXGameDisplay.h`, `WXMapDisplay.h`, `WXPlayerDisplay.h`, and wx headers.
- [`src/strategic/FGame.cpp`](/home/tstephen/repos/SSW/src/strategic/FGame.cpp) still directly calls `wxMessageBox`, `wxMessageDialog`, dialog `ShowModal()`, and `m_parent->Refresh()` for strategic flows.
- [`include/strategic/FGame.h`](/home/tstephen/repos/SSW/include/strategic/FGame.h) still exposes `create(wxWindow*)`, `init(wxWindow*)`, stores `wxWindow* m_parent`, and forward-declares `wxWindow`, `wxDC`, and `wxMouseEvent`.
- [`src/FMainFrame.cpp`](/home/tstephen/repos/SSW/src/FMainFrame.cpp) currently constructs the game with `FGame::create(m_drawingPanel)` in both `onNew()` and `onOpen()`.
- [`src/gui/Makefile`](/home/tstephen/repos/SSW/src/gui/Makefile) does not yet list `WXStrategicUI.o`.
- [`src/gui/BattleResultsGUI.cpp`](/home/tstephen/repos/SSW/src/gui/BattleResultsGUI.cpp) calls `FGame::create()` with no arguments, so any `create()` signature change must preserve a no-argument call path.

### Strategic dialog / notification hotspots already identified in `FGame`

- Startup and setup:
  - `init()`
  - `addUPFUnattached()`
  - `addSatharShips()`
- Player/system/fleet dialogs:
  - `showPlayers()`
  - `onLeftDClick()`
  - `onAddSatharShips()`
  - `showRetreatConditions()`
- Runtime notifications:
  - `moveFleets()` failed-jump dialog
  - `checkForVictory()` victory/draw dialogs
- Combat selection:
  - `resolveCombat()`

### Important milestone-boundary tension

- The design document says Milestone 3 should remove gui headers and wx includes from
  `FGame`, but [`include/strategic/FGame.h`](/home/tstephen/repos/SSW/include/strategic/FGame.h) still declares rendering and event APIs (`draw(wxDC&)`, `onLeftDClick(wxMouseEvent&)`, `onLeftUp(wxMouseEvent&)`) that inherently reference wx types.
- The same design document defers rendering/event extraction to Milestone 4.
- Practical interpretation for Milestone 3:
  remove direct dialog and message-box coupling from `FGame`, introduce `IStrategicUI`
  ownership there, and defer the remaining rendering/event-related wx type exposure until
  Milestone 4.

---

## Files to Modify

### New GUI adapter files
- `include/gui/WXStrategicUI.h`
- `src/gui/WXStrategicUI.cpp`

### Strategic logic files
- `include/strategic/FGame.h`
- `src/strategic/FGame.cpp`

### GUI entry / integration files
- `include/FMainFrame.h`
- `src/FMainFrame.cpp`
- `src/gui/Makefile`

### Verification targets
- `src/strategic/Makefile` *(likely no functional change required, but verify no new gui link dependency is introduced there)*
- `src/gui/BattleResultsGUI.cpp` *(verify compatibility with `FGame::create()` after the signature change; edit only if compatibility cannot be preserved in `FGame`)*

### Planning artifacts
- `plans/milestone3-wxstrategicui-plan.md`
- `artifacts/gui_sep/milestone3-wxstrategicui/planning-context.md`
- `artifacts/gui_sep/milestone3-wxstrategicui/reporter_report.md`

---

## Overall Documentation Impact

- No end-user documentation update is expected because Milestone 3 is an internal
  architecture refactor.
- Developer-facing architecture notes should eventually mention that strategic game
  logic now interacts with UI through `IStrategicUI` and `WXStrategicUI`.
- If implementation leaves rendering/event wx references in `FGame` until Milestone 4,
  that milestone boundary should be documented in developer notes or milestone artifacts
  so the partial separation is explicit.

---

## Subtasks

### T1 — Add `WXStrategicUI` as the concrete wx adapter for strategic UI operations

**Depends on:** nothing  
**Blocks:** T2, T3

**Work:**
- Add [`include/gui/WXStrategicUI.h`](/home/tstephen/repos/SSW/include/gui/WXStrategicUI.h) declaring
  `class WXStrategicUI : public Frontier::IStrategicUI`.
- Add [`src/gui/WXStrategicUI.cpp`](/home/tstephen/repos/SSW/src/gui/WXStrategicUI.cpp) implementing each
  interface method with the current wx/dialog behavior that lives in `FGame`.
- Constructor should accept and store a `wxWindow* parent`.
- Keep all wx includes and concrete dialog includes confined to the gui module.
- Implement wrappers for:
  - generic informational messages
  - failed jump notification
  - victory/draw notification
  - retreat-condition selection
  - UPF unattached setup
  - Sathar fleet setup
  - system dialog
  - fleet dialog
  - retreat-condition display
  - combat selection
  - redraw requests
- Make `requestRedraw()` null-safe so non-GUI paths do not crash if a null parent is ever used.

**Acceptance criteria:**
- `WXStrategicUI` exists in the gui module and fully implements `IStrategicUI`.
- All wx dialog construction for strategic flows is isolated to `WXStrategicUI.cpp`.
- `WXStrategicUI.h` includes only the minimal wx/interface headers required.
- `requestRedraw()` refreshes the strategic parent window when available.
- No strategic-module source file includes the concrete dialog headers added in this subtask.

**Documentation impact:**
- Add brief class/file comments stating that `WXStrategicUI` is the concrete bridge from
  strategic game logic to wx dialogs.
- No end-user documentation update expected.

---

### T2 — Refactor `FGame` to depend on `IStrategicUI` instead of concrete wx dialogs

**Depends on:** T1  
**Blocks:** T3, T4

**Work:**
- Update [`include/strategic/FGame.h`](/home/tstephen/repos/SSW/include/strategic/FGame.h) and
  [`src/strategic/FGame.cpp`](/home/tstephen/repos/SSW/src/strategic/FGame.cpp) so `FGame` stores an
  `IStrategicUI* m_ui` instead of `wxWindow* m_parent` for strategic interactions.
- Change construction/creation so the strategic UI adapter is injected:
  - replace `FGame(wxWindow* win)` with an `IStrategicUI*`-based constructor
  - replace `create(wxWindow* win = NULL)` with an overload or defaulted interface-based
    form that still preserves existing no-argument `create()` callers
- Update `init()` to stop using a raw window parameter for redraw and instead request redraw
  through the UI interface. If the legacy `init(wxWindow*)` signature must be retained for
  call-site stability, keep it as a compatibility shim and move the actual redraw path to
  `m_ui->requestRedraw()`.
- Replace direct dialog/message-box code in these methods with `m_ui` calls:
  - `init()`
  - `showPlayers()`
  - `addUPFUnattached()`
  - `addSatharShips()`
  - `moveFleets()`
  - `onLeftDClick()`
  - `onAddSatharShips()`
  - `showRetreatConditions()`
  - `checkForVictory()`
  - `resolveCombat()`
- Remove concrete GUI dialog includes from `FGame.cpp`.
- Remove `m_parent` from `FGame`.
- Preserve `m_gui` as a convenience boolean derived from whether `m_ui` is non-null.
- Keep serialization, combat logic, and existing non-UI game mechanics unchanged.

**Acceptance criteria:**
- `FGame` no longer includes `SystemDialogGUI.h`, `UPFUnattachedGUI.h`,
  `SatharFleetsGUI.h`, `ViewFleetGUI.h`, `SatharRetreatGUI.h`, or `SelectCombatGUI.h`.
- All strategic dialog and notification behavior in `FGame` is routed through `IStrategicUI`.
- `FGame::create()` still supports existing no-argument call sites used elsewhere in the repo.
- `m_parent` is removed from `FGame`.
- `m_gui` behavior remains correct for GUI and non-GUI flows.
- No gameplay rules, combat outcomes, or fleet/state mutations change as part of this refactor.

**Documentation impact:**
- Update class comments if needed so `FGame` is described as using a strategic UI interface
  rather than owning dialog construction directly.
- No user documentation update expected.

---

### T3 — Wire `FMainFrame` to own the strategic adapter and register it in the gui build

**Depends on:** T1, T2  
**Blocks:** T4

**Work:**
- Update [`include/FMainFrame.h`](/home/tstephen/repos/SSW/include/FMainFrame.h) and
  [`src/FMainFrame.cpp`](/home/tstephen/repos/SSW/src/FMainFrame.cpp) so the frame constructs and owns
  a `WXStrategicUI` instance tied to the drawing panel or frame window.
- Pass that adapter into `FGame::create(...)` from both `onNew()` and `onOpen()`.
- Ensure lifetime management is explicit and safe when the frame resets or closes a game.
- Update [`src/gui/Makefile`](/home/tstephen/repos/SSW/src/gui/Makefile) to compile and archive
  `WXStrategicUI.o`.
- Verify that legacy call sites such as [`src/gui/BattleResultsGUI.cpp`](/home/tstephen/repos/SSW/src/gui/BattleResultsGUI.cpp) still compile without needing to know about `WXStrategicUI`.

**Acceptance criteria:**
- `FMainFrame` creates a `WXStrategicUI` and passes it to `FGame::create(...)`.
- Both new-game and open-game flows use the same injected strategic UI path.
- `src/gui/Makefile` includes `WXStrategicUI.o`.
- Existing non-frame `FGame::create()` callers continue to compile.
- No ownership leak or double-delete is introduced for the strategic UI adapter.

**Documentation impact:**
- No product documentation update expected.
- Inline comments are acceptable if they clarify adapter ownership/lifetime.

---

### T4 — Verify buildability and record deferred Milestone 4 boundary work

**Depends on:** T2, T3  
**Blocks:** none

**Work:**
- Run targeted builds after the refactor:
  - `make` in `src/gui/`
  - `make` in `src/strategic/`
  - optionally `make` in `tests/strategic/` if the first two succeed and the build surface
    is still uncertain
- Confirm that strategic logic no longer depends on concrete GUI dialog headers.
- Confirm that any remaining wx references in `FGame` are limited to the rendering/event
  APIs deferred to Milestone 4.
- Record any newly discovered follow-on items, especially if complete wx header removal
  from `FGame` proves impossible before the rendering/event extraction milestone.

**Acceptance criteria:**
- The gui and strategic modules build successfully after the refactor.
- `FGame` compiles without concrete strategic dialog header dependencies.
- Remaining `FGame` wx coupling, if any, is explicitly limited to rendering/event paths
  that belong to Milestone 4.
- Any deferred issues are documented in milestone artifacts rather than silently ignored.

**Documentation impact:**
- Add a short milestone artifact note if Milestone 3 leaves intentional residual wx
  exposure in `FGame` for Milestone 4.
- No user documentation update expected.

---

## Dependency Ordering

```text
T1 (add WXStrategicUI)
  └─> T2 (refactor FGame to use IStrategicUI)
        └─> T3 (wire FMainFrame and gui build integration)
              └─> T4 (verification and follow-up capture)
```

Parallelism:
- T1 should land first because it defines the concrete adapter `FGame` will consume.
- T2 is the central refactor and should complete before frame wiring is finalized.
- T3 and T4 should remain sequential because T4 needs the full integration in place.

---

## Implementer Agent Prompts

### T1 Prompt

You are the implementer agent.

Allowed files:
- `include/gui/WXStrategicUI.h`
- `src/gui/WXStrategicUI.cpp`

Task:
- Add a concrete `WXStrategicUI` implementation of `Frontier::IStrategicUI` in the gui
  module. Move the strategic dialog/message-box behavior currently embedded in `FGame`
  into this adapter while keeping all wx and dialog includes confined to the gui layer.

Acceptance criteria:
- `WXStrategicUI` fully implements `IStrategicUI`.
- The implementation wraps the current strategic dialogs and notifications used by `FGame`.
- `requestRedraw()` refreshes the parent window safely.
- No strategic source file needs concrete dialog headers from this subtask.

### T2 Prompt

You are the implementer agent.

Allowed files:
- `include/strategic/FGame.h`
- `src/strategic/FGame.cpp`
- `include/strategic/IStrategicUI.h`

Task:
- Refactor `FGame` so strategic UI behavior is driven through `IStrategicUI* m_ui`
  instead of direct wx dialog construction. Preserve existing gameplay behavior and keep
  compatibility with existing no-argument `FGame::create()` callers.

Acceptance criteria:
- `FGame` stores an `IStrategicUI*` for strategic UI interactions.
- Direct strategic dialog/message-box code in `FGame` is replaced by interface calls.
- `m_parent` is removed from `FGame`.
- `FGame::create()` remains usable from existing no-argument call sites.
- No gameplay logic changes are introduced.

### T3 Prompt

You are the implementer agent.

Allowed files:
- `include/FMainFrame.h`
- `src/FMainFrame.cpp`
- `src/gui/Makefile`
- `src/gui/BattleResultsGUI.cpp` *(only if needed for build compatibility)*

Task:
- Update the main frame to construct and own a `WXStrategicUI`, pass it into
  `FGame::create(...)` for both new and open flows, and register the new gui adapter in
  the gui static-library build.

Acceptance criteria:
- `FMainFrame` owns a `WXStrategicUI` instance with safe lifetime management.
- Both new and open flows inject that adapter into `FGame`.
- `src/gui/Makefile` builds `WXStrategicUI.o`.
- Existing `FGame::create()` call sites still compile after the change.

### T4 Prompt

You are the implementer agent.

Allowed files:
- `artifacts/gui_sep/milestone3-wxstrategicui/verification_notes.md`
- build outputs only

Task:
- Run targeted builds for the gui and strategic modules after the refactor, verify that
  strategic dialog coupling has been removed from `FGame`, and record any remaining
  Milestone 4 follow-up items in a verification artifact.

Acceptance criteria:
- `src/gui` and `src/strategic` build successfully, or any failure is captured precisely.
- The verification artifact clearly states whether `FGame` still contains only the
  expected rendering/event wx exposure.
- Any deferred Milestone 4 issues are documented explicitly.

---

## Output Artifact Paths

- Plan markdown: [`plans/milestone3-wxstrategicui-plan.md`](/home/tstephen/repos/SSW/plans/milestone3-wxstrategicui-plan.md)
- Planning context: [`artifacts/gui_sep/milestone3-wxstrategicui/planning-context.md`](/home/tstephen/repos/SSW/artifacts/gui_sep/milestone3-wxstrategicui/planning-context.md)
- Reporter output: [`artifacts/gui_sep/milestone3-wxstrategicui/reporter_report.md`](/home/tstephen/repos/SSW/artifacts/gui_sep/milestone3-wxstrategicui/reporter_report.md)
