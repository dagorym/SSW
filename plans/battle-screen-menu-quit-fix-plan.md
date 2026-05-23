# Plan: battle-screen-menu-quit-fix

## Output Artifact Path

`plans/battle-screen-menu-quit-fix-plan.md`

---

## Feature Summary

Fix two regressions in `FBattleScreen` close-path handling:

1. **File → Quit menu item does not close the window** (and test windows do not
   close automatically when tests are run on a real display).
2. **Failing tactical test**: `FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults`
   fails because it asserts `m_closeInProgress = false;` inside `FTacticalGame::reset()`,
   but that field was moved to `FBattleScreen` in a prior fix.

---

## Root Cause Analysis

### Issue 1 — File → Quit: synchronous delete-this inside GTK menu dispatch

`FBattleScreen::onMenuQuit` calls `Close(true)`, which in wxWidgets processes
`wxEVT_CLOSE_WINDOW` **synchronously** (via `HandleWindowEvent`).  The synchronous
close chain calls `closeBattleScreen` → `Hide()` → `Destroy()`.

In wxWidgets 3.3.2, `wxWindowBase::Destroy()` calls `delete this` immediately
(confirmed in `src/common/wincmn.cpp` lines 559-573 of the local wx source tree).
After `delete this` the object and all its children are gone, but the call stack
still holds live references to:

- `this` in `closeBattleScreen` after `Destroy()` returns:
  `if (!IsBeingDeleted())` — **use-after-free**
- `this` in `Close()`'s `HandleWindowEvent` return path
- `win` (the `FBattleScreen*`) in `wxMenuBase::DoProcessEvent::win->HandleWindowEvent(menuEvent)`
- The `menu` pointer passed into `menu->SendEvent(id, -1)` inside the GTK
  `menuitem_activate` callback — `menu` is a child of the deleted frame (dangling pointer)

This undefined behaviour causes the GTK window to remain visually open on a real
display after the C++ object has been destroyed.  The title-bar close works because
the OS delivers `wxEVT_CLOSE_WINDOW` as a **posted** (asynchronous) event, decoupled
from any active GTK callback stack.

**Fix**: Change `onMenuQuit` to **post** the close event instead of processing it
synchronously:

```cpp
void FBattleScreen::onMenuQuit(wxCommandEvent & WXUNUSED(event)) {
    wxCloseEvent closeEvent(wxEVT_CLOSE_WINDOW);
    closeEvent.SetEventObject(this);
    wxPostEvent(this, closeEvent);
}
```

This makes the menu quit path identical to the title-bar close path already proven
by the GUI tests.  The menu event handler returns cleanly; GTK finishes its
`menuitem_activate` callback with all pointers still valid; the posted close event
is dispatched in the next event-loop iteration and calls `onClose` →
`closeBattleScreen` → `Hide()` → `Destroy()` with no active GTK or wx menu stack.

### Issue 2 — Stale source-token assertion in tactical test

`FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults`
(line 113) asserts:

```
assertContains(body, "m_closeInProgress = false;");
```

This checks that `FTacticalGame::reset()` initialises `m_closeInProgress`.
`m_closeInProgress` was migrated from `FTacticalGame` to `FBattleScreen` in a prior
fix; `FTacticalGame::reset()` no longer contains that line.  The assertion is stale
and produces one test failure every run.

---

## Confirmed Repository Facts

| Fact | Source |
|------|--------|
| `wxWindowBase::Destroy()` calls `delete this` synchronously (no deferral on Linux) | local wx source `src/common/wincmn.cpp` lines 559-573 |
| `onMenuQuit` calls `Close(true)` (synchronous) | `src/tactical/FBattleScreen.cpp:768-770` |
| `closeBattleScreen` calls `Destroy()` on the non-modal path | `src/tactical/FBattleScreen.cpp:739-757` |
| `m_closeInProgress` lives only in `FBattleScreen` | `include/tactical/FBattleScreen.h:305` and `src/tactical/FBattleScreen.cpp` |
| `FTacticalGame::reset()` does not contain `m_closeInProgress = false;` | `src/tactical/FTacticalGame.cpp:245-286` |
| Stale test assertion at line 113 | `tests/tactical/FTacticalGameMechanicsTest.cpp:113` |
| GUI tests pass under xvfb (41/41) | `xvfb-run -a ./tests/gui/GuiTests` — verified |
| Tactical test fails (1/157) on `m_closeInProgress` assertion | `./tests/tactical/TacticalTests` — verified |

---

## Subtasks

### BSMQF-001 — Fix `onMenuQuit` to post close event asynchronously

