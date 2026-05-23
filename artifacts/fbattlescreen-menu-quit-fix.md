# FBattleScreen File→Quit Fix: Diagnosis and Resolution

## Summary

After multiple failed attempts, the `File→Quit` menu item in `FBattleScreen`
was fixed by identifying a deliberate wxWidgets/GTK event-routing guard that
silently blocks menu events from `wxFrame` windows when any `wxDialog` is
running a modal event loop.  The fix bypasses the guard using a low-level GTK
signal hook.

---

## What Was Broken

`FBattleScreen` is the tactical battle screen.  It is a `wxFrame` subclass
with a custom `ShowModal()` / `EndModal()` shim (it was originally a
`wxDialog`, but was converted to `wxFrame` so it could use `SetMenuBar()`).

Three symptoms were reported:

1. **File→Quit did nothing in the real app.**  Clicking the menu item produced
   no visible effect; the window stayed open and the game continued normally.

2. **Test windows never closed automatically.**  When running the GUI test
   suite, any test that launched `FBattleScreen` via `ShowModal()` would block
   indefinitely, requiring manual window close.

3. **A stale tactical test assertion** referencing `m_closeInProgress` that
   was left over from an earlier implementation.

Symptoms 2 and 3 were resolved in an earlier commit (`2af0d75`).  Symptom 1
required five investigation cycles and is the subject of this document.

---

## Investigation History

### Attempt 1 — AddGrab()

**Hypothesis:** `SelectCombatGUI::ShowModal()` (a `wxDialog`) calls
`gtk_window_set_modal(TRUE)` before `Show()`, which invokes `gtk_grab_add()`
and makes SelectCombatGUI the GTK input-grab owner.  FBattleScreen never gets
the grab, so menu `activate` signals are silently swallowed.

**Fix tried:** Call `AddGrab()` on FBattleScreen after `Show(true)` inside
`ShowModal()`.

**Result:** All tests passed.  File→Quit in the real app still did nothing. ✗

### Attempt 2 — gtk_window_set_modal() before Show()

**Hypothesis:** `AddGrab()` fires too late (after `Show()`).  The grab must be
established during the `Show()` phase.  Calling
`gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE)` before `Show(true)` should
put FBattleScreen at the top of the GTK grab stack.

**Fix tried:** Added `gtk_window_set_modal(TRUE)` before `Show(true)` in
`ShowModal()`, removed `wxWindowDisabler`, and fixed `EndModal()` ordering to
match `wxDialog::EndModal()` (exit loop before `Show(false)`).

**Result:** All tests passed.  File→Quit in the real app still did nothing. ✗

### Attempt 3 — Hide() SelectCombatGUI before launching FBattleScreen

**Hypothesis:** Even with `gtk_window_set_modal(TRUE)`, SelectCombatGUI still
holds a GTK grab.  Calling `Hide()` on it (which should call
`gtk_grab_remove()`) would release the grab.

**Fix tried:** Added `SelectCombatGUI::Hide()` in `onAttack()` before
creating FBattleScreen.

**Result:** Two new failures:
- File→Quit still did nothing.
- The titlebar X button triggered an assertion:
  `"either wxDialog::EndModal called twice or ShowModal wasn't called"`

**Critical discovery:** `wxDialog::Show(false)` (called by `Hide()`) internally
calls `EndModal(wxID_CANCEL)` when the dialog is modal (see
`src/gtk/dialog.cpp` line 62–64).  This ended SelectCombatGUI's modal loop
immediately, and the subsequent `finishDialog(1)` → `EndModal(1)` fired on an
already non-modal dialog — hence the assertion.

More importantly: **even after `Hide()` completely released SelectCombatGUI's
GTK grab, File→Quit still did not work.**  This definitively disproved the
GTK grab theory.  The root cause had to be in the wxWidgets layer, not GTK.

`Hide()` was reverted.

### Instrumentation Phase

A GTK-level `g_signal_connect` was added directly to the Quit `GtkMenuItem`'s
`activate` signal to determine whether GTK was routing the click at all.

**Output when File→Quit was clicked:**
```
[GTK-ACTIVATE] Quit GtkMenuItem::activate fired at GTK level!
```

But `[BATTLESCREEN] onMenuQuit called` never appeared.

