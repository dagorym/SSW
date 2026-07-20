Security Review Report

Scope reviewed:
- Committed diff of subtask FF2-3 (reviewer follow-up FR-D) of plans/save-load-robustness-followups-2-plan.md on branch phase5-FF2-3-security-20260719 relative to base branch phase5.
- Production surface under review: src/strategic/FPlayer.cpp (FPlayer::load) and src/strategic/FFleet.cpp (FFleet::load), which now return-check every container-level own scalar read against a truncated/failed untrusted save stream.
- Supporting surface reviewed for the trust-boundary claim: read primitives in include/core/FPObject.h and src/core/FPObject.cpp (read<T>, readU32, readString), the aggregate-abort caller FGame::load in src/strategic/FGame.cpp, and createShip in src/ships/FVehicle.cpp.
- Behavioral coverage reviewed: tests/strategic/FPlayerTest.cpp/.h, FFleetTest.cpp/.h, FGameSaveFormatTest.cpp/.h (6 new truncation tests). Doc/notes changes in include/strategic/FPlayer.h, include/strategic/FFleet.h, and AGENTS.md.
- Built and ran the top-level SSWTests suite in the foreground to confirm behavior.

Why specialist review was triggered:
- Save files are untrusted input crossing a deserialization trust boundary. FF2-3 is the level-up of the FF-2 silent-success fix into the FPlayer/FFleet container loaders: before this change, every one of those two loaders' OWN scalar reads (ids, names, icon strings, and the uSize/fSize/dSize/sCount count fields) discarded its return value, so a stream truncated strictly inside a player's or fleet's own scalar region silently returned 0 and a partial/garbage object was accepted as valid.
- The plan explicitly marks FF2-3 'Security review: required'. The failure class is memory-safety- and integrity-adjacent (partial object commit, downstream NULL/garbage-ID dereference, potential over-allocation/unbounded loop from adversarial count fields).

Acceptance criteria / plan reference:
- plans/save-load-robustness-followups-2-plan.md subtask FF2-3 (reviewer follow-up FR-D): extend the aggregate-abort guarantee to the container-level scalar reads in FPlayer::load() and FFleet::load(), mirroring FVehicle::load()'s FF-2 pattern; preserve the existing FR-1 nested-load cleanup, the H3 next-ID advance, and the H4 no-normalization behavior unchanged.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/strategic/FPlayer.cpp:160, 201; src/strategic/FFleet.cpp:233 - The per-element type-tag reads readString(is,type) inside the uSize/dSize (FPlayer) and sCount (FFleet) loops remain unchecked, consistent with pre-existing behavior and outside FR-D's container-scalar scope.
  Confirmed safe, not a hole: on a failed readString the local 'type' string is left at its empty-constructed value, createShip("") returns NULL (src/ships/FVehicle.cpp:466-469), and the loop aborts with return 1 before any dereference. readString also caps the decoded length against kMaxSerializedStringBytes BEFORE allocating, so a corrupt length cannot over-allocate. No action required for FF2-3; an explicit return-check could be added later purely for uniformity.
- src/strategic/FFleet.cpp:176-224 - The native-representation read<T> calls this task now return-checks (m_inTransit, m_transitTime, m_jumpLength, m_speed, m_isMilitia, m_isHolding, m_pos[0..1], m_dx, m_dy) remain endianness/width-non-portable, as does the weapon/defense m_type enum ordinal (tracked as deferred SF-enum-codes).
  Unchanged by FF2-3, which only adds return-checks (a strict robustness improvement). This is the documented deferred non-portability concern in doc/deferred-tasks.md, not a regression and not a trust-boundary failure; no bounded-input over-read is introduced. Recorded for continuity only.

Test sufficiency assessment:
- Sufficient and behavioral. FPlayerTest and FFleetTest each add two truncation tests that construct a real stream, truncate it strictly inside the loader's own scalar region (first field boundary and last count field mid-4-bytes), call the real load(), and assert nonzero return plus empty containers (no partial commit / no leak) -- these fail on the pre-FF2-3 code (which returned 0) and pass on the fix.
- FGameSaveFormatTest adds two end-to-end aggregate-path tests driving FGame::load() over a real save truncated inside a player's own scalar region and inside a fleet's own scalar region, asserting nonzero return, exactly one IStrategicUI::showMessage call (single report), and zero committed players (no live half-built singleton). The fleet test deliberately targets the last fleet and truncates after m_destination/m_jumpRouteID to avoid passing for the wrong reason via FF-1's independent ID validation -- a correct fix-discrimination design.
- Ran SSWTests in the foreground: Run 315, Failures 9, Errors 0. All 9 failures are the pre-existing FGameHeaderDependencyTest source-inspection fixture ('Expected at least one candidate file path to be readable'), which is not part of the FF2-3 diff and is unrelated to the FPlayer/FFleet loaders; every FF2-3 behavioral fixture passes.
- Success path unchanged: the existing valid multi-player / multi-fleet round-trip, H3 next-ID advance, and H4 no-normalization tests continue to pass; no false rejection observed.

Documentation / operational guidance assessment:
- Sufficient. In-code Doxygen/rationale comments in both loaders explain the FR-D return-check intent and its FF-2 lineage, and the pre-existing FR-1 nested-load cleanup comments correctly remain. include/strategic/FPlayer.h and FFleet.h Doxygen and the AGENTS.md contributor/testing notes were updated to describe the extended aggregate-abort guarantee. No operational/rollout guidance gaps: this is a hardening change to an existing untrusted-input path with no new config, defaults, or external surface.

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-3/security_report.md
- artifacts/save-load-robustness-followups-2/FF2-3/security_result.json

Outcome:
- PASS
