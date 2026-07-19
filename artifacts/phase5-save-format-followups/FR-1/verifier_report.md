Verifier Report

Scope reviewed:
- Implementer (5da780fe): FPlayer::load() now checks the nested v->load(is)/f->load(is) return in the m_unattached loop, fleet loop, and m_destroyed loop; FFleet::load() now checks the nested v->load(is) return in its ship loop. On any nonzero nested return the just-allocated (not-yet-owned) sub-object is deleted and the enclosing load() returns nonzero, mirroring the pre-existing createShip()==NULL abort pattern. Doxygen headers on include/strategic/FPlayer.h, include/strategic/FFleet.h, src/strategic/FPlayer.cpp, src/strategic/FFleet.cpp updated in the same commit; doc/deferred-tasks.md's SF-nested-load-returns entry removed.
- Tester (4e6b1185): added testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce to tests/strategic/FGameSaveFormatTest.{h,cpp} -- a fully behavioral test building a real valid FGame save, truncating immediately before a fleet's first ship's own type-tag, calling the real FGame::load(), and asserting rc != 0 and the mock IStrategicUI's showMessageCalls increments by exactly 1.
- Documenter (ef12f02b): one-sentence addendum to AGENTS.md's save-format Contributor Notes paragraph documenting that FR-1 makes the aggregate-abort guarantee hold even for deep-truncation-inside-a-fleet input; no other files touched.
- Security (6a9c4ea3): specialist review of the untrusted-save-input trust boundary -- PASS, 0 blocking, 0 warning, 2 non-blocking notes (FVehicle::load() internal-read-checking gap out of scope for FR-1; the three non-fleet-loop nested-check branches are covered by structural inspection rather than independent behavioral tests each).
- This Verifier pass independently re-read the full 5da780fe and 4e6b1185 diffs, traced FVehicle/FFleet destructors to confirm no leak/double-free/dangling-entry on the delete-then-return-nonzero paths, confirmed the doc/deferred-tasks.md entry was fully removed (grep, no remaining SF-nested-load-returns references outside AGENTS.md's descriptive prose), confirmed the AGENTS.md addendum is accurate against the shipped code, confirmed doc/rules/tactical_operations_manual.md is untouched, and independently rebuilt and ran the full `make check` suite from a clean invocation in this worktree.

Acceptance criteria / plan reference:
- plans/phase5-save-format-followups-plan.md, FR-1 subtask (Reviewer follow-up from artifacts/phase5-save-format-hardening/reviewer_report.md, which flagged SF-nested-load-returns as a non-blocking deferral).
- FR-1's stated acceptance criteria (implementation-outcome): every nested v->load(is)/f->load(is) call in FPlayer::load()/FFleet::load() has its return checked; a nonzero nested return frees the just-created sub-object and returns nonzero with no dangling entry, leak, or double-free; the existing unknown-ship-type (createShip==NULL) abort behavior is preserved; a save stream truncated inside a fleet's ship record makes FGame::load() return nonzero and route through reportLoadError()/IStrategicUI::showMessage(), committing no live game singleton; make check is green.
- FR-1's Documentation impact note: Doxygen save/load headers on the touched FPlayer/FFleet functions; doc/deferred-tasks.md mark SF-nested-load-returns resolved/removed; AGENTS.md change only if the existing save-format note's error-contract wording needs a one-line accuracy tweak -- all satisfied.

Convention files considered:
- AGENTS.md (Contributor Notes save-format paragraph; Function Comments/Doxygen convention; Single-Source-of-Truth Rule; Non-negotiable Constraints re: doc/rules/tactical_operations_manual.md)
- CLAUDE.md (pointer file, no independent facts)
- doc/deferred-tasks.md (resolved-entry-full-removal convention, per F2-serialization/H2 precedent)
- plans/phase5-save-format-followups-plan.md (FR-1 subtask: allowed files, task description, acceptance criteria, documentation impact)

Findings

BLOCKING
- None

WARNING
- None

NOTE
- tests/strategic/FGameSaveFormatTest.cpp:313-368 - The single new behavioral test exercises only the FPlayer::load() fleet-loop's new f->load(is)!=0 check (via a truncation that trips FFleet::load()'s pre-existing createShip("")==NULL abort one level down). The other three newly-added nested-check sites -- FPlayer::load()'s m_unattached-loop and m_destroyed-loop v->load(is)!=0 checks, and FFleet::load()'s own ship-loop v->load(is)!=0 check triggered by a genuine post-type-tag vehicle-body failure rather than createShip()==NULL -- are not each independently exercised by a dedicated behavioral test.
  This was already identified and explicitly accepted as non-blocking by the specialist Security review (security_report.md notes, lines 33-34/41), which reasoned that all four sites are byte-for-byte identical delete-then-return-nonzero patterns (confirmed independently in this Verifier pass via direct diff/read of src/strategic/FPlayer.cpp and src/strategic/FFleet.cpp), that FVehicle::load() cannot currently return nonzero for a truncation strictly inside its own scalar-field region (a separately tracked, deliberately out-of-scope gap), and that the one chosen truncation point is the only one that deterministically exercises the propagation contract without depending on that unchecked FVehicle internal-read gap. Concur with Security's non-blocking classification: the security-critical trust-boundary property (the exact SF-nested-load-returns defect scenario) is demonstrated end-to-end by a real behavioral test that fails pre-fix and passes post-fix, and the remaining three sites are verified safe by direct code inspection (no leak/double-free: FVehicle::~FVehicle() and FFleet::~FFleet() both cleanly free owned sub-objects, and the deleted v/f pointers are local variables not yet reachable from any container at the point of delete).

