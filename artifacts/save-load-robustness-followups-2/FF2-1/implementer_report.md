# Implementer Report

Status:
- success

Task summary:
- Reject two currently-unvalidated corrupt-save fleet states at load time (reviewer follow-ups FR-A, FR-B) by extending FGame::load()'s existing FF-1 per-fleet validation loop: (FR-A) an out-of-range non-NO_DESTINATION fleet destination (system) ID, and (FR-B) the illegal getInTransit()==true with getLocation()==0 combination. Both previously reached unguarded FMap::getSystem(...) NULL dereferences inside FGame::moveFleets() on the next turn advancement. Fixed at load time (root cause); moveFleets() and all gui files are unmodified.

Changed files:
- src/strategic/FGame.cpp
- include/strategic/FGame.h
- AGENTS.md

Validation commands run:
- make -C src/strategic
- make check

Validation outcome:
- make check is green: SSWTests 297/297 passed, TacticalTests 253/253 passed, GuiTests (xvfb-run) 85/85 passed. One transient regression was found and fixed during implementation: the new in-code comment initially referenced the literal string "WXPlayerDisplay" inside src/strategic/FGame.cpp, which tripped the existing model/gui-separation source-contract test FGameHeaderDependencyTest::testSourceDoesNotUseDisplayPaintingHelpers (it asserts FGame.cpp contains no textual reference to WXPlayerDisplay/WXMapDisplay/WXGameDisplay, even in comments). Reworded the FGame.cpp comment to describe the crash sink generically ("a gui fleet-draw path") without naming the class; the equivalent Doxygen block in include/strategic/FGame.h (not covered by that source-contract test) still names WXPlayerDisplay::drawFleets() for precision. Re-ran make check clean afterward.

Implementation/code commit hash:
- 5b8ac381a1501b9bb85b666a19e5f633e9aac3ec

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-1/implementer_report.md
- artifacts/save-load-robustness-followups-2/FF2-1/tester_prompt.txt
- artifacts/save-load-robustness-followups-2/FF2-1/implementer_result.json

Implementation context:
- Fix site: FGame::load() in src/strategic/FGame.cpp, inside the existing post-player-load per-fleet validation loop (originally ~lines 717-743, now longer after this change) that runs after each successful FPlayer::load() and BEFORE fleet-to-system wiring and before m_players.push_back(p). The loop already validated location (system) ID (exempting the 0 'not in a system' sentinel) and jump-route ID (exempting FFleet::NO_ROUTE); this change adds two more checks inside the same per-fleet for-loop, after the existing jump-route check and before the fleet-to-system wiring loop, preserving the exact `delete p` -> reportLoadError(...) -> `return 1` abort pattern and ordering invariant.
- FR-A (destination): `unsigned int destination = (*f)->getDestination(); if (destination != FFleet::NO_DESTINATION && m_universe->getSystem(destination) == NULL) { delete p; reportLoadError("Cannot load save file: a fleet references a destination (system) ID that does not exist."); return 1; }`
- FR-B (illegal in-transit/location-0): `if ((*f)->getInTransit() && location == 0) { delete p; reportLoadError("Cannot load save file: a fleet is marked in-transit but has no valid origin location."); return 1; }` (reuses the `location` local already computed earlier in the loop for the FF-1 location check).
- Confirmed from include/strategic/FFleet.h: `static constexpr unsigned int NO_DESTINATION = static_cast<unsigned int>(-1);` (same value as NO_ROUTE), `getDestination()` (FFleet.h:326), `getInTransit()` (FFleet.h:167), `getLocation()` (FFleet.h:158).
- FFleet::save() field order (src/strategic/FFleet.cpp:118-142), all little-endian: m_ID (writeU32, 4B) -> m_name (writeString: writeU32 length prefix + bytes) -> m_owner (writeU32, 4B) -> m_location (writeU32, 4B) -> m_inTransit (write<bool>, native size, typically 1B) -> m_destination (writeU32, 4B) -> m_transitTime (write<int>, native, typically 4B) -> m_jumpLength (write<int>) -> m_speed (write<int>) -> m_jumpRouteID (writeU32, 4B) -> m_iconFile (writeString) -> m_isMilitia (write<bool>) -> m_home (writeString) -> m_isHolding (write<bool>) -> m_pos[0] (write<double or similar>) -> m_pos[1] -> m_dx -> m_dy -> ship count (writeU32) -> each ship's own save(). The m_destination field is the readU32 field immediately following the m_inTransit byte, matching the acceptance-criteria note. FFleet::load() mirrors this order exactly (src/strategic/FFleet.cpp:145-210).
- Model precondition worth knowing for building a valid in-transit positive-control fleet: FFleet::setLocation(FSystem*, bool transit, int time, unsigned int dest, int speed, unsigned int route) is the normal in-model way to set a fleet in transit with a real destination/route; for the byte-patched save tests it is simpler to build a valid save via the existing buildValidSaveBytes() helper (which likely places fleets stationary, i.e. m_inTransit=false) and then flip the m_inTransit byte plus patch m_destination directly, OR construct an FFleet with setLocation(...) driving a real in-transit state with a resolvable destination/route before saving, per whichever pattern buildValidSaveBytes() already follows for FF-1's jump-route positive control.
- Crash sinks these load-time checks close (reference only, NOT touched by this change): FGame::moveFleets() (src/strategic/FGame.cpp ~561-597) dereferences getSystem(getLocation())->removeFleet(...) at ~576 (failed-jump path) and ~590 (completed-jump path), and getSystem(getDestination())->addFleet(f) at ~591. getSystem(0) is always NULL because system IDs start at 1. moveFleets() itself was intentionally left unmodified per the plan's resolved design decision D1 (reject at load, not null-guard the sink).
- Doxygen updated: FGame::load()'s Doxygen block in include/strategic/FGame.h (author list already included 'Tom Stephens, Claude Sonnet 5 (medium)'; @date Last Modified was already Jul 19, 2026 and remains current) now describes destination validation and the illegal in-transit/location-0 rejection. The in-code FF-1 comment in src/strategic/FGame.cpp above the validation loop was reworded from 'located-object reference IDs' to explicitly enumerate location, jump-route, and destination, and to describe the illegal-state rejection -- but does NOT name any WX*Display class (see validation_outcome note on the source-contract test constraint). The AGENTS.md save-format contributor paragraph (the long single-source-of-truth paragraph beginning '`include/core/FSaveFormat.h` is the core-pure home...') was extended with the same tightened wording plus a new sentence describing the FF2-1/FR-B illegal-state rejection.
- No gui file was modified. FGame::moveFleets() was not modified -- confirmed via `git diff` scope (only src/strategic/FGame.cpp, include/strategic/FGame.h, AGENTS.md changed) and via re-reading moveFleets() unchanged in the final diff.
- Regression caught and fixed during implementation (see validation_outcome): watch for FGameHeaderDependencyTest's source-contract tests when touching src/strategic/FGame.cpp comments -- they forbid literal WXMapDisplay/WXPlayerDisplay/WXGameDisplay substrings anywhere in that file, including comments, to enforce the strategic/gui module boundary.

Expected validation failures carried forward:
- None
