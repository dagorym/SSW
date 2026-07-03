# Battle-Screen File→Quit `EndModal` Assert Fix — Implementation Plan

Feature slug: `battle-quit-endmodal-fix`
Plan artifact: `plans/battle-quit-endmodal-fix-plan.md`
Coordination base branch: dedicated per-plan branch off `fixes` (never `master`/`main`).
Specialist Security stage: **not required** for the subtask (no permissions,
secrets, untrusted input, external integrations, or destructive operations).

## Feature Summary

Root-cause and fix a **pre-existing** runtime defect: in the tactical battle
screen (`FBattleScreen`), invoking **File → Quit** throws a wxGTK assertion from
`wxDialog::EndModal` (historically `src/gtk/dialog.cpp:193`,
"either `wxDialog::EndModal` called twice or `ShowModal` wasn't called"). The
user has manually confirmed this in the real app, and it still fires at the
current `fixes` tip (`7b8720f`), whose battle-window close/Quit/modality code was
just rolled back (RB-01, `d0e1134`) to the `fixes`-vs-`master` merge-base
(`5c3f91a`) plus the TMF-05 turn-button wiring. **This is not a regression from
recent work — it predates it.**

## Critical History — two prior fixes on this exact area FAILED and were rolled back

Do **NOT** blindly repeat either approach:

- **TMF-03 (`233c1ad`, reverted):** added a `WXTacticalUI::dismissActiveDialog()`
  / `hasPendingDialog()` seam tracking a single `m_activeDialog` pointer, and
  called it from `closeBattleScreen()` before hiding the frame. Passed
  source-contract / headless tests; failed at runtime. Reverted.
- **TMFR-01 (`d4699c6`, reverted):** replaced the GTK modal grab with a
  `wxWindowDisabler`, added a title-bar-X "delete-event" GTK bypass, converted
  the dismissal to an innermost-first `m_dialogStack`, and hid `SelectCombatGUI`
  around `bb.ShowModal()`. Passed headless tests; made Quit **worse** at runtime
  and still did not fix it. Reverted.

Both were verified "PASS" purely on source-contract / logic-level tests because
the live `ShowModal` path cannot run in the headless harness. That verification
gap is precisely why they shipped broken. This plan does not rely on a headless
behavioral test to prove the fix (see Verification Policy).

## Confirmed Facts (current code on `fixes` @ `7b8720f`)

All file:line citations verified against the current tree.

1. **`FBattleScreen` is a `wxFrame`, not a `wxDialog`.** It has a hand-rolled
   modal shim: `ShowModal()` (`src/tactical/FBattleScreen.cpp:237-269`) sets
   `gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE)` (`:253`), `Show(true)`,
   then runs a nested plain `wxEventLoop` (`:259-262`); `EndModal(int)`
   (`:271-287`) is custom, guarded by `m_modalActive`, and exits that loop then
   `Show(false)`. Because it is a `wxFrame`, its custom `EndModal` **never calls
   `wxDialog::EndModal`** and cannot itself be the object that trips
   `dialog.cpp:193`.
2. **The launcher `SelectCombatGUI` is a real `wxDialog`.** `onAttack()`
   (`src/gui/SelectCombatGUI.cpp:212-296`) stack-allocates `FBattleScreen bb;`
   and calls `bb.ShowModal()` (`:285`) **from inside `SelectCombatGUI::ShowModal()`'s
   own modal loop**, and does **not** hide itself first (the TMFR-01 hide was
   reverted; the docstring at `:197-211` is stale). So for the entire battle,
   `SelectCombatGUI`'s modal loop (and its `m_modalLoop`) is live, and
   `wxModalDialogHook::GetOpenCount() >= 1`.
