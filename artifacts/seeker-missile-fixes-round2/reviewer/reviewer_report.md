Reviewer Report

Feature plan reviewed:
- plans/seeker-missile-fixes-round2-plan.md (Seeker Missile Fixes Round 2; subtasks SMF-01 through SMF-07)

Inputs reviewed:
- Governing plan: plans/seeker-missile-fixes-round2-plan.md (features 1-8, 7 subtasks, resolved design decisions, report-cleanup scope)
- All 7 subtask artifact sets in artifacts/seeker-missile-fixes-round2/SMF-01..SMF-07 (implementer/tester/documenter/verifier reports + result JSON)
- All 7 verifier verdicts: every subtask PASS, 0 blocking findings (SMF-03 had 2 WARNING + 3 NOTE, all non-blocking)
- Delivered source: include/Frontier.h (BS_PlaceMines, BS_PlaceSeekers), include/tactical/FTacticalGame.h + src/tactical/FTacticalGame.cpp, include/tactical/FBattleScreen.h + .cpp, src/tactical/FBattleDisplay.cpp, src/tactical/FBattleBoard.cpp + include/tactical/FBattleBoard.h
- Documentation: AGENTS.md SMF-02 locked contract entry, doc/DesignNotes.md design notes
- Tests: tests/tactical/FTacticalGameMechanicsTest.*, FTacticalBattleDisplayFireFlowTest.*, FTacticalSeekerMovementTest.cpp, tests/gui/TacticalGuiLiveTest.*
- Independent re-validation: built and ran tests/tactical/TacticalTests => OK (198 tests), clean compile with no warnings

Overall feature completeness:
- COMPLETE. All 8 plan features map to delivered code across SMF-01..SMF-07, each verified directly against source (not only via verifier labels).
- SMF-01 (two-phase deployment state machine): BS_PlaceSeekers added to include/Frontier.h alongside BS_PlaceMines; beginSeekerPlacement(), completeMinePlacement() (advances to seeker phase or skips when empty), completeSeekerPlacement() (advances to BS_SetupAttackFleet) present in FTacticalGame.h with M/SM source filtering and empty-phase skip; FBattleScreen delegations present. Model stays wx-free.
- SMF-02 (two-phase deployment UI): drawPlaceSeekers() added, draw() dispatches BS_PlaceSeekers; m_buttonMinePlacementDone ('Mine Placement Done') and m_buttonSeekerPlacementDone ('Seeker Placement Done' routing to completeSeekerPlacement()) wired with per-phase show/hide; Decision-2 prompt and selection wording present verbatim; no 'Weapon Placement Done' combined-label path remains anywhere in src/ or tests/.
- SMF-03 (pending-seeker list relocation): drawOffensiveSeekerPendingRows removed from drawCurrentShipStats and relocated into draw() at (leftOffset, getActionButtonRowBottom()+BORDER), left of ship-status with height auto-expansion; recall path preserved.
- SMF-04 (per-activation-phase tracking): activationPhaseIndex field on FTacticalSeekerMissileState, m_seekerActivationPhaseIndex counter, stamping on activation, getActiveSeekersByMovingPlayerThisPhase() this-phase accessor, FBattleScreen read-only delegation, full-list accessor unchanged for movement, field excluded from persistence.
- SMF-05 (activation widget overlap fix): drawSeekerActivation block now starts at getActionButtonRowBottom() instead of BORDER, and the list is sourced from getActiveSeekersByMovingPlayerThisPhase().
- SMF-06 (seeker-hit visual lifecycle): impacting seeker kept in model with movementPath; m_ui->requestRedraw() called at all 3 resolution sites in FTacticalGame.cpp before/after dialogs; drawSeekerPaths gate widened to PH_MOVE || PH_SEEKER_ACTIVATION; model stays wx-free.
- SMF-07 (move-count overlay + cleanup): drawSeekerMoveCountOverlay() draws movementPath.size()-1 as an upright count with vertical stacking when co-located; drawSeekerPaths() Doxygen documents cyan #00CCCC; the weak cross-turn assertion in FTacticalSeekerMovementTest.cpp strengthened to movementPath.size() >= 2.
- Report-cleanup scope (item 8) satisfied: drawSeekerPaths #00CCCC Doxygen added (SMF-07); FTacticalSeekerMovementTest assertion strengthened (SMF-07); getAllPendingOffensiveFireSeekerHexes signature prose corrected (no spurious leading const; SMF-03); obsolete combined-label assertion removed from production behavior. SMC03/SMC06 process backfill correctly excluded per user decision.
- Cross-subtask integration is consistent: SMF-02 consumes SMF-01 states/methods; SMF-05 consumes SMF-04's this-phase accessor; shared-file serialization order (FTacticalGame/FBattleScreen, FBattleDisplay, FBattleBoard) was honored. Module boundary rule preserved (no wx in tactical model).
- Independent validation reproduces the claimed 198/198 tactical tests passing. GUI suite reported 9 pre-existing unrelated failures and 0 new failures across subtasks; not re-run here (requires X/xvfb) but consistently corroborated by all subtask verifiers.

