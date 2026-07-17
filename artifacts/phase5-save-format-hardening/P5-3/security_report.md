Security Review Report

Scope reviewed:
- Subtask P5-3 (Player/fleet save-load hardening) of the phase5-save-format-hardening plan, reviewed as committed on branch phase5-P5-3-security-20260717 (Implementer+Tester+Documenter chain already committed; no code/test edits made).
- In-scope changed surface: include/strategic/FPlayer.h, src/strategic/FPlayer.cpp (save/load, m_destroyed serialization, destructor ownership); include/strategic/FFleet.h, src/strategic/FFleet.cpp (save/load, H3/H4); tests/strategic/FPlayerTest.*, tests/strategic/FFleetTest.*; doc/deferred-tasks.md and doc/synthesized-roadmap.md doc updates.
- Supporting (unchanged) surfaces read for context: FPObject::readU32/writeU32/readString caps (src/core/FPObject.cpp, include/core/FSaveFormat.h), createShip() factory (src/ships/FVehicle.cpp:364), FMap::getJumpRoute(id) route lookup (src/strategic/FMap.cpp:305) and its FGame consumer (src/strategic/FGame.cpp:580-586).
- Threat model: loading a save file is an UNTRUSTED-INPUT path; attacker controls player IDs/counts, the unattached ship list, the fleet list and each fleet's ship list, the newly-serialized destroyed-ship list, and navigation fields (owner/location/destination/jumpRoute).

Why specialist review was triggered:
- Plan Section 7 (Security Notes) flags the FPlayer/FFleet wire layer as parsing attacker-controllable data and requires specialist Security review between Documenter and Verifier.
- Adds a new attacker-reachable deserialization surface: FPlayer::m_destroyed (F2-serialization), whose ships are sole-owned by FPlayer and freed only in ~FPlayer() -- double-free / leak risk if the ownership contract is broken on a partial/aborted load.
- Three separate factory-backed ship-list loops (FPlayer m_unattached, FPlayer m_destroyed, FFleet ships) each construct objects from an untrusted type tag via createShip(), which returns NULL for unknown types -- NULL-deref crash risk (DoS) if unchecked.
- H4 removes a load-side normalization (route 0 -> NO_ROUTE); H3 restores static ID counters from untrusted IDs -- risk of out-of-range lookups, ID collision, integer wraparound, or UB.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md -- subtask P5-3 acceptance criteria (F2-serialization of m_destroyed, H3 ID-counter restore, H4 route-0 removal, createShip() null-checks, fixed-width writeU32/readU32 IDs/counts) and Section 7 Security Notes.
- Precedent subtasks P5-1 (FSaveFormat magic/version + readString cap) and P5-2 (FVehicle wire hardening + factory null-checks), whose patterns P5-3 mirrors.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/strategic/FFleet.cpp:150 - H3 next-ID advance can integer-wrap under an extreme crafted ID (near UINT32_MAX), marginally weakening the ID-uniqueness intent; not a memory-safety defect.
  For FFleet, m_ID = 0xFFFFFFFF makes (int)(m_ID+1) = 0 and m_ID = 0x7FFFFFFF makes (int)(m_ID+1) = INT_MIN, seeding a zero/negative counter; for FPlayer (src/strategic/FPlayer.cpp:138) m_ID = 0xFFFFFFFF makes m_nextID wrap to 0. No UB (unsigned->signed out-of-range conversion is implementation-defined/modular on all target platforms and the FFleet comparison is cast-guarded) and no memory hazard, but a freshly constructed object could then reuse a low ID. Exploitability is negligible in practice (real IDs are tiny). Optional hardening: validate/clamp loaded IDs at the aggregate FGame::load boundary (P5-5).
