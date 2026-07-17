Security Review Report

Scope reviewed:
- Specialist security review of subtask P5-4 (map/system/jump-route save-load hardening & ID pointer-mask fix) of the phase5-save-format-hardening plan, reviewing the committed Implementer+Tester+Documenter chain on branch phase5-P5-4-security-20260717 read-only (no code/test edits).
- In-scope source: include/strategic/FMap.h, src/strategic/FMap.cpp; include/strategic/FSystem.h, src/strategic/FSystem.cpp; include/strategic/FJumpRoute.h, src/strategic/FJumpRoute.cpp.
- In-scope tests: tests/strategic/FMapTest.*, FSystemTest.*, FJumpRouteTest.*. In-scope docs: AGENTS.md, doc/synthesized-roadmap.md.
- Supporting primitives reviewed for the trust boundary: core wire helpers FPObject::readU32/writeU32/readString/writeString and FSaveFormat.h (kMaxSerializedStringBytes cap), plus FMap::getSystem(unsigned int) resolution semantics.
- Verification run synchronously in the worktree: full `make -C tests` build succeeded; `SSWTests` ran green (OK, 269 tests, exit 0), matching the Tester report (258 pre-existing + 11 new P5-4 behavioral tests).

Why specialist review was triggered:
- Loading a save file is an UNTRUSTED-INPUT path (plan Section 7 Security Notes); a malformed/adversarial file must not corrupt memory, store fabricated/dangling pointers, over-allocate, or loop unboundedly.
- P5-4 removes a memory-safety-relevant defect: the prior code smuggled jump-route start/end system IDs inside FSystem* pointer fields and unpacked them with a 16-bit mask (& 0x0000FFFFL), which truncated IDs > 65535 (wrong-object resolution) and stored a fabricated pointer value in a pointer field.
- The plan marks this subtask as requiring a specialist Security stage between Documenter and Verifier.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md (subtask P5-4; Security Notes Section 7).
- Acceptance criteria AC1-AC5 as reflected in the committed tester coverage and implementer report at artifacts/phase5-save-format-hardening/P5-4/.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/strategic/FSystem.cpp:115-119 - H3 ID-counter restore at the extreme boundary (a crafted m_ID at/near UINT_MAX) can push the class counter to a wrapped/near-zero value, risking a same-session ID collision for subsequently constructed objects; FSystem uses static_cast<int>(m_ID) (implementation-defined but not UB under C++17, well-defined under C++20) and FJumpRoute (src/strategic/FJumpRoute.cpp, m_nextID = m_ID + 1 on unsigned) wraps mod 2^32.
  Impact is bounded to logical ID-collision within the running process, not memory corruption: IDs are only used for equality lookup via getSystem()/getJumpRoute() linear searches (never as array indices), and getSystem() returns NULL on no-match, so no out-of-bounds access or invalid-pointer dereference can result. This is a strict improvement over the pre-existing code, which had no H3 guard at all. Non-blocking; a future hardening pass could reject IDs at the sentinel/wrap boundary if desired.
- src/strategic/FJumpRoute.cpp:111 - m_length (float) and FSystem m_coords (float[3]) are still serialized via the native-representation write<T>/read<T> template rather than a fixed-width helper; only IDs and counts were converted to writeU32/readU32 per P5-4's stated scope.
  No memory-safety impact: read<T> reads exactly sizeof(T) bytes and returns nonzero on stream failure (checked by callers), so a truncated float still aborts the load. This is a portability/data-fidelity note (native float layout is not endian/word-size portable, and a crafted NaN/Inf is accepted as data), explicitly outside P5-4's ID/count scope. Non-blocking.
- tests/strategic/FMapTest.cpp:0 - There is no dedicated FMap-level test that truncates a nested FSystem or FJumpRoute record mid-stream and asserts FMap::load returns nonzero without leaving a half-built map.
  Coverage is effectively present transitively: FSystem::load and FJumpRoute::load truncation propagation are each covered behaviorally (testLoadReturnsNonzeroOnTruncatedStream), FMap::load's abort-and-delete wiring for a failed nested load is straightforward and code-verified (delete s / delete j; return 1), and the FMap unknown-system-ID abort path is covered directly (testLoadRejectsJumpRouteWithUnknownSystemIdAndLeavesJumpListEmpty). A direct FMap nested-truncation test would add defense in depth but its absence is not a blocking gap. Non-blocking.

Test sufficiency assessment:
- SUFFICIENT for the security-sensitive behaviors. Coverage is behavioral (constructs real objects/streams, drives load()/save(), asserts observed runtime results), not source-inspection.
- Pointer-mask removal / no truncation: FJumpRouteTest.testLoadPreservesSystemIdAbove65535 asserts start/end IDs 65537 and 131073 round-trip exactly AND that getStart()/getEnd() are NULL after load() (no fabricated pointer); FMapTest.testLoadResolvesSystemIdAbove65535ViaHandCraftedStream drives FMap::load end-to-end with IDs 70005/131079 and asserts resolution by exact ID and pointer identity.
- Absent-system null-guard: FMapTest.testLoadRejectsJumpRouteWithUnknownSystemIdAndLeavesJumpListEmpty asserts FMap::load returns nonzero and getJumpList() stays empty (no invalid pointer stored, route not pushed).
- Nested-load/stream-failure propagation: FSystemTest and FJumpRouteTest testLoadReturnsNonzeroOnTruncatedStream both assert nonzero on truncation; core readU32/read/readString all check is.fail() and readString caps at kMaxSerializedStringBytes before allocating.
- H3 ID-counter restore: FSystemTest.testLoadAdvancesNextIdPastLoadedID (pre-increment) and FJumpRouteTest.testLoadAdvancesNextIdPastLoadedID (post-increment, ID 5000000) each prove a subsequently constructed object gets a strictly greater ID.
- Wire encoding: LE byte-offset assertions in all three fixtures (testSaveWires... ) confirm fixed-width little-endian output. Full suite verified green in this run (269 tests, exit 0).

Documentation / operational guidance assessment:
- SUFFICIENT. Doxygen headers on FMap/FSystem/FJumpRoute save()/load() accurately describe the removed pointer-smuggling+0x0000FFFFL mask, the real fixed-width m_startSystemID/m_endSystemID fields, the FMap::load getSystem(id) null-guard/abort behavior, nested-load failure propagation, and the H3 pre-/post-increment counter conventions.
- getStartSystemID()/getEndSystemID() are documented as valid only after load() and that load() leaves m_start/m_end NULL for FMap::load to resolve.
- AGENTS.md and doc/synthesized-roadmap.md updates are consistent with the change. Strategic module remains wx-free (only a pre-existing commented-out //#include <wx/wx.h> in FMap.h, not introduced by P5-4).

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-4/security_report.md
- artifacts/phase5-save-format-hardening/P5-4/security_result.json

Outcome:
- PASS
