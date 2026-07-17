Verifier Report

Scope reviewed:
- Combined Implementer (5486b5a3) + Tester (5e96e227) + Documenter (780b1150) + specialist Security (fe06e343, branch phase5-P5-3-security-20260717) changes for P5-3, diffed against base commit 4fb9c67e (P5-2 Verifier PASS).
- Implementation: include/strategic/FPlayer.h, src/strategic/FPlayer.cpp, include/strategic/FFleet.h, src/strategic/FFleet.cpp -- fixed-width writeU32/readU32 for all IDs/counts, F2 m_destroyed save/load persistence, H3 static m_nextID advance-past-loaded-ID guard for both classes, H4 removal of the jump-route-0-to-NO_ROUTE normalization, and createShip() NULL-check/abort-on-unknown-type in all three ship-list loops (FPlayer::m_unattached, FPlayer::m_destroyed, FFleet::m_ships).
- Tests: tests/strategic/FPlayerTest.cpp/.h, tests/strategic/FFleetTest.cpp/.h -- 8 new behavioral CppUnit cases exercising real save()/load() round trips via std::stringstream and hand-crafted wire streams via the inherited FPObject writeU32/readU32/writeString/write helpers (mirrors the FVehicleTest pattern).
- Docs: doc/deferred-tasks.md (F2-serialization entry removed as resolved), doc/synthesized-roadmap.md (H3 and H4 rows updated with RESOLVED annotations and test references; F2 note in the risk-register section updated).
- Verified independently in this pass: full repo `make` (top-level) and `src/strategic` clean/build both completed with exit code 0; ./SSWTests run from tests/ (271/271 OK) and ./TacticalTests run from tests/tactical (253/253 OK) reproduced the upstream-reported totals exactly; git status confirmed clean before and after this review.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md, subtask P5-3 (lines ~200-246): allowed files (FPlayer.h/.cpp, FFleet.h/.cpp) matched exactly by the Implementer's changed-file list; all 6 listed acceptance criteria (full round-trip incl. m_destroyed with no double-free, jump-route-0 preservation, nonzero-return on unknown ship type, next-ID advance past loaded ID, fixed-width uint32 wire format, clean wx-free build) verified against the corresponding named test(s).
- Security review requirement satisfied: plan marks P5-3 'Security review: required'; specialist Security stage ran on branch phase5-P5-3-security-20260717 and produced artifacts/phase5-save-format-hardening/P5-3/security_report.md + security_result.json with outcome PASS (0 blocking, 0 warning, 2 non-blocking notes), consulted as part of this review.

Convention files considered:
- AGENTS.md -- Doxygen/@author/@date policy (verified: every touched save()/load() Doxygen block and file header carries an updated @date Last Modified: Jul 17, 2026 and 'Claude Sonnet 5 (medium)' appended to @author without removing prior authors); Single-Source-of-Truth Rule (no project facts duplicated into CLAUDE.md); module boundary rule (src/strategic/Makefile confirmed free of wx-config/WX_CXXFLAGS both by direct grep and by a from-scratch strategic-module build); Behavioral Verification Is Mandatory policy (all 8 new tests construct real FPlayer/FFleet objects and real std::stringstream streams and assert on runtime save()/load() results -- no source-text/source-structure-only assertions).
- CLAUDE.md -- pointer-only file, no facts duplicated; confirmed compliant.
- doc/rules/tactical_operations_manual.md -- confirmed not touched by this diff (git diff --stat shows no changes to doc/rules/).

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/strategic/FFleet.cpp:150-154 - H3 next-ID advance can integer-wrap under an adversarial loaded ID near UINT32_MAX/INT32_MAX (also src/strategic/FPlayer.cpp:138).
  Concurs with the specialist Security stage's NOTE (security_report.md, first NOTE): a crafted m_ID of 0xFFFFFFFF or 0x7FFFFFFF can wrap m_nextID to 0/INT_MIN, weakening the ID-uniqueness guarantee for a subsequently constructed object. No memory-safety hazard (unsigned overflow is well-defined; the FFleet comparison is cast-guarded) and negligible real-world exploitability (legitimate IDs are small). Non-blocking; optional hardening at the aggregate FGame::load boundary is explicitly deferred to P5-5 by the plan/security note.
- src/strategic/FPlayer.cpp:160 - Nested sub-load return values (FFleet::load() in the fleet loop; FVehicle::load() in all three ship loops) are not checked/propagated by FPlayer::load()/FFleet::load().
  Concurs with the specialist Security stage's second NOTE: this is pre-existing behavior unchanged by P5-3 (confirmed by diff -- the ignored return values were already ignored before this subtask) and is explicitly the responsibility of P5-5's aggregate FGame::load()/stream-state boundary per the plan's dependency ordering. The P5-3-specific failure mode this subtask was scoped to fix (unknown ship type -> createShip() NULL) IS correctly propagated as a nonzero return in all three loops. Non-blocking for this subtask.

