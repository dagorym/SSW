Verifier Report

Scope reviewed:
- Second (final) Verifier pass for P5-5, reviewing the Verifier-driven remediation that defers FMainFrame::onOpen()'s m_drawingPanel->setGame(m_game) call from immediately after FGame::create() to the success-only branch (after load()==0 and is.is_open()), closing the onOpen()-reachable NULL-FMap paint crash flagged by the pass-1 Security review.
- Combined diff reviewed: 673a9832 (Archive pass-1 stage artifacts) .. 038d2e27 (pass-2 Security PASS), covering Implementer commit c28c489a, Tester commit 925ad377, Documenter commit a8d3c55b, and Security commit 038d2e27.
- Files in scope: src/FMainFrame.cpp, include/FMainFrame.h (Implementer); tests/gui/StrategicGuiLiveTest.cpp/.h (Tester); AGENTS.md, doc/deferred-tasks.md (Documenter); plus the P5-5 stage artifact files (implementer/tester/security/documenter reports and results) under artifacts/phase5-save-format-hardening/P5-5/.
- Consulted the committed Security re-run findings (security_report.md / security_result.json, PASS, 0 blocking / 0 warning / 3 note) alongside independent source review and an independent full test-suite re-run.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md -- Section 4 (P5-5 acceptance criteria), Section 7 (Security Notes: untrusted-input path).
- Coordinator-provided acceptance criteria for this pass-2 remediation (onOpen setGame deferral, behavioral crash-window coverage, FGame.h/FGame.cpp non-regression, make check green, doc/deferred-tasks.md follow-up records, Doxygen/AGENTS.md accuracy).
- artifacts/phase5-save-format-hardening/P5-5/history/pass-1/verifier_report.md and security_report.md (pass-1 baseline findings this pass remediates).