Findings

BLOCKING
- None

WARNING
- tests/gui/TacticalGuiLiveTest.cpp:1624 - static_cast downcast from FBattleDisplay* to FBattleDisplayTestPeer* on an object not of the derived type (technically UB).
  Pre-existing test-probe idiom flagged by the SMF-03 verifier; harmless in practice (no added data/virtuals) but standard-undefined. Test-only, not a product defect. Carried as a non-blocking follow-up.
- src/tactical/FBattleDisplay.cpp:662 - In LOWER_PANEL_LAYOUT_STACKED (narrow-panel) mode the pending list and ship-stats share leftOffset, so the 'left of ship-status' guarantee degrades.
  Functional in the normal wide/right-split case (the in-game configuration); only a layout-mode ambiguity for very narrow panels, with no hard vertical overlap. Acceptance criterion met for the common case; minor edge case for follow-up if narrow panels become supported.

NOTE
- tests/gui/TacticalGuiLiveTest.cpp:1455 - Legacy test name testMinePlacementDoneButtonLabelReflectsOrdnanceTypes was repurposed (now asserts the two-phase mine/seeker button lifecycle) rather than removed; AGENTS.md describes the new behavior under the same name.
  Plan item 8 said to 'remove/replace' the obsolete combined-label entry. The intent (no stale combined-label assertion remains; AGENTS.md accurate) is satisfied via replace-in-place, but the name no longer matches its behavior and could mislead future readers. Cosmetic; non-blocking.
- src/tactical/FBattleDisplay.cpp:600 - m_pendingSeekerRecallRegions is cleared twice (in draw() and at the top of drawOffensiveSeekerPendingRows()).
  Harmless redundancy noted by the SMF-03 verifier; no functional impact.

Missed functionality or edge cases:
- No missed functionality found. Every plan feature (1-8) and every per-subtask acceptance criterion is implemented and verified, and the report-cleanup items folded into SMF-02/03/07 were all addressed.
- GUI-visible layout/visual behaviors (pending-list visibility, activation-widget non-overlap, seeker-impact persistence, move-count overlay) are validated at source-contract / model level plus offscreen wxMemoryDC checks rather than by full visual playback; this matches the repository's stated 'no fully automated end-to-end wx GUI playback' approach and the plan's coverage assumption (Assumption 7), so it is an accepted methodology limit, not a gap.
- Narrow-panel stacked-layout placement of the pending list (WARNING above) is the only behavior where an acceptance phrase ('left of ship-status') is not strictly guaranteed; it does not affect the normal in-game layout.

Follow-up feature requests for planning:
- (Optional, low priority) Replace the FBattleDisplayTestPeer static_cast downcast in tests/gui/TacticalGuiLiveTest.cpp with a defined access pattern (public const accessors on FBattleDisplay, or a panel-factory injection seam in FBattleScreen) to remove the standard-undefined-behavior test probe.
- (Optional, low priority) Either rename testMinePlacementDoneButtonLabelReflectsOrdnanceTypes to reflect its current two-phase button-lifecycle assertions, or split it, and align the AGENTS.md SMF-02 entry name accordingly.
- (Optional, low priority) If narrow-panel (LOWER_PANEL_LAYOUT_STACKED) deployment becomes a supported in-game configuration, define and enforce the pending-list horizontal placement relative to ship-status in that mode.

Artifacts written:
- artifacts/seeker-missile-fixes-round2/reviewer/reviewer_report.md
- artifacts/seeker-missile-fixes-round2/reviewer/reviewer_result.json

Final outcome:
- PASS
