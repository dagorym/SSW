Security Review Report

Scope reviewed:
- Specialist security review of subtask P5-4 (map/system/jump-route save-load hardening & ID pointer-mask fix) of the phase5-save-format-hardening plan, REMEDIATION pass 2, on branch phase5-P5-4-security-20260717 (read-only for all repository files except the two security artifacts).
- This pass re-reviews the committed Implementer + Tester(remediation) + Documenter chain. The pass-1 specialist Security review already returned PASS (0 blocking, 0 warning, 3 non-blocking notes) and is archived at artifacts/phase5-save-format-hardening/P5-4/history/pass-1/security_report.md + security_result.json.
- The implementation is UNCHANGED from the pass-1 security-reviewed state. Confirmed independently: git diff cbba0ae0..HEAD -- src include produces an empty diff (cbba0ae0 = the pass-1 'P5-4: Security review artifacts -- PASS' commit), and git diff cbba0ae0..HEAD -- tests shows the ONLY change is tests/strategic/FSystemTest.cpp (+83/-13, commit 74fbcf03). In-scope impl files (include/strategic/{FSystem,FJumpRoute,FMap}.h and src/strategic/{FSystem,FJumpRoute,FMap}.cpp) are byte-identical to the pass-1 review baseline; all pass-1 controls remain intact: removed pointer-value smuggling / 0x0000FFFFL mask, fixed-width writeU32/readU32 wire fields, FMap::load's null-guarded getSystem(id) resolution, and nested-load failure propagation (delete + return 1).
- The FVehicle.h/.cpp deltas visible in a raw phase5..HEAD diff are branch-base divergence from later phase5 work, not part of P5-4 and not modified by this remediation; they are out of scope for this subtask.
- Change under review this pass is TEST-ONLY: FSystemTest::testLoadAdvancesNextIdPastLoadedID was rewritten (per the pass-1 Verifier CONDITIONAL PASS finding that it was empirically vacuous) to hand-craft a raw large-ID (5,000,000) FSystem::load wire buffer and call load() directly with no intervening FSystem construction, mirroring the already-correct FJumpRouteTest AC4 pattern; testSaveWiresIdLittleEndian gained a byte-level planet-count assertion supplementing the AC5 wire-width coverage.
- Verification run synchronously in the worktree: make -C tests built clean; SSWTests ran green (OK, 269 tests, exit 0), matching the pass-1 result count.

Why specialist review was triggered:
- Loading a save file is an UNTRUSTED-INPUT path (plan Section 7 Security Notes); a malformed/adversarial file must not corrupt memory, store fabricated/dangling pointers, over-allocate, or loop unboundedly. The plan marks P5-4 as requiring a specialist Security stage.
- P5-4 removes a memory-safety-relevant defect (jump-route start/end system IDs smuggled inside FSystem* pointer fields and unpacked with a 16-bit mask). This pass confirms that fix is unregressed and that the remediation (test-only) does not weaken any security control.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md (subtask P5-4; Security Notes Section 7).
- Acceptance criteria AC1-AC5 as reflected in the committed tester coverage and implementer report at artifacts/phase5-save-format-hardening/P5-4/ and its history/pass-1/ archive.
- Pass-1 specialist security report: artifacts/phase5-save-format-hardening/P5-4/history/pass-1/security_report.md (PASS).

Findings

BLOCKING
- None

WARNING
- None

NOTE
- tests/strategic/FSystemTest.cpp:108-156 - Remediation test change reviewed clean: no security control weakened, no unsafe helper leaks into production.
  The rewritten testLoadAdvancesNextIdPastLoadedID strengthens (does not weaken) coverage of the H3 non-colliding ID guard on the untrusted load() path: it hand-crafts a deliberately large-ID (5,000,000) wire buffer and calls FSystem::load() directly, exercising exactly the adversarial-input surface, and asserts a subsequently constructed object gets a strictly greater ID. The new appendU32LE helper is defined in an unnamed (internal-linkage) namespace inside the test .cpp only; it cannot leak into production or any other translation unit. The added byte-level planet-count assertion in testSaveWiresIdLittleEndian only reads emitted bytes. No implementation source changed (empty git diff cbba0ae0..HEAD -- src include). No new attack surface, no disabled check. Non-blocking; informational.
