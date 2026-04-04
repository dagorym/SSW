# Milestone 4 Plan: Extract Rendering and Event Handling from `FGame`

## Feature Restatement

Milestone 4 removes strategic rendering and wx mouse-event handling from `FGame` so the strategic model no longer owns `wxDC`, pixel-space hit testing, or direct event objects. Rendering moves into GUI display helpers, and `FMainFrame`/`FGamePanel` become responsible for translating wx input into model-safe commands.

## Confirmed Repository Facts

- [`include/strategic/FGame.h`](/home/tstephen/repos/SSW/include/strategic/FGame.h) still declares `draw()`, `draw(wxDC&)`, `drawTurnCounter(wxDC&)`, `onLeftDClick(wxMouseEvent&)`, and `onLeftUp(wxMouseEvent&)`.
- [`src/strategic/FGame.cpp`](/home/tstephen/repos/SSW/src/strategic/FGame.cpp) still contains the strategic rendering implementation, turn-counter drawing, and screen-to-map coordinate conversion.
- [`src/FGamePanel.cpp`](/home/tstephen/repos/SSW/src/FGamePanel.cpp) paints by calling `m_game->draw(dc)`.
- [`src/FMainFrame.cpp`](/home/tstephen/repos/SSW/src/FMainFrame.cpp) delegates left-double-click and left-up events directly to `FGame`.
- [`include/gui/WXGameDisplay.h`](/home/tstephen/repos/SSW/include/gui/WXGameDisplay.h) and [`src/gui/WXGameDisplay.cpp`](/home/tstephen/repos/SSW/src/gui/WXGameDisplay.cpp) exist but are effectively stubs.
- [`include/gui/WXMapDisplay.h`](/home/tstephen/repos/SSW/include/gui/WXMapDisplay.h) already exposes `draw(wxDC&)` and `getScale(wxDC&)`.
- [`include/gui/WXPlayerDisplay.h`](/home/tstephen/repos/SSW/include/gui/WXPlayerDisplay.h) already exposes `drawFleets(wxDC&, FPlayer*)`.
- [`tests/strategic/FGameHeaderDependencyTest.cpp`](/home/tstephen/repos/SSW/tests/strategic/FGameHeaderDependencyTest.cpp) already verifies milestone 3 boundary work and is the natural place to extend boundary assertions for milestone 4.

## Files To Modify

- [`include/strategic/FGame.h`](/home/tstephen/repos/SSW/include/strategic/FGame.h)
- [`src/strategic/FGame.cpp`](/home/tstephen/repos/SSW/src/strategic/FGame.cpp)
- [`include/gui/WXGameDisplay.h`](/home/tstephen/repos/SSW/include/gui/WXGameDisplay.h)
- [`src/gui/WXGameDisplay.cpp`](/home/tstephen/repos/SSW/src/gui/WXGameDisplay.cpp)
- [`include/FGamePanel.h`](/home/tstephen/repos/SSW/include/FGamePanel.h)
- [`src/FGamePanel.cpp`](/home/tstephen/repos/SSW/src/FGamePanel.cpp)
- [`include/FMainFrame.h`](/home/tstephen/repos/SSW/include/FMainFrame.h)
- [`src/FMainFrame.cpp`](/home/tstephen/repos/SSW/src/FMainFrame.cpp)
- [`tests/strategic/FGameHeaderDependencyTest.cpp`](/home/tstephen/repos/SSW/tests/strategic/FGameHeaderDependencyTest.cpp)
- Likely [`tests/SSWTests.cpp`](/home/tstephen/repos/SSW/tests/SSWTests.cpp) only if a new strategic test class is added instead of extending an existing one.

## Overall Documentation Impact

- `plans/ui-model-separation-design.md` already captures the target architecture, so no design rewrite should be necessary.
- If the project keeps architecture notes outside milestone plans, the strategic UI boundary documentation should be updated later to state that `FGame` accepts logical map coordinates only and that `WXGameDisplay` owns turn-counter rendering.
- No end-user documentation changes are expected for this milestone.

## Subtasks

### Subtask 1: Move strategic rendering from `FGame` into `WXGameDisplay`

**Description**

Extend `WXGameDisplay` so it becomes the GUI-side composition point for strategic drawing. It should own the former `drawTurnCounter()` logic and orchestrate map rendering plus fleet rendering by calling the existing `WXMapDisplay` and `WXPlayerDisplay` helpers. `FGamePanel::onPaint()` should then render through `WXGameDisplay`, not through `FGame`.

**Acceptance Criteria**