**Problem**: `onMenuQuit` calls `Close(true)` synchronously, which causes
`delete this` while the GTK `menuitem_activate` callback stack is still active.
Use-after-free UB prevents the window from visually closing on a real display.

**Change**: Replace `Close(true)` with a `wxPostEvent` call posting a
`wxEVT_CLOSE_WINDOW` event to `this`.

**Allowed files**:
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleScreen.h`

**Acceptance criteria**:
- `FBattleScreen::onMenuQuit` no longer calls `Close(true)` or any other synchronous close.
- `onMenuQuit` posts a `wxEVT_CLOSE_WINDOW` event via `wxPostEvent(this, ...)`.
- `cd tests/gui && make && xvfb-run -a ./GuiTests` reports `OK (41 tests)`.
- `cd tests && make tactical-tests && ./tactical/TacticalTests` does not introduce
  new failures beyond the pre-existing stale-assertion failure addressed by BSMQF-002.

#### Documentation Impact

Update the Doxygen comment on `onMenuQuit` in `src/tactical/FBattleScreen.cpp`
(and `include/tactical/FBattleScreen.h` if a comment exists there) to describe
the posted-event approach and the reason: avoid synchronous `delete this` inside
the GTK menu callback.  Follow project Doxygen conventions (`@brief`, `@date Last
Modified`, author list appended with current agent model).

---

### BSMQF-002 — Remove stale `m_closeInProgress` assertion from tactical test

**Problem**: `FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults`
asserts `m_closeInProgress = false;` in `FTacticalGame::reset()`.  That field now
lives in `FBattleScreen`; the assertion causes 1 tactical test failure every run.

**Change**: Remove line 113 from `testResetInitializesSafeLegacyCompatibleDefaults`
and add an inline comment explaining that `m_closeInProgress` is owned by `FBattleScreen`.

**Allowed files**:
- `tests/tactical/FTacticalGameMechanicsTest.cpp`

**Acceptance criteria**:
- `assertContains(body, "m_closeInProgress = false;")` no longer appears in
  `testResetInitializesSafeLegacyCompatibleDefaults`.
- A brief inline comment explains that `m_closeInProgress` is owned by `FBattleScreen`,
  not `FTacticalGame`.
- `cd tests && make tactical-tests && ./tactical/TacticalTests` reports `OK` with
  0 failures (157/157 pass).

#### Documentation Impact

No documentation file changes required.  The inline test comment is sufficient.

---

## Dependency Ordering

| Subtask | Depends on | Parallelizable |
|---------|-----------|----------------|
| BSMQF-001 | — | Yes |
| BSMQF-002 | — | Yes |

Both subtasks touch non-overlapping files.  They are safe to execute in parallel,
though sequential execution is recommended for simpler artifact tracking.

---

## Overall Documentation Impact

- `src/tactical/FBattleScreen.cpp` — Doxygen comment update on `onMenuQuit` (BSMQF-001).
- `include/tactical/FBattleScreen.h` — Update `onMenuQuit` comment if present (BSMQF-001).
- `doc/DesignNotes.md` — No change required; the close-lifecycle contract is unchanged.
- `AGENTS.md` — No change required.

---

## Implementer Prompts

### BSMQF-001 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

**Feature**: battle-screen-menu-quit-fix — BSMQF-001: Fix `onMenuQuit` to post close event asynchronously

**Background**:

`FBattleScreen::onMenuQuit` (in `src/tactical/FBattleScreen.cpp`) currently calls
`Close(true)`, which processes `wxEVT_CLOSE_WINDOW` synchronously.  In wxWidgets
3.3.2 (Linux/GTK), `wxWindowBase::Destroy()` calls `delete this` immediately.
When the synchronous chain reaches `Destroy()`, the `FBattleScreen` object and all
its children (including menu objects) are deleted while the GTK `menuitem_activate`
callback stack is still active.  All code that accesses `this` after `Destroy()`
returns — including the `if (!IsBeingDeleted())` guard in `closeBattleScreen` and
the `win->HandleWindowEvent` call in `wxMenuBase::DoProcessEvent` — is use-after-free
undefined behaviour.  On a real display this prevents the window from visually closing.

The title-bar close works because the OS delivers `wxEVT_CLOSE_WINDOW` as a posted
(asynchronous) event, decoupled from any active callback stack.

**Task**:

1. In `src/tactical/FBattleScreen.cpp`, change `FBattleScreen::onMenuQuit` from:
   ```cpp
   void FBattleScreen::onMenuQuit(wxCommandEvent & WXUNUSED(event)) {
       Close(true);
   }
   ```
   to:
   ```cpp
   void FBattleScreen::onMenuQuit(wxCommandEvent & WXUNUSED(event)) {
       wxCloseEvent closeEvent(wxEVT_CLOSE_WINDOW);
       closeEvent.SetEventObject(this);
       wxPostEvent(this, closeEvent);
   }
   ```

2. Update the Doxygen comment for `onMenuQuit` in `src/tactical/FBattleScreen.cpp`
   (and in `include/tactical/FBattleScreen.h` if a comment exists there) to document:
   - the posted-event approach
   - the reason: avoid synchronous `delete this` inside GTK `menuitem_activate` callback
   - follow project conventions (`@brief`, `@date Last Modified`, current agent model in author list)

**Allowed files**:
- `src/tactical/FBattleScreen.cpp`
- `include/tactical/FBattleScreen.h`

**Acceptance criteria**:
- `onMenuQuit` does not call `Close(true)` or any other synchronous close.
- `onMenuQuit` constructs a `wxCloseEvent(wxEVT_CLOSE_WINDOW)`, sets its event
  object to `this`, and calls `wxPostEvent(this, closeEvent)`.
- `cd tests/gui && make && xvfb-run -a ./GuiTests` reports `OK (41 tests)`.
- `cd tests && make tactical-tests && ./tactical/TacticalTests` does not introduce
  new failures beyond the pre-existing stale-assertion failure.

**Tester handoff**: Tests live in `tests/gui/TacticalGuiLiveTest.cpp` (specifically
`testBattleScreenMenuQuitClosesViaSharedClosePath` and
`testBattleScreenTitleBarCloseClosesViaSharedClosePath`) and in
`tests/gui/BattleSimGuiLiveTest.cpp` (`testScenarioDialogMenuQuitUnwindsBattleScreenModalCaller`).
The Tester should confirm both close vectors and the modal BattleSim path still pass.

**Artifact directory**: `artifacts/battle-screen-menu-quit-fix/BSMQF-001/`

**Validation**:
```bash
cd tests/gui && make && xvfb-run -a ./GuiTests
cd tests && make tactical-tests && ./tactical/TacticalTests
```

**Continuation**: Proceed directly to implementation — there are no unresolved
design decisions.

Do not report success unless all required artifacts exist and all changes are committed.

---

### BSMQF-002 Implementer Prompt

Your role is 'implementer'. Your task is as follows:

**Feature**: battle-screen-menu-quit-fix — BSMQF-002: Remove stale `m_closeInProgress` assertion from tactical test

**Background**:

`FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults` in
`tests/tactical/FTacticalGameMechanicsTest.cpp` contains this assertion at line 113:

```cpp
assertContains(body, "m_closeInProgress = false;");
```

This checks that `FTacticalGame::reset()` initialises `m_closeInProgress`.
However, `m_closeInProgress` was moved from `FTacticalGame` to `FBattleScreen` in
a prior fix; `FTacticalGame::reset()` no longer contains that line.  The assertion
causes 1 failure in every tactical test run:

```
1) test: FrontierTests::FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults (F)
   - Expected to find 'm_closeInProgress = false;' in inspected source
