Reviewer Report

Feature plan reviewed:
- plans/seeker-missile-fixes-round3-plan.md (governing plan; Overall Acceptance Criteria section is the feature-level bar). Five subtasks SMFR-01..SMFR-05, all implemented/tested/documented/verified and merged into coordination base branch sf2 (reviewer HEAD == sf2 tip 3720877).

Inputs reviewed:
- SMFR-01 (lower-panel list-widget visibility): per-subtask implementer/tester/documenter/verifier artifacts plus history/pass-1 and history/pass-2; verifier verdict PASS (1 WARNING for a stale test comment).
- SMFR-02 (pre-game mine/seeker placement regression): per-subtask artifacts; verifier verdict PASS, 0 warnings; behavioral test testPreGameOrdnancePlacementRecordingBehavior.
- SMFR-03 (minefield damage summary + triggered-hex highlight): per-subtask artifacts plus history/pass-1; verifier verdict PASS, 0 warnings; behavioral test testMineEncounterHighlightHexesNonEmptyDuringShowDamageSummaryCallback.
- SMFR-04 (persist seeker speed label): per-subtask artifacts plus history/pass-1; verifier verdict PASS, 0 warnings; behavioral GUI test testSeekerMoveCountOverlayRendersInAllBattlePhases.
- SMFR-05 (seeker movement-path persistence lifecycle): per-subtask artifacts plus history/pass-1; verifier verdict PASS, 0 warnings; behavioral GUI test testSeekerPathRendersInPHMoveWithMovementPath.
- Direct source inspection of src/tactical/FTacticalGame.cpp, FBattleBoard.cpp, FBattleDisplay.cpp, FBattleScreen.cpp and their headers across master..HEAD (4331 insertions / 192 deletions in the tactical surfaces over the feature range).
- Built and ran the tactical test suite: 214/214 tactical tests PASS (cd tests && make tactical-tests && ./tactical/TacticalTests). Built the GUI test module clean. GUI suite not executed here because xvfb-run is not installed in this environment; GUI render tests for SMFR-01/04/05 compile cleanly and were recorded PASS under xvfb by their respective verifiers (9-10 pre-existing strategic/battlesim failures are out of scope).
- AGENTS.md SMFR-02/03/04/05 and TSM-010 (SMFR-05) contract entries.

Overall feature completeness:
- Complete. All eight Overall Acceptance Criteria are satisfied by the delivered, merged work taken together, and all five subtask verifiers returned PASS with zero blocking findings.
- OAC-1 (pre-game placement records ammo/hex/ordnance for BS_PlaceMines and BS_PlaceSeekers, with board highlight for both): SMFR-02 added the missing handleHexClick BS_PlaceSeekers case (src/tactical/FTacticalGame.cpp:3308), extended FBattleBoard::draw() to render placed ordnance for both BS_PlaceMines and BS_PlaceSeekers, and type-filtered the deployable source rebuilds. Confirmed in code and by behavioral test.
- OAC-2 (behavioral test proves recording and fails against the regression): testPreGameOrdnancePlacementRecordingBehavior (FTacticalGameMechanicsTest) constructs a real FTacticalGame, enters both placement phases, clicks hexes, and asserts ammo decrement, getMinedHexes()/getSeekerMissiles() membership, and placed-ordnance recording; verifier confirmed it fails pre-fix (handleHexClick returned false for BS_PlaceSeekers). Registered and passing.
- OAC-3 (placement-removal and seeker activation/deactivation lists render in the visible lower-panel middle region and are clickable): SMFR-01 added auto-expansion to drawPlaceMines/drawPlaceSeekers/drawSeekerActivation plus a max-preservation guard in ensureLowerPanelLayoutState() so expansions survive the stats layout pass; backed by behavioral GUI test testOrdnancePlacementAndActivationPanelHeightAutoExpands.
- OAC-4 (minefield passage shows the damage summary including zero-damage and highlights triggered hexes in pre-game-placement green, clearing on dialog close): SMFR-03 makes applyMineDamage() show the summary unconditionally, tracks m_lastTriggeredMineHexes (populated before redraw, cleared after showDamageSummary returns), and FBattleBoard::drawTriggeredMineHexes shades hexes green inside the BS_Battle guard. Backed by end-to-end behavioral test.
- OAC-5 (seeker speed label visible for every active seeker from activation to impact/exhaustion): SMFR-04 moved drawSeekerMoveCountOverlay outside the phase guard with a movementAllowance fallback; backed by pixel-level behavioral GUI test plus a model field test.
- OAC-6 (seeker paths persist through ICM/summary; impacting paths clear on dialog close; non-impacting clear at completeMovePhase without losing bookkeeping): SMFR-05 adds clearNonImpactingSeekerMovementPaths() called from completeMovePhase() after applyMovementSeekerDamage() and before applyMineDamage(); only movementPath is cleared, movementAllowance/movementTurn preserved. Backed by behavioral model tests and an AC1 render-side behavioral GUI test.
- OAC-7 (existing mine damage, seeker detonation, ICM allocation, destroyed-ship cleanup run exactly once per resolution): the canonical completeMovePhase ordering (applyMovementSeekerDamage -> clearNonImpactingSeekerMovementPaths -> applyMineDamage) is intact at src/tactical/FTacticalGame.cpp:3151-3156, and TSM-010 plus per-subtask verifiers confirmed the once-per-resolution invariant is preserved.
- OAC-8 (no change edits doc/rules/tactical_operations_manual.md): confirmed byte-identical to master across the feature; git diff master..HEAD for that path is empty and there are zero commits touching it in master..sf2.
- Cross-subtask integration is sound: SMFR-02 placement recording feeds SMFR-03 mine damage + highlight (shared m_lastTriggeredMineHexes / drawTriggeredMineHexes); SMFR-04 label rendering and SMFR-05 path lifecycle co-exist on the same FBattleBoard render helpers without conflict (drawSeekerMoveCountOverlay and drawSeekerPaths are independently gated); SMFR-01's lower-panel layout is not regressed by later subtasks (214 tactical tests green).
- No security review was required: the plan marks no subtask for specialist security review, and the changes are tactical GUI rendering and in-process state-sequencing fixes with no new trust boundary, untrusted-input parsing, serialization-format change, or privilege surface. Documentation obligations (header Doxygen, AGENTS.md contract entries, optional UsersGuide notes) are met per the documenter/verifier records.

