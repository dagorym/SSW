Reviewer Report

Feature plan reviewed:
- plans/pregame-ordnance-placement-plan.md (governing plan; feature 'Pre-Game Ordnance Placement Fixes', subtasks PGS-01 through PGS-04)

Inputs reviewed:
- Governing plan: plans/pregame-ordnance-placement-plan.md (Feature Restatement, Resolved Design Decisions D1-D5, Carried-Over Items Carry-A/Carry-B, Overall Acceptance Criteria 1-8, per-subtask acceptance).
- Feature commits 32a1da6..HEAD on sf2 (33 commits: PGS-01 6f9241d.., PGS-02 8627871.., PGS-03 db2ec98.. incl. remediation pass-2 94de5a9.., PGS-04 1a34316..).
- Merged production diff: src/tactical/FTacticalGame.cpp, include/tactical/FTacticalGame.h, src/tactical/FBattleDisplay.cpp, include/tactical/FBattleDisplay.h, src/tactical/FBattleScreen.cpp, include/tactical/FBattleScreen.h.
- Merged test diff: tests/tactical/FTacticalGameMechanicsTest.{h,cpp}, tests/tactical/FTacticalPreGameOrdnanceTest.{h,cpp}, tests/tactical/TacticalTests.cpp, tests/gui/TacticalGuiLiveTest.{h,cpp}.
- Merged doc diff: doc/DesignNotes.md (+69), doc/UsersGuide.md (+5/-1). doc/rules/tactical_operations_manual.md verified UNCHANGED (empty diff vs pre-feature base).
- Per-subtask artifacts under artifacts/pregame-ordnance-placement/PGS-0N/ (implementer/tester/documenter/verifier reports + result JSON); PGS-03 history/pass-1 superseded first pass.
- Live build + execution: 'make tactical-tests' (clean, no warnings on new files) and ./tactical/TacticalTests => OK (222 tests).

Overall feature completeness:
- COMPLETE. All four subtasks are merged into sf2 and the delivered behavior matches the plan's Overall Acceptance Criteria, verified behaviorally (not just by Verifier verdict labels).
- AC1 (mine toggle place/remove, ammo return to placing ship regardless of selected source): PGS-01 fixes the entry-state regression (onSetSpeed no longer nulls m_curShip after a successful beginMinePlacement; guarded setShip(NULL)); PGS-04 implements any-source mine removal in placeOrdnanceAtHex() returning ammo to the recorded placing source. Behavioral tests testPreGameMinePlacementPreservesShipAfterBeginMinePlacement and testMineRemovalWithNonPlacingSourceSelectedRestoresAmmoToPlacingShip confirm both.
- AC2 (additive seeker stacking, no board-click removal): PGS-03 short-circuits placeOrdnanceAtHex() to placeSeekerFromSelection() during BS_PlaceSeekers; testPreGameSeekerPlacementIsAdditive confirms ammo decrement and record accumulation per click on the same hex.
- AC3 (multi-ship source selection; single-ship auto-select): PGS-02 fixes row clipping via GetParent()->SendSizeEvent() in applyRequestedDisplayHeight(); behavioral GUI test testPlacementSourceRowsArePopulatedAndClickSelectionUpdatesSources covers two source rows, click-selection updating m_curShip/m_curWeapon, non-clipped regions, source-switch board placement, and single-ship auto-select.
- AC4 (centered seeker undeploy list, undeploy-one returns ammo): PGS-04 adds getPlacedSeekerHexGroups()/recallPlacedSeekerAtHexSource() (model), FBattleScreen forwarders, and drawPlaceSeekers()/checkPreGameSeekerRecallSelection() centered list; FTacticalPreGameOrdnanceTest covers grouping, decrement+ammo restore, stacked-recall-leaves-remainder, and recall-returns-false-when-empty.
- AC5 (placing-ship recorded; ammo returned to correct ship on all removals/undeploys): placed-ordnance source records drive restoreAmmoForSource() on both mine removal and seeker undeploy; confirmed by the PGS-04 tests.
- AC6 (behavioral coverage for the real mine-entry regression, additive stacking, undeploy-by-list): all three present and behavioral; the PGS-01 test replicates the GUI beginMinePlacement()+setShip(NULL) sequence (Part 1 proves the bug drops the click, Part 2 proves the fix records the mine).
- AC7 (Carry-A and Carry-B): Carry-A done in PGS-02 Tester stage (commit d66e1fc) reworded the stale [DEFECT] comment/assert while preserving the heightAfterActivation >= heightBeforeActivation regression guard. Carry-B satisfied by PGS-04 undeploy-by-list behavioral tests (board-click undo path no longer exists).
- AC8 (rules manual untouched): doc/rules/tactical_operations_manual.md diff is empty vs the pre-feature base.
- Dependency ordering, file-boundary constraints (tactical model/HUD/delegation only), and serial execution were all respected. Live tactical suite passes (222 tests).
- Security posture: the change surface is tactical model state + lower-panel HUD rendering/click routing only. No new trust boundary, network, persistence, serialization, or auth surface is introduced. The plan marked no subtask as requiring specialist security review, so no Security stage was run; that omission is appropriate for this change surface and no specialist security review was warranted.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/tactical/FTacticalGame.cpp:3351 - Now-unreachable 'else if (getState() == BS_PlaceSeekers)' branch in placeOrdnanceAtHex(). PGS-03's early return at line 3298 takes all BS_PlaceSeekers SM placements, so the removePlacedOrdnanceForSelection() SM-removal/rebuild path for that state can no longer execute.
  Dead code only; no behavioral impact and no acceptance criterion is affected. It is mildly misleading to future maintainers and could be pruned in a future cleanup. Flagged non-blocking by the PGS-03 Verifier and confirmed here.