```

**Task**:

1. In `tests/tactical/FTacticalGameMechanicsTest.cpp`, remove line 113:
   ```cpp
   assertContains(body, "m_closeInProgress = false;");
   ```
2. Add a brief inline comment in the test body (near the surrounding assertions) explaining
   that `m_closeInProgress` is now owned by `FBattleScreen`, not `FTacticalGame`, and is
   therefore not initialised in `FTacticalGame::reset()`.

**Allowed files**:
- `tests/tactical/FTacticalGameMechanicsTest.cpp`

**Acceptance criteria**:
- `assertContains(body, "m_closeInProgress = false;")` no longer appears in
  `testResetInitializesSafeLegacyCompatibleDefaults`.
- A brief inline comment explains ownership of `m_closeInProgress`.
- `cd tests && make tactical-tests && ./tactical/TacticalTests` reports `OK` with
  0 failures (157/157 pass).

**Tester handoff**: The tactical test suite runner is `tests/tactical/TacticalTests`.
The Tester should run `cd tests && make tactical-tests && ./tactical/TacticalTests`
and confirm 0 failures.

**Artifact directory**: `artifacts/battle-screen-menu-quit-fix/BSMQF-002/`

**Validation**:
```bash
cd tests && make tactical-tests && ./tactical/TacticalTests
```

**Continuation**: Proceed directly to the single-line removal — no design decisions required.

Do not report success unless all required artifacts exist and all changes are committed.
