# Milestone 6 Plan: Define `ITacticalUI` and Implement `WXTacticalUI`

## Feature Restatement

Milestone 6 introduces the tactical UI boundary without rewiring the running tactical battle flow. The deliverable is a new pure-virtual `ITacticalUI` interface plus a wx-backed `WXTacticalUI` implementation that wraps the tactical dialogs and redraw requests currently handled directly inside tactical wx classes. This milestone remains additive: `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` stay on the active runtime path until Milestone 7.

## Planner Agent Activation

- Requested agent: `planner`
- Repository-local definition found: no
- Shared definition found: yes
- Active definition path: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition from `~/repos/agents` applies because no repository-local `planner` definition was found

## Confirmed Repository Facts

- [`include/tactical/FTacticalGame.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h) already forward-declares `ITacticalUI` and stores an `ITacticalUI * m_ui;` placeholder added in Milestone 5.
- No tactical UI boundary files exist yet: there is currently no [`include/tactical/ITacticalUI.h`](/home/tstephen/repos/SSW/include/tactical/ITacticalUI.h), no [`include/gui/WXTacticalUI.h`](/home/tstephen/repos/SSW/include/gui/WXTacticalUI.h), and no [`src/gui/WXTacticalUI.cpp`](/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp).
- [`src/tactical/FBattleScreen.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp) still calls the tactical wx dialogs directly:
  - `showTacticalDamageSummaryDialog()` constructs `TacticalDamageSummaryGUI`
  - `fireICM()` constructs `ICMSelectionGUI`
  - `declareWinner()` shows the winner message with `wxMessageBox`
- [`include/gui/TacticalDamageSummaryGUI.h`](/home/tstephen/repos/SSW/include/gui/TacticalDamageSummaryGUI.h) already exposes the modal summary dialog around `FTacticalCombatReportSummary`.
- [`include/gui/ICMSelectionGUI.h`](/home/tstephen/repos/SSW/include/gui/ICMSelectionGUI.h) already exposes the modal ICM assignment dialog taking `std::vector<ICMData *> *`.
- [`src/gui/Makefile`](/home/tstephen/repos/SSW/src/gui/Makefile) already builds `WXStrategicUI.o` but has no `WXTacticalUI.o` entry yet.
- [`include/gui/WXStrategicUI.h`](/home/tstephen/repos/SSW/include/gui/WXStrategicUI.h) and [`src/gui/WXStrategicUI.cpp`](/home/tstephen/repos/SSW/src/gui/WXStrategicUI.cpp) provide the repository’s existing adapter pattern for a wx-backed UI boundary.
- [`tests/tactical/Makefile`](/home/tstephen/repos/SSW/tests/tactical/Makefile) and [`tests/tactical/TacticalTests.cpp`](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp) already host Milestone 5 additive tactical boundary tests, so they are the natural place to add Milestone 6 tactical-boundary verification.

## Assumptions

- Milestones 1-5 are considered complete enough that Milestone 6 should not reopen earlier strategic or tactical extraction work.
- “Purely additive” means no rewiring of `FBattleScreen`, `FBattleBoard`, `FBattleDisplay`, `BattleSim`, or existing tactical entry points in this milestone.
- Because `VehicleList` and `ICMData` are typedef-based tactical argument types declared in [`include/ships/FVehicle.h`](/home/tstephen/repos/SSW/include/ships/FVehicle.h), `ITacticalUI.h` will likely need that non-wx include rather than relying on forward declarations alone.

## Files To Modify

- [`include/tactical/ITacticalUI.h`](/home/tstephen/repos/SSW/include/tactical/ITacticalUI.h)
- [`include/gui/WXTacticalUI.h`](/home/tstephen/repos/SSW/include/gui/WXTacticalUI.h)
- [`src/gui/WXTacticalUI.cpp`](/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp)
- [`src/gui/Makefile`](/home/tstephen/repos/SSW/src/gui/Makefile)
- Likely new tactical tests under [`tests/tactical`](/home/tstephen/repos/SSW/tests/tactical), preferably:
  - [`tests/tactical/FTacticalUIBoundaryTest.h`](/home/tstephen/repos/SSW/tests/tactical/FTacticalUIBoundaryTest.h)
  - [`tests/tactical/FTacticalUIBoundaryTest.cpp`](/home/tstephen/repos/SSW/tests/tactical/FTacticalUIBoundaryTest.cpp)
  - [`tests/tactical/WXTacticalUITest.h`](/home/tstephen/repos/SSW/tests/tactical/WXTacticalUITest.h)
  - [`tests/tactical/WXTacticalUITest.cpp`](/home/tstephen/repos/SSW/tests/tactical/WXTacticalUITest.cpp)