3. **The Quit menu event only reaches the frame via a GTK bypass.** Because a
   `wxDialog` (`SelectCombatGUI`) is always modally open during the battle,
   wxGTK's `IsMenuEventAllowed()` (`src/gtk/menu.cpp`) drops menu events from the
   `wxFrame`. To work around this, the constructor connects a
   `g_signal_connect_after` "activate" hook on the Quit `GtkMenuItem`
   (`FBattleScreen.cpp:181-219`) that, when `GetOpenCount() > 0`, **synchronously**
   dispatches the Quit command via `self->HandleWindowEvent(evt)` — i.e. from
   deep inside GTK's menu-activation signal emission. (Background:
   `artifacts/fbattlescreen-menu-quit-fix.md`.)
4. **Quit teardown runs synchronously and reentrantly.** The dispatched command
   → `onMenuQuit()` (`:1019-1021`) → `Close(true)` → `wxEVT_CLOSE_WINDOW` →
   `onClose()` (`:1010-1017`) → `closeBattleScreen(GetReturnCode())`
   (`:990-1008`). With `IsModal()` true it calls the custom `EndModal()`, which
   exits the nested `wxEventLoop` and `Show(false)`s the frame — **all still on
   the call stack of the GTK "activate" callback**, while GTK's menu-shell
   nested loop/grab is still unwinding.
