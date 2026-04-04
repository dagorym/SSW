# Reviewer Report

## Feature plan reviewed
- `plans/milestone1-wximage-removal-plan.md`

## Review scope
Feature-level review of the `milestone1-wximage-removal` orchestration run on branch `gui_sep`, covering:
- Plan goals, subtasks, dependencies, and acceptance criteria
- Implemented repository state
- Subtask artifacts under `artifacts/milestone1-wximage-removal/`
- Related tester/documenter/verifier outputs where present

## Subtasks and artifacts reviewed

### Artifacts available per subtask
- **T1–T6:** `implementer_result.json`, `tester_prompt.txt` only
- **T7:** full implementer/tester/documenter/verifier artifact set
- **T8:** full implementer/tester/documenter/verifier artifact set
- **T9:** `implementer_result.json` only

## Overall feature completeness
The core milestone is **substantially implemented**. `FVehicle`, `FFleet`, and `FPlayer` no longer store `wxImage` icon state; GUI/tactical icon lookup is centralized in `WXIconCache`; GUI call sites were migrated; tests were updated; T9 reports a full build and `169/169` tests passing.

However, **not all original plan acceptance criteria are fully satisfied**:
1. The milestone's clean-boundary goal for the `strategic` module is only partially achieved
2. Runtime/icon-path behavior is under-verified
3. Some updated tests are weaker than the plan requested
4. The artifact trail is incomplete for T1–T6 and T9

## Acceptance criteria assessment

### Confirmed satisfied
- `FVehicle::getIconName()` and `FFleet::getIconName()` accessors present (T1)
- No `wxImage` member remains in `FVehicle`, `FFleet`, or `FPlayer` (T2/T3/T4)
- `include/gui/GuiTypes.h` exists; `Frontier.h` no longer defines `ImageList` (T5)
- `WXIconCache` exists, is wired into all GUI/tactical call sites, and is in the gui Makefile (T6/T7)
- All `getIcon()`/`m_icon` references removed from tests (T8)
- Both executables (`SSW`, `BattleSim`) link; `169/169` tests pass (T9)
- `AGENTS.md` updated with icon-loading boundary guidance

### Not fully satisfied / not evidenced
- Plan boundary statement — "ships and strategic modules contain no wxWidgets types" — **not fully met**: `include/strategic/FGame.h` still includes `<wx/wx.h>` and exposes `wxWindow*`
- T6 cache behavior is only evidenced by code inspection; no tester/verifier trail for T6
- No smoke-test or runtime display evidence for T7 icon path correctness

## Findings

### BLOCKING
- None.

### WARNING

1. **Strategic-layer wx boundary remains incomplete**
   `include/strategic/FGame.h` · `src/strategic/FPlayer.cpp`
   The plan framed Milestone 1 as establishing a clean boundary where `ships` and `strategic` no longer carry wxWidgets coupling. `FGame.h` still exposes `wxWindow*` in its public API; `FPlayer.cpp` still includes `gui/WXMapDisplay.h`. The delivered work removes the `wxImage` ownership targeted by the subtasks, but the broader strategic-module boundary is incomplete.

2. **`WXIconCache` may have changed icon path resolution semantics**
   `src/gui/WXIconCache.cpp` · `src/ships/FVehicle.cpp` · `src/strategic/FFleet.cpp`
   Prior model-side loading prepended `FGameConfig::getBasePath()` to the icon filename. `WXIconCache::get()` now loads `wxImage(filename)` directly, making icon loading working-directory-sensitive. This was not validated at the milestone level and could fail when the executable is launched from a path other than the repository root.

3. **Migrated setter tests are weaker than the plan requested**
   `tests/ships/FVehicleTest.cpp` · `tests/strategic/FFleetTest.cpp`
   After `setIcon()` calls, tests assert only `!getIconName().empty()` rather than the exact stored filename. These would not catch path rewriting, truncation, or wrong-file storage.

4. **Artifact evidence chain is incomplete for T1–T6 and T9**
   `artifacts/milestone1-wximage-removal/T1`–`T6`, `T9`
   Missing tester/documenter/verifier outputs reduce confidence in acceptance-criteria closure for those subtasks and leave T6/T9 verification dependent on implementer self-report.

### NOTE

1. **Primary implementation is coherent.** Model classes store icon names only; GUI/tactical callers use `WXIconCache` consistently.

2. **`AGENTS.md` documentation update matches the delivered architecture.** Contributor guidance accurately states that model objects keep icon filenames while GUI/tactical paths resolve `wxImage` assets through `WXIconCache`.

3. **T7 tester_report.md is stale** (still reports include-order failure) but `tester_result.json` and `verifier_report.md` correctly reflect the fixed state.

## Missed functionality / edge cases
- Icon loading when the executable is launched from a working directory other than the repository root
- Graceful runtime behavior when icon files are missing or invalid
- Completion of broader strategic/gui decoupling beyond `wxImage` ownership
- Exact stored-filename verification in migrated setter tests

## Follow-up feature requests for the planner

1. Complete the strategic-layer decoupling: remove `wxWindow` usage from `include/strategic/FGame.h` or isolate it behind the GUI layer; remove the `gui/WXMapDisplay.h` dependency from `src/strategic/FPlayer.cpp`.

2. Fix or document `WXIconCache` path resolution: preserve `FGameConfig::getBasePath()` semantics or explicitly document and validate the new contract, with automated verification that icons load correctly from all supported launch contexts.

3. Strengthen setter tests: assert exact stored filenames in `FVehicleTest` and `FFleetTest` after `setIcon()` calls.

4. Backfill missing artifacts for T1–T6 and T9 to provide a complete implementation/testing/documentation/verification audit trail.

## Final verdict: CONDITIONAL PASS

The milestone's primary implementation goal is substantially complete and the build/test suite passes. Full acceptance-criteria closure is not evidenced due to the remaining strategic-layer wx coupling, unverified icon path runtime behavior, weaker-than-planned setter tests, and incomplete downstream artifact coverage for T1–T6 and T9.