- [`tests/tactical/Makefile`](/home/tstephen/repos/SSW/tests/tactical/Makefile)
- [`tests/tactical/TacticalTests.cpp`](/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp)

## Overall Documentation Impact

- [`plans/ui-model-separation-design.md`](/home/tstephen/repos/SSW/plans/ui-model-separation-design.md) already defines the Milestone 6 target architecture and should remain the canonical design reference.
- No end-user documentation changes are expected.
- Internal architecture notes should eventually state that `ITacticalUI` and `WXTacticalUI` now exist as additive tactical UI adapters, while the live wx tactical runtime still routes through `FBattleScreen` until Milestone 7.

## Subtasks

### Subtask 1: Add the pure-virtual tactical UI boundary header

**Description**

Create `include/tactical/ITacticalUI.h` as the tactical equivalent of `IStrategicUI`. The header should expose only the tactical UI operations called out in the Milestone 6 design, use non-wx types only, and keep its include surface as small as possible while still supporting `FTacticalCombatReportSummary`, `VehicleList`, and `ICMData`.

**Acceptance Criteria**

- `include/tactical/ITacticalUI.h` exists and declares `Frontier::ITacticalUI`.
- The interface includes exactly the Milestone 6 tactical callbacks:
  - `requestRedraw()`
  - `showMessage(const std::string& title, const std::string& body)`
  - `showDamageSummary(const FTacticalCombatReportSummary& summary)`
  - `runICMSelection(std::vector<ICMData*>& icmData, VehicleList* defenders)`
  - `notifyWinner(bool attackerWins)`
- The header contains no wx includes and no wx type references.
- The interface compiles cleanly as a standalone tactical boundary header using only the required non-wx dependencies.

**Documentation Impact**

- No direct documentation update expected for this subtask.

### Subtask 2: Implement the wx-backed tactical adapter in `WXTacticalUI`

**Description**

Create `include/gui/WXTacticalUI.h` and `src/gui/WXTacticalUI.cpp` following the existing `WXStrategicUI` pattern. The class should hold a `wxWindow* parent`, implement the new interface, wrap the existing tactical dialogs, and preserve current winner-message semantics without modifying the current `FBattleScreen` runtime path.

**Acceptance Criteria**

- `WXTacticalUI` publicly implements `ITacticalUI`.
- The constructor takes `wxWindow* parent` and stores it for redraws and modal dialog parenting.
- `requestRedraw()` calls `m_parent->Refresh()` when a parent exists and remains null-safe otherwise.
- `showMessage()` uses a wx informational message path consistent with repository conventions.
- `showDamageSummary()` wraps `TacticalDamageSummaryGUI` and returns its modal result.
- `runICMSelection()` wraps `ICMSelectionGUI` and returns its modal result.
- `notifyWinner(bool attackerWins)` reproduces the current winner dialog meaning from `FBattleScreen::declareWinner()`.
- No existing tactical wx class is rewired to call `WXTacticalUI` in this milestone.

**Documentation Impact**

- Internal handoff notes should mention that `WXTacticalUI` is additive infrastructure for Milestone 7, not yet the live tactical control path.

### Subtask 3: Wire the GUI library build for the new adapter

**Description**

Update the GUI module build so `WXTacticalUI.cpp` is compiled into `libgui.a`, and add any dependency lines needed to keep the legacy Makefile dependency section consistent with the rest of the module.

**Acceptance Criteria**

- [`src/gui/Makefile`](/home/tstephen/repos/SSW/src/gui/Makefile) includes `WXTacticalUI.o` in `OBJS`.
- The dependency section includes the new header/source dependencies required to rebuild `WXTacticalUI.o` correctly.
- The GUI static library builds with `WXTacticalUI.o` present.
- The change does not introduce any new tactical-to-gui reverse dependency beyond the intended adapter implementation.

**Documentation Impact**

- No documentation update expected for this subtask.

### Subtask 4: Add additive verification for the new tactical UI boundary

**Description**

Add focused regression coverage that verifies the new interface and adapter exist, remain wx-free at the interface boundary, and are safe to construct in additive mode without a parent window. The tests should follow current repository patterns: interface-boundary assertions can inspect headers or compile them in isolation, while adapter behavior can follow the null-parent safety checks already used for `WXStrategicUI`.

**Acceptance Criteria**

