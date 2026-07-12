# Tester Report — P2-6

Status:
- PASS

## Task summary
Validate the P2-6 hardening of the strategic Save/Open file-dialog paths
(reviewer follow-up F4 / roadmap H15). `FMainFrame::onSave()` and `onOpen()` now
check `wxFileDialog::ShowModal()` and treat Cancel/non-OK as a no-op (no
ofstream/ifstream opened, no `FGame` created, no `FGame::save()/load()` called),
and use the dialog's full `GetPath()` instead of the cwd-dependent
`GetFilename()` on a confirmed OK.

## Implementation under test
- `src/FMainFrame.cpp` — `onSave()` / `onOpen()` (fix commit c49823c7)
- `include/FMainFrame.h` — Doxygen for `onSave()` / `onOpen()`

## Tests authored (all in `tests/gui/StrategicGuiLiveTest.{h,cpp}`)
Registered in the existing `StrategicGuiLiveTest` `CPPUNIT_TEST_SUITE` block; the
fixture is already linked into `tests/gui/GuiTests.cpp`'s runner. Each drives the
real `FMainFrame::onSave()`/`onOpen()` flow through the live native
GtkFileChooser modal and asserts observed runtime outcomes (behavioral, not
source-inspection).

| Test | Acceptance criterion | Behavioral observation |
| --- | --- | --- |
| `testMainFrameOnSaveCancelLeavesFilesystemUntouched` | Cancelling Save leaves the filesystem untouched and does not call `FGame::save` | After a Cancel-driven `onSave()`, the chosen absolute target path does not exist (no open/truncate; with no game, `FGame::save()` is unreachable) |
| `testMainFrameOnSaveConfirmWritesToDialogFullPath` | Confirming Save writes to the dialog's full `GetPath()` location | A sentinel-seeded target file at the dialog's `GetPath()` is truncated to 0 bytes by the confirm branch's `std::ofstream(GetPath())` |
| `testMainFrameOnOpenCancelCreatesNoGameAndLeavesFrameConsistent` | Cancelling Open creates no `FGame` and attempts no load; frame stays consistent | After a Cancel-driven `onOpen()`, every game-dependent menu item (Save/Close/Sathar Retreat/End-turn/Add-Sathar) remains disabled — the post-load menu-enable block only runs inside the `wxID_OK` branch |
| `testMainFrameOnOpenConfirmLoadsFromFullPathAndRestoresPostLoadState` | Confirming Open loads from `GetPath()` and restores post-load menu/turn state | A real saved game (built via a mock `IStrategicUI` + `FGame::save()`) is loaded from the dialog's `GetPath()`; the Sathar-turn menu/turn state is restored exactly (Save/Close/Retreat/EndSathar/AddSathar enabled; EndUPF/PlaceNova disabled) |

## Test execution results
- Repo-root `make`: success (exit 0), full build clean including `tests/gui`.
- `cd tests/gui && make && xvfb-run -a ./GuiTests`: **OK (75 tests)** — 71 baseline
  + 4 new. No hangs, crashes, or failures. (Run under `timeout` in the foreground.)
- `cd tests && ./SSWTests`: **OK (217 tests)** — no regressions in the non-GUI suite.

## Negative / red verification (temporary pre-fix revert; not committed)
Reverted `src/FMainFrame.cpp` to the pre-fix version (`37fb564b`), rebuilt, and
re-ran the GUI suite; the fixed source was restored via `git checkout HEAD --`
before any commit.

- `testMainFrameOnOpenCancelCreatesNoGameAndLeavesFrameConsistent` — **RED
  (segfault)** against pre-fix. gdb backtrace:
  `FMainFrame::onOpen (src/FMainFrame.cpp:157)` → `FGame::load` → `FMap::load` →
  `FSystem::load` → `FPlanet::load` → SIGSEGV. The unfixed `onOpen()`
  unconditionally creates an `FGame` and calls `load()` on the empty/garbage
  Cancel stream, corrupting/crashing. The fix makes it green. This is the
  definitive regression-catching test for the primary defect.
- `testMainFrameOnSaveCancelLeavesFilesystemUntouched`,
  `testMainFrameOnSaveConfirmWritesToDialogFullPath`,
  `testMainFrameOnOpenConfirmLoadsFromFullPathAndRestoresPostLoadState` — green
  against **both** pre-fix and post-fix in the harness-observable scenario, and
  therefore serve as post-fix contract/behavioral locks rather than pre/post
  discriminators. This is inherent, not a test weakness:
  - Save Cancel: a cancelled native dialog with no selection returns an empty
    `GetFilename()`, so the pre-fix `std::ofstream("")` also fails to open — no
    file either way.
  - Both Confirm cases: `wxFD_OVERWRITE_PROMPT|wxFD_SAVE|wxFD_CHANGE_DIR` /
    `wxFD_OPEN|wxFD_CHANGE_DIR` cause `ShowModal()` to `chdir` into the chosen
    file's folder on accept, so the pre-fix `GetFilename()` (basename resolved
    against the new cwd) resolves to the same absolute path as the fixed
    `GetPath()`. The behavior coincides on the happy path.

## Test-infrastructure notes (documented inline in the test file)
Driving the real native GtkFileChooser deterministically in a headless run
required several deliberate mechanisms, each commented at its use site:
- `NativeFileDialogResponder` polls GTK's own top-level list for the visible
  `GtkFileChooser` (the native fallback dialog is not in wx's
  `wxTopLevelWindows`), mirroring the existing `SequentialCloseDialogRecorder`.
- Accept modes select a pre-existing file via `gtk_file_chooser_set_filename()`
  and **wait across timer ticks** until `gtk_file_chooser_get_filename()` is
  non-null before responding `GTK_RESPONSE_ACCEPT` — the selection is applied
  asynchronously and responding early is silently swallowed (hang).
- `wxLogNull` guards each test: the GtkFileChooser path enqueues `wxLog`
  messages that `wxLogGui` later flushes inside a **modal** box (via
  `wxModalDialogHook`) when the next modal opens, hanging a later test.
- OpenConfirm tears down the loaded `FGame`/`FMap` singletons via an `onClose()`
  "No" drive (the harness event pump does not run `wxPendingDelete`, so the
  frame's deferred `Destroy()` would otherwise leave the loaded `FMap` installed
  as the singleton and crash the next test's `ensureFrontierMap()`).
- cwd is restored before deferred paints, and `wxInitAllImageHandlers()` is
  called before the first game render so the `WXIconCache` singleton does not
  cache a failed `day.png` load (the harness app's `OnInit()` registers no image
  handlers), which would otherwise break the later
  `testWXGameDisplayOffscreenRendersTurnCounterAndIcons` assertion.

## Files changed by this Tester stage
- `tests/gui/StrategicGuiLiveTest.cpp`
- `tests/gui/StrategicGuiLiveTest.h`

No implementation code was modified. The temporary pre-fix revert of
`src/FMainFrame.cpp` was reverted (restored to HEAD) and never committed.

## Commit
- Test commit: `2961c923258c676e52c9a8faf2a96d8576f12582`

## Cleanup
- Temporary `/tmp/sswp26*` files from earlier crashed diagnostic runs were
  removed. The passing tests clean up their own temp files. No non-handoff
  byproducts remain in the worktree.
