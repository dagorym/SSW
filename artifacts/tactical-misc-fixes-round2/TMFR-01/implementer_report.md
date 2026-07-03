# Implementer Report

Status:
- PASS

Task summary:
- Reworked FBattleScreen modality so the BattleBoard is a normal top-level window with a working minimize button and title-bar X, while still blocking interaction with the strategic layer during battle, and eliminated the residual wxDialog::EndModal assert. Dropped the GTK gtk_window_set_modal() grab in ShowModal()/EndModal() in favor of a wxWindowDisabler that disables every other top-level window (strategic main frame, launching SelectCombatGUI) for the battle's duration and restores them on close. Generalized the existing File->Quit GTK 'activate' bypass to the title-bar X via a matching 'delete-event' bypass so both close vectors reach onClose()/closeBattleScreen() even while a child WXTacticalUI dialog is modal. Converted WXTacticalUI's single m_activeDialog pointer into an innermost-first m_dialogStack (std::vector<wxDialog*>) so dismissActiveDialog() dismisses every live child dialog, fixing the duplicate EndModal assert when more than one dialog was live. Updated SelectCombatGUI::onAttack() to hide itself around bb.ShowModal() since SelectCombatGUI is itself a GTK-modal wxDialog whose grab would otherwise block the now-non-GTK-modal FBattleScreen frame from receiving any input.

Changed files:
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp
- include/gui/WXTacticalUI.h
- src/gui/WXTacticalUI.cpp
- src/gui/SelectCombatGUI.cpp

Validation commands run:
- make (repo root)
- cd tests && make tactical-tests && ./tactical/TacticalTests
- cd tests/gui && make && ./GuiTests (live X display was available in this environment; xvfb-run was not installed)
- cd tests && ./SSWTests

Validation outcome:
- make: clean build, exit 0. tests/tactical/TacticalTests: 249/249 pass. tests/gui/GuiTests: 63/65 pass; 2 failures, both classified below (1 expected consequence of the approved behavior change, 1 pre-existing/unrelated flake confirmed present on the unmodified baseline). tests/SSWTests: 195/195 pass on rerun (one transient unrelated FFleetTest::testDecTransitTime failure on a single run, not reproducible on rerun, unrelated to any changed file).

Implementation/code commit hash:
- d4699c6

Artifacts written:
- artifacts/tactical-misc-fixes-round2/TMFR-01/implementer_report.md
- artifacts/tactical-misc-fixes-round2/TMFR-01/tester_prompt.txt
- artifacts/tactical-misc-fixes-round2/TMFR-01/implementer_result.json

