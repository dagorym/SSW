Verifier Report

Scope reviewed:
- Reviewer follow-up FF-2 (folding in FF-3) on the Save-Load Robustness Follow-Ups plan: deepen the save-load aggregate-abort guarantee into FVehicle::load() so every scalar read (readU32/readString/read<T>) and every nested weapon/defense load() call has its own return checked, aborting nonzero on any failure.
- Reviewed combined diff base b8ceee82 (FF-1 Verifier PASS) .. d4fcdf3b (FF-2 Security PASS, current HEAD) across Implementer commit da076ba2, Tester commit 82993ccc, Documenter commit 4ba19678, and Security commit d4fcdf3b.
- Implementation: src/ships/FVehicle.cpp (FVehicle::load() return-checking on every scalar read and nested w->load(is)/d->load(is) call), include/ships/FVehicle.h (Doxygen).
- Tests: tests/ships/FVehicleTest.{h,cpp} (unit-level in-vehicle scalar-region truncation, +1 test), tests/strategic/FGameSaveFormatTest.{h,cpp} (3 full-game FR-1 propagation-site tests: single-ship fleet, m_unattached, m_destroyed, plus a lengthPrefixedTag helper).
- Documentation: AGENTS.md save-format Contributor Notes paragraph and the Testing section's FGameSaveFormatTest bullet extended for FF-2/FF-3.
- Consulted committed artifacts/save-load-robustness-followups/FF-2/security_report.md (PASS, 2 informational NOTEs out of FF-2 scope) and security_result.json.
- Independently re-ran make check (SSWTests 297, TacticalTests 253, GuiTests 85 -- all OK) and independently re-verified the behavioral-test mandate by reverting src/ships/FVehicle.cpp to da076ba2^ (pre-fix), rebuilding, and confirming all four new tests (testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion, testLoadTruncatedInsideFleetShipScalarRegionReturnsNonzeroAndReportsExactlyOnce, testLoadTruncatedInsideUnattachedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce, testLoadTruncatedInsideDestroyedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce) fail against the unfixed code, then restoring the fix via git checkout and re-confirming a fully green make check.

Acceptance criteria / plan reference:
- plans/save-load-robustness-followups-plan.md, FF-2 subtask (folding in FF-3): 'Deepen the load aggregate-abort guarantee into FVehicle::load()'. Allowed files: src/ships/FVehicle.cpp, include/ships/FVehicle.h. Acceptance criteria: every scalar read and nested weapon/defense load() has its return checked and aborts nonzero; a stream truncated strictly inside a vehicle's own scalar/weapon/defense region makes FGame::load() return nonzero, report once via mock IStrategicUI, and commit no live singleton; no leak/dangle/double-free on abort; success path and unknown-type abort unchanged; make check green.
- Tester guidance in the same plan section explicitly requires covering all four FR-1 propagation sites (fleet ship-loop + player fleet-loop, m_unattached, m_destroyed) plus an in-vehicle scalar-region unit case -- confirmed delivered as three FGameSaveFormatTest cases plus one FVehicleTest case.
- Section 6 (Documentation impact): AGENTS.md save-format Contributor Notes extended concisely and non-duplicatively; no doc/deferred-tasks.md change (FF-2 was never a tracked deferred item) -- confirmed.

Convention files considered:
- AGENTS.md (Single-Source-of-Truth Rule; Function Comments Doxygen @author/@date policy; module boundary rules; save-format Contributor Notes)
- CLAUDE.md (pointer to AGENTS.md, no independent facts)
- doc/rules/tactical_operations_manual.md (non-negotiable constraint check: confirmed untouched)

Findings

BLOCKING
- None

WARNING
- src/strategic/FPlanet.cpp:56-65 - Pre-existing, out-of-FF-2-scope defect discovered during review: FPlanet::load() unconditionally returns 0 regardless of whether any of its reads or its station's load() succeeded, and worse, calls m_station->load(is) without checking createShip(type)==NULL first -- an unrecognized station type on an untrusted/corrupt save file (m_station stays NULL) causes a NULL-pointer virtual-call dereference (crash) rather than a graceful abort. FSystem::load() (src/strategic/FSystem.cpp:105) does check `if (p->load(is) != 0)`, but that check is neutralized because FPlanet::load() can never return nonzero.
  This is a genuine untrusted-save-input robustness/crash-risk gap in the same trust boundary this whole plan (and FF-2 specifically) targets, and it is more severe than the silent-continue class of bug FF-2 fixes (it is an unguarded NULL dereference, not just silent success on garbage data). It predates FF-2's diff entirely and sits in a file (src/strategic/FPlanet.cpp) that is not in FF-2's Allowed Files (src/ships/FVehicle.cpp, include/ships/FVehicle.h only), so it is out of scope for FF-2's acceptance criteria and does not block this subtask's PASS. Recommend it be captured as a new follow-up subtask (e.g. FF-4) for a future planning pass, parallel to how the specialist Security reviewer already logged the analogous FFleet/FPlayer container-level-scalar-read gap as an out-of-scope NOTE in security_report.md.