**Conclusion:** GTK routes the click correctly.  The `activate` signal fires.
The problem is entirely in the bridge between GTK's signal dispatch and
wxWidgets' `wxEVT_MENU` processing.

---

## Root Cause

### The wx/GTK Menu Event Guard

In `~/wxWidgets/wxWidgets-3.3.2/src/gtk/menu.cpp`, the GTK `activate` signal
callback for every `wxMenuItem` calls `IsMenuEventAllowed()` before dispatching
the event:

```cpp
static void menuitem_activate(GtkWidget*, wxMenuItem* item)
{
    if (!item->IsEnabled())
        return;

    if ( !IsMenuEventAllowed(item->GetMenu()) )
        return;  // <-- event is silently dropped here

    // ...
    menu->SendEvent(id, ...);
}
```

`IsMenuEventAllowed` is defined as:

```cpp
static bool IsMenuEventAllowed(wxMenu* menu)
{
    if ( wxModalDialogHook::GetOpenCount() )
    {
        wxWindow* tlw = wxGetTopLevelParent(menu->GetWindow());
        if ( !tlw || !wxDynamicCast(tlw, wxDialog) )
        {
            // This must be an event from a menu bar of one of the frames.
            return false;
        }
    }

    return true;
}
```

This guard is intentional: when any `wxDialog` is open modally, wxWidgets
blocks all menu events from `wxFrame` windows.  The assumption is that frames
are background windows that should not be interactable during modal dialogs.

### Why FBattleScreen Was Affected

`FBattleScreen` is launched from `SelectCombatGUI::onAttack()`, which runs
**inside** `SelectCombatGUI::ShowModal()`.  `SelectCombatGUI` is a `wxDialog`.

When `wxDialog::ShowModal()` is called, it calls
`wxModalDialogHook::CallEnter()`, which increments the static counter
`wxModalDialogHook::ms_countOpen`.  That counter remains > 0 for the entire
duration of `SelectCombatGUI`'s modal event loop — which spans the entire
lifetime of `FBattleScreen`.

Therefore, for every single `activate` signal fired on any of FBattleScreen's
menu items:

1. `IsMenuEventAllowed()` is called.
2. `GetOpenCount()` returns 1 (SelectCombatGUI is modal).
3. `wxGetTopLevelParent(menu->GetWindow())` returns `FBattleScreen`.
4. `wxDynamicCast(FBattleScreen, wxDialog)` returns `NULL` (it is a `wxFrame`).
5. `IsMenuEventAllowed()` returns `false`.
6. `menuitem_activate` returns early — **the event is never dispatched.**

This is why all previous GTK-level fixes (grabs, modality flags) made no
difference: the block was not in GTK at all.  It was in wxWidgets' own event
routing layer, after GTK had already fired the signal.

### Why the Titlebar X Button Worked

The titlebar close button (`WM_DELETE_WINDOW` / `GDK_DELETE`) is a window
manager protocol, not a GTK input event.  It bypasses GTK's grab and input
routing entirely and goes directly to the WM-protocol handler in wxWidgets,
which fires `wxEVT_CLOSE_WINDOW`.  `IsMenuEventAllowed` is never consulted.

### Why Tests Worked (After the Earlier Fix)

The test harness `AnyModalDismissTimer::Notify()` was extended to detect
`FBattleScreen::IsModal()` and call `EndModal()` directly — bypassing the menu
system entirely.  Tests therefore never needed `wxEVT_MENU` to work.

---

## The Fix

### Approach

Since `FBattleScreen` is a `wxFrame` and cannot be changed to `wxDialog`
(dialogs do not support `SetMenuBar()` in wxGTK), the only way to get menu
events through is to bypass `IsMenuEventAllowed`.

The GTK `activate` signal fires correctly.  wx's handler returns early.  By
connecting a second handler with `g_signal_connect_after`, we can intercept
the event after wx's handler has already given up, and dispatch it directly.

### Implementation

In `FBattleScreen::FBattleScreen()`, after `SetMenuBar(menuBar)`:

