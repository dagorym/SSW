Verifier Report

Scope reviewed:
- SMF-03: Pending-seeker list widget relocation. drawOffensiveSeekerPendingRows() moved from drawCurrentShipStats() into draw() inside a PH_ATTACK_FIRE guard, placed at (leftOffset, getActionButtonRowBottom()+BORDER) left of ship-status widget. Panel auto-expands height when rows exceed requestedDisplayHeight. Recall trigger via m_pendingSeekerRecallRegions click path unchanged. Implementer commit 7c57b64; Tester commit 271844b; Documenter commit 3caeca0.

Acceptance criteria / plan reference:
- plans/seeker-missile-fixes-round2-plan.md, SMF-03 subtask acceptance criteria

Convention files considered:
- AGENTS.md
- CLAUDE.md

Findings

BLOCKING
- None

WARNING
- tests/gui/TacticalGuiLiveTest.cpp:1624 — Unsafe static_cast from FBattleDisplay* (base) to FBattleDisplayTestPeer* (derived subclass) is technically undefined behavior.
  FBattleScreen creates a real FBattleDisplay (src/tactical/FBattleScreen.cpp:138), not a FBattleDisplayTestPeer. The static_cast at line 1624 performs a downcast on an object that is not of the derived type. In practice, since FBattleDisplayTestPeer adds no data members or virtual methods, all common compilers handle this correctly and the pattern is a recognized test-probe idiom. However, it is undefined behavior per the C++ standard. A safer pattern would be to expose the protected members via public const accessors on FBattleDisplay directly, or to have FBattleScreen accept a panel factory so a FBattleDisplayTestPeer could be injected.

- src/tactical/FBattleDisplay.cpp:662 — In LOWER_PANEL_LAYOUT_STACKED mode, the pending list and ship-stats share the same leftOffset x-coordinate, making the acceptance criterion "left of ship-status widget" semantically imprecise for narrow panels.
  In right-split mode (the normal wide-panel case), drawOffensiveSeekerPendingRows draws at leftOffset while ship-stats draw at shipStatsLeftMargin (much further right), satisfying "left of ship-status." In stacked mode (triggered when the panel is too narrow for a side-by-side layout), both regions share leftOffset at the x axis, with the pending list starting 3px above ship-stats (BORDER=5 vs ACTION_PROMPT_BUTTON_GAP=8). They do not hard-overlap vertically in normal usage, but the "left of" placement guarantee breaks down for narrow panels. This is a layout-mode ambiguity rather than a functional defect in the common case.

NOTE
- tests/gui/TacticalGuiLiveTest.cpp:1558 — AC2 positional verification (region left of ship-stats, below button row) is deferred to model tests; the GUI live test only smoke-checks buttonRowBottom > 0.
  The GUI test at lines 1672-1683 checks buttonRowBottom > 0 and >= actionPromptReservedBottomY(), not that recall regions appear left of ship-stats or below the button row. Source-contract test at FTacticalBattleDisplayFireFlowTest.cpp:652 confirms the call signature in draw() but not the pendingRegionTop assignment. Model-level test (FTacticalBattleDisplayFireFlowTest.cpp:748) covers recall mechanics. Acceptable given GUI layout position is inherently display-size-dependent.

- src/tactical/FBattleDisplay.cpp:1606 — Early return when pending.empty() skips height-expansion logic but still draws the "Pending seeker deployments:" header line.
  When the pending list is empty, the function draws the header and "None" then returns early without checking whether those lines extend below requestedDisplayHeight. Overflow is not expected in practice since the panel is already sized for ship-stats. Non-issue functionally.

- src/tactical/FBattleDisplay.cpp:600 — Double-clear of m_pendingSeekerRecallRegions: once at the top of draw() and again at the top of drawOffensiveSeekerPendingRows().
  Harmless redundancy. The clear in draw() ensures regions are empty when drawOffensiveSeekerPendingRows is not reached. No functional impact.

Test sufficiency assessment:
- SUFFICIENT. 196/196 tactical tests pass; 0 new GUI failures. Source-contract test verifies drawCurrentShipStats does not call drawOffensiveSeekerPendingRows and draw() contains the relocated call. Model-level recall is covered by testOffensiveSeekerDeploymentRuntimeFlowSupportsPendingRecallAndCommit. GUI live test covers empty-pending draw, non-PH_ATTACK_FIRE guard, and failed-recall guard. Click-deletable recall path verified at source-contract level.

Documentation accuracy assessment:
- ACCURATE. AGENTS.md SMF-03 contract entry correctly describes call site, height behavior, and test contracts. DesignNotes.md design note accurately describes relocation rationale, layout, height expansion, recall mechanics, and validation results. FBattleDisplay.h Doxygen updated correctly.

Artifacts written:
- artifacts/seeker-missile-fixes-round2/SMF-03/verifier_report.md
- artifacts/seeker-missile-fixes-round2/SMF-03/verifier_result.json

Verdict:
- PASS