- tests/tactical/FTacticalPreGameOrdnanceTest.h:15 - Include-guard typo: FTACTICALPREAGAMEORDNANCETEST_H_ (PREAGAME instead of PREGAME).
  Cosmetic. The guard is self-consistent (define/ifndef/endif all use the same misspelled token) so it functions correctly and the suite compiles and passes; it only deviates from the intended class-name-derived spelling. Confirmed by the PGS-04 Verifier; harmless.
- src/tactical/FTacticalGame.cpp:3306-3358 - Low-risk mine-removal fallthrough: if a hex is present in m_minedHexList but no matching FWeapon::M placed-ordnance record exists, the PGS-04 any-source removal loop finds nothing and control falls through to removePlacedOrdnanceForSelection(), which only matches the current selection, so the stale minefield could remain when a non-placing source is selected.
  Reachable only from an inconsistent internal state (mined-hex set and placed-ordnance vector out of sync) that the normal placement flow never produces, since every mine placement appends both records together. No observed defect and no acceptance criterion is broken in practice. Flagged non-blocking by the PGS-04 Verifier; a defensive cleanup (erase from m_minedHexList even when no placed record is found) could be considered in a future hardening pass.

Missed functionality or edge cases:
- No missed functionality found at the feature level. Every Overall Acceptance Criterion (1-8) is delivered and backed by behavioral coverage that exercises the real model/HUD flow.
- No cross-subtask integration gap found: PGS-01's stabilized entry state, PGS-02's visible source rows, PGS-03's additive seeker dispatch, and PGS-04's undeploy list + any-source mine removal compose coherently in placeOrdnanceAtHex()/drawPlaceSeekers() and are co-verified by the passing tactical suite.
- The three NOTE items above are the only edge cases observed; none rises to a feature-level concern. No specialist security review was required given the tactical model/HUD-only change surface.

Follow-up feature requests for planning:
- (Optional, non-blocking cleanup) Remove the now-unreachable 'else if (getState() == BS_PlaceSeekers)' SM-removal branch in FTacticalGame::placeOrdnanceAtHex() and add a defensive m_minedHexList erase for the rare mined-hex-without-placed-record fallthrough; correct the include-guard spelling in tests/tactical/FTacticalPreGameOrdnanceTest.h (PREAGAME -> PREGAME). All three are cosmetic/hardening only and do not block this feature.

Artifacts written:
- artifacts/pregame-ordnance-placement/reviewer_report.md
- artifacts/pregame-ordnance-placement/reviewer_result.json

Final outcome:
- PASS