Test sufficiency assessment:
- testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce is a genuine behavioral test: it builds a real valid FGame save via buildValidSaveBytes(), truncates the stream immediately before a fleet's first ship's type-tag, calls the real FGame::load() (not a mock), and asserts the observed runtime outcome (rc != 0, mock IStrategicUI showMessageCalls incremented by exactly 1). Verified by direct read of tests/strategic/FGameSaveFormatTest.cpp:313-368 that this fails against the pre-fix code (pre-FR-1, FPlayer::load()'s fleet loop discarded FFleet::load()'s nonzero return and returned 0) and passes after the fix.
- Pre-existing negative coverage remains intact and green: wrong-magic, unsupported-version, header-truncation, unknown-factory-type, and no-UI console-fallback tests are unmodified and still pass, confirming the existing createShip()==NULL abort path is preserved unchanged.
- This Verifier pass independently rebuilt and ran the full suite from a clean invocation in this worktree (make check): SSWTests 290/290, TacticalTests 253/253, GuiTests 83/83, all green, no failures reported in output -- consistent with, and independently confirming, the Tester's and Security's reported results.
- One test-sufficiency observation (see NOTE finding): three of the four newly-added nested-return-check branches are verified by structural code inspection and destructor-based memory-safety reasoning rather than each having an independent behavioral test. Non-blocking, concurring with Security's assessment, given the identical three-line pattern across all four sites and the confirmed absence of any discrepancy between them.

Documentation accuracy assessment:
- Doxygen headers on FPlayer::load()/FFleet::load() (include/strategic/FPlayer.h:294-313, include/strategic/FFleet.h:262-279) accurately describe the new nested-return-checking contract, with @date Last Modified bumped to Jul 19, 2026 and Claude Sonnet 5 (medium) present in @author alongside prior authors (no authors removed). Same for the .cpp file-header dates.
- doc/deferred-tasks.md's SF-nested-load-returns entry is fully removed (verified via grep across doc/deferred-tasks.md, AGENTS.md, and doc/synthesized-roadmap.md -- the only remaining textual reference is descriptive prose in AGENTS.md's Contributor Notes, not a deferred-item listing), consistent with this repository's full-removal-on-resolution convention (F2-serialization, H2 precedent).
- AGENTS.md's save-format Contributor Notes paragraph (line 404) was accurately synced by the Documenter's one-line addendum (commit ef12f02b): it now states that FR-1 made FPlayer::load()/FFleet::load() check every nested FVehicle::load()/FFleet::load() return in their respective loops, deleting the just-allocated sub-object and returning nonzero on failure, so the aggregate-abort guarantee holds even for truncation deep inside a single ship's or fleet's own record. This neither overstates (it does not claim FVehicle-internal scalar-field truncation is caught -- a separately tracked gap) nor understates (it no longer implies the guarantee is only as strong as the top-level per-player check) the actual guarantee. Verified against the underlying code by direct read.
- doc/rules/tactical_operations_manual.md is confirmed untouched (git diff 817485a7..HEAD on that path is empty), honoring the non-negotiable constraint in AGENTS.md.
- No duplication introduced: the AGENTS.md addendum states the FR-1 fact once, in the location already discussing the related FGame::load() top-level guarantee; doc/synthesized-roadmap.md was deliberately left unchanged, consistent with the Single-Source-of-Truth Rule.

Artifacts written:
- artifacts/phase5-save-format-followups/FR-1/verifier_report.md
- artifacts/phase5-save-format-followups/FR-1/verifier_result.json

Verdict:
- PASS