Implementation context:
- FBattleScreen::ShowModal()/EndModal() (src/tactical/FBattleScreen.cpp, include/tactical/FBattleScreen.h): new wxWindowDisabler* m_windowDisabler member; created in ShowModal() before Show(true), deleted (restoring other windows) in EndModal() and, as a safety net, in ~FBattleScreen() if still set.
- FBattleScreen constructor GTK bypass block (src/tactical/FBattleScreen.cpp, inside #ifdef __WXGTK__): existing Quit 'activate' bypass kept as-is; new bypass connects g_signal_connect(m_widget, "delete-event", ...) that, when wxModalDialogHook::GetOpenCount() > 0, synthesizes and dispatches a wxCloseEvent via self->HandleWindowEvent(evt) and returns TRUE; otherwise returns FALSE to let wx's normal delete-event routing continue. This generalizes the Quit bypass to the title-bar X per the plan's explicit instruction.
- closeBattleScreen()'s existing m_closeInProgress guard is unchanged and still makes the close path idempotent regardless of which vector (title-bar X bypass, wx's own close routing, or File->Quit) reaches it first or more than once.
- WXTacticalUI (include/gui/WXTacticalUI.h, src/gui/WXTacticalUI.cpp): m_activeDialog (single wxDialog*) replaced by m_dialogStack (std::vector<wxDialog*>). showMessage()/showDamageSummary()/runICMSelection() each push_back the dialog immediately before their own ShowModal() call and pop_back immediately after it returns, so the stack always reflects live nested dialogs with the most recently opened one at the back. hasPendingDialog() and dismissActiveDialog() both walk the stack from rbegin() (innermost/most recent) to rend() (outermost); dismissActiveDialog() calls EndModal(wxID_CANCEL) on every entry still modal (IsModal() true) without mutating the stack itself -- popping happens naturally as each dialog's own ShowModal() call unwinds. Public method signatures (hasPendingDialog(), dismissActiveDialog()) are unchanged, so existing tests exercising them (e.g. TMF-03 tests in tests/gui/TacticalGuiLiveTest.cpp) continue to compile and pass unmodified.
- SelectCombatGUI::onAttack() (src/gui/SelectCombatGUI.cpp): added this->Show(false) immediately before bb.ShowModal() and this->Show(true) immediately after it returns (mirroring the existing switchRoles() pattern already in the same file), because SelectCombatGUI is itself a genuine wxDialog holding its own native GTK modal grab while onAttack() runs; with FBattleScreen's own grab now removed, that still-active SelectCombatGUI grab would otherwise block all input to the new FBattleScreen top-level. FBattleScreen::ShowModal()'s wxWindowDisabler separately keeps SelectCombatGUI's own parent (the strategic main frame) and SelectCombatGUI itself non-interactive for the same duration, restoring both when ShowModal() returns.
- include/gui/SelectCombatGUI.h was in the approved allowed-file list but required no changes -- the fix is entirely in the .cpp body of an existing handler.
- TMFR-02 (Turn Left/Right enable logic), TMFR-03 (turn-panel layout/caption), and TMFR-04 (GUI test-suite robustness/author-tag dedup) are separate, file-disjoint subtasks per the plan and were intentionally left untouched by this subtask.
- IMPORTANT VERIFICATION-POLICY CONSTRAINT: per the plan's Verification Policy, the minimize decoration, the live mid-battle X close, and the Quit-no-assert behavior cannot be exercised in the headless/automated harness. FBattleScreen::ShowModal() runs its own blocking custom event loop, and driving it live under the test harness has previously caused runner blocking / heap corruption (see existing TMF-02/TMF-03 source-contract-only tests in tests/gui/TacticalGuiLiveTest.cpp for precedent). Do NOT write or accept any test that asserts a PASS on these three runtime behaviors from source structure alone -- cover everything else (dismissal-stack ordering via the WXTacticalUI dialog-stack API, closeBattleScreen()/onClose() state-transition logic, and the wxWindowDisabler-based enable/disable bookkeeping) with real behavioral tests, and explicitly state that the minimize/X-close/Quit-no-assert runtime behaviors are deferred to the required user manual verification pass described in the plan.

Expected validation failures carried forward:
- tests/gui/TacticalGuiLiveTest.cpp: FrontierTests::TacticalGuiLiveTest::testBattleScreenShowModalContainsGtkWindowSetModal (TMF-02 AC2 source-contract test) now fails because it asserts FBattleScreen::ShowModal() still contains the literal string 'gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE)'. This is an EXPECTED CONSEQUENCE of TMFR-01's approved behavior change: the plan's TMFR-01 acceptance criteria explicitly require 'The GTK modal grab is no longer used to enforce battle modality' -- removing that exact call is the whole point of this subtask. Per repository instruction ('Do not modify test files; behavioral coverage is owned by the Tester stage'), this implementer run did not edit the test. The Tester must retire or rewrite this obsolete source-contract assertion (it now contradicts the approved TMFR-01 acceptance criteria) and, per the plan's Verification Policy, must NOT replace it with a new source-contract assertion that FBattleScreen::ShowModal() contains a wxWindowDisabler construction as a substitute for real behavioral coverage of modality -- prefer behavioral coverage of the enable/disable bookkeeping where feasible without a live ShowModal() loop.
- tests/gui/BattleSimGuiLiveTest.cpp: FrontierTests::BattleSimGuiLiveTest::testLocalGameDialogLaunchesPredefinedAndCustomModalChains fails on an unrelated 'centered' assertion. Confirmed via `git stash` + clean rebuild that this failure is PRE-EXISTING on the unmodified baseline (same failure, same assertion, same test) and is unrelated to any file touched by TMFR-01 (BattleSimGuiLiveTest / LocalGameDialog are not in the TMFR-01 allowed-file list). This is a pre-existing flake/regression outside TMFR-01 scope, not introduced by this change; it is documented here for visibility but is not this subtask's responsibility to fix.
- tests/SSWTests: FrontierTests::FFleetTest::testDecTransitTime failed once out of three runs; passed on immediate rerun with no code changes in between. FFleetTest is in the strategic module, entirely disjoint from every file touched by TMFR-01, so this is transient/flaky and not attributable to this change.