- `WXGameDisplay` exposes a `draw(wxDC&, Frontier::FGame&)` style API that renders the map, fleet icons, and turn counter.
- Turn-counter rendering no longer lives in `FGame`.
- `FGamePanel::onPaint()` no longer calls `m_game->draw(dc)`.
- Strategic rendering still uses existing `WXMapDisplay` and `WXPlayerDisplay` rather than duplicating their behavior.
- The turn counter still renders the day and tenday markers using GUI-side image loading or cache access.

**Documentation Impact**

- No direct doc update required, but the handoff should note that `WXGameDisplay` is now the strategic display entry point.

### Subtask 2: Replace `FGame` wx event handlers with model-safe commands and getters

**Description**

Remove wx-specific input handling APIs from `FGame` and replace them with command-style methods that operate on logical coordinates or turn-processing intent. Add any minimal read-only getters needed by the new display layer, such as round, current player ID, and player list access.

**Acceptance Criteria**

- `FGame` no longer declares or defines `draw()`, `draw(wxDC&)`, `drawTurnCounter(wxDC&)`, `onLeftDClick(wxMouseEvent&)`, or `onLeftUp(wxMouseEvent&)`.
- `FGame` exposes model-safe replacements for strategic interactions, such as `handleMapClick(double mapX, double mapY)` and an end-turn command path such as `processEndTurn()` or equivalent.
- `FGame` exposes the minimal getters needed by `WXGameDisplay` without leaking GUI types.
- `FGame.cpp` no longer depends on `WXGameDisplay`, `WXMapDisplay`, `WXPlayerDisplay`, `wxDC`, or `wxMouseEvent`.
- Strategic behavior remains unchanged: double-clicking a system or fleet still opens the same UI dialogs, and end-turn processing still returns enough state to update menus correctly.

**Documentation Impact**

- Architecture notes should eventually document the coordinate boundary: GUI converts screen space to map space before invoking `FGame`.

### Subtask 3: Move screen-to-map and end-turn hit testing into `FMainFrame`/`FGamePanel`

**Description**

Update the wx event path so the GUI layer owns pixel-space math. `FMainFrame::onLeftDClick()` should compute map-space coordinates using `WXMapDisplay::getScale()` and then call the new `FGame` command. `FMainFrame::onLeftUp()` should own the end-turn button hit testing and invoke the new end-turn processing method. Any window refreshes should continue to route through the established UI path.

**Acceptance Criteria**

- `FMainFrame::onLeftDClick()` no longer forwards the raw wx event to `FGame`.
- Pixel-space to map-space conversion now occurs in GUI code using `WXMapDisplay::getScale()`.
- `FMainFrame::onLeftUp()` no longer forwards the raw wx event to `FGame`.
- End-turn button hit testing is performed in the GUI layer, and the resulting game command preserves the existing menu-enable behavior for UPF and Sathar turns.
- The GUI still refreshes correctly after dialog-driven interaction and after ending turns.

**Documentation Impact**

- No user-facing docs expected.
- Internal notes should record that GUI code owns all screen-coordinate math for the strategic map.

### Subtask 4: Update regression tests for the new boundary

**Description**

Extend or add tests that verify the strategic header and source no longer expose rendering or wx event handling in `FGame`, and that the new GUI-side display class is the expected owner of rendering responsibilities.

**Acceptance Criteria**

- Strategic tests fail if `FGame.h` reintroduces `draw`, `drawTurnCounter`, `onLeftDClick`, `onLeftUp`, or direct wx type usage for rendering/input APIs.
- Strategic tests verify that `FGame.cpp` no longer references `WXMapDisplay`, `WXPlayerDisplay`, or `WXGameDisplay` for rendering.
- Tests or source assertions verify that `WXGameDisplay` now contains a real draw entry point.
- The relevant strategic test target builds and passes.

**Documentation Impact**

- No documentation update expected.

## Dependency Ordering

1. Subtask 2 must begin first because the new `FGame` API defines the model-side boundary the GUI layer will call.
2. Subtask 1 depends partly on Subtask 2 because `WXGameDisplay` needs the new `FGame` getters.
3. Subtask 3 depends on Subtask 2 and Subtask 1 because the frame/panel event and paint paths must call the new model and display interfaces.
4. Subtask 4 should land after the code shape is settled, though test scaffolding can start in parallel once the intended API names are fixed.

## Parallelization Notes

- Subtask 1 and the test-planning portion of Subtask 4 can proceed in parallel after the replacement `FGame` API is agreed.
- Subtask 3 should not finalize until Subtask 2 settles the new command signatures.

