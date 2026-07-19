Verifier Report

Scope reviewed:
- Second (final) Verifier pass for FR-2 (closes deferred item SF-nullfmap-paint-guard). Reviewed the combined pass-1 + pass-2 diff against coordination base 4c668207 (post-FR-1 Verifier) through HEAD (d5f3fb11): pass-1 added FMap::hasMap() (include/strategic/FMap.h, src/strategic/FMap.cpp) and guarded WXMapDisplay::draw()/getScale() (include/gui/WXMapDisplay.h, src/gui/WXMapDisplay.cpp); pass-2 closed the residual sibling path by guarding WXPlayerDisplay::drawFleets() (include/gui/WXPlayerDisplay.h, src/gui/WXPlayerDisplay.cpp).
- Reviewed both new behavioral tests in tests/gui/StrategicGuiLiveTest.{h,cpp}: testGamePanelRepaintWithNoLiveFMapDoesNotCrash (pass-1, WXMapDisplay path via a player-less FGame) and testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash (pass-2, direct drawFleets() call with a real fleet-bearing FPlayer/FFleet and FMap::hasMap() false).
- Reviewed doc/deferred-tasks.md (SF-nullfmap-paint-guard section fully removed) and the AGENTS.md GUI-tests guard-coverage note (updated to describe both guards as root-cause-complete and to name both confirming tests).
- Reviewed the committed pass-2 security_report.md / security_result.json (PASS, 0 blocking, 0 warning, 2 non-blocking notes) and confirmed src/FMainFrame.cpp is untouched by the combined FR-2 diff.
- Independently ran `make all_clean` then `make check` from a clean tree in the isolated worktree /home/tstephen/repos/worktrees/phase5-FR-2-verifier-20260719.

Acceptance criteria / plan reference:
- plans/phase5-save-format-followups-plan.md, section 'FR-2 -- Root-cause NULL/invalid-FMap guard in the gui draw chain' (acceptance criteria: gui draw chain no longer dereferences a nonexistent FMap; rendering unchanged when a valid FMap exists; guard lives in the gui draw path itself, onOpen mitigation unchanged; make check green).
- doc/deferred-tasks.md (pre-fix) entry 'SF-nullfmap-paint-guard' as the originating deferred item being closed.
- artifacts/phase5-save-format-followups/FR-2/history/pass-1/verifier_report.md (pass-1 Verifier CONDITIONAL PASS / WARNING that drove this pass-2 remediation) and artifacts/phase5-save-format-followups/FR-2/security_report.md (pass-2 Security PASS).

Convention files considered:
- AGENTS.md (Doxygen function-comment mandate incl. @author/@date Last Modified rules; Behavioral Verification Is Mandatory policy; module boundary rules for gui/strategic includes; Contributor Notes FMap/gui paint-path guard note)
- CLAUDE.md (pointer to AGENTS.md as sole source of truth; no restatement)

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/gui/WXPlayerDisplay.cpp:100 - Carried-forward, out-of-scope Security NOTE: once FMap exists, drawFleets() dereferences map->getSystem(location) without a NULL check; a valid FMap plus an out-of-range fleet-location ID would crash here.
  This is a distinct save-input-validation failure mode (a valid FMap that cannot resolve an untrusted ID), not the NULL/half-built-FMap defect FR-2 was scoped to fix, and is not introduced or regressed by this change. Confirmed as pre-existing and correctly flagged by pass-2 Security as a separate follow-up rather than an FR-2 defect; non-blocking.
- src/gui/WXPlayerDisplay.cpp:42-48 - drawFleets() checks FMap::hasMap() itself and then calls mapDisplay.getScale(dc), which re-checks FMap::hasMap() internally -- a harmless redundant double-check.
  No correctness or security impact (both checks are cheap, side-effect-free, single-threaded reads of a static pointer); noted only for completeness, matching the pass-2 Security note. No action required.
- plans/phase5-save-format-followups-plan.md:178-185 - The FR-2 plan's 'Allowed files' list for the original subtask did not name include/gui/WXPlayerDisplay.h or src/gui/WXPlayerDisplay.cpp; the pass-2 remediation touched both.
  This is an expected and correctly-scoped expansion, not a scope violation: the pass-1 Verifier itself identified the residual drawFleets() gap as the one thing keeping FR-2 from meeting its own acceptance criterion ('the gui draw chain no longer dereferences the FMap singleton when it does not exist' -- singular chain, not just the WXMapDisplay sub-path), and the remediation loop exists precisely to close Verifier-identified gaps. Non-blocking; recorded for completeness only.