Findings

BLOCKING
- None

WARNING
- tests/gui/TacticalGuiLiveTest.cpp:1893-1900 - Stale inline comment and assertion message in the SMFR-01 drawSeekerActivation behavioral test still describe the (now-fixed) panel-expansion behavior as an active '[DEFECT: ...]'.
  The assertion (heightAfterActivation >= heightBeforeActivation) is correct and passes because the SMFR-01 max-preservation guard in ensureLowerPanelLayoutState() fixed the clobbering, but the comment text claims the defect 'currently does not' persist and labels it a DEFECT. This is misleading to future maintainers and risks the assertion being weakened or removed on the false belief that it documents a known-broken state. Cosmetic only; no behavior, coverage, or acceptance-criterion impact. This is the known non-blocking residual flagged on SMFR-01's second verifier pass.

NOTE
- src/tactical/FTacticalGame.cpp:3308 - SMFR-02's BS_PlaceSeekers undo/removal path is structurally correct and mirrors the verified mine path but is not independently exercised by a dedicated behavioral test.
  The forward seeker-placement recording path is behaviorally covered; the undo branch is covered only structurally. Low risk because it is a direct mirror of the behaviorally verified mine undo path, but a focused behavioral test would close the gap. Non-blocking.
- tests/gui - GUI render behavioral tests (SMFR-01/04/05) were not re-executed in this review environment because xvfb-run is not installed here.
  These tests are registered, compile cleanly against the merged tree, and were recorded PASS under xvfb by each subtask's verifier (e.g. SMFR-05: 39/48 GUI PASS with 9 pre-existing failures). The 214 model-level tactical tests were re-run and pass here. No evidence of regression; recorded for transparency only.

Missed functionality or edge cases:
- No missed functionality at the feature level. Every Overall Acceptance Criterion maps to delivered, merged code and to a behavioral test for its runtime-behavior claims (placement recording, zero-damage minefield summary + highlight lifecycle, seeker-label persistence across phases, and seeker-path persistence/clearing lifecycle). Source-contract tests are used only as supplements, consistent with the repository mandate.
- Minor residual edge case not independently behavior-tested: the BS_PlaceSeekers placement-undo (removal) path (covered structurally and by analogy to the behaviorally verified mine undo path). Captured as a NOTE and follow-up.

Follow-up feature requests for planning:
- Cleanup: Correct the stale '[DEFECT: ...]' inline comment and CPPUNIT_ASSERT_MESSAGE text in tests/gui/TacticalGuiLiveTest.cpp (~lines 1893-1900) so they describe the SMFR-01 drawSeekerActivation panel-height persistence as the intended, now-correct behavior rather than as an active defect, while preserving the existing passing assertion as a regression guard. Traceable to SMFR-01 second verifier-pass WARNING.
- Test hardening: Add a focused behavioral model test for the pre-game BS_PlaceSeekers placement-undo/removal path (click-to-place then remove), asserting seeker-launcher ammo restoration, inactive-seeker record removal, and placed-ordnance record removal, mirroring the existing behaviorally verified mine undo coverage. Traceable to SMFR-02 verifier NOTE.

Artifacts written:
- artifacts/seeker-missile-fixes-round3/reviewer/reviewer_report.md
- artifacts/seeker-missile-fixes-round3/reviewer/reviewer_result.json

Final outcome:
- CONDITIONAL PASS