## Implementer Agent Prompts

### Prompt 1

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/gui/WXGameDisplay.h`
- `/home/tstephen/repos/SSW/src/gui/WXGameDisplay.cpp`
- `/home/tstephen/repos/SSW/include/FGamePanel.h`
- `/home/tstephen/repos/SSW/src/FGamePanel.cpp`
- `/home/tstephen/repos/SSW/include/strategic/FGame.h`

Task:
- Implement the GUI-side strategic drawing entry point for Milestone 4.
- Extend `WXGameDisplay` with a real `draw(wxDC&, FGame&)` API.
- Move the strategic turn-counter rendering out of `FGame` and into `WXGameDisplay`.
- Update `FGamePanel::onPaint()` so it renders through `WXGameDisplay` instead of `FGame::draw(dc)`.
- Use the minimal `FGame` getters needed for rendering and avoid putting any rendering logic back into `FGame`.

Acceptance criteria:
- `WXGameDisplay` draws the map, fleets, and turn counter.
- `FGamePanel` paints through `WXGameDisplay`.
- No rendering methods remain declared on `FGame` for strategic display.
- Rendering behavior remains visually equivalent to the pre-refactor path.

### Prompt 2

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/strategic/FGame.h`
- `/home/tstephen/repos/SSW/src/strategic/FGame.cpp`

Task:
- Refactor `FGame` so it no longer exposes or implements wx-specific strategic rendering and input event APIs.
- Remove the `draw`, `draw(wxDC&)`, `drawTurnCounter`, `onLeftDClick`, and `onLeftUp` APIs.
- Add model-safe replacements for strategic map interaction and end-turn processing.
- Add minimal read-only getters needed by the GUI display layer.
- Ensure the logic for system/fleet selection and end-turn state transitions remains intact after the API shift.

Acceptance criteria:
- `FGame` no longer owns `wxDC` or `wxMouseEvent`-based strategic methods.
- `FGame.cpp` no longer performs screen-to-map conversion or end-turn hit testing.
- Replacement command methods preserve strategic behavior.
- New getters expose only model data needed by the GUI layer.

### Prompt 3

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/FMainFrame.h`
- `/home/tstephen/repos/SSW/src/FMainFrame.cpp`
- `/home/tstephen/repos/SSW/include/FGamePanel.h`
- `/home/tstephen/repos/SSW/src/FGamePanel.cpp`
- `/home/tstephen/repos/SSW/include/gui/WXMapDisplay.h`
- `/home/tstephen/repos/SSW/src/gui/WXMapDisplay.cpp`

Task:
- Move strategic screen-coordinate handling fully into the GUI layer for Milestone 4.
- Update `FMainFrame::onLeftDClick()` to convert from screen coordinates to logical map coordinates using `WXMapDisplay::getScale()`, then call the new `FGame` interaction method.
- Update `FMainFrame::onLeftUp()` to own end-turn button hit testing and invoke the new end-turn command path.
- Preserve existing menu-state updates and refresh behavior.

Acceptance criteria:
- `FMainFrame` no longer passes raw wx mouse events into `FGame`.
- GUI-side coordinate conversion uses `WXMapDisplay::getScale()`.
- End-turn button behavior remains unchanged from the user’s perspective.
- Strategic interaction still opens the same dialogs and refreshes correctly.

### Prompt 4

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/strategic/FGameHeaderDependencyTest.cpp`
- `/home/tstephen/repos/SSW/tests/strategic/FGameHeaderDependencyTest.h`
- `/home/tstephen/repos/SSW/tests/strategic/Makefile`
- `/home/tstephen/repos/SSW/tests/SSWTests.cpp`

Task:
- Update the strategic regression tests to enforce the Milestone 4 boundary.
- Extend existing dependency tests or add a focused strategic test so the test suite catches reintroduction of rendering methods or wx mouse-event APIs on `FGame`.
- Add assertions that the GUI display layer, not `FGame`, owns the strategic draw entry point.

Acceptance criteria:
- Tests fail if `FGame` reintroduces strategic draw methods or wx mouse-event handlers.
- Tests verify that `FGame.cpp` no longer references GUI display helpers for painting.
- The updated strategic test target builds and passes.

## Output Artifact Path

- Plan file: `/home/tstephen/repos/SSW/plans/milestone4-fgame-rendering-extraction-plan.md`
- Supporting artifacts directory: `/home/tstephen/repos/SSW/artifacts/gui_sep/milestone4_plan`