Test sufficiency assessment:
- ADEQUATE and fully behavioral. Every acceptance criterion maps to a named test that constructs real FPlayer/FFleet/FVehicle objects, serializes through a real std::stringstream, calls the real save()/load() methods, and asserts on observed runtime state or return codes -- no criterion rests on source-text/source-structure inspection, satisfying AGENTS.md's Behavioral Verification Is Mandatory policy.
- Full round trip (unattached + fleet-with-ship + non-empty m_destroyed, no double-free): FPlayerTest::testFullRoundTripPreservesUnattachedFleetsAndDestroyedShips asserts unattached/fleet state field-by-field via getShipList()/getFleetList(); m_destroyed (which has no public accessor by design) is proven via re-save byte-for-byte equality against the original stream -- a sound indirect behavioral proof since the destroyed-ship save() section is a pure function of the restored m_destroyed contents. No-double-free is covered by the pre-existing, unmodified FPlayerTest::testDestroyedShipFreedExactlyOnceOnPlayerDestruction and testSurvivingFleetShipNotDoubleDeletedByDestroyedList (confirmed still present and registered in the CPPUNIT_TEST_SUITE).
- Jump-route-0 preservation (H4): FFleetTest::testJumpRouteZeroPreservedAcrossSaveLoad drives the real setLocation(...) API to place a fleet in transit on route 0, round-trips through save()/load(), and asserts getJumpRoute() == 0 and != FFleet::NO_ROUTE.
- Nonzero-return on unknown ship type (no NULL deref): FPlayerTest::testLoadReturnsNonzeroOnUnknownUnattachedShipType, testLoadReturnsNonzeroOnUnknownDestroyedShipType, and FFleetTest::testLoadReturnsNonzeroOnUnknownShipType each hand-craft a minimal valid wire stream with an unresolvable type tag and assert load() returns nonzero; verified createShip() (src/ships/FVehicle.cpp:364-395) does return NULL for unrecognized type strings, matching the code path under test.
- Next-ID advance past loaded ID (H3): FPlayerTest::testLoadAdvancesNextIDPastLoadedID and FFleetTest::testLoadAdvancesNextIDPastLoadedID each hand-craft a stream with m_ID = 5,000,000, load it, then default-construct a fresh object and assert its ID is strictly greater than the loaded ID.
- Fixed-width little-endian uint32_t wire format: not asserted by a dedicated byte-layout test, but exercised behaviorally by every new test via the shared writeU32/readU32 helpers (already covered by pre-existing FPObject-level tests) and by the multi-ship round trip below; acceptable per the task's own framing of this criterion as covered structurally plus exercised behaviorally through the round-trip tests.
- Multi-ship fleet round trip: FFleetTest::testSerializeRoundTripsMultipleShipsAndFleetState covers two distinct ships' count/ID/name/HP plus fleet owner/location/destination/jump-route state.
- Independently reproduced in this review: ./SSWTests run from tests/ (271/271 OK, matching Tester's reported 271) and ./TacticalTests run from tests/tactical (253/253 OK, matching Tester's reported 253); the top-level `make` and `make -C src/strategic` both completed with exit code 0, and src/strategic/Makefile was grepped directly for wx-config/WX_CXXFLAGS with zero matches, confirming the module stays wx-free.
- Two low-value, non-blocking coverage gaps noted by the specialist Security stage and independently concurred with here: no test for a partial mid-population abort (proving no leak across a half-populated ship list) and no UINT32_MAX H3-wraparound test. Neither is required by the plan's acceptance criteria and neither represents a memory-safety gap.

Documentation accuracy assessment:
- ACCURATE. doc/deferred-tasks.md's F2-serialization entry was fully removed (grep confirms zero remaining references to F2-serialization or the old 'm_destroyed... not currently serialized' language anywhere in the file), matching the plan's Documentation Impact instruction to mark it resolved/removed.
- doc/synthesized-roadmap.md's H3 row was updated to record that the FFleet/FPlayer instances of the next-ID pattern are now resolved (citing FFleetTest::testLoadAdvancesNextIDPastLoadedID and FPlayerTest::testLoadAdvancesNextIDPastLoadedID) while correctly leaving the FSystem/FJumpRoute instances open and attributed to P5-4. The H4 row was changed to '-- RESOLVED (P5-3, artifacts/phase5-save-format-hardening/P5-3)' with an inline explanation matching the actual code change (removal of the `if (m_jumpRouteID == 0) m_jumpRouteID = NO_ROUTE;` line) and a citation of FFleetTest::testJumpRouteZeroPreservedAcrossSaveLoad. The F2 risk-register bullet under Part III was updated from 'Serialization remains intentionally deferred' to describe the actual implemented type-tag + createShip() + FVehicle::load() pattern.
- include/strategic/FPlayer.h and include/strategic/FFleet.h Doxygen blocks (class headers, save()/load() method blocks, the m_destroyed member comment, and the addDestroyedShip() ownership-contract comment) were all updated to describe: the fixed-width writeU32/readU32 encoding of IDs/counts, the F2 m_destroyed persistence and why it has no public accessor (with a pointer to the specific re-save-byte-equality test that verifies its round trip indirectly, satisfying the task's explicit documentation-impact requirement on this point), the H3 next-ID advance, and (FFleet only, correctly) the H4 route-0 preservation and createShip() null-check/abort contract. Verified by direct reading of both headers' current diffed content -- no stale or contradictory statements remain (e.g. no leftover 'not currently serialized' language).

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-3/verifier_report.md
- artifacts/phase5-save-format-hardening/P5-3/verifier_result.json

Verdict:
- PASS
