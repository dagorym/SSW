# Milestone 2 — Define `IStrategicUI` Abstract Interface: Implementation Plan

## Problem Statement

Milestone 2 introduces the first explicit strategic-layer UI boundary without changing
runtime behavior. The goal is to add a pure-virtual `IStrategicUI` interface that
captures the dialogs, notifications, and redraw hooks `FGame` currently drives through
wxWidgets, while keeping the codebase buildable and leaving the actual `FGame` migration
for Milestone 3.

---

## Verified Context (Codebase Inspection Results)

The following facts were confirmed by direct file inspection.

### Confirmed current starting point

- `include/strategic/FGame.h` does not yet define or reference `IStrategicUI`.
- `src/strategic/FGame.cpp` directly includes GUI dialog headers:
  `SystemDialogGUI.h`, `UPFUnattachedGUI.h`, `SatharFleetsGUI.h`,
  `ViewFleetGUI.h`, `SatharRetreatGUI.h`, and `SelectCombatGUI.h`.
- `src/strategic/FGame.cpp` directly calls wx UI APIs such as `wxMessageBox` and
  `wxMessageDialog`, matching the design document’s coupling inventory.
- `include/gui/WXGameDisplay.h` exists only as a placeholder and does not overlap with
  the strategic UI interface work.

### Important repo-specific constraint

- `FleetList` is currently defined at the bottom of
  `include/strategic/FFleet.h`.
- `PlayerList` is currently defined at the bottom of
  `include/strategic/FPlayer.h`.
- Because `IStrategicUI` needs `FleetList` and `PlayerList` in its method signatures,
  a minimal interface header cannot rely on forward declarations alone unless those list
  aliases are moved into a shared forward/types header first.

### Likely consequence for Milestone 2 scope

- A new shared forward/types header is likely required so `IStrategicUI.h` can stay light
  and avoid including full strategic model headers.
- Existing headers that currently own or consume `FleetList` / `PlayerList` will likely
  need small include adjustments so the alias definitions have one canonical home.
- No `.cpp` behavior change is required in this milestone if the work stays limited to
  header declarations and alias relocation.

---

## Files to Modify

### New headers
- `include/strategic/IStrategicUI.h`
- `include/strategic/StrategicTypes.h` *(new shared forward/types header; name may be
  adjusted if the implementer finds a repo-consistent alternative, but it should remain
  in `include/strategic/`)*

### Existing headers likely needing include cleanup
- `include/strategic/FFleet.h`
- `include/strategic/FPlayer.h`
- `include/strategic/FSystem.h`
- `include/strategic/FGame.h`

### Documentation / artifacts
- `plans/milestone2-istrategicui-plan.md`
- `artifacts/gui_sep/milestone2-istrategicui/planning-context.md`

---

## Overall Documentation Impact

- No end-user documentation update is expected for Milestone 2 because behavior does not
  change.
- Developer-facing architecture notes should eventually mention that strategic/UI
  interaction is now represented by `IStrategicUI`, but this can reasonably wait until
  Milestone 3 wires the interface into `FGame`.
- If Doxygen is generated between milestones, new interface/type headers may appear in the
  API docs and should use clear class and file comments.

---

## Subtasks

### T1 — Create a shared strategic forward/types header

**Depends on:** nothing  
**Blocks:** T2, T3

**Work:**
- Add a new header under `include/strategic/` to centralize:
  - forward declarations for `FPlayer`, `FFleet`, `FMap`, and `FSystem`
  - `typedef std::vector<FFleet *> FleetList;`
  - `typedef std::vector<FPlayer *> PlayerList;`
- Keep this header lightweight:
  - include `<vector>`
  - include `<string>` only if actually required
  - do not include wx headers
  - do not include heavy model headers
- Use the `Frontier` namespace consistently with the rest of the module.

**Acceptance criteria:**
- A new shared strategic types header exists under `include/strategic/`.
- That header defines `FleetList` and `PlayerList` without including `FFleet.h`,
  `FPlayer.h`, or wx headers.
- The header compiles cleanly when included on its own.
- No runtime or `.cpp` behavior changes are introduced in this subtask.

**Documentation impact:**
- Add brief file/class comments so Doxygen output explains why this header exists.
- No user documentation update expected.

---

### T2 — Add `IStrategicUI.h` as a pure abstract boundary for strategic UI operations

**Depends on:** T1  
**Blocks:** T4

**Work:**
- Create `include/strategic/IStrategicUI.h`.
- Define `class IStrategicUI` in namespace `Frontier` with a virtual destructor and the
  method set described in `plans/ui-model-separation-design.md`:
  - informational dialogs
  - setup dialogs
  - in-game dialogs
  - combat selection
  - redraw request
- Match the design signatures closely, using pointers and list aliases rather than
  concrete GUI types.
- Keep the header free of wx includes and free of GUI includes.
- Include only the new shared strategic types header and any standard headers required by
  the interface (for example `<string>`).

**Acceptance criteria:**
- `include/strategic/IStrategicUI.h` exists and declares a pure abstract
  `Frontier::IStrategicUI`.
- Every method in the Milestone 2 design section is present with pointer/list-based
  signatures and no wxWidgets types.
- The header includes no GUI headers and no wx headers.
- The header can be included by a translation unit without pulling in `wx/wx.h`.
- No implementation class or `.cpp` file is added in this milestone.

**Documentation impact:**
- Add succinct API comments describing the intent of the interface and any non-obvious
  return-code conventions.
- No user documentation update expected.

---

### T3 — Re-home `FleetList` / `PlayerList` ownership and update dependent headers

**Depends on:** T1  
**Blocks:** T4

