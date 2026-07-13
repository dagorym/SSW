Reviewer Report

Feature plan reviewed:
- plans/phase4-tactical-gui-hybrid-cleanup-plan.md (Phase 4 - Tactical GUI Hybrid Cleanup; H9 + H7 + §3.6 dead-code cleanup); planner baseline commit 70637a7e

Inputs reviewed:
- Governing plan: plans/phase4-tactical-gui-hybrid-cleanup-plan.md (all acceptance criteria, Dependency Ordering, Overall Documentation Impact, Out of Scope)
- P4-1 (H9) artifacts + impl commit bb3ebfa5: include/tactical/FBattleDisplay.h, src/tactical/FBattleDisplay.cpp, include/tactical/FBattleScreen.h, src/tactical/FBattleScreen.cpp
- P4-2 (H7) artifacts (incl. pass-1 history) + impl commits b3ee8cee/a2161ae1: include/tactical/FBattleBoard.h, src/tactical/FBattleBoard.cpp, include/FGamePanel.h, src/FGamePanel.cpp
- P4-3 (§3.6) artifacts + impl commit 35bc7411: FMainBattleSimFrame.cpp deletion, BattleSim.cpp, FMainFrame.cpp/.h, 19 gui/strategic/ships/core .cpp files, FBattleScreen.cpp commented-cerr removal
- Documentation: doc/synthesized-roadmap.md (H7/H9/§3.6/Part III item 4 RESOLVED), AGENTS.md contributor notes, doc/DesignNotes.md validation log
- Behavioral tests: tests/gui/TacticalGuiLiveTest.{h,cpp}, tests/tactical/FTacticalBattleBoardRendererDelegationTest.cpp, FTacticalBattleDisplayFireFlowTest.{h,cpp}
- All six per-subtask Verifier reports/results (P4-1 PASS; P4-2 CONDITIONAL->PASS after AC5 remediation; P4-3 PASS)
- Integrated build/test gate: make check (SSWTests 245, TacticalTests 253, GuiTests 81 under xvfb) all PASS

Overall feature completeness:
- COMPLETE. All three findings are fully delivered and integrated with no cross-subtask conflict.
- H9 (P4-1): m_first is gone from FBattleDisplay (only historical comment references remain); all seven phase buttons and both turn buttons bind click handlers exactly once in the ctor; new public non-paint updateForPhase() drives Show/Enable/turn-panel state purely from FBattleScreen::getState()/getPhase() and model queries with no DC creation, wired into setState()/setPhase() (via reDraw()) and onSize() (direct). AC1-7 met.
- H7 (P4-2): drawCenteredOnHex/drawShips/drawSeekerMissiles take a caller-supplied wxDC&; onPaint uses a single wxAutoBufferedPaintDC with SetBackgroundStyle(wxBG_STYLE_PAINT) in the ctor; zero wxClientDC remains in FBattleBoard.cpp; FGamePanel::onPaint uses wxPaintDC on all platforms with the dead LINUX branch and commented block removed. AC1-6 met. Pass-1 AC5 behavioral-render gap closed by testShipAndPlanetIconsRenderThroughCallerSuppliedDC (offscreen wxMemoryDC), proven load-bearing.
- §3.6 (P4-3): src/FMainBattleSimFrame.cpp deleted (no longer git-tracked, build still links); BattleSim.cpp ghost event-table block removed; ~120 commented-out + the 3 live std::cerr sites removed across the allowed files; all four user-visible dialog typos corrected (destroyed/Statistics/would/capital).
- Cross-subtask integration: P4-1 and P4-3 edits to src/tactical/FBattleScreen.cpp are in disjoint regions (P4-1 = updateForPhase wiring in reDraw/setState/setPhase/onSize; P4-3 = 4 commented-cerr deletions); merged linearly with no conflict. File-ownership boundaries respected.
- Module-boundary/wx-isolation policy honored: no new wx #include added to any of the five pure non-GUI modules (core/strategic/ships/weapons/defenses); the only new wx include is <wx/dcbuffer.h> in src/tactical/FBattleBoard.cpp, an already-approved tactical wx-bridge file.
- Documentation close-out complete: roadmap marks H7 (l.125), H9 (l.127), the §3.6 dead-code bullet (l.436-445), and Part III 'Now' item 4 (l.712-719) RESOLVED with per-subtask artifact references; AGENTS.md contributor notes updated for bind-once/updateForPhase() and single-buffered-paint-DC contracts; DesignNotes.md carries validation-log entries; doc/rules/tactical_operations_manual.md is byte-for-byte untouched (non-negotiable constraint honored).
- Behavioral-verification policy satisfied: H9 and H7 have genuine behavioral GUI tests (real phase transitions + offscreen-DC pixel assertions); source-contract tests only supplement.
- Deferred/out-of-scope items correctly left undone: no drawClickableRowList renderer, no FromDIP/DPI pass, no facade slimming, no src/gui migration, placeMineAtHex dead branch retained, captialShipCount identifier not renamed, comment-only typos (destoryed/Statisitics in comments) left unchanged.
- Integrated make check passes green: SSWTests 245, TacticalTests 253, GuiTests 81 (including the new H9 parity/onsize/done-click/seeker/mine tests and the H7 ship-planet-dc test).

Findings

BLOCKING
- None

WARNING
- None

NOTE
- include/FMainFrame.h:162-167 - P4-3 edited include/FMainFrame.h, which is not in P4-3's literal allowed-files list (only src/FMainFrame.cpp was listed).
  The edit is a doc-comment-only Doxygen header update (Last Modified + @author on onShowPlayers()) mandated by AGENTS.md because that function's body changed (live std::cerr removal). It is directly tied to the allowed src/FMainFrame.cpp change, introduces no functional/logic change, and does not overlap any other subtask's files. Not a scope violation of concern; recorded only for traceability.
- tests/strategic/FGameTest.cpp - Two dice-driven, unseeded-RNG combat-cleanup tests (testCleanUpShipsUpdatesLossCountersWithoutInflation, testCleanUpShipsExcludesSatharFightersFromTendaySatharCounter) are pre-existing flaky and unrelated to Phase 4; both passed in this reviewer's make check run.
  P4-3's only touch to src/strategic/FGame.cpp is comment-only, so this determinism risk is not a Phase 4 regression. It is a repository-level test-hygiene issue worth a separate Planner follow-up (seed the RNG) to prevent intermittent CI red.

Missed functionality or edge cases:
- None. Every plan acceptance criterion across P4-1/P4-2/P4-3 is satisfied, all documentation obligations are met, and all explicitly-deferred items were correctly left out of scope. No cross-subtask gap, integration conflict, wx-isolation breach, or documentation omission was found.
- The only pre-existing flakiness (unseeded FGameTest RNG) is outside Phase 4's change surface and passed this run; it is captured as a NOTE plus a Planner follow-up rather than a feature gap.

Follow-up feature requests for planning:
- Test-determinism hardening: seed (or inject a deterministic RNG into) the dice-driven FGameTest combat-cleanup tests (testCleanUpShipsUpdatesLossCountersWithoutInflation, testCleanUpShipsExcludesSatharFightersFromTendaySatharCounter) so the strategic combat-cleanup suite is reproducible and cannot intermittently fail CI. Pre-existing and unrelated to Phase 4; low risk, model-layer only.

Artifacts written:
- artifacts/phase4-tactical-gui-hybrid-cleanup/reviewer_report.md
- artifacts/phase4-tactical-gui-hybrid-cleanup/reviewer_result.json

Final outcome:
- PASS