5. **The child `WXTacticalUI` dialogs are real `wxDialog`s.**
   `showMessage` (winner/info, `wxGenericMessageDialog`), `showDamageSummary`
   (`TacticalDamageSummaryGUI`), and `runICMSelection` (`ICMSelectionGUI`) each
   run a real `wxDialog::ShowModal()` (`src/gui/WXTacticalUI.cpp:30-85`) parented
   to the `FBattleScreen`. At the merge-base state there is **no** dialog-tracking
   or dismissal seam (TMF-03's was reverted).
6. **wxGTK behavior (from the prior investigation, `artifacts/fbattlescreen-menu-quit-fix.md`
   §Attempt 3):** `wxDialog::Show(false)` internally calls `EndModal(wxID_CANCEL)`
   when the dialog `IsModal()`. A second `EndModal` on that now-non-modal dialog
   (whose `m_modalLoop` is cleared) trips the `dialog.cpp:193` assert. The wx
   header confirms the modal state member is `m_modalLoop`
   (`/usr/local/include/wx-3.3/wx/gtk/dialog.h:47`), which is what the assert
   tests.

## Root-Cause Hypotheses (ranked) — to be disambiguated by the captured trace

The assert is raised by a **real `wxDialog`** (Fact 1 rules out `FBattleScreen`
itself). Static analysis yields two credible mechanisms. **The user's captured
runtime stack trace is the decisive input** and MUST be used to select between
them before writing the fix. Which object appears as the `this` of the failing
`wxDialog::EndModal`, and what calls it, cleanly separates the hypotheses:

- **H1 (PRIMARY) — Reentrant teardown from the GTK "activate" callback corrupts
  modal-loop bookkeeping.** File→Quit is unique in that its teardown runs
  synchronously inside GTK's menu-activation emission (Facts 3-4), while GTK's
  menu-shell nested loop/grab is still live. Exiting `FBattleScreen`'s nested
  `wxEventLoop` and calling `Show(false)` from that reentrant point leaves the
  wx/GTK modal-loop or grab state inconsistent, so a subsequent
  `wxDialog::EndModal` — most likely on the launching **`SelectCombatGUI`**
  (via `finishDialog(1)` at `SelectCombatGUI.cpp:294` once `bb.ShowModal()`
  returns), or on a re-entered frame close — fires when `m_modalLoop` is already
  gone → `dialog.cpp:193`. **Trace signature:** the failing `EndModal`'s object
  is `SelectCombatGUI` (or the crash unwinds through `onMenuQuit` →
  `Close`/`HandleWindowEvent` → the GTK `activate` hook synchronously). This
  hypothesis best explains why File→Quit specifically asserts even with **no**
  child dialog open, and why the title-bar X (a clean WM-protocol event, not a
  reentrant signal callback) historically produced a *different* symptom.
  **Fix direction H1:** defer the Quit teardown off the reentrant stack — process
  the close at a clean top-of-loop point instead of synchronously. Concretely,
  have `onMenuQuit()` schedule the close via `CallAfter(...)` (or have the GTK
  "activate" bypass `QueueEvent`/`wxPostEvent` the Quit command instead of
  calling `HandleWindowEvent(...)` synchronously) so the nested `wxEventLoop`
  exit and `Show(false)` run after GTK's menu emission has fully unwound. This is
  minimal, low-risk, and **distinct from both failed attempts** (neither deferred
  the teardown).

- **H2 (SECONDARY) — A live child `WXTacticalUI` dialog is auto-`EndModal`ed when
  the parent frame hides.** If a child `wxDialog` (damage summary / ICM /
  winner-info) still has a live modal loop when `closeBattleScreen()` →
  `EndModal()` → `Show(false)` hides the parent frame, wxGTK auto-`EndModal`s the
  child (Fact 6); the child's own loop unwind then double-ends it →
  `dialog.cpp:193`. This is the exact mechanism TMF-03 targeted (for the
  **title-bar X**). Note the menu path's reachability caveat: while a child
  dialog holds a GTK grab, the Quit menu item is normally not clickable, so for
  **File→Quit specifically** H2 is less likely than H1 — but the captured trace
  may show a child-dialog object, which would promote H2. **Trace signature:**
  the failing `EndModal`'s object is `TacticalDamageSummaryGUI`,
  `ICMSelectionGUI`, or `wxGenericMessageDialog`, and the call unwinds through
  `FBattleScreen::EndModal`/`Show(false)`. **Fix direction H2:** dismiss any live
  child dialog **before** the frame hides — but done correctly this time: an
  innermost-first stack of tracked live dialogs, each `EndModal`'d only while
  still `IsModal()`, dismissed *before* `Show(false)`, with `FBattleScreen`'s
  custom `m_modalActive` / `m_closeInProgress` guards intact. (This resembles
  TMF-03/TMFR-01; if the trace selects H2, the reason those failed must be
  understood from the trace before re-attempting — e.g. single-pointer tracking,
  or dismissing after rather than before the hide.)

If the trace corroborates the reentrancy signature (H1), implement Fix H1 only.
If it shows a child-dialog object (H2), implement Fix H2 only. If it shows both
conditions present, apply H1 (deferral) first — it removes the reentrancy that
also makes H2's teardown ordering fragile — and add the H2 pre-hide dismissal
only if the trace shows a child dialog is genuinely live at close time. Do not
speculatively apply both without trace justification; over-broad reworks are what
failed before.

## Confirmed vs Assumed

- **Confirmed:** Facts 1-6 above (verified against the current tree and the wx
  3.3.2 gtk dialog header; corroborated by `artifacts/fbattlescreen-menu-quit-fix.md`).
- **Assumed until the trace confirms:** which concrete `wxDialog` object trips
  `dialog.cpp:193` on File→Quit, and therefore whether H1 or H2 is the operative
  mechanism. The captured stack trace resolves this. It would **confirm H1** if
  the failing `EndModal` object is `SelectCombatGUI` and/or the stack unwinds
  through the GTK "activate" hook / `onMenuQuit` synchronously; it would
  **confirm H2** if the failing object is a `WXTacticalUI`-owned child dialog
  unwinding through `FBattleScreen::EndModal`/`Show(false)`.

## Governing Rules Constraint

- `doc/rules/tactical_operations_manual.md` MUST NOT be edited (non-volatile
  external artifact). This fix is UI lifecycle only and does not touch game rules.
- Module boundaries: `FBattleScreen.{h,cpp}` is a tactical GUI-bridge surface
  that already includes wx / GTK headers (`src/tactical/FBattleScreen.cpp:12-18`);
  editing its wx close/modal logic is in-boundary. `WXTacticalUI` and
  `SelectCombatGUI` are gui-module. **Do not** introduce wx into any non-GUI
  model module; the fix must stay confined to the allowed GUI-bridge files.

## Verification Policy (mandatory)

The defect lives in `FBattleScreen`'s live custom `ShowModal()` blocking loop and
nested `wxDialog::ShowModal()` interactions. Prior rounds documented that driving
that live loop or a nested `ShowModal` in the headless GUI harness caused runner
hangs and heap corruption. **Therefore the fix MUST NOT depend on a headless
behavioral test that drives a live `ShowModal` loop to prove it.** Verification
relies on three legs:

1. **Static control-flow reasoning** against the confirmed facts above and the
   captured trace (root-cause selection between H1/H2).
2. **The user's captured runtime assert stack trace / conditions** (provided by
   the Coordinator) as the confirming input for root cause and for the after-fix
   check that the assert no longer fires.
