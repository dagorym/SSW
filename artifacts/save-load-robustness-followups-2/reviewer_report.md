Reviewer Report

Feature plan reviewed:
- plans/save-load-robustness-followups-2-plan.md (FF2-1 [FR-A+FR-B], FF2-2 [FR-C], FF2-3 [FR-D]) -- the follow-up increment closing the four out-of-scope trust-boundary residuals (FR-A..FR-D) surfaced by the prior save-load-robustness-followups Reviewer PASS; all three material design decisions (D1 reject-at-load, D2 FR-A+FR-B combined, D3 FR-D included) RESOLVED and user-confirmed.

Inputs reviewed:
- Governing plan: plans/save-load-robustness-followups-2-plan.md (Sections 4 scope, 5 acceptance criteria, 6 documentation, 7 dependency ordering).
- Feature diff 0f6a5bd2..HEAD on phase5 (17 files, +1070/-66): src/strategic/{FGame,FPlanet,FPlayer,FFleet}.cpp, include/strategic/{FGame,FPlanet,FPlayer,FFleet}.h, AGENTS.md, and tests/strategic/{FGameSaveFormatTest,FPlanetTest,FPlayerTest,FFleetTest}.{h,cpp}.
- Per-subtask artifacts under artifacts/save-load-robustness-followups-2/FF2-1, FF2-2, FF2-3 (implementer/tester/documenter/security/verifier reports+results).
- All three Verifier results: PASS, 0 blocking / 0 warning; each independently re-ran the behavioral-test mandate (reverted the fix, confirmed the new tests fail, restored byte-exact) and re-ran make check green.
- All three specialist Security results: PASS (outcome PASS).
- Independent reviewer spot-check: built the default tests target (clean) and ran ./SSWTests from tests/ -> OK (315 tests).

Overall feature completeness:
- COMPLETE. All three subtasks match their plan Section 4 IN-scope items and satisfy every Section 5 acceptance criterion; all OUT-of-scope boundaries are respected.
- FF2-1 (FR-A+FR-B): FGame::load()'s existing post-player-load fleet-validation loop now additionally rejects a non-NO_DESTINATION fleet m_destination that does not resolve via FMap::getSystem(id), and the illegal getInTransit()==true && location==0 state, each via the existing delete-p -> reportLoadError(...) -> return 1 path. The delete-p-before-push_back ordering invariant is preserved. Per D1, moveFleets() is byte-unchanged (both diff hunks are inside FGame::load(); moveFleets appears only in explanatory comments) and no gui file is touched.
- FF2-2 (FR-C): FPlanet::load() now return-checks each own read (m_ID, m_name, stationCount, station type tag), NULL-checks createShip(type) before m_station->load(), checks the nested m_station->load() return, and returns nonzero on any failure instead of the former unconditional return 0 -- so FSystem::load()'s pre-existing if(p->load(is)!=0) check (formerly dead code) now fires and the abort propagates through FMap::load() to FGame::load(). Confined to FPlanet; no field converted to readU32.
- FF2-3 (FR-D): FPlayer::load() and FFleet::load() now return-check every one of their own container-level scalar reads, aborting nonzero before any sub-object is allocated. The H3 next-ID advance, H4 no-normalization, and existing createShip()==NULL / nested-load checks are unchanged. Confined to FPlayer/FFleet.
- Cross-subtask + cross-plan integration is consistent and complete for the claimed scope: the fleet-reference trust boundary now validates location + jump-route (FF-1) + destination + illegal in-transit/location-0 (FF2-1); the aggregate-abort guarantee now reaches FVehicle scalar/nested reads (FF-2), FPlanet station-record reads (FF2-2), and the FPlayer/FFleet container-scalar reads (FF2-3) -- closing the container-scalar depth one level up from FF-2. No reachable NULL-deref or silent-accept sink that this plan claimed to close remains open within the delivered scope.
- Documentation obligations met: Doxygen updated on every touched load() (bumped @date, appended implementing model to @author, prior authors preserved); the AGENTS.md save-format Contributor Notes paragraph and the Testing-section strategic-tests bullet both extended for FF2-1/FF2-2/FF2-3 with the FF-1 'located-object reference IDs' wording tightened to enumerate location, jump-route, and destination; doc/deferred-tasks.md correctly unchanged; doc/rules/tactical_operations_manual.md untouched.
- Security obligations met: all three subtasks (untrusted-save-input parsing) received specialist Security review, all PASS -- appropriate for the risk profile; no feature-level security gap remains within delivered scope.
- Test sufficiency confirmed at the feature level: coverage is behavioral (constructs real FGame/FPlanet/FPlayer/FFleet objects and real save byte streams, drives the real load(), and asserts observed outcomes -- return code, RecordingUI showMessage count, committed-player/container size), not source-inspection. Each verifier independently confirmed the fail-against-unfixed / pass-against-fixed mandate. The Tester/Verifier judgment that a full-game truncation only discriminates the fix at a point the unfixed code would otherwise accept held: FF2-2's truncated-station-record scenario is correctly placed at the FPlanetTest unit level (a full-game version aborts on unfixed code via upstream FSystem/FMap checks), and FF2-3's fleet-truncation test deliberately targets m_iconFile after all FF-1-validated fields and the last fleet in the list to avoid false-passing via FF-1's own validation.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- tests/strategic/FGameSaveFormatTest.h - Stale Doxygen on testLoadTruncatedInsideFleetOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce describes the wrong truncation target: the comment says 'UPF's first fleet (Task Force Prenglar) ... partway through its m_owner field', but the .cpp implementation dynamically targets the LAST fleet (getFleetList().back()) and truncates partway into m_iconFile (after all of m_owner/m_location/m_inTransit/m_destination/transit/jumpLength/speed/jumpRouteID).
  Test-comment accuracy nit only; the .cpp behavior is correct and fix-discriminating (the divergent target and offset are the deliberate fix-discrimination choices explained in the .cpp inline comment). No behavioral impact. Already surfaced by the FF2-3 Verifier as a non-blocking NOTE.