- src/strategic/FPlayer.cpp:160 - Return values of nested sub-loads (FFleet::load in the fleet loop, and FVehicle::load in all three ship loops) are ignored, so a mid-object corruption is not surfaced as a distinct nonzero by the immediate caller. Pre-existing; unchanged by P5-3; out of this subtask's scope.
  A failure inside a nested load leaves the stream in a failed state; the next readU32 then yields count 0 (readU32 leaves its out-param unmodified on failure) and FPlayer::load can return 0 for trailing sections, masking the nested failure as success. This is caught by the aggregate stream-state / magic-version check at the FGame::load boundary, which the plan explicitly assigns to P5-5. The specific P5-3 failure mode (unknown ship type -> createShip NULL) IS propagated as nonzero. Recommend P5-5 confirm the FGame boundary rejects a stream left in a failed state so masked nested failures cannot pass.

Test sufficiency assessment:
- ADEQUATE for the P5-3 acceptance surface. All coverage is behavioral (constructs real FPlayer/FFleet, serializes to a stringstream, loads, and asserts on runtime state / return codes), satisfying the repository's behavioral-verification mandate.
- Unknown/corrupt ship type abort covered for all three loops: FFleetTest::testLoadReturnsNonzeroOnUnknownShipType (fleet ships), FPlayerTest::testLoadReturnsNonzeroOnUnknownUnattachedShipType (m_unattached), FPlayerTest::testLoadReturnsNonzeroOnUnknownDestroyedShipType (m_destroyed) -- each asserts load() returns nonzero with no NULL deref.
- F2 m_destroyed round-trip verified in FPlayerTest::testFullRoundTripPreservesUnattachedFleetsAndDestroyedShips via re-save byte equality (m_destroyed has no public accessor); unattached + fleet + fleet-ship state also asserted field-by-field. FFleetTest::testSerializeRoundTripsMultipleShipsAndFleetState covers multi-ship fleet round-trip.
- H3 next-ID advance covered for both classes (FPlayerTest / FFleetTest testLoadAdvancesNextIDPastLoadedID). H4 route-0 preservation covered (FFleetTest::testJumpRouteZeroPreservedAcrossSaveLoad asserts loaded route 0 stays 0 and != NO_ROUTE).
- No-double-free ownership across the m_destroyed / fleet boundary is covered by the pre-existing FPlayerTest::testSurvivingFleetShipNotDoubleDeletedByDestroyedList.
- Verified locally: built tests/ and ran ./SSWTests (Run: 271, Failures: 9, Errors: 0). All 9 failures are the pre-existing FGameHeaderDependencyTest source-inspection tests failing on a worktree file-path-resolution artifact ('Expected at least one candidate file path to be readable'); they inspect FGame wx-purity, are unrelated to P5-3 (last modified in commit 0c0d1212, milestone 4), and are not security-relevant. Every FPlayerTest and FFleetTest case passed.
- Minor (non-blocking) coverage gaps: no explicit test for a partial mid-population abort (some ships loaded, then an unknown type) proving no leak/double-free, and no UINT32_MAX H3-wraparound test. Both are low value -- the abort-path tests already construct and destruct the objects under normal teardown without crashing, and the wraparound case is only a uniqueness edge, not a memory hazard.

Documentation / operational guidance assessment:
- ADEQUATE. Doxygen save/load headers in FPlayer.h and FFleet.h were updated to document the fixed-width writeU32/readU32 encoding of IDs and counts, the F2 serialization of m_destroyed (with its sole-ownership contract, freed in ~FPlayer()), the H3 next-ID advance, the H4 removal of the route-0->NO_ROUTE normalization, and the createShip() null-check/abort contract.
- The m_destroyed member comment and ~FPlayer() destructor comment describe the ownership/no-double-free contract that keeps the aborted-load path safe.
- deferred-tasks.md F2-serialization entry removed (now resolved) and synthesized-roadmap.md synced with RESOLVED annotations, keeping the deferred-work ledger accurate.
- Operational note for downstream: the FPlayer/FFleet layer propagates the targeted unknown-ship-type failure as a nonzero return, but final aggregate rejection of a corrupt/truncated stream (magic/version + end-of-stream state) is documented as P5-5's responsibility -- see NOTE finding on nested error propagation.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-3/security_report.md
- artifacts/phase5-save-format-hardening/P5-3/security_result.json

Outcome:
- PASS