3. **A REQUIRED user manual-verification pass as the acceptance gate.** Before
   this feature is declared accepted, the user must confirm in the real app that
   **File → Quit closes the battle screen cleanly with no `wxDialog::EndModal`
   assert**, at representative points in a battle (at minimum: during normal
   move/fire play; and, if reproducible, immediately after a damage-summary or
   winner/info dialog has been shown and dismissed). The Coordinator MUST surface
   this gate and MUST NOT declare the feature accepted until the user signs off.
   This gate is outside the automated Implementer→…→Verifier chain.

Automated tests are permitted **only** for what is genuinely testable at the
logic level without a live `ShowModal` loop, and must be genuinely behavioral
(not source-inspection-only). Acceptable examples: (a) that `closeBattleScreen()`
remains idempotent under a reentrant/duplicate close (drive a reentrant
`closeBattleScreen()` via an overridden `Show()` and assert the
`m_closeInProgress` guard and final hidden state — this pattern already existed
under TMFR-01 and is safe because it does not run a live modal loop); (b) if Fix
H2 is chosen, that a child-dialog dismissal seam dismisses tracked live dialogs
innermost-first and is a safe no-op on an empty set. A source-contract assertion
(e.g. "`onMenuQuit` contains `CallAfter`") MAY be used only as a supplement, and
never as the sole backing for the Quit-no-assert behavior — that behavior is
proven by the manual pass, per the repository's behavioral-verification rule.

## Overall Acceptance Criteria

- **AC1:** The concrete `wxDialog` object and `EndModal` call site that trip
  `dialog.cpp:193` on File→Quit are identified from the captured trace and stated
  in the implementer report, and the applied fix (H1 or H2) directly targets that
  mechanism.
- **AC2:** File → Quit closes the battle screen cleanly with **no**
  `wxDialog::EndModal` assert, confirmed by the required user manual-verification
  pass. The battle screen hides/returns and the strategic layer resumes normally
  (no orphaned window, no hang).
- **AC3:** No regression to the normal battle-end (winner) close path, the
  title-bar-X close path, or the launching `SelectCombatGUI` flow, confirmed by
  the manual pass and by the existing tactical + GUI suites still passing.
- **AC4:** The change is minimal and confined to the allowed GUI-bridge files;
  the GTK modal-grab / menu-bypass architecture is not broadly reworked (no
  reintroduction of the reverted `wxWindowDisabler` / `m_dialogStack` /
  hide-`SelectCombatGUI` approach unless the trace specifically justifies it and
  the implementer report explains why it will not recur).
- **AC5:** `doc/rules/tactical_operations_manual.md` is unedited; no wx
  dependency is introduced into any non-GUI model module; the tactical and GUI
  test suites build and pass with no new regressions.

## Overall Documentation Impact

- `AGENTS.md`: update the `FBattleScreen` close-vector / modality ownership notes
  to record the corrected File→Quit teardown behavior (deferred close, and/or the
  pre-hide child-dialog dismissal, per the chosen fix).