- src/strategic/FPlanet.cpp - Per-element type-tag reads inside the FPlanet station and FPlayer/FFleet ship loops (readString(is,type)) are not each explicitly return-checked; they are safely backstopped by the existing createShip()==NULL guard on the immediately following factory call.
  Pre-existing, intentional pattern (a failed type-tag read yields an empty/garbage string that createShip() rejects, aborting the load). Surfaced by the FF2-2/FF2-3 Security passes as safe. Not a reachable defect; noted only for completeness/consistency.
- AGENTS.md - The native-representation read<T> scalar fields and the weapon/defense m_type enum ordinal remain a documented non-portability concern (SF-enum-codes), unchanged and out of scope for this plan.
  Pre-existing deferred item, correctly left in doc/deferred-tasks.md and untouched per plan Section 4. Not actionable within this plan; recorded so it is not mistaken for a new gap.

Missed functionality or edge cases:
- None within the delivered scope. FR-A/FR-B/FR-C/FR-D are each closed with behavioral coverage that discriminates the fix. moveFleets() and the gui draw paths are intentionally unmodified per resolved decision D1 (root-cause reject-at-load makes the crash impossible rather than merely survivable), which is consistent with the FF-1/FF-2 pattern and is the correct choice.
- No remaining reachable NULL-deref or silent-accept sink that this plan claimed to close was found: destination + illegal-state (FF2-1), planet station NULL-virtual-call + guaranteed-0 return (FF2-2), and container-scalar silent success (FF2-3) are all closed and propagate to FGame::load()'s aggregate-abort/no-live-singleton path.

Follow-up feature requests for planning:
- DOC-NIT (LOW, doc-only, optional): Correct the stale Doxygen comment on FGameSaveFormatTest::testLoadTruncatedInsideFleetOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce in tests/strategic/FGameSaveFormatTest.h so it matches the .cpp: the test targets UPF's LAST fleet (via getFleetList().back(), not 'first fleet Task Force Prenglar') and truncates partway into the m_iconFile field (not m_owner). Test-comment accuracy only; no code or behavior change. Can be folded into any future touch of that file rather than warranting a standalone task.
- SF-enum-codes (pre-existing DEFERRED, out of scope here; already tracked in doc/deferred-tasks.md): stable serialization codes for the weapon/defense m_type enum ordinal and portable encoding of native read<T> scalar fields remain the one outstanding save-format portability concern. Carry forward as already tracked; not reopened by this plan.

Artifacts written:
- artifacts/save-load-robustness-followups-2/reviewer_report.md
- artifacts/save-load-robustness-followups-2/reviewer_result.json

Final outcome:
- PASS