- src/strategic/FSystem.cpp:115-119 - Carried forward from pass-1 (unchanged code): H3 ID-counter restore at the extreme boundary (crafted m_ID near UINT_MAX) can wrap the class counter, risking a same-session logical ID collision.
  Impact is bounded to logical ID-collision within the running process, not memory corruption: IDs are used only for equality lookup via getSystem()/getJumpRoute() linear searches (never as array indices), and getSystem() returns NULL on no-match, so no out-of-bounds access or invalid-pointer dereference can result. Strict improvement over the pre-existing code, which had no H3 guard at all. Code is byte-identical to the pass-1 baseline; assessment is reaffirmed unchanged. Non-blocking.
- src/strategic/FJumpRoute.cpp:111 - Carried forward from pass-1 (unchanged code): m_length (float) and FSystem m_coords (float[3]) are still serialized via the native-representation write<T>/read<T> template rather than a fixed-width helper.
  No memory-safety impact: read<T> reads exactly sizeof(T) bytes and returns nonzero on stream failure (checked by callers), so a truncated float still aborts the load. Portability/data-fidelity note only (native float layout is not endian/word-size portable; a crafted NaN/Inf is accepted as data), explicitly outside P5-4's ID/count scope. Reaffirmed unchanged. Non-blocking.

Test sufficiency assessment:
- SUFFICIENT for the security-sensitive behaviors, and improved by this remediation. Coverage is behavioral (constructs real objects/streams, drives load()/save(), asserts observed runtime results), not source-inspection.
- Remediation strengthened AC4 (H3 ID-counter restore): FSystemTest.testLoadAdvancesNextIdPastLoadedID no longer default-constructs an intervening FSystem before load() (which alone advanced the shared static m_nextID and made the old assertion vacuous). It now hand-crafts a raw large-ID (5,000,000) wire buffer and calls load() directly, so the assertion depends on the guard actually firing; the Tester documented confirming it fails with the H3 guard disabled and passes with it restored (implementation left unmodified).
- AC5 wire-width coverage strengthened: testSaveWiresIdLittleEndian now also asserts the planet-count field's raw little-endian bytes at its known offset, giving a second writeU32 field independent of the ID field (which is byte-identical between the old and new paths on sizeof(int)==4 hosts).
- Unregressed pass-1 coverage remains in force: pointer-mask removal / no fabricated pointer (FJumpRouteTest.testLoadPreservesSystemIdAbove65535, IDs 65537/131073 round-trip and getStart()/getEnd() NULL after load); FMap end-to-end resolution above 65535 (FMapTest.testLoadResolvesSystemIdAbove65535ViaHandCraftedStream); absent-system null-guard abort (FMapTest.testLoadRejectsJumpRouteWithUnknownSystemIdAndLeavesJumpListEmpty); nested-load/stream-failure propagation (FSystemTest/FJumpRouteTest testLoadReturnsNonzeroOnTruncatedStream).
- Full suite verified green synchronously in this pass: SSWTests OK, 269 tests, exit 0.

Documentation / operational guidance assessment:
- SUFFICIENT and unchanged. No documentation (AGENTS.md, doc/synthesized-roadmap.md, Doxygen headers) was modified in this remediation pass; the pass-1 documentation assessment stands.
- Doxygen headers on FMap/FSystem/FJumpRoute save()/load() accurately describe the removed pointer-smuggling + 0x0000FFFFL mask, the real fixed-width m_startSystemID/m_endSystemID fields, FMap::load's getSystem(id) null-guard/abort behavior, nested-load failure propagation, and the H3 pre-/post-increment counter conventions; getStartSystemID()/getEndSystemID() are documented as valid only after load().
- The rewritten test carries an in-source comment block explaining why the hand-crafted buffer is required for non-vacuity, which is adequate operational guidance for future maintainers.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-4/security_report.md
- artifacts/phase5-save-format-hardening/P5-4/security_result.json

Outcome:
- PASS