Convention files considered:
- AGENTS.md (Doxygen/@author/@date policy; Single-Source-of-Truth Rule; GUI-tests invariant bullet under Testing; Behavioral Verification Is Mandatory policy)
- CLAUDE.md (pointer to AGENTS.md)
- doc/deferred-tasks.md (standing Description/Origin/Why deferred/Target phase convention, matched against existing SF-enum-codes/SF-fhexmap entries)
- doc/rules/tactical_operations_manual.md (confirmed untouched -- non-negotiable constraint)

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/FMainFrame.cpp:255-263 - FMainFrame::m_game (distinct from the panel's own m_game) is still set synchronously at onOpen() line 156, before load() runs, so onLeftDClick()/onLeftUp() (guarded on FMainFrame::m_game != NULL) could in principle call into a half-built FGame if a mouse event were delivered during the load-error nested modal loop.
  This is pre-existing behavior unchanged by this remediation (FMainFrame::m_game was already assigned at this point before pass 2) and is not exercised in practice because GTK's modal dialogs capture input, blocking delivery of mouse events to the underlying frame. It is out of scope for this pass's acceptance criteria (which target the FGamePanel paint crash specifically) and is not a regression; noting it only for completeness alongside the already-tracked SF-nullfmap-paint-guard follow-up.
- artifacts/phase5-save-format-hardening/P5-5/history/pass-1/ - Pass-1 history artifacts remain unmodified, as required.
  Confirms the archival record of the pass-1 review outcome is preserved intact for audit purposes; verified via git diff scoping and directory listing showing no changes under history/pass-1/.
- artifacts/phase5-save-format-hardening/P5-5/security_report.md:1-49 - Security re-run findings independently corroborated: 0 blocking, 0 warning, 3 note, FGame layer confirmed byte-identical to pass-1 by this Verifier's own git diff (673a9832..038d2e27 and 7baddaca..038d2e27 over src/strategic/FGame.cpp and include/strategic/FGame.h both produce empty output).
  Independent corroboration (rather than trusting the Security report alone) satisfies the Verifier's obligation to independently confirm FGame.h/FGame.cpp are unchanged and the magic/version gate plus aggregate load-error propagation are untouched.

Test sufficiency assessment:
- Sufficient. The mitigation's proof is a genuine behavioral test, not source-inspection: tests/gui/StrategicGuiLiveTest.cpp::testMainFrameOnOpenFailedLoadWithFrameShownSurvivesForcedRepaint (registered in StrategicGuiLiveTest.h's CPPUNIT_TEST_SUITE) shows the FMainFrame, installs a RepaintPumpTimer that forces frame->Refresh() every 5ms across onOpen()'s entire nested modal sequence (native file chooser then the load-error dialog) while opening a corrupt save file, and asserts the process survives plus all 7 game-dependent menu items stay disabled -- directly exercising the exact FGamePanel::onPaint -> WXGameDisplay::draw -> WXMapDisplay::getScale -> FMap::getMaxSize() NULL-this crash window pass-1 identified.
- Independently re-ran the full suite synchronously from this worktree after `make all_clean`: `make check` from the repo root built everything and ran all three CppUnit suites in sequence with zero failures -- tests/SSWTests -> OK (289 tests), tests/tactical/TacticalTests -> OK (253 tests), tests/gui/GuiTests under xvfb-run -a -> OK (83 tests). GuiTests count increased from pass-1's independently-confirmed 82 (per artifacts/phase5-save-format-hardening/P5-5/history/pass-1/verifier_report.md) to 83, which is exactly +1 and directly corroborates that the new behavioral test is compiled, registered, and executed as part of the suite (not merely present in source).
- The pre-existing sibling test testMainFrameOnOpenFailedLoadResetsGameAndLeavesMenuItemsDisabled (unshown-frame failed-load path) and testMainFrameOnOpenConfirmLoadsFromFullPathAndRestoresPostLoadState (success path) are unmodified by this diff and passed as part of the same green GuiTests run, confirming no regression to either the pre-existing failed-load coverage or the success path.
- No acceptance criterion in this pass is backed solely by source-inspection; the one behavior-critical claim (crash window closed) has direct behavioral coverage as described above.

Documentation accuracy assessment:
- In-code Doxygen on FMainFrame::onOpen() (include/FMainFrame.h lines 95-117, src/FMainFrame.cpp lines 154-199 inline comments) accurately describes the deferred-setGame mitigation, the failure-path NULL-panel-pointer invariant, and the success-path setGame() timing; independently verified against the actual code by direct reading, not just the comment text.
- New test Doxygen (tests/gui/StrategicGuiLiveTest.h testMainFrameOnOpenFailedLoadWithFrameShownSurvivesForcedRepaint, and the RepaintPumpTimer class header in StrategicGuiLiveTest.cpp) accurately describes what the test does and why, matching the actual test body reviewed.
- AGENTS.md's GUI-tests invariant bullet under Testing was updated (verified via git diff 673a9832..038d2e27 -- AGENTS.md) to add a sentence describing the new sibling test and correctly changes 'is a pre-existing' crash description to 'was a' crash for the now-mitigated onOpen()-reachable trigger, while accurately noting the root-cause NULL-FMap paint-path guard remains a tracked follow-up (SF-nullfmap-paint-guard).
- doc/deferred-tasks.md records both required follow-up defects (SF-nested-load-returns at line 67, SF-nullfmap-paint-guard at line 102), each following the repository's standing Description/Origin/Why deferred/Target phase convention and matching the structure of the pre-existing SF-enum-codes and SF-fhexmap entries. Both descriptions were independently checked for technical accuracy against the code and the pass-1 Security findings they originate from and found correct.
- doc/rules/tactical_operations_manual.md was confirmed untouched (not present in the diff), respecting the repository's non-negotiable constraint.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-5/verifier_report.md
- artifacts/phase5-save-format-hardening/P5-5/verifier_result.json

Verdict:
- PASS
