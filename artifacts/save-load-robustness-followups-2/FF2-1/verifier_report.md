Verifier Report

Scope reviewed:
- Reviewed the combined Implementer (5b8ac381), Tester (f44ac0c1), and Documenter (6214a17b) diffs against base plan commit 0f6a5bd2 on branch phase5-FF2-1-verifier-20260719 (HEAD 6c7549e4, which also includes the committed FF2-1 security review).
- Implementation: src/strategic/FGame.cpp -- FGame::load()'s existing per-fleet validation loop (post-player-load, pre-fleet-wiring) extended with FR-A (destination (system) ID must resolve via FMap::getSystem(id) unless FFleet::NO_DESTINATION) and FR-B (reject getInTransit()==true with location==0) checks; include/strategic/FGame.h -- load() Doxygen updated to describe both new checks and both crash sinks (WXPlayerDisplay::drawFleets() and FGame::moveFleets()).
- Tests: tests/strategic/FGameSaveFormatTest.{h,cpp} -- three new behavioral CppUnit tests: testLoadFleetWithOutOfRangeDestinationIdReturnsNonzeroAndReportsExactlyOnce (FR-A negative), testLoadFleetWithInTransitAndZeroLocationReturnsNonzeroAndReportsExactlyOnce (FR-B negative), testLoadValidInTransitFleetWithResolvableDestinationSucceeds (positive control).
- Documentation: AGENTS.md Testing-section FGameSaveFormatTest bullet (names the three new tests) and the save-format contributor paragraph (Implementer edit describing the FR-A/FR-B rejection contract and the now-two-sink crash description).
- Specialist Security review already ran and is committed at artifacts/save-load-robustness-followups-2/FF2-1/security_report.md: PASS, 0 BLOCKING/WARNING, 2 informational NOTEs (FSystem::load() ID-range validation gap, and moveFleets():590 unchecked removeFleet() return) both explicitly scoped out of FF2-1 and confirmed not to weaken this fix.

Acceptance criteria / plan reference:
- plans/save-load-robustness-followups-2-plan.md, subtask FF2-1 (FR-A + FR-B), lines ~198-230 and ~437-480 (FF2-1 prompt) for the exact fix-site and check-order specification; design decision D1 (line 70, RESOLVED user-confirmed) mandates reject-at-load with moveFleets() and gui files left unmodified; design decision A2 (line 97) confirms FR-A validates m_destination unconditionally whenever it is not NO_DESTINATION (not gated on getInTransit()).

Convention files considered:
- AGENTS.md (Doxygen mandate, module boundary rules -- no wx-literal substrings in src/strategic/FGame.cpp per FGameHeaderDependencyTest, Testing section, single-source-of-truth contributor-notes paragraph)
- CLAUDE.md (pointer to AGENTS.md, no independent facts)
- doc/rules/tactical_operations_manual.md -- not touched, not relevant to this subtask

Findings

BLOCKING
- None

WARNING
- None

NOTE
- None

Test sufficiency assessment:
- Sufficient and behavioral, not source-inspection: all three new tests construct a real FGame singleton, a real byte-accurate save stream (either byte-surgery on a genuine buildValidSaveBytes() save or a genuine FFleet::setLocation(...)-built in-transit fleet), call the real FGame::load(std::istream&), and assert on observed runtime outcomes (return code, RecordingUI::showMessageCalls count, g.getPlayers().size()).
- Independently re-ran the behavioral-test mandate myself: temporarily commented out both the FR-A and FR-B blocks in src/strategic/FGame.cpp (lines ~751-769), rebuilt src/strategic and tests/SSWTests, and re-ran -- both testLoadFleetWithOutOfRangeDestinationIdReturnsNonzeroAndReportsExactlyOnce and testLoadFleetWithInTransitAndZeroLocationReturnsNonzeroAndReportsExactlyOnce FAILED (rc != 0 assertion failed, i.e. load() incorrectly returned 0/success) while testLoadValidInTransitFleetWithResolvableDestinationSucceeds continued to PASS in both the unfixed and fixed states. Restored the file afterward; git diff confirmed FGame.cpp returned to byte-identical with the committed version before re-running the full make check.
- Byte-offset math in the two negative tests was independently verified against FFleet::save()'s actual field order (src/strategic/FFleet.cpp:118-142: id, name, owner, location, inTransit, destination, ...) -- the computed ownerPos/locationPos/inTransitPos/destinationPos offsets are correct.
- Positive control is a meaningful non-degenerate case: it builds a genuine in-transit fleet via the normal in-model FFleet::setLocation(FSystem*, true, ...) API with a real nonzero origin and a real, different, resolvable destination system, round-trips it through a real FGame::save()/load(), and asserts rc==0 with zero showMessage() calls -- proving the two new checks do not falsely reject a legitimate in-transit save.
- Edge case coverage confirmed adequate: FR-A fires independent of getInTransit() (matching plan decision A2) and the negative tests correctly isolate each check (FR-A test leaves location resolvable so only FR-A fires; FR-B test leaves destination at NO_DESTINATION so only FR-B fires), avoiding an ambiguous multi-cause failure.
- make check (repo root) re-run independently by the verifier: SSWTests OK (300), TacticalTests OK (253), GuiTests OK (85 under xvfb-run) -- exact match to the stated acceptance target.

Documentation accuracy assessment:
- include/strategic/FGame.h's load() Doxygen block accurately and completely describes the extended validation (location, jump-route, destination) and the illegal in-transit/location-0 rejection, and correctly names both crash sinks (WXPlayerDisplay::drawFleets() and FGame::moveFleets()) that the load-time rejection forecloses -- verified against the actual moveFleets() source (src/strategic/FGame.cpp:561-597, unmodified) which does contain the described unguarded getSystem(getLocation())->removeFleet(...)/getSystem(getDestination())->addFleet(...) NULL-dereference sinks at lines 576/590/591.
- src/strategic/FGame.cpp's in-code FF-1/FF2-1 comment above the validation loop was correctly tightened to enumerate location, jump-route, and destination, and deliberately avoids any literal WXMapDisplay/WXPlayerDisplay/WXGameDisplay substring (confirmed via grep: zero matches in src/strategic/FGame.cpp) so as not to trip FGameHeaderDependencyTest's model/gui-separation source-contract checks -- the Implementer's report documents this as a caught-and-fixed transient regression during development, and the final committed state is clean (make check passes FGameHeaderDependencyTest along with everything else).
- AGENTS.md's Testing-section FGameSaveFormatTest bullet (Documenter edit) accurately names all three new tests and their behavior; the save-format contributor paragraph (Implementer edit) accurately describes the FF2-1 FR-A/FR-B rejection contract, correctly attributes it to the save-load-robustness-followups-2-plan.md FF2-1 subtask, and correctly states neither the gui draw path nor FGame::moveFleets() is modified -- independently confirmed true via diff scope (only src/strategic/FGame.cpp, include/strategic/FGame.h, AGENTS.md, and the two test files changed; moveFleets() body byte-identical to base).
- No duplication or contradiction found between the FGame.h Doxygen, the FGame.cpp inline comment, and the AGENTS.md paragraph -- each is calibrated to its audience (API contract, implementation rationale, cross-cutting save-format policy) and the facts are mutually consistent.

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-1/verifier_report.md
- artifacts/save-load-robustness-followups-2/FF2-1/verifier_result.json

Verdict:
- PASS
