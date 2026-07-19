# Implementer Report

Status:
- success

Task summary:
- Harden the save-load path (closes deferred item SF-nested-load-returns / reviewer FR-1): FPlayer::load() and FFleet::load() previously discarded the return value of nested v->load(is)/f->load(is) calls, so a stream truncated or corrupted inside a fleet's or ship's own record let the discarding caller return 0 while the underlying data was incomplete, silently bypassing FGame::load()'s per-player aggregate-abort guarantee. FPlayer::load() now checks the nested return in all three loops (m_unattached, fleet, m_destroyed) and FFleet::load() checks it in the ship loop; on a nonzero nested return the just-allocated (not-yet-owned) sub-object is deleted and the enclosing load() returns nonzero, mirroring the existing createShip()==NULL abort pattern. No behavior change on success, and the pre-existing unknown-ship-type abort path is unchanged.

Changed files:
- src/strategic/FPlayer.cpp
- include/strategic/FPlayer.h
- src/strategic/FFleet.cpp
- include/strategic/FFleet.h
- doc/deferred-tasks.md

Validation commands run:
- make -C src/strategic
- make -C . check (top-level: builds all + runs SSWTests, TacticalTests, GuiTests under xvfb-run)

Validation outcome:
- src/strategic builds clean with no new warnings. Top-level `make check` from the worktree root completed with exit code 0: SSWTests OK (289 tests), TacticalTests OK (253 tests), GuiTests OK (83 tests) -- all three counts match the pre-change baseline recorded in the Phase 5 reviewer report, confirming no regression. No behavioral test for the new nested-load-return propagation was added by this run since authoring new/updated tests is the Tester's responsibility (tests/strategic/FGameSaveFormatTest.{h,cpp}).

Implementation/code commit hash:
- 5da780fe

Artifacts written:
- artifacts/phase5-save-format-followups/FR-1/implementer_report.md
- artifacts/phase5-save-format-followups/FR-1/tester_prompt.txt
- artifacts/phase5-save-format-followups/FR-1/implementer_result.json

Implementation context:
- Fix locations: src/strategic/FPlayer.cpp FPlayer::load() -- m_unattached loop (`if (v->load(is) != 0){ delete v; return 1; }` before push_back), fleet loop (`if (f->load(is) != 0){ delete f; return 1; }` before push_back), m_destroyed loop (same pattern as m_unattached). src/strategic/FFleet.cpp FFleet::load() -- ship loop (same pattern). In every case the freshly-allocated object is NOT yet in the owning container at the point of the nonzero check, so `delete` there cannot double-free or leave a dangling container entry, and no push_back happens for the failed object -- no leak, no double-free, no partially-populated object committed.
- IMPORTANT caveat for the Tester -- pick the truncation point carefully: FVehicle::load() (src/ships/FVehicle.cpp, out of this subtask's allowed-file scope) does NOT itself check the return codes of its own internal readU32()/readString()/read<T>() calls or of its nested w->load(is)/d->load(is) weapon/defense calls; it only returns nonzero via its pre-existing createWeapon()==NULL / createDefense()==NULL abort checks. This means a stream truncated strictly inside a vehicle's own scalar-field region (HP, MR, ADF, etc., before the weapon/defense factory checks) will NOT make FVehicle::load() return nonzero today -- FVehicle::load() will read defaults/garbage and still return 0, so FFleet::load()'s new v->load(is) check will not fire for that truncation point. This is a separate, deeper gap outside FR-1's allowed files (FPlayer.cpp/FFleet.cpp only) and is not something this subtask's fix can close.
- RECOMMENDED reliable truncation point (deterministic, no reliance on uninitialized-memory behavior): truncate the byte stream to end immediately after a FLEET's own header fields and its ship-count (sCount) field have been fully written, but before the first byte of that fleet's first ship's type-tag string (i.e. cut right where FFleet::save()'s per-ship loop would begin writing `m_ships[0]->save(os)`, which starts with the ship's type string). With this cut: readString(is,type) inside FFleet::load()'s ship loop hits EOF immediately on the length-prefix read, readU32 returns 1, readString returns 1 without touching `type` (so `type` stays the empty string from its `std::string type;` declaration), createShip("") deterministically returns NULL (see the else-branch of createShip() in src/ships/FVehicle.cpp), and FFleet::load()'s PRE-EXISTING `if (v == NULL) return 1;` check fires. Before this FR-1 fix, FPlayer::load()'s fleet loop discarded that `f->load(is)` return entirely (`f->load(is); m_fleets.push_back(f);` with no check) and would push the half-built FFleet and return 0 (success) despite the truncation. After this fix, FPlayer::load() checks `f->load(is) != 0`, deletes f, and returns 1 -- so this exact truncation point is a clean, deterministic way to exercise the new FPlayer-level check landing on FFleet::load()'s existing internal abort, without depending on undefined/uninitialized memory contents anywhere in the chain.
- To build the truncation target concretely: use the existing buildValidSaveBytes() helper in tests/strategic/FGameSaveFormatTest.cpp to get a full valid save (a default-constructed game's UPF/Sathar players already have non-empty fleets with real ships, per the file's existing testLoadUnknownFactoryTypeReturnsNonzeroAndReportsExactlyOnce() comment). Locate the byte offset of the first fleet-owning player's first fleet's first ship-type string (searchable as the known wire-format encoding of a ship type name -- a 4-byte little-endian length prefix followed by the ASCII type bytes, same encoding FPObject::writeString() produces, exactly as the existing unknown-factory-type test already does for an unattached ship) and truncate the stream to end at that offset (not after it) so no byte of the type string itself is present.
- Assert on the truncated stream: FGame::load() returns nonzero; the mock IStrategicUI's showMessage() call count increments by exactly 1 (reportLoadError() reports exactly once, matching the existing truncated-header test's assertion style); and no live game singleton is committed (the existing tests do not need to inspect internal state further since FGame::load()'s existing per-player check already tears down via resetGame() at the FMainFrame layer -- FGameSaveFormatTest itself only needs to assert the return code and showMessage() count, consistent with its existing sibling tests).
- Doxygen: include/strategic/FPlayer.h and include/strategic/FFleet.h load() blocks were updated to describe the new nested-return-checking contract (bumped @date Last Modified to Jul 19, 2026; Claude Sonnet 5 (medium) was already present in both @author lists from prior P5-3 work). src/strategic/FPlayer.cpp and src/strategic/FFleet.cpp file-header @date Last Modified were likewise bumped to Jul 19, 2026.
- doc/deferred-tasks.md: the SF-nested-load-returns entry was removed entirely (following the repository's established precedent of fully removing -- not merely annotating -- resolved deferred-task entries, e.g. the prior F2-serialization and H2 entries).

Expected validation failures carried forward:
- None
