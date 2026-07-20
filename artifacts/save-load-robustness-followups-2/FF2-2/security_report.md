Security Review Report

Scope reviewed:
- Subtask FF2-2 (reviewer follow-up FR-C) of plans/save-load-robustness-followups-2-plan.md: hardening of FPlanet::load() (src/strategic/FPlanet.cpp) against a corrupt/malicious save at the untrusted-save-input trust boundary.
- Reviewed the full committed diff on branch phase5-FF2-2-security-20260719 relative to base branch phase5: src/strategic/FPlanet.cpp, include/strategic/FPlanet.h, AGENTS.md, tests/strategic/FPlanetTest.{h,cpp}, tests/strategic/FGameSaveFormatTest.{h,cpp}.
- Cross-checked the load-abort propagation chain: FPlanet::load() -> FSystem::load() (src/strategic/FSystem.cpp) -> FMap::load() (src/strategic/FMap.cpp) -> FGame::load() aggregate-abort/no-live-singleton path.
- Cross-checked the shared save-format primitives read()/readString()/readU32() (include/core/FPObject.h, src/core/FPObject.cpp) and the createShip() factory (src/ships/FVehicle.cpp).
- Built and ran the SSWTests suite from the worktree to confirm behavioral coverage passes.

Why specialist review was triggered:
- Save files are untrusted input. Before FF2-2, FPlanet::load() called m_station = createShip(type) then m_station->load(is) with no NULL check: an unknown/corrupt station type left m_station NULL and triggered a NULL virtual-call crash (memory-safety / DoS reachable from a crafted save).
- FPlanet::load() also unconditionally returned 0, neutralizing FSystem::load()'s existing `if (p->load(is) != 0)` aggregate-abort check, so a truncated/garbage station record was silently accepted at the FPlanet level (silent-accept hole).
- The plan marks FF2-2 as 'Security review: required' because it hardens this trust boundary.

Acceptance criteria / plan reference:
- plans/save-load-robustness-followups-2-plan.md, subtask FF2-2 (reviewer follow-up FR-C).
- Acceptance intent: every own read plus the nested station load() return is checked; createShip()==NULL is NULL-checked before any m_station-> use; the nonzero return propagates to FGame::load()'s aggregate-abort/no-live-singleton path; no memory-safety regression (no NULL virtual-call, leak, double-free, or dangling pointer); success paths unchanged.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/strategic/FPlanet.cpp:63 - On a station-present record, `m_station = createShip(type)` overwrites m_station without first deleting a prior non-NULL station, unlike addStation(). If FPlanet::load() were ever invoked on an already-populated FPlanet, the prior station would leak.
  Not reachable via the untrusted-save / FSystem::load() path: FSystem::load() always constructs a fresh `new FPlanet` (m_station == NULL) before calling load(). This is a pre-existing load-on-fresh-object invariant shared with FVehicle/FFleet loads and is out of scope for FR-C. No action required for this subtask; recording for awareness only.
- src/strategic/FPlanet.cpp:58-59 - `stationCount` is read with the native-representation read<int>() rather than readU32(), matching FPlanet::save()'s write<int>() of a 0/1 flag. It is used only as a boolean gate (`if (stationCount)`) and drives at most one station allocation, never a loop, so no over-allocation or unbounded loop is possible.
  Bounded and read-checked; the only residual is cross-platform int-width portability, which is the separately-tracked SF-enum-codes / native-scalar concern, not an FR-C memory-safety issue. No action required for this subtask.

Test sufficiency assessment:
- Sufficient and behavioral. FPlanetTest adds 8 tests exercising FPlanet::load() directly against real saved bytes: nonzero return on truncated m_ID, m_name, stationCount, and station type-tag reads; unknown station type returns nonzero AND asserts getStation()==NULL with no NULL virtual-call crash (the fix-discriminating memory-safety case); truncated nested station record returns nonzero (fails on unfixed code, passes on fix); and two positive controls (planet with a FortifiedStation, and planet with no station) that assert return 0 and preserved data.
- FGameSaveFormatTest adds a full-game aggregate-abort test (unknown station type on Madderly's Star's Fortress Kdikit): asserts FGame::load() returns nonzero, exactly one IStrategicUI::showMessage() call, and zero players committed to the live singleton, proving propagation through FMap/FSystem before any player loads.
- The tester's documented decision to cover the truncated-station-record scenario only at the FPlanet unit level (because at full-game level FSystem/FMap upstream reads already abort the shortened stream on unfixed code and would not discriminate the fix) is sound and correctly reasoned.
- Built SSWTests in the worktree and ran it: the 8 FPlanetTest cases and the new FGameSaveFormatTest case pass. The 9 reported suite failures are all pre-existing FGameHeaderDependencyTest source-file-path lookups (unrelated to FF2-2; they fail only because the binary was run outside the repo-root cwd), not FPlanet/save-format regressions.

Documentation / operational guidance assessment:
- Sufficient. include/strategic/FPlanet.h Doxygen for load() accurately documents the per-read checking, the createShip() NULL-check, the nested load() check, and the FSystem::load()/FMap::load()/FGame::load() propagation-and-cleanup chain, with author/date updated per project convention.
- AGENTS.md contributor and testing notes describe the FF2-2 (FR-C) fix and its coverage consistently with the code, including the full-game vs unit-level test-placement rationale.
- The inline code comments correctly state the ownership/cleanup reasoning on both abort paths.

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-2/security_report.md
- artifacts/save-load-robustness-followups-2/FF2-2/security_result.json

Outcome:
- PASS