**Work:**
- Update `include/strategic/FFleet.h` and `include/strategic/FPlayer.h` so they consume
  the shared strategic types header instead of owning duplicate typedefs locally.
- Update `include/strategic/FSystem.h` and `include/strategic/FGame.h` to include the
  shared types header if they still reference `FleetList` / `PlayerList` after the alias
  move.
- Preserve existing public APIs and behavior; this is only header dependency cleanup.
- Avoid broad include churn outside strategic headers unless compilation proves a specific
  include is required.

**Acceptance criteria:**
- `FleetList` and `PlayerList` have one canonical definition in the new shared strategic
  types header.
- `FFleet.h`, `FPlayer.h`, `FSystem.h`, and `FGame.h` still compile with the relocated
  aliases.
- No duplicate typedef definitions remain in the strategic headers.
- No method signatures or semantics change in existing classes.

**Documentation impact:**
- No separate documentation update expected beyond comments in the new shared header.
- This alias relocation may later be referenced in architecture notes once Milestone 3
  uses the new interface.

---

### T4 — Verify the header boundary is compile-safe and ready for Milestone 3 wiring

**Depends on:** T2, T3  
**Blocks:** none

**Work:**
- Build the strategic module headers through normal compilation to confirm the new
  interface/types headers do not introduce circular include failures.
- Prefer a targeted verification pass:
  - `make` in `src/strategic/`
  - if needed, `make` in `tests/strategic/`
- Verify that adding `IStrategicUI.h` does not require any `.cpp` implementation changes.
- Capture any residual boundary gaps that should be deferred to Milestone 3 rather than
  patched prematurely in this milestone.

**Acceptance criteria:**
- The strategic module builds successfully after the new headers are added.
- If strategic tests are built, they compile without new include-cycle or type-resolution
  failures.
- `FGame.cpp` remains behaviorally unchanged and still owns the direct wx/UI calls at the
  end of this milestone.
- Any newly discovered Milestone 3 follow-up items are recorded in the implementation
  handoff notes rather than folded into Milestone 2 scope.

**Documentation impact:**
- No product documentation update expected.
- Add a short note to milestone artifacts if verification reveals follow-on work for
  Milestone 3.

---

## Dependency Ordering

```text
T1 (shared strategic types header)
 ├─> T2 (define IStrategicUI)
 └─> T3 (re-home list aliases in existing headers)

T2 + T3
 └─> T4 (compile-safe verification)
```

Parallelism:
- T2 and T3 can run in parallel after T1 lands.
- T4 should run after both header-definition tasks are complete.

---

## Implementer Agent Prompts

### T1 Prompt

You are the implementer agent.

Allowed files:
- `include/strategic/StrategicTypes.h`

Task:
- Add a lightweight shared strategic forward/types header that forward-declares the
  strategic classes needed by Milestone 2 and defines canonical `FleetList` and
  `PlayerList` aliases without including wx or the full model headers.

Acceptance criteria:
- The new header lives under `include/strategic/`.
- It defines `FleetList` and `PlayerList` using forward-declared `FFleet` and `FPlayer`
  pointer types.
- It includes only the minimal standard headers required.
- It introduces no runtime behavior changes.

### T2 Prompt

You are the implementer agent.

Allowed files:
- `include/strategic/IStrategicUI.h`
- `include/strategic/StrategicTypes.h`

Task:
- Add `IStrategicUI.h` as a pure abstract interface for the strategic UI boundary using
  the method set defined in the UI/model separation design and the shared list/type
  aliases from the new strategic types header.

Acceptance criteria:
- `IStrategicUI.h` declares a pure abstract `Frontier::IStrategicUI`.
- The interface uses no wx types and includes no GUI headers.
- The method set matches the Milestone 2 design section closely, including redraw,
  setup, dialog, and combat-selection hooks.
- No `.cpp` implementation file is added.

### T3 Prompt

You are the implementer agent.

Allowed files:
- `include/strategic/StrategicTypes.h`
- `include/strategic/FFleet.h`
- `include/strategic/FPlayer.h`
- `include/strategic/FSystem.h`
- `include/strategic/FGame.h`

Task:
- Move canonical ownership of `FleetList` and `PlayerList` into the shared strategic
  types header, then update the existing strategic headers that reference those aliases
  so they compile cleanly without duplicate typedef definitions.

Acceptance criteria:
- `FleetList` and `PlayerList` are defined in one canonical location.
- The affected strategic headers include the new shared header as needed.
- Existing class APIs and behavior remain unchanged.
- No duplicate typedef definitions remain.

### T4 Prompt

You are the implementer agent.

Allowed files:
- `include/strategic/IStrategicUI.h`
- `include/strategic/StrategicTypes.h`
- `include/strategic/FFleet.h`
- `include/strategic/FPlayer.h`
- `include/strategic/FSystem.h`
- `include/strategic/FGame.h`
- `artifacts/gui_sep/milestone2-istrategicui/`

Task:
- Verify that the Milestone 2 header-only boundary work compiles cleanly and record any
  Milestone 3 follow-up issues in the milestone artifact directory without expanding
  Milestone 2 into implementation work.

Acceptance criteria:
- The strategic module compiles successfully after the header updates.
- Any compile or include-cycle issue discovered during verification is either fixed within
  the allowed header scope or recorded as a Milestone 3 follow-up.
- `FGame.cpp` behavior remains unchanged at the end of this milestone.
- Verification notes are written under `artifacts/gui_sep/milestone2-istrategicui/`.

---

## Output Artifacts

- Plan markdown file:
  `plans/milestone2-istrategicui-plan.md`
- Artifact directory:
  `artifacts/gui_sep/milestone2-istrategicui/`
- Supporting planning context:
  `artifacts/gui_sep/milestone2-istrategicui/planning-context.md`