NOTE
- None

Test sufficiency assessment:
- Sufficient and behavioral, matching the repo's behavioral-test mandate. All four new tests construct a real save byte stream, truncate it at a precise, justified offset, call the real FGame::load()/FVehicle::load() path, and assert observed runtime outcomes (return code, IStrategicUI::showMessage() call count, committed-player/live-singleton state, non-dangling getCurrentDefense()) rather than source-text shape.
- tests/ships/FVehicleTest.cpp::testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion (unit-level) truncates strictly inside FVehicle::load()'s own scalar region (after m_ID/m_name, before m_iconName) and asserts rc != 0 plus a non-dangling, correctly-typed getCurrentDefense() -- proving leak/dangle safety on the abort path.
- tests/strategic/FGameSaveFormatTest.cpp adds three full-game tests, each truncating 2 of a target ship's 4 m_ID bytes (type tag fully present, so the pre-existing createShip()==NULL guard cannot be what fires) in, respectively, a deliberately single-ship fleet (isolates the fix from the createShip()==NULL guard and exercises both FFleet::load()'s ship loop and FPlayer::load()'s fleet loop), FPlayer::m_unattached, and FPlayer::m_destroyed. Each asserts rc != 0, exactly one showMessage() call, and zero players committed to the live FGame singleton -- covering all four FR-1 propagation sites named in the plan's Tester guidance.
- Independently re-confirmed the behavioral-test mandate myself: reverted src/ships/FVehicle.cpp to its pre-FF-2 state (da076ba2^), rebuilt, and observed all four new tests fail (the other 9 failures observed in that run were pre-existing FGameHeaderDependencyTest cwd-relative source-path lookups, an artifact of running the binary via absolute path rather than `cd tests`, unrelated to FF-2 and not present when run correctly). Restored the fix via `git checkout -- src/ships/FVehicle.cpp`, rebuilt, and re-ran the full make check: SSWTests OK (297 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests), matching the reported final outcome exactly.
- No production file outside src/ships/FVehicle.cpp and include/ships/FVehicle.h was touched by the Implementer; no production file was touched by the Tester -- confirmed via diff inspection.

Documentation accuracy assessment:
- include/ships/FVehicle.h's Doxygen on load() (lines 156-172) accurately and completely documents the FF-2 return-checking discipline, the delete-just-allocated-weapon/defense-before-return ownership rule, reliance on the already-FR-1-hardened caller to delete the not-yet-committed vehicle on abort, and that the success/unknown-type-abort paths are unchanged. @author/@date Last Modified correctly updated.
- tests/ships/FVehicleTest.h and tests/strategic/FGameSaveFormatTest.h carry complete, accurate per-function Doxygen for every new test method, correctly cross-referencing the FF-2/FF-3 propagation sites each test exercises.
- AGENTS.md's save-format Contributor Notes paragraph and the Testing section's FGameSaveFormatTest bullet were extended concisely, accurately, and without duplicating the FVehicle.h/FVehicleTest.h/FGameSaveFormatTest.h Doxygen -- verified by direct diff read (AGENTS.md lines ~243 and ~404). doc/deferred-tasks.md correctly left unchanged per the plan.
- Minor, non-blocking observation: the FVehicleTest class-level Doxygen block (tests/ships/FVehicleTest.h ~line 18-26) was not bumped to Jul 19, 2026 / did not add Claude Sonnet 5 (medium) to its @author list when the new test method was added to the class; the same pattern (class-level doc not bumped when a new member test is added, only the new function's own Doxygen block updated) is pre-existing and was already present -- and implicitly accepted -- across the prior FR-1/FR-2/FF-1 subtasks in this same plan sequence, so this is not treated as a new violation introduced by FF-2.

Artifacts written:
- artifacts/save-load-robustness-followups/FF-2/verifier_report.md
- artifacts/save-load-robustness-followups/FF-2/verifier_result.json

Verdict:
- PASS
