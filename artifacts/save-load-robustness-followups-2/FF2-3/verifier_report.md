Verifier Report

Scope reviewed:
- Implementer commit b2891996 (base e2ee606c): src/strategic/FPlayer.cpp, include/strategic/FPlayer.h, src/strategic/FFleet.cpp, include/strategic/FFleet.h -- return-checks every container-level own scalar read in FPlayer::load() and FFleet::load(), aborting nonzero on the first failed read, mirroring FF-2's FVehicle::load() pattern.
- Tester commit 87abcd7f: 6 new behavioral tests -- FPlayerTest (2), FFleetTest (2), FGameSaveFormatTest (2 full-game aggregate-abort) -- each constructs a real truncated stream/save and calls the real load()/FGame::load(), asserting on observed return codes, container emptiness, showMessage() call count, and committed-player count.
- Documenter commit 79fd2544: AGENTS.md-only update recording the FF2-3/FR-D closure in the save-format Contributor Notes paragraph and the strategic-tests Testing-section bullet.
- Specialist Security review (commit e3ede89f, artifacts/save-load-robustness-followups-2/FF2-3/security_report.md): outcome PASS, 0 BLOCKING, 0 WARNING, 2 informational NOTEs (both out of scope for FR-D: per-element type-tag reads inside sub-object loops, and pre-existing native-representation read<T> non-portability tracked as SF-enum-codes).

Acceptance criteria / plan reference:
- plans/save-load-robustness-followups-2-plan.md, Section 5 subtask 'FF2-3 -- Extend aggregate-abort to container-scalar reads in FPlayer::load()/FFleet::load() (FR-D)' (lines 331-391) and Section 6 'Documentation Impact (overall)' (lines 394-412).

Convention files considered:
- AGENTS.md (save-format Contributor Notes paragraph, Single-Source-of-Truth Rule, Function Comments/Doxygen convention, Behavioral Verification Is Mandatory policy)
- CLAUDE.md (pointer only)
- doc/rules/tactical_operations_manual.md (confirmed untouched, as required)

Findings

BLOCKING
- None

WARNING
- None

NOTE
- tests/strategic/FGameSaveFormatTest.h:273 - Doxygen for testLoadTruncatedInsideFleetOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce says the truncation targets UPF's FIRST fleet ('Task Force Prenglar') partway through its m_owner field, but the .cpp implementation (FGameSaveFormatTest.cpp, deliberately, per its own in-source rationale comment) truncates UPF's LAST fleet (looked up dynamically via getFleetList().back()) partway into its m_iconFile field.
  Stale test-comment accuracy nit only; the .cpp in-source comment correctly documents the deliberate choice (avoiding two identified fix-discrimination traps: false-passing via FF-1's independent destination/jump-route validation, and cross-fleet cascade corruption when a non-last fleet is truncated). The .cpp behavior itself is correct and does discriminate the fix (confirmed by the revert-and-rerun below). Flagged for awareness per the Documenter's handoff; not a behavioral defect and not blocking.
- artifacts/save-load-robustness-followups-2/FF2-3/security_report.md:34 - Security report's own SSWTests run recorded 9 pre-existing FGameHeaderDependencyTest failures ('Expected at least one candidate file path to be readable'). Independently re-running the SSWTests binary directly via an absolute path (outside tests/) reproduced the same 9 failures, but the canonical `make check` invocation (which cd's into tests/ before running) is fully green with 0 failures in two independent from-clean runs performed during this verification pass.
  Confirms this is a cwd-relative-path artifact of how the binary is invoked (FGameHeaderDependencyTest resolves candidate source paths relative to the process's working directory), not a regression introduced by FF2-3. Recorded for continuity/awareness only; the acceptance criterion is satisfied via the canonical `make check` path, which both the Tester and this Verifier independently confirmed green.

Test sufficiency assessment:
- Sufficient and fully behavioral (no source-inspection-only coverage of a behavioral criterion). FPlayerTest::testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion and ::testLoadReturnsNonzeroWhenTruncatedInsideCountFields, and FFleetTest::testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion and ::testLoadReturnsNonzeroWhenTruncatedInsideShipCountField, each construct a real truncated std::stringstream, call the real FPlayer::load()/FFleet::load(), and assert a nonzero return plus empty ship/fleet containers (no partial commit).
- FGameSaveFormatTest::testLoadTruncatedInsidePlayerOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce and ::testLoadTruncatedInsideFleetOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce drive the full FGame::load() aggregate-abort path over a real byte-patched save, asserting nonzero return, exactly one RecordingUI::showMessage() call, and zero players committed to the live singleton.
- Behavioral-test mandate independently re-confirmed by this Verifier: reverted src/strategic/FPlayer.cpp and src/strategic/FFleet.cpp to their e2ee606c (pre-fix) content, rebuilt, and reran SSWTests -- all 6 new tests failed as expected (assertion 'rc != 0' not satisfied / return-0 phantom-object commit), confirmed by name in the failure list (FGameSaveFormatTest::testLoadTruncatedInsidePlayerOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce, ::testLoadTruncatedInsideFleetOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce, FFleetTest::testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion, ::testLoadReturnsNonzeroWhenTruncatedInsideShipCountField, FPlayerTest::testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion, ::testLoadReturnsNonzeroWhenTruncatedInsideCountFields). Restored the fixed files afterward; `git status --porcelain` confirmed a byte-exact restore (clean working tree) before rebuilding the fixed code.
- Positive-control / no-regression coverage confirmed: existing FPlayerTest/FFleetTest/FGameSaveFormatTest round-trip, H3 next-ID-advance, H4 no-normalization, unknown-ship-type-abort, and FF-1/FF2-1/FF2-2 full-game-abort tests all remain green in the fixed-code make check run.
- make check independently re-run twice from a clean tree (make all_clean; make check) by this Verifier: SSWTests OK (315 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests) -- exact match to the required counts, 0 failures across all three suites both times.

Documentation accuracy assessment:
- Doxygen on FPlayer::load() (include/strategic/FPlayer.h) and FFleet::load() (include/strategic/FFleet.h) updated with an FF2-3/FR-D paragraph describing the new container-scalar return-checking and its FF-2 FVehicle::load() lineage; @date Last Modified bumped to Jul 19, 2026; implementing model (Claude Sonnet 5 (medium)) appended to @author without removing prior authors -- matches the repository's append-only author convention.
- AGENTS.md save-format Contributor Notes paragraph (single-source-of-truth location) extended with one sentence recording that FPlayer::load()/FFleet::load() now check every container-level own scalar read, completing the aggregate-abort depth begun at FVehicle::load() by FF-2; the strategic-tests Testing-section bullet extended with a matching sentence naming the 6 new tests and their coverage intent. No duplication of facts elsewhere; doc/deferred-tasks.md correctly left unchanged (FR-D was not a tracked deferred item); doc/rules/tactical_operations_manual.md confirmed untouched.
- One documentation accuracy NOTE (non-blocking, per explicit task guidance): stale field/target description in tests/strategic/FGameSaveFormatTest.h's Doxygen for the fleet-truncation test (says first fleet / m_owner; actual .cpp targets last fleet / m_iconFile, for documented fix-discrimination reasons). Test-comment-only nit; does not affect the correctness or discrimination quality of the test itself.

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-3/verifier_report.md
- artifacts/save-load-robustness-followups-2/FF2-3/verifier_result.json

Verdict:
- PASS
