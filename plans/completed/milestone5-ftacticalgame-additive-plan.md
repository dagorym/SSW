# Milestone 5 Plan: Create `FTacticalGame` (Additive)

## Feature Restatement

Milestone 5 introduces a new pure-C++ tactical model class, `FTacticalGame`, that gathers tactical mechanics state currently spread across `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`. This milestone is additive: the existing wx tactical flow remains the runtime path, while the new model compiles cleanly and is populated with the state and mechanics needed for later delegation work in Milestones 6-8.

## Planner Agent Activation

- Requested agent: `Planner`
- Repository-local definition found: no
- Shared definition found: yes
- Active definition path: `/home/tstephen/repos/agents/agents/planner.md`
- Precedence decision: shared `planner` definition from `~/repos/agents` applies because no repository-local `planner` definition was found

## Confirmed Repository Facts

- [`include/tactical/FBattleScreen.h`](/home/tstephen/repos/SSW/include/tactical/FBattleScreen.h) currently owns tactical state such as `m_phase`, `m_activePlayer`, `m_movingPlayer`, `m_curShip`, `m_curWeapon`, attacker/defender ship lists, movement-complete state, `m_ICMData`, and `FTacticalCombatReport`.
- [`src/tactical/FBattleScreen.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleScreen.cpp) contains setup and phase-transition logic including `setupFleets()`, `setState()`, `setPhase()`, `applyFireDamage()`, `fireICM()`, and winner declaration flow.
- [`include/tactical/FBattleBoard.h`](/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h) defines the tactical hex-map state types `hexData` and `turnData`, and stores `m_hexData[100][100]`, `m_turnInfo`, mined-hex data, movement highlight lists, and gravity-turn state on the wx board class.
- [`src/tactical/FBattleBoard.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleBoard.cpp) implements movement validation, path computation, gravity-turn logic, mine checks, weapon-range computation, and final movement application against `m_hexData` and `m_turnInfo`.
- [`include/tactical/FBattleDisplay.h`](/home/tstephen/repos/SSW/include/tactical/FBattleDisplay.h) still declares `fireAllWeapons()` on the display class.
- [`src/tactical/FBattleDisplay.cpp`](/home/tstephen/repos/SSW/src/tactical/FBattleDisplay.cpp) currently runs combat-phase mechanics in `fireAllWeapons()`, including building `FTacticalCombatReportContext`, calling weapon `fire()`, appending attack reports, showing the damage summary, and clearing destroyed ships.
- [`include/tactical/FTacticalCombatReport.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalCombatReport.h) already provides the tactical reporting types `FTacticalCombatReport`, `FTacticalCombatReportContext`, `FTacticalAttackReport`, and `FTacticalCombatReportSummary` that `FTacticalGame` should reuse rather than duplicate.
- [`src/tactical/Makefile`](/home/tstephen/repos/SSW/src/tactical/Makefile) currently builds `libtactical.a` from `FBattleBoard.o`, `FBattleScreen.o`, and `FBattleDisplay.o`; `FTacticalGame.cpp` is not yet part of the tactical library.

## Assumptions

- Milestone 5 should not change the active runtime path for tactical battles; `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` remain intact and functional.
- `ITacticalUI` does not exist until Milestone 6, so `FTacticalGame` should use a forward-declared placeholder pointer member without introducing wx includes.
- Because Milestone 5 is additive, some tactical type definitions may temporarily exist in both legacy wx classes and the new model until Milestone 7/8 remove the legacy copies.
- Unit tests are not explicitly required by the design for this milestone; compile integration is the minimum verification target unless the implementer can add an isolated additive test without increasing migration risk.

## Files To Modify

- [`include/tactical/FTacticalGame.h`](/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h)
- [`src/tactical/FTacticalGame.cpp`](/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp)
- [`src/tactical/Makefile`](/home/tstephen/repos/SSW/src/tactical/Makefile)
- Likely [`tests/tactical/Makefile`](/home/tstephen/repos/SSW/tests/tactical/Makefile) only if an additive compile-only or construction smoke test is introduced
- Likely new test file under [`tests/tactical`](/home/tstephen/repos/SSW/tests/tactical) only if the team decides Milestone 5 should enforce `FTacticalGame` construction before Milestone 6 wiring

## Overall Documentation Impact

- `plans/ui-model-separation-design.md` already defines the milestone target and should remain the source design document.
- No end-user documentation changes are expected.
- Internal architecture notes should eventually record that tactical mechanics are moving out of wx classes into `FTacticalGame`, but that note can wait until the class is wired in during Milestones 6-8.

## Subtasks

### Subtask 1: Define the additive tactical model surface in `FTacticalGame.h`

**Description**

Create the new pure-C++ `FTacticalGame` header and declare the tactical data it will own. The header should gather the mechanics-side state currently stored in `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`, while staying free of wx types. It should also define or relocate the tactical state structs needed for later migration, including the hex-map and movement-record types that currently live in `FBattleBoard.h`.

**Acceptance Criteria**

- `include/tactical/FTacticalGame.h` declares a non-wx `Frontier::FTacticalGame` class.
- The header does not include wx headers and does not inherit from a wx base class.
- `FTacticalGame` contains members covering the tactical mechanics categories called out in the design: battle state/phase, active and moving player flags, selected ship/weapon, fleet and ship lists, movement-complete state, ICM data, tactical report state, winner/end-of-combat bookkeeping, and hex-map movement state.
- The hex-map state types needed by `FTacticalGame` are available from the new header without depending on `FBattleBoard`.
- The header forward-declares `ITacticalUI` and uses `ITacticalUI * m_ui` or an equivalent placeholder pointer without requiring the interface definition to exist yet.

**Documentation Impact**

- No documentation update expected for this subtask.

### Subtask 2: Implement the additive mechanics container in `FTacticalGame.cpp`

**Description**

Implement constructors, reset/setup helpers, state accessors, and mechanics methods that move Milestone 5 logic into the new class without changing current callers. The focus is not full delegation yet; the goal is to make `FTacticalGame` a coherent owner of the tactical mechanics model so later milestones can forward `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay` behavior into it.

**Acceptance Criteria**

- `src/tactical/FTacticalGame.cpp` exists and compiles into the tactical library.
- The implementation initializes all tactical state to safe defaults equivalent to the legacy wx class defaults where applicable.
- `FTacticalGame` provides additive mechanics methods for the major migration targets called out in the design, including battle setup/state transitions, movement-state reset/finalization helpers, tactical report lifecycle, `fireAllWeapons()` ownership, and end-of-combat detection/winner resolution support.
- The implementation reuses existing tactical report types from `FTacticalCombatReport.h` instead of introducing duplicate reporting structures.
- The implementation remains self-contained and does not require modifying `FBattleScreen`, `FBattleBoard`, or `FBattleDisplay` yet.

**Documentation Impact**

- No documentation update expected for this subtask.

### Subtask 3: Isolate legacy tactical type dependencies so later delegation is straightforward

**Description**

Make the new model usable by later milestones by ensuring its data model does not depend on the wx class headers that currently own tactical state. This includes deciding which legacy types are duplicated versus relocated in Milestone 5 and capturing the compatibility boundary clearly enough that Milestones 7 and 8 can remove legacy state without redesign.

**Acceptance Criteria**

- `FTacticalGame` can be included without including `FBattleScreen.h`, `FBattleBoard.h`, or `FBattleDisplay.h`.
- Any state types imported from legacy tactical headers are either moved into `FTacticalGame.h` or redefined there in a way that does not break current wx classes.
- The implementer documents, in code comments where needed, which `FTacticalGame` members correspond to legacy owners in `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`.
- No existing tactical wx class loses state or behavior in Milestone 5.

**Documentation Impact**

- Internal handoff notes should mention any temporary duplication of tactical state structs so Milestones 7 and 8 remove the right legacy copies.

### Subtask 4: Wire the build and add minimal verification for the additive class

**Description**

Update the tactical build so `FTacticalGame` is compiled as part of `libtactical.a`, and add only the smallest verification needed for an additive milestone. The goal is to catch integration mistakes early without forcing delegation work into Milestone 5.

**Acceptance Criteria**

- [`src/tactical/Makefile`](/home/tstephen/repos/SSW/src/tactical/Makefile) includes `FTacticalGame.o` in `libtactical.a`.
- A tactical module build succeeds with the new source file present.
- If a new test is added, it is additive and limited to construction/default-state or compile inclusion of `FTacticalGame`; it must not require Milestone 6 UI abstractions.
- Existing tactical runtime behavior is unchanged because no current battle entry point is rewired to use `FTacticalGame` yet.

**Documentation Impact**

- No documentation update expected unless a new tactical test requires a note in test inventory.

## Dependency Ordering

1. Subtask 1 must land first because it defines the public model surface and type ownership for the milestone.
2. Subtask 2 depends on Subtask 1 because the implementation needs the finalized member layout and public API.
3. Subtask 3 can begin once Subtask 1 has defined the new types, but it should finish before Subtask 4 so the build is wired against the correct isolated header surface.
4. Subtask 4 lands last because the makefile and any optional smoke coverage should validate the settled class shape, not a moving target.

## Parallelization Notes

- Subtasks 2 and 3 can overlap after the header shape is agreed, as long as one owner controls the public `FTacticalGame.h` write path.
- Subtask 4 can be prepared in parallel, but final build wiring should wait until the class API and type-isolation decisions are stable.

## Implementer Agent Prompts

### Prompt 1

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`

Task:
- Create the additive `FTacticalGame` header for Milestone 5.
- Define a pure-C++ tactical model class that gathers the mechanics state now spread across `FBattleScreen`, `FBattleBoard`, and `FBattleDisplay`.
- Keep the header free of wx includes and wx inheritance.
- Define or relocate the tactical hex-map and movement state types that the new class needs so later milestones do not depend on `FBattleBoard.h` for model data.
- Include a forward declaration for `ITacticalUI` and a placeholder `m_ui` pointer without introducing the interface implementation yet.

Acceptance criteria:
- `FTacticalGame.h` declares a compilable, non-wx tactical model class.
- The header covers battle state, movement state, selection state, report state, ICM state, and hex-map mechanics state.
- The header can be included without including any wx header.
- The new types needed for tactical model ownership are available from the new header.

### Prompt 2

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`

Task:
- Implement the additive `FTacticalGame` mechanics container for Milestone 5.
- Add constructors, reset/setup helpers, tactical report lifecycle support, battle-state helpers, movement-state helpers, and the combat-phase ownership currently represented by `FBattleDisplay::fireAllWeapons()`.
- Reuse `FTacticalCombatReport` and related tactical reporting types already in the repository.
- Keep the implementation additive only; do not rewire `FBattleScreen`, `FBattleBoard`, or `FBattleDisplay` to call this class yet.

Acceptance criteria:
- `FTacticalGame.cpp` compiles cleanly.
- The class initializes legacy-equivalent default state safely.
- The new class owns additive methods covering setup, phase/state transitions, reporting, movement bookkeeping, and winner/end-of-combat support.
- No existing tactical wx class is modified to delegate to `FTacticalGame` in this milestone.

### Prompt 3

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/include/tactical/FTacticalGame.h`
- `/home/tstephen/repos/SSW/src/tactical/FTacticalGame.cpp`
- `/home/tstephen/repos/SSW/include/tactical/FBattleBoard.h`

Task:
- Isolate the new tactical model from legacy wx tactical headers.
- Ensure `FTacticalGame` does not require `FBattleBoard.h`, `FBattleScreen.h`, or `FBattleDisplay.h` to expose its model types.
- If tactical state structs must be duplicated or relocated for Milestone 5, make the minimum safe change that preserves current runtime behavior while preparing Milestones 7 and 8.
- Leave the legacy wx tactical classes operational and unchanged in behavior.

Acceptance criteria:
- `FTacticalGame.h` is self-sufficient for tactical model types.
- No wx tactical header becomes a required dependency to include `FTacticalGame`.
- Legacy tactical classes still compile and retain their current state definitions/behavior for the active runtime path.
- Any temporary duplication or relocation is explicit and maintainable for later cleanup.

### Prompt 4

You are the implementer agent

Allowed files:
- `/home/tstephen/repos/SSW/src/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical/Makefile`
- `/home/tstephen/repos/SSW/tests/tactical`

Task:
- Wire `FTacticalGame.cpp` into the tactical module build for Milestone 5.
- Add only minimal additive verification if needed, such as a compile/construction smoke test that does not depend on `ITacticalUI` or later delegation work.
- Keep the milestone additive and avoid changes that alter the current tactical runtime path.

Acceptance criteria:
- `FTacticalGame.o` is built into `libtactical.a`.
- The tactical module builds successfully with the new class added.
- Any new test is narrowly scoped to additive existence/default-state verification.
- Existing tactical flow remains untouched at runtime.

## Output Artifact Path

- Plan file: `/home/tstephen/repos/SSW/plans/milestone5-ftacticalgame-additive-plan.md`
- Supporting artifacts directory: `/home/tstephen/repos/SSW/artifacts/gui_sep/milestone5-ftacticalgame-plan`