- `doc/test-contracts.md`: update the battle-window close/Quit contracts to
  reflect the fix and record that the Quit-no-assert runtime behavior is backed by
  the user manual-verification pass (not a headless behavioral test).
- `doc/UsersGuide.md`: no change expected (behavior is "Quit works without
  crashing", not a new user-facing feature) — update only if the tactical Quit is
  explicitly documented.
- `doc/rules/tactical_operations_manual.md`: MUST NOT change.
- Doxygen headers updated on every edited function/class per repository
  convention (`@author` with the implementer's model+reasoning level, `Last
  Modified` set to the current date).

## Dependency Ordering and Parallelization

Single subtask — no ordering or parallelization concerns.

## Output Artifact Path

- This plan artifact: `plans/battle-quit-endmodal-fix-plan.md` (repo-root-relative).
- Per-subtask orchestration artifacts:
  `artifacts/battle-quit-endmodal-fix/BQEM-01/` (repo-root-relative).

## Implementer Prompt

The subtask below provides its launch-ready Implementer prompt inline. It begins
with `Your role is 'implementer'. Your task is as follows:` and ends with the
explicit completion gate.

---

## BQEM-01 — Root-cause-confirm and fix the File→Quit `wxDialog::EndModal` assert

Security review: not required.

### Task

Using the user's captured runtime assert stack trace (provided by the
Coordinator), confirm which concrete `wxDialog` object and `EndModal` call site
trip the wxGTK assert (`dialog.cpp:193`, "EndModal called twice or ShowModal
wasn't called") on **File → Quit** in `FBattleScreen`, then apply the **minimal**
targeted fix for that mechanism. Select between the two ranked hypotheses in the
plan by matching the trace signature:

- **If the trace confirms H1 (reentrant teardown; failing object is
  `SelectCombatGUI` and/or the stack unwinds synchronously through the GTK
  "activate" bypass → `onMenuQuit` → `Close`/`HandleWindowEvent`):** defer the
  Quit teardown off the reentrant GTK-signal call stack. Change `onMenuQuit()`
  (`src/tactical/FBattleScreen.cpp:1019-1021`) to schedule the close at a clean
  top-of-loop point via `CallAfter(...)` instead of calling `Close(true)`
  synchronously, and/or change the GTK "activate" bypass
  (`FBattleScreen.cpp:181-219`) to `QueueEvent`/`wxPostEvent` the Quit command
  instead of `HandleWindowEvent(...)`. The nested `wxEventLoop` exit and
  `Show(false)` must run after GTK's menu-activation emission has fully unwound.
- **If the trace confirms H2 (a live child `WXTacticalUI` dialog —
  `TacticalDamageSummaryGUI` / `ICMSelectionGUI` / `wxGenericMessageDialog` — is
  auto-`EndModal`ed when the parent frame hides):** dismiss every live child
  dialog **before** `Show(false)`/`EndModal` hides the frame, innermost-first,
  `EndModal`'ing each only while it is still `IsModal()`. Reinstate a minimal
  dialog-tracking seam on `WXTacticalUI` (a stack of live dialogs, not a single
  pointer) set around each child `ShowModal` in `showMessage`,
  `showDamageSummary`, and `runICMSelection`, and dismiss it from
  `closeBattleScreen()` before the hide. Keep the custom `FBattleScreen::EndModal`
  `m_modalActive` guard and the `closeBattleScreen()` `m_closeInProgress` guard
  intact.

In both cases keep `FBattleScreen`'s custom modal-shim contract (`ShowModal()` /
`EndModal()` / `IsModal()`) and the existing winner and title-bar-X close paths
working. Do NOT broadly rework modality: do NOT reintroduce the reverted TMFR-01
approach (`wxWindowDisabler` replacing the GTK grab, a title-bar-X delete-event
bypass, an `m_dialogStack` rename, or hiding `SelectCombatGUI` around
`bb.ShowModal()`) unless the trace specifically requires it, and if so explain in
the report why the recurrence that got TMFR-01 reverted will not happen.

State in the implementer report the exact asserting object and call path from the
trace, which hypothesis was selected, and why the applied fix eliminates that
specific `EndModal` double-call.

### Allowed Files

- `include/tactical/FBattleScreen.h`
- `src/tactical/FBattleScreen.cpp`
- `include/gui/WXTacticalUI.h` (only if Fix H2 is selected)
- `src/gui/WXTacticalUI.cpp` (only if Fix H2 is selected)
- `src/gui/SelectCombatGUI.cpp` (only if the trace shows the launcher's teardown
  must change; prefer leaving it untouched)

### Acceptance Criteria

- The concrete `wxDialog` object and `EndModal` call site are identified from the
  captured trace and named in the implementer report; the applied fix targets
  that exact mechanism.
- The Quit teardown no longer produces a duplicate/`m_modalLoop`-cleared
  `wxDialog::EndModal` call: either the teardown runs at a clean top-of-loop point
  off the GTK "activate" reentrant stack (H1), or every live child dialog is
  dismissed while still `IsModal()` before the parent frame hides (H2).
- `closeBattleScreen()` remains idempotent under a reentrant/duplicate close; the
  `m_modalActive` and `m_closeInProgress` guards are intact.
- The normal battle-end (winner) close path, the title-bar-X close path, and the
  `SelectCombatGUI` launch/return flow are unchanged in behavior.
- The change is confined to the allowed GUI-bridge files; no wx dependency is
  added to any non-GUI model module; `doc/rules/tactical_operations_manual.md` is
  unedited.
- Doxygen headers on all edited functions/classes are updated (`@author` +
  `Last Modified`) per `AGENTS.md`.
- The File→Quit-no-assert runtime behavior is confirmed by the required user
  manual-verification pass (see the plan's Verification Policy); the report must
  state that this runtime behavior depends on that manual pass and must NOT claim
  a runtime PASS from source structure or a headless test that drives a live
  `ShowModal`.

### Documentation Impact

- `AGENTS.md`: update the `FBattleScreen` close-vector / modality ownership notes
  for the corrected Quit teardown (and, if H2, the pre-hide child-dialog
  dismissal seam on `WXTacticalUI`).
- `doc/test-contracts.md`: update the battle-window close/Quit contracts; record
  that Quit-no-assert is backed by the user manual-verification pass.
- `doc/rules/tactical_operations_manual.md`: MUST NOT change.
- Doxygen headers on all edited functions updated.

### Tester Guidance

- Cover only what is safe at the logic level without a live `ShowModal` loop:
  - `closeBattleScreen()` idempotency under a reentrant/duplicate close (drive a
    reentrant `closeBattleScreen()` via an overridden `Show()`, assert the
    `m_closeInProgress` guard and final hidden state) — behavioral and safe.
  - If Fix H2: a behavioral test that the child-dialog dismissal seam dismisses
    tracked live dialogs innermost-first and is a safe no-op on an empty set
    (using a mock/real `wxDialog` without driving `FBattleScreen::ShowModal()`).
- Do NOT write or accept any test that drives `FBattleScreen::ShowModal()`'s live
  custom event loop or a nested child `wxDialog::ShowModal()` — prior rounds
  documented runner hangs / heap corruption. Explicitly defer the File→Quit
  no-assert runtime behavior to the user manual-verification pass rather than
  asserting a PASS from source structure. Live tactical/GUI files:
  `tests/tactical/*` (e.g. `FTacticalGameMockUITest`, `WXTacticalUIAdapterTest`)
  and `tests/gui/TacticalGuiLiveTest.{cpp,h}`.

### Implementer Prompt

```
Your role is 'implementer'. Your task is as follows:

Root-cause-confirm and fix a pre-existing runtime bug: in the tactical battle
screen FBattleScreen, File -> Quit throws a wxGTK assertion from
wxDialog::EndModal (historically src/gtk/dialog.cpp:193, "either
wxDialog::EndModal called twice or ShowModal wasn't called"). It reproduces in
the real app at the current fixes tip. TWO prior fixes on this exact area FAILED
and were reverted -- do NOT blindly repeat them:
- TMF-03 (233c1ad): a single-pointer WXTacticalUI::dismissActiveDialog() seam
  called from closeBattleScreen(). Reverted.
- TMFR-01 (d4699c6): wxWindowDisabler replacing the GTK grab + a title-bar-X
  delete-event bypass + an m_dialogStack + hiding SelectCombatGUI around
  bb.ShowModal(). Reverted (made Quit worse).

Confirmed facts (verified against the current tree):
- FBattleScreen is a wxFrame with a hand-rolled modal shim: ShowModal()
  (src/tactical/FBattleScreen.cpp:237-269) sets gtk_window_set_modal(TRUE) then
  runs a nested plain wxEventLoop; custom EndModal() (:271-287) exits that loop
  then Show(false). Its custom EndModal never calls wxDialog::EndModal, so
  FBattleScreen itself is NOT the object that trips dialog.cpp:193.
- SelectCombatGUI (a real wxDialog) launches the battle from inside its own
  ShowModal loop via bb.ShowModal() (src/gui/SelectCombatGUI.cpp:285) and does
  NOT hide itself. So GetOpenCount() >= 1 the entire battle, and wxGTK's
  IsMenuEventAllowed() drops the Quit menu event; a GTK "activate" bypass
  (FBattleScreen.cpp:181-219) synchronously dispatches Quit via
  HandleWindowEvent from inside GTK's menu-activation emission.
- onMenuQuit() (:1019-1021) calls Close(true) -> onClose() (:1010-1017) ->
  closeBattleScreen() (:990-1008) -> custom EndModal(), all still on the GTK
  "activate" callback stack.
- The WXTacticalUI child dialogs (damage summary, ICM, winner/info) are real
  wxDialog::ShowModal() calls (src/gui/WXTacticalUI.cpp:30-85) parented to the
  frame; there is currently NO dialog-tracking/dismissal seam.
- wxGTK: wxDialog::Show(false) auto-calls EndModal(wxID_CANCEL) on a still-modal
  dialog; a second EndModal on it (m_modalLoop already cleared) trips
  dialog.cpp:193.

Step 1 (REQUIRED, before writing any fix): read the user's captured runtime
assert stack trace that the Coordinator provides. Identify the concrete wxDialog
object (the `this` of the failing wxDialog::EndModal) and the call path. Match it
to one of the two hypotheses in plans/battle-quit-endmodal-fix-plan.md:
- H1 (PRIMARY): reentrant teardown from the GTK "activate" callback corrupts
  modal-loop bookkeeping; the failing object is SelectCombatGUI and/or the stack
  unwinds synchronously through the GTK activate bypass -> onMenuQuit ->
  Close/HandleWindowEvent. FIX: defer the Quit teardown off the reentrant stack --
  change onMenuQuit() to schedule the close via CallAfter(...) instead of
  synchronous Close(true), and/or change the GTK "activate" bypass to
  QueueEvent/wxPostEvent the Quit command instead of HandleWindowEvent(...), so
  the nested wxEventLoop exit and Show(false) run after GTK's menu emission has
  fully unwound.
- H2 (SECONDARY): a live child WXTacticalUI dialog (TacticalDamageSummaryGUI /
  ICMSelectionGUI / wxGenericMessageDialog) is auto-EndModal'd when the parent
  frame hides. FIX: dismiss every live child dialog BEFORE Show(false), innermost
  -first, EndModal'ing each only while still IsModal(); track live dialogs with a
  STACK (not a single pointer) set around each ShowModal in showMessage,
  showDamageSummary, runICMSelection, dismissed from closeBattleScreen() before
  the hide. Keep FBattleScreen's m_modalActive and m_closeInProgress guards intact.

Apply ONLY the fix matching the trace. If both conditions are present, apply H1
first and add H2's pre-hide dismissal only if the trace shows a child dialog is
genuinely live at close time. Do NOT broadly rework modality and do NOT
reintroduce the reverted TMFR-01 approach unless the trace specifically requires
it (and if so, explain in the report why the recurrence that got TMFR-01 reverted
will not happen). Keep FBattleScreen's ShowModal/EndModal/IsModal contract and
the winner and title-bar-X close paths working.

Allowed files (modify only these):
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp
- include/gui/WXTacticalUI.h            (only if Fix H2 is selected)
- src/gui/WXTacticalUI.cpp              (only if Fix H2 is selected)
- src/gui/SelectCombatGUI.cpp           (only if the trace shows the launcher's
                                         teardown must change; prefer untouched)

Acceptance criteria (implementation outcomes):
- The asserting wxDialog object and EndModal call site are identified from the
  trace and named in your report; the fix targets that exact mechanism, and you
  explain why it eliminates the duplicate/m_modalLoop-cleared EndModal call.
- Either the Quit teardown runs at a clean top-of-loop point off the GTK
  "activate" reentrant stack (H1), or every live child dialog is dismissed while
  still IsModal() before the parent frame hides (H2).
- closeBattleScreen() stays idempotent under a reentrant/duplicate close;
  m_modalActive and m_closeInProgress guards intact.
- The winner close path, the title-bar-X close path, and the SelectCombatGUI
  launch/return flow are behaviorally unchanged.
- Change confined to allowed GUI-bridge files; no wx dependency added to any
  non-GUI model module.

Verification note: the File->Quit-no-assert runtime behavior lives in
FBattleScreen's live custom ShowModal loop and cannot be exercised in the
headless GUI harness (prior rounds hung the runner / caused heap corruption when
driving a live ShowModal or nested ShowModal). Do NOT write or rely on a headless
test that drives a live ShowModal loop, and do NOT claim a runtime PASS from
source structure. Cover only safe logic-level behavior with real behavioral tests
(closeBattleScreen() idempotency; and, if H2, the innermost-first dialog-dismissal
seam), and explicitly defer the Quit-no-assert runtime behavior to the REQUIRED
user manual-verification pass described in the plan. State this in your report.

Constraints:
- Do NOT edit doc/rules/tactical_operations_manual.md.
- Update the Doxygen headers on all edited functions/classes (@author with your
  model and reasoning level, Last Modified to today) per AGENTS.md.
- Do NOT modify test files; behavioral coverage is owned by the Tester stage.

Validation guidance:
- Build from the repo root with `make` (or `make -C src all` for the production
  build).
- Run the tactical suite: `cd tests && make tactical-tests && ./tactical/TacticalTests`.
- Run the GUI suite headless: `cd tests/gui && make && xvfb-run -a ./GuiTests`.
- The definitive proof is the user manual-verification pass (File -> Quit closes
  the battle screen with no wxDialog::EndModal assert); flag it in your report as
  the acceptance gate.

Tester handoff: closeBattleScreen() idempotency and (if H2) the innermost-first
child-dialog dismissal seam in tests/tactical/* (FTacticalGameMockUITest,
WXTacticalUIAdapterTest) and tests/gui/TacticalGuiLiveTest.{cpp,h}, without
driving a live ShowModal loop; File->Quit no-assert deferred to the user manual
verification pass per the plan.

Artifacts: write your report to artifacts/battle-quit-endmodal-fix/BQEM-01/
(implementer_report.md and implementer_result.json), repo-root-relative.

Continue past preflight and complete the work when no blocking issues are present.

Do not report success unless all required artifacts exist and all changes are committed.
```