```cpp
#ifdef __WXGTK__
{
    GtkWidget* gtkMenuBar = GTK_WIDGET(menuBar->GetHandle());
    GList* barItems = gtk_container_get_children(GTK_CONTAINER(gtkMenuBar));
    if (barItems) {
        GtkWidget* fileMenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(barItems->data));
        if (fileMenu) {
            GList* items = gtk_container_get_children(GTK_CONTAINER(fileMenu));
            // File menu: Load(0), Save(1), Separator(2), Quit(3)
            GList* quitNode = g_list_nth(items, 3);
            if (quitNode) {
                g_signal_connect_after(quitNode->data, "activate",
                    G_CALLBACK(+[](GtkMenuItem*, void* data) {
                        FBattleScreen* self = static_cast<FBattleScreen*>(data);
                        // Only dispatch when wx's handler was blocked.
                        // When GetOpenCount()==0, wx already dispatched normally.
                        if (wxModalDialogHook::GetOpenCount() > 0) {
                            wxCommandEvent evt(wxEVT_MENU, ID_TacticalQuit);
                            evt.SetEventObject(self);
                            self->HandleWindowEvent(evt);
                        }
                    }), this);
            }
            g_list_free(items);
        }
        g_list_free(barItems);
    }
}
#endif
```

### Why This Is Correct

- `g_signal_connect_after` ensures our handler fires **after** wx's
  `menuitem_activate`, which already returned early without dispatching.
- `wxModalDialogHook::GetOpenCount() > 0` is the exact condition under which
  `IsMenuEventAllowed` returns `false`.  Checking it here means we only act
  when wx would have blocked the event — no double-dispatch.
- When `GetOpenCount() == 0` (no modal dialogs open), wx's handler dispatched
  normally.  Our handler does nothing.
- `HandleWindowEvent()` goes directly to the `Bind(wxEVT_MENU, ...)` handler,
  completely bypassing the menu event guard.
- Memory safety: the `GtkMenuItem` is owned by `menuFile` which is owned by
  `menuBar` which is owned by `FBattleScreen`.  The signal handler is
  disconnected automatically when the `GtkMenuItem` is destroyed (when
  FBattleScreen is destroyed), so `this` is always valid while the handler
  can fire.

---

## Other Changes in This Work

### Test Hang Fix (`2af0d75`)

`WXGuiTestHarness::AnyModalDismissTimer::Notify()` used `dynamic_cast<wxDialog*>`
to find modal windows to dismiss.  Since `FBattleScreen` is a `wxFrame`, this
always failed and the timer never fired.

**Fix:** Extended `Notify()` to also scan `wxTopLevelWindows` for
`FBattleScreen` instances via `dynamic_cast<FBattleScreen*>`, check
`IsModal()`, and call `EndModal(m_returnCode)`.

### Stale Test Assertion (`2af0d75`)

`FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults`
contained `assertContains(body, "m_closeInProgress = false;")` — a leftover
check that had never been valid (the member belongs to FBattleScreen, not
FTacticalGame).  Removed.

### wxWindowDisabler Removed from ShowModal()

`wxDialog::ShowModal()` on GTK does not use `wxWindowDisabler` — it relies
purely on `gtk_window_set_modal()`.  The `wxWindowDisabler` that had been added
to `FBattleScreen::ShowModal()` was unnecessary and potentially disruptive
(it called `gtk_widget_set_sensitive(FALSE)` on all other windows).  It was
removed, and the `m_modalDisabler` member was removed from the header.

---

## Files Changed

| File | Change |
|------|--------|
| `src/tactical/FBattleScreen.cpp` | GTK activate bypass hook; removed `wxWindowDisabler`; removed all debug output; added `<wx/modalhook.h>` |
| `include/tactical/FBattleScreen.h` | Removed `m_modalDisabler` member; updated Doxygen header |
| `src/gui/SelectCombatGUI.cpp` | Reverted `Hide()` attempt; retained Doxygen header on `onAttack()` |
| `tests/gui/WXGuiTestHarness.cpp` | Extended `AnyModalDismissTimer` to detect `FBattleScreen::IsModal()` |
| `tests/tactical/FTacticalGameMechanicsTest.cpp` | Removed stale `m_closeInProgress` assertion |

---

## Commits

| Hash | Description |
|------|-------------|
| `2af0d75` | Fix FBattleScreen modal grab so File→Quit works and tests close automatically (initial attempt; fixed test hang and stale assertion) |
| `3484471` | Hide SelectCombatGUI before ShowModal to release GTK grab (disproved grab theory — reverted in next commit) |
| `895c648` | Fix File→Quit menu in FBattleScreen when launched from modal dialog (root cause found and fixed) |

---

*Written May 23, 2026.*
