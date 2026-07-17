Security Review Report

Scope reviewed:
- Specialist security review of subtask P5-1 (Core wire-format primitives & format constants) of the phase5-save-format-hardening plan, reviewed as committed (Implementer+Tester+Documenter chain already on branch); no code or tests modified.
- Files in scope: include/core/FSaveFormat.h (kSaveMagic, kSaveFormatVersion, kMaxSerializedStringBytes); include/core/FPObject.h (writeU32/readU32 decls, write<T>/read<T> templates); src/core/FPObject.cpp (writeU32/readU32/writeString/readString); tests/core/FPObjectSerializationTest.h/.cpp (behavioral coverage).
- Focus: malformed/adversarial-input handling on the untrusted save-load path -- readString length validation before allocation, stream-state/EOF checks on every helper, little-endian portability of writeU32/readU32, writeString oversize guard, and absence of new secrets/weakened checks/purity violations.

Why specialist review was triggered:
- Loading a save file is an UNTRUSTED-INPUT path (plan Section 7, Security Notes). P5-1 is the foundational serialization layer every later loader (P5-2..P5-5) depends on to bound and validate adversarial input.
- Security-critical controls introduced here: the 64 KB readString cap (kMaxSerializedStringBytes) guarding against unbounded-allocation/huge-alloc DoS; the nonzero-on-failure error contract that every downstream loader uses as its abort hook; the rewritten writeString that fixes the embedded-NUL wire-truncation bug; and the fixed-width little-endian writeU32/readU32 replacing non-portable native size_t emission.
- The plan marks every stream-parsing subtask in this phase 'Security review: required' because the hardening IS the security fix.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md -- Section 5 (P5-1 acceptance criteria, lines ~127-135), Section 7 Security Notes (lines ~388-398), Section 8 P5-1 Implementer/Tester prompts (lines ~416-473).
- Acceptance criteria evaluated: (a) readString returns nonzero and performs no allocation when the encoded length exceeds 64 KB or the stream failed; (b) writeString/readString and the uint32_t helpers round-trip including embedded-NUL strings; (c) write/read/writeString/readString return nonzero on a failed stream; (d) the 64 KB cap and magic/version are named constants.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/core/FPObject.cpp:31-42 - readU32 leaves the out-parameter v unchanged (not zeroed) on a failed/truncated read, returning 1 without touching v.
  This matches the documented contract ('unspecified on a nonzero return') and readString consumes it correctly (initializes size=0 and checks the return before use), so there is no defect in P5-1. Recorded as a contract that downstream loaders (P5-2..P5-5) must honor: they must check the nonzero return and abort before using any value produced by readU32/read<T>, never propagate a partial/stale decoded value. No change requested.
- src/core/FPObject.cpp:58-81 - The 64 KB cap bounds a SINGLE string allocation; it does not bound aggregate allocation driven by an attacker-controlled element/count field read repeatedly by a caller.
  Correct and by design for a primitive-layer subtask -- aggregate/count bounding is explicitly assigned to the downstream loader subtasks (P5-2..P5-5) per the plan. Flagged so the Verifier and later Security stages confirm attacker-controlled counts that drive loops of readString/read<T> are themselves range-checked; a huge count with valid per-element lengths could still exhaust memory if a later loader omits count validation. Not a P5-1 defect.
- src/core/FPObject.cpp:70-76 - readString allocates a transient std::vector<char>(size) and then copies into the target string via assign(), a transient double buffer up to ~128 KB.
  No security impact -- the total is still bounded by the cap. Reading directly into a resized std::string would avoid the extra copy. Optional cleanup only; not required for this subtask.

Test sufficiency assessment:
- SUFFICIENT for the security-sensitive behavior. tests/core/FPObjectSerializationTest.cpp provides behavioral (not source-inspection) coverage that constructs real streams and asserts observed runtime outcomes, exercised through a concrete FPObject subclass forwarding the protected helpers.
- Cap-before-allocation is directly observed: testReadStringRejectsOversizeLengthNoPayloadRead crafts a 131072-byte length prefix followed by only 10 payload bytes and asserts the stream read position is exactly 4 afterward (only the length prefix consumed -- no payload read and no allocation on the bad path). testReadStringBoundaryExactCapAllowed (65536 accepted) and testReadStringBoundaryOneOverCapRejected (65537 rejected via a hand-crafted writeU32 prefix) pin the boundary exactly.
- Stream-state/EOF failure is covered for every helper by testFailedStreamReturnsNonzeroForAllHelpers, including write<T>/read<T>, writeU32/readU32, writeString on a failed stream, readString on an empty stream (length-prefix failure), and readString with a valid length prefix but truncated payload.
- Wire integrity is behaviorally proven: testWriteU32LittleEndianByteLayout and testWriteU32MagicByteLayout assert observed little-endian bytes ('S','S','W','1' for kSaveMagic); testWriteStringReadStringRoundTripEmbeddedNul round-trips a 5-byte string containing an embedded NUL, proving the c_str()-truncation bug is fixed; empty and normal round-trips and full uint32_t range (0 and 0xFFFFFFFF) are covered. testWriteStringOversizeGuardRejectsWrite asserts nothing (not even the length prefix) is written when the oversize guard fires.
- The fixture is registered in tests/SSWTests.cpp and auto-compiled via the tests/core wildcard Makefile. Build succeeded and the suite ran: 258 tests, 9 failures -- all 9 are the pre-existing, environment-dependent FrontierTests::FGameHeaderDependencyTest source-inspection tests (they fail on 'Expected at least one candidate file path to be readable' due to worktree path resolution and touch none of the P5-1 files). All 13 FPObjectSerializationTest methods pass.

Documentation / operational guidance assessment:
- SUFFICIENT. FSaveFormat.h, FPObject.h, and FPObject.cpp carry accurate Doxygen headers describing the cap-before-allocation guarantee, the nonzero-on-failure contract, the little-endian primitive rationale, and the embedded-NUL fix; author/date fields updated per repo convention.
- AGENTS.md (Contributor Notes) was updated to document include/core/FSaveFormat.h as the core-pure home for the wire constants and to direct new save()/load() count/ID fields through writeU32/readU32; it honestly notes that strategic/ships/weapons/defenses call sites still use the legacy write<T>/read<T> templates pending later subtasks -- an accurate current-state description, not an overstatement.
- Core purity preserved: FSaveFormat.h includes only <cstddef>/<cstdint>; FPObject.h/.cpp introduce no wx dependency, so the non-GUI core module remains wx-free.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-1/security_report.md
- artifacts/phase5-save-format-hardening/P5-1/security_result.json

Outcome:
- PASS