- Tests fail if `ITacticalUI.h` introduces wx includes or deviates from the Milestone 6 method surface.
- Tests verify `WXTacticalUI` can be constructed with `NULL` parent and that guarded modal methods return a non-zero status when no UI context is available.
- Tests verify `requestRedraw()` remains null-safe in additive mode.
- Tactical test build wiring includes the new tests and the tactical module runner executes them.
- Existing tactical runtime classes remain unchanged and tactical board behavior is still driven by the old direct-calling code after the tests land.

**Documentation Impact**

- No documentation update expected unless the team keeps a milestone-specific test inventory.

## Dependency Ordering

1. Subtask 1 must land first because it defines the interface contract that `WXTacticalUI` implements.
2. Subtask 2 depends on Subtask 1 because the adapter must compile against the new interface.
3. Subtask 3 depends on Subtask 2 because build wiring should target the settled adapter file names and include dependencies.
4. Subtask 4 depends on Subtasks 1-3 for final integration, though test skeletons can be prepared earlier once the interface signatures are fixed.

## Parallelization Notes

- Subtasks 2 and 4 can overlap after the `ITacticalUI` method signatures are locked.
- Subtask 3 is small, but it should finalize only after the adapter file names and include list are stable.

## Implementer Agent Prompts

### Prompt 1

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/ITacticalUI.h`

Task:
- Create the new tactical UI abstraction header for Milestone 6.
- Define the pure-virtual `Frontier::ITacticalUI` interface exactly for the additive tactical callbacks described in the design.
- Keep the interface free of wx types and wx includes.
- Use only the non-wx repository types needed for `FTacticalCombatReportSummary`, `VehicleList`, and `ICMData`.

Acceptance criteria:
- `ITacticalUI.h` exists and declares the full Milestone 6 interface surface.
- The header contains no wx includes or wx type references.
- The interface is suitable for use by `FTacticalGame` without pulling in GUI headers.

### Prompt 2

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/gui/WXTacticalUI.h`
- `/home/tstephen/repos/SSW/src/gui/WXTacticalUI.cpp`

Task:
- Implement the wx-backed tactical UI adapter for Milestone 6.
- Follow the existing `WXStrategicUI` style and constructor pattern.
- Wrap `TacticalDamageSummaryGUI`, `ICMSelectionGUI`, redraw requests, general message display, and the winner notification dialog semantics currently found in `FBattleScreen`.
- Keep the milestone additive: do not modify `FBattleScreen`, `FBattleBoard`, `FBattleDisplay`, or `FTacticalGame` wiring.

Acceptance criteria:
- `WXTacticalUI` implements `ITacticalUI`.
- Modal tactical dialogs are wrapped through the new adapter.
- Null-parent behavior is guarded safely.
- No runtime tactical caller is rewired yet.

### Prompt 3

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/gui/Makefile`

Task:
- Update the GUI library build for the new tactical adapter.
- Add `WXTacticalUI.o` to the GUI object list and update the dependency section consistently with the existing Makefile style.

Acceptance criteria:
- `WXTacticalUI.o` is built into `libgui.a`.
- The Makefile includes the new dependency lines needed for reliable rebuilds.
- No unrelated build wiring changes are introduced.

### Prompt 4

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalUIBoundaryTest.h`
- `/home/tstephen/repos/SSW/tests/tactical/FTacticalUIBoundaryTest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/WXTacticalUITest.h`
- `/home/tstephen/repos/SSW/tests/tactical/WXTacticalUITest.cpp`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/TacticalTests.cpp`

Task:
- Add additive regression coverage for the Milestone 6 tactical UI boundary.
- Verify the interface surface and non-wx include boundary of `ITacticalUI.h`.
- Add a focused adapter test for `WXTacticalUI` similar in spirit to `WXStrategicUITest`, especially around null-parent safety and guarded modal methods.
- Wire the new tactical tests into the tactical test runner.

Acceptance criteria:
- Tactical tests fail if `ITacticalUI.h` reintroduces wx includes or loses required methods.
- `WXTacticalUI` null-parent safety is exercised by the test suite.
- The tactical module test runner builds and includes the new tests.
- The tests do not require Milestone 7 delegation work to exist.

## Output Artifact Path

- Plan file: `/home/tstephen/repos/SSW/plans/milestone6-itacticalui-wxtacticalui-plan.md`
- Supporting artifacts directory: `/home/tstephen/repos/SSW/artifacts/gui_sep/milestone6-itacticalui-wxtacticalui`
