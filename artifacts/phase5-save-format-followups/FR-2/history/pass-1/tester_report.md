# Tester Report

Status:
- success

Task summary:
- Validated FR-2 (closes deferred item SF-nullfmap-paint-guard): `FMap::hasMap()` is a new public static predicate (`include/strategic/FMap.h` / `src/strategic/FMap.cpp`) that tests `m_map != NULL` without dereferencing it, and `WXMapDisplay::draw()`/`WXMapDisplay::getScale()` (`src/gui/WXMapDisplay.cpp`) now consult it before ever calling `FMap::getMap()`: `draw()` returns immediately (no-op) and `getScale()` returns a safe default of 1.0 when no `FMap` singleton exists, instead of dereferencing `FMap::getMap()`'s null reference and crashing inside `FMap::getMaxSize()` with `this=0x0`. This is defense-in-depth in the gui draw path itself, independent of and in addition to the already-existing, unchanged P5-5 `FMainFrame::onOpen()` setGame-deferral mitigation. Added the one required behavioral acceptance criterion (a forced repaint of a live `FGamePanel`/`WXGameDisplay` whose `FMap` singleton does not exist, proving no crash) to `tests/gui/StrategicGuiLiveTest.{h,cpp}`; all other criteria (Doxygen updates, deferred-tasks.md removal, AGENTS.md update, unchanged rendering behavior with a valid map, unchanged onOpen mitigation) were verified by direct code/doc inspection against the already-committed implementer diff (commit `09cc698a`) plus the existing, still-passing `WXMapDisplayOffscreenRendering`/`WXGameDisplayOffscreenRendersTurnCounterAndIcons`/`MainFrameOnOpenFailedLoad*` tests.

Branch name:
- phase5-FR-2-tester-20260719

Test commit hash:
- 92ae3f38

Test files added or modified:
- tests/gui/StrategicGuiLiveTest.h
- tests/gui/StrategicGuiLiveTest.cpp

Commands run:
- make -C src/strategic (clean build, no warnings)
- make -C src/gui (clean build, no warnings)
- make -C tests/gui (clean build, no warnings)
- xvfb-run -a ./GuiTests (run from tests/gui/) -> OK (84 tests)
- make check (top-level, from worktree root) -> SSWTests OK (290 tests), TacticalTests OK (253 tests), GuiTests OK (84 tests), exit 0

Pass/fail totals:
- SSWTests: 290 run / 0 failures (unchanged from the FR-1 baseline; no strategic-module test files were touched by FR-2)
- TacticalTests: 253 run / 0 failures (unchanged)
- GuiTests: 84 run / 0 failures (83 baseline + 1 new)

Unmet acceptance criteria:
- None

Final test outcomes:
- make check (top-level) exit 0: SSWTests OK (290 tests), TacticalTests OK (253 tests), GuiTests OK (84 tests) -- no regressions vs. the pre-FR-2 baseline (290/253/83); the +1 in GuiTests is the new StrategicGuiLiveTest case.
- New test `testGamePanelRepaintWithNoLiveFMapDoesNotCrash` (tests/gui/StrategicGuiLiveTest.cpp) is fully behavioral: it first defensively tears down any live `FMap` singleton a prior test in the suite may have left behind (`FMap::hasMap()` is process-wide static and CppUnit does not guarantee suite-wide ordering) so the false precondition is deterministic; constructs a live `FGame` via the no-arg `FGame::create()` overload, which has no `FMap` side effect (`m_universe` stays NULL; `FMap::create(...)` is only ever called from `FGame`'s setup/newGame/load flows); installs that game on a shown `FGamePanel` via `setGame(...)` without ever creating an `FMap`; forces a real repaint (`Refresh()`/`Update()`/`pumpEvents(...)`) that drives `FGamePanel::onPaint()`'s `if (m_game != NULL)` guard through to `WXGameDisplay::draw()` -> `WXMapDisplay::draw()`/`getScale()` -- exactly the call chain that, before this fix, dereferenced `FMap::getMap()`'s null reference and crashed the process inside `FMap::getMaxSize()` with `this=0x0`; and asserts `!FMap::hasMap()` is still true after the repaint (i.e. the repaint did not implicitly or accidentally create a map) with reaching that final assertion at all being the primary behavioral proof the process did not crash, matching the established convention of the sibling P5-5 `testMainFrameOnOpenFailedLoadWithFrameShownSurvivesForcedRepaint` test.
- Verified by direct source inspection (`src/gui/WXMapDisplay.cpp`, `include/strategic/FMap.h`, `src/strategic/FMap.cpp`, all already committed by the Implementer at `09cc698a`) that `WXMapDisplay::draw()` and `WXMapDisplay::getScale()` both check `FMap::hasMap()` before their first call to `FMap::getMap()`, and that `FMap::hasMap()` tests the `m_map` pointer itself (not a value obtained by dereferencing it), so it is safe to call unconditionally, including before `FMap` has ever been created.
- Verified rendering is unchanged when a valid `FMap` exists: the pre-existing `testWXMapDisplayOffscreenRendering` and `testWXGameDisplayOffscreenRendersTurnCounterAndIcons` tests (both of which create a real `FMap` via `ensureFrontierMap(...)` before drawing) are unmodified and still pass with identical pixel-region assertions, confirming the new `hasMap()` guard is a no-op on the valid-map path.
- Verified the guard lives in the gui draw path itself and not solely in `FMainFrame::onOpen()`: `src/FMainFrame.cpp` was not touched by the implementer's diff (confirmed via `git show 09cc698a --stat`), and the new test reaches `WXMapDisplay` directly through `FGamePanel`/`WXGameDisplay` without going through `FMainFrame::onOpen()` at all.
- Verified include/strategic/FMap.h's new `hasMap()` Doxygen block (author Claude Sonnet 5 (medium), Created/Last Modified Jul 19, 2026) and the file header's bumped Last Modified date; verified include/gui/WXMapDisplay.h and src/gui/WXMapDisplay.cpp's `draw()`/`getScale()` Doxygen blocks were updated to describe the new guard, with author lists extended to include Claude Sonnet 5 (medium) and Last Modified bumped to Jul 19, 2026 on both the function blocks and the file headers.
- Verified `doc/deferred-tasks.md` no longer contains any `SF-nullfmap-paint-guard` entry (grep returns no matches), consistent with the repository's full-removal precedent for resolved deferred-task entries (matching FR-1's `SF-nested-load-returns` removal).
- Verified `AGENTS.md`'s `StrategicGuiLiveTest` notes paragraph was updated: its closing sentence no longer states the root-cause gui-paint-path guard "remains a tracked follow-up"; it now names `FMap::hasMap()` and the `WXMapDisplay::draw()`/`getScale()` guard as the closing root-cause fix, while the description of the unchanged `onOpen()` mitigation is explicitly preserved verbatim above it.

Cleanup status:
- No temporary or non-handoff byproducts remain in the worktree; `git status` shows only the two test files as changes prior to this artifact commit.
- Rebuilt binaries/object files under src/ and tests/ are pre-existing, gitignored build outputs and are not part of the handoff.
- The new test cleans up its own process-wide static state: it deletes any live `FMap` at the start of the test (deterministic precondition) and never creates one during the test body, so no live `FMap` is left behind for later tests; the `FGame` singleton and the test's own frame/panel are explicitly deleted/destroyed at the end of the test.

Artifacts written:
- artifacts/phase5-save-format-followups/FR-2/tester_report.md
- artifacts/phase5-save-format-followups/FR-2/tester_result.json
- artifacts/phase5-save-format-followups/FR-2/documenter_prompt.txt
