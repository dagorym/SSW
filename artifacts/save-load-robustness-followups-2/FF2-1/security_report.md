Security Review Report

Scope reviewed:
- Subtask FF2-1 (reviewer follow-ups FR-A + FR-B) of plans/save-load-robustness-followups-2-plan.md: hardening the untrusted-save-input trust boundary in FGame::load() against two crafted-save fleet states that previously reached NULL-dereference crashes (denial of service) in FGame::moveFleets() on the next turn advancement.
- Committed diff on branch phase5-FF2-1-security-20260719 relative to coordination base phase5. Production change: src/strategic/FGame.cpp (FGame::load() per-fleet validation loop only) and include/strategic/FGame.h (Doxygen). Doc: AGENTS.md. Tests: tests/strategic/FGameSaveFormatTest.{h,cpp} (3 behavioral tests).
- FR-A: reject a fleet whose destination (system) ID is non-sentinel but does not resolve against the already-loaded FMap. FR-B: reject the illegal combination getInTransit()==true with location==0.
- Verified against source: FGame::load() (src/strategic/FGame.cpp:708-780), FGame::moveFleets() sinks (src/strategic/FGame.cpp:561-597), FMap::getSystem(unsigned int) (src/strategic/FMap.cpp:72-79), FFleet sentinel/getters/constructor (include/strategic/FFleet.h:41-42,158-326; src/strategic/FFleet.cpp:22-35,145-166), FSystem ID assignment (src/strategic/FSystem.cpp:15-35), FPlayer destructor ownership (src/strategic/FPlayer.cpp:30-53).

Why specialist review was triggered:
- The plan explicitly marks FF2-1 'Security review: required'. Save files are untrusted input crossing a trust boundary into FGame::load().
- Two corrupt-save fleet states were previously accepted at load and crashed on the next turn advancement: FR-A an out-of-range non-sentinel destination reaching unguarded getSystem(getDestination())->addFleet(f) (FGame.cpp:591); FR-B inTransit==true && location==0 reaching unguarded getSystem(getLocation())->removeFleet(...) (FGame.cpp:576/590), where getSystem(0) is always NULL because system IDs start at 1.
- The remediation must reject both at load time (design decision D1: reject at the source, do not null-guard the moveFleets() sink or gui files), so trust-boundary completeness, sentinel correctness, and abort-path memory safety all fall to specialist review.

Acceptance criteria / plan reference:
- plans/save-load-robustness-followups-2-plan.md (subtask FF2-1, reviewer follow-ups FR-A + FR-B; design decision D1 reject-at-load).
- Builds on FF-1 (save-load-robustness-followups): the pre-existing per-fleet location/jump-route validation loop that FF2-1 extends.
- AGENTS.md Contributor Notes / Testing sections documenting the FGame::load() aggregate-abort and fleet located-object-ID rejection contract.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/strategic/FSystem.cpp:15-35 - System IDs themselves are not range-validated at load: FSystem::load() reads m_ID directly from the untrusted stream, so a crafted save could carry a system with ID 0 or 0xFFFFFFFF.
  This is a distinct trust-boundary concern (system-ID validation), not part of FF2-1's fleet-reference scope. It does NOT weaken FF2-1: FR-B rejects inTransit && location==0 unconditionally (the comment and code do not rely on getSystem(0) being NULL), so a phantom ID-0 system cannot re-open the FR-B crash. A phantom ID-0 system only affects a NON-transit location-0 fleet, which moveFleets() never processes, so no NULL deref results. A phantom 0xFFFFFFFF system collides with the NO_DESTINATION/NO_ROUTE sentinels but those are always treated as 'none' and skipped. Recommend tracking system-ID-range validation as a possible sibling item (FF2-2/FF2-3) if not already covered; informational only, no crash impact identified for FF2-1's scope.
- src/strategic/FGame.cpp:590 - moveFleets() line 590 does getSystem(location)->removeFleet(id) then addFleet(f) with no NULL check on the returned fleet f.
  Pre-existing behavior, not introduced or changed by FF2-1. After a validated load the load() wiring loop (FGame.cpp:772-777) guarantees the fleet is present in getSystem(location)'s list, so removeFleet returns the real fleet and consistency holds. Informational; unchanged by design decision D1.

Test sufficiency assessment:
- Sufficient. Three behavioral tests added to tests/strategic/FGameSaveFormatTest.cpp, all registered via CPPUNIT_TEST and passing (full SSWTests suite: OK, 300 tests, run from tests/ dir).
- FR-A negative (testLoadFleetWithOutOfRangeDestinationIdReturnsNonzeroAndReportsExactlyOnce): patches a real save's fleet destination field to 0x0000BEEF (non-sentinel, non-resolvable) with inTransit set, asserts load() returns nonzero, exactly one IStrategicUI::showMessage, and zero players committed.
- FR-B negative (testLoadFleetWithInTransitAndZeroLocationReturnsNonzeroAndReportsExactlyOnce): patches location=0 + inTransit=1 (destination left at NO_DESTINATION so only FR-B fires), asserts nonzero return, one message, zero players.
- Positive control (testLoadValidInTransitFleetWithResolvableDestinationSucceeds): builds a genuine in-transit fleet via FFleet::setLocation(...) with a nonzero origin and a resolvable, different destination system, round-trips through save/load, asserts rc==0 and zero messages -- proving the FR-A/FR-B checks do not falsely reject legitimate in-transit saves.
- All three construct real objects/save bytes, execute FGame::load(), and assert on observed runtime outcomes (return code, message count, committed-player count) -- genuine behavioral coverage, not source-inspection.
- Reject-at-load is the correct behavioral target under design decision D1; the moveFleets() crash itself is no longer reachable through the public load path once load rejects, so it is not separately drivable (which is the intended effect).

Documentation / operational guidance assessment:
- Sufficient. include/strategic/FGame.h Doxygen for load() updated to describe destination-ID validation and the illegal in-transit/location-0 rejection, and to name both sinks (WXPlayerDisplay::drawFleets() and FGame::moveFleets()).
- src/strategic/FGame.cpp inline comments in the validation loop clearly attribute FR-A/FR-B, explain the sentinels (NO_DESTINATION=0xFFFFFFFF, NO_ROUTE, location-0), the moveFleets() sinks, and the unconditional nature of the FR-B rejection.
- AGENTS.md updated with the FF2-1 rejection contract and test coverage note, extending the FF-1 documentation.

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-1/security_report.md
- artifacts/save-load-robustness-followups-2/FF2-1/security_result.json

Outcome:
- PASS