Test sufficiency assessment:
- SUFFICIENT and genuinely behavioral, not source-inspection. Confirmed both new tests actually construct real objects and exercise the guarded code paths end-to-end: testGamePanelRepaintWithNoLiveFMapDoesNotCrash builds a live FGame (no-arg FGame::create(), which has no FMap side effect), installs it on a shown FGamePanel via setGame(), forces a real repaint through FGamePanel::onPaint() -> WXGameDisplay::draw() -> WXMapDisplay::draw()/getScale(), and asserts FMap::hasMap() still false afterward -- reaching that assertion at all is the behavioral proof (pre-fix, this repaint dereferenced a null FMap reference and crashed the process).
- testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash (pass-2) is the sibling proof for the path this remediation closes: it builds a real (non-mock) FPlayer owning a real FFleet with a nonzero, non-transit getLocation() (via FFleet::setLocation(unsigned int), confirmed to exist in include/strategic/FFleet.h:322) -- exactly the state that previously drove drawFleets() into the 'it's in a system' branch that binds `FMap *map = &(FMap::getMap())` against a NULL m_map (undefined behavior) and dereferences it via map->getSystem(...) -- then calls WXPlayerDisplay::drawFleets() directly on an offscreen wxMemoryDC with FMap::hasMap() false, and asserts FMap::hasMap() still false and the rendered image IsOk() afterward. All FPlayer/FFleet APIs used (setFleetIcon, getFleetIconName, setOwner, setIcon, addShip, setLocation(unsigned int), getLocation(), getInTransit(), addFleet, getFleetList()) were checked against include/strategic/FPlayer.h and include/strategic/FFleet.h and match established usage patterns already present elsewhere in the same test file.
- Confirmed WXGameDisplay::draw() (src/gui/WXGameDisplay.cpp) has exactly two FMap-touching call paths -- WXMapDisplay::draw() and per-player WXPlayerDisplay::drawFleets() -- with drawTurnCounter() touching no FMap state at all, so the two behavioral tests genuinely cover the full paint-chain surface the AGENTS.md note claims is now root-cause-complete; no third unguarded path was found.
- Both tests are registered via CPPUNIT_TEST in tests/gui/StrategicGuiLiveTest.h and defensively tear down any process-static FMap a prior CppUnit test may have left live before asserting the starting precondition, which is correct given FMap::m_map is a process-wide static and CppUnit does not guarantee suite ordering.
- Independently rebuilt from a clean tree (`make all_clean` then `make check`) and confirmed all three suites green: SSWTests OK (290 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests, xvfb-run) -- consistent with the Tester/Security-reported test-count deltas (GUI suite grew 83 -> 84 in pass-1, 84 -> 85 in pass-2).

Documentation accuracy assessment:
- ACCURATE. doc/deferred-tasks.md's entire 'SF-nullfmap-paint-guard' section is removed (not merely reworded/re-deferred) in the combined diff vs base 4c668207, and a repo-wide grep confirms no remaining reference to SF-nullfmap-paint-guard anywhere outside the plan, AGENTS.md's resolved-history note, source-code comments naming the closed tag, and archived/history artifact files -- i.e. it is fully closed, not re-deferred.
- AGENTS.md's GUI-tests guard-coverage note is accurate and not overstated: it correctly names both guarded functions (WXMapDisplay::draw()/getScale() and WXPlayerDisplay::drawFleets()), both confirming tests, states the guard is additive defense-in-depth that does not change rendering when a valid FMap exists, explicitly preserves the unchanged P5-5 onOpen() setGame-deferral description, and claims completeness only 'across both of WXGameDisplay::draw()'s FMap-touching call paths' -- which was independently verified to be the full and only set of such paths (see test sufficiency assessment above), so the claim is not an overstatement.
- Doxygen is present and accurate on all four touched/new production symbols: FMap::hasMap() (include/strategic/FMap.h, new block, @author Claude Sonnet 5 (medium), @date Created/Last Modified Jul 19, 2026), WXMapDisplay::draw()/getScale() (include/gui/WXMapDisplay.h, @author line appends 'Claude Sonnet 5 (medium)' without removing 'Tom Stephens', @date Last Modified bumped to Jul 19, 2026, plus an added Note line), and WXPlayerDisplay::drawFleets() (include/gui/WXPlayerDisplay.h, same pattern). Both new test methods carry full Doxygen blocks in tests/gui/StrategicGuiLiveTest.h with accurate descriptions of what each proves and why the sibling test exists.
- src/FMainFrame.cpp is confirmed untouched by the combined FR-2 diff (git diff 4c668207..HEAD -- src/FMainFrame.cpp is empty), so the P5-5 onOpen setGame-deferral mitigation is unchanged as required, and AGENTS.md's description of it is left verbatim.

Artifacts written:
- artifacts/phase5-save-format-followups/FR-2/verifier_report.md
- artifacts/phase5-save-format-followups/FR-2/verifier_result.json

Verdict:
- PASS
