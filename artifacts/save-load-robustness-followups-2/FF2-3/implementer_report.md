# Implementer Report

Status:
- success

Task summary:
- Extend the save-load aggregate-abort guarantee to the container-level scalar reads in FPlayer::load() (src/strategic/FPlayer.cpp) and FFleet::load() (src/strategic/FFleet.cpp) (reviewer follow-up FR-D), mirroring FF-2's per-read return-checking pattern already applied to FVehicle::load(). Both loaders previously discarded every one of their own container-level scalar read returns (FPlayer: player ID, name, icon name, and the unattached/fleet/destroyed-ship count reads; FFleet: ID, name, owner, location, in-transit flag, destination, transit time, jump length, speed, jump-route ID, icon file, militia flag, home, holding flag, position[0]/[1], dx, dy, and the ship-list count), so a stream truncated strictly inside a player's or a fleet's own scalar region (not inside a nested vehicle record) still returned 0 -- the same silent-success hole FF-2 closed one level down at FVehicle::load(). Fixed by wrapping every one of those reads in `if (readU32/readString/read(is,...)) { return 1; }`, exactly mirroring the FVehicle::load() pattern, while leaving the existing per-element createShip()==NULL checks, the nested v->load()/f->load() checks and their not-yet-owned-object cleanup, the H3 next-ID advance, and the H4 no-normalization jump-route behavior unchanged. A nonzero return now propagates through the already-FR-1-hardened FPlayer/FFleet callers up to FGame::load()'s aggregate-abort/no-live-singleton path.

Changed files:
- src/strategic/FPlayer.cpp
- include/strategic/FPlayer.h
- src/strategic/FFleet.cpp
- include/strategic/FFleet.h

Validation commands run:
- cd tests/strategic && make
- cd tests && make
- make check

Validation outcome:
- make check is green from a rebuilt tree: SSWTests 309/309 passed, TacticalTests 253/253 passed, GuiTests (xvfb-run) 85/85 passed (all three suites exit 0, run sequentially by the repo-root make check target). No regressions observed; no existing test needed updating; tests/strategic (libstrategicTests.a) and the full tests/ tree rebuilt clean with no new warnings ahead of the make check run.

Implementation/code commit hash:
- b2891996d4747eaf14564374602a2c1ecbcf7ca5

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-3/implementer_report.md
- artifacts/save-load-robustness-followups-2/FF2-3/tester_prompt.txt
- artifacts/save-load-robustness-followups-2/FF2-3/implementer_result.json

Implementation context:
- FPlayer::load() field/read order on the wire (mirrors FPlayer::save(), src/strategic/FPlayer.cpp:108-129): id (readU32) -> m_name (readString) -> m_iconName (readString) -> uSize (readU32, unattached-ship count) -> [uSize x (type readString, createShip(), FVehicle::load())] -> fSize (readU32, fleet count) -> [fSize x FFleet::load()] -> dSize (readU32, destroyed-ship count) -> [dSize x (type readString, createShip(), FVehicle::load())]. Every one of the id/m_name/m_iconName/uSize/fSize/dSize reads is now wrapped in `if (readU32(...)) { return 1; }` / `if (readString(...)) { return 1; }`, placed immediately after each read call and before the value is consumed/assigned (the H3 next-ID-advance logic that reads m_ID still runs only after the id read succeeds). No object is allocated before these reads run, so returning 1 on failure requires no cleanup.
- FFleet::load() field/read order on the wire (mirrors FFleet::save(), src/strategic/FFleet.cpp:118-143): id (readU32) -> m_name (readString) -> owner (readU32) -> location (readU32) -> m_inTransit (read<bool>) -> destination (readU32) -> m_transitTime (read<int>) -> m_jumpLength (read<int>) -> m_speed (read<int>) -> jumpRouteID (readU32) -> m_iconFile (readString) -> m_isMilitia (read<bool>) -> m_home (readString) -> m_isHolding (read<bool>) -> m_pos[0] (read<double>) -> m_pos[1] (read<double>) -> m_dx (read<double>) -> m_dy (read<double>) -> sCount (readU32, ship count) -> [sCount x (type readString, createShip(), FVehicle::load())]. Every one of these 18 own-scalar reads is now wrapped in the same if-return-1 pattern, in the same order, with the same H3 next-ID-advance and H4 no-normalization-of-route-0 comments/behavior preserved exactly where they were.
- Exact mirrored pattern (from FVehicle::load(), src/ships/FVehicle.cpp:215-279, the FF-2 reference implementation): `if (readU32(is,id)){ return 1; } m_ID = id;` etc. -- read-then-check-then-assign, never assign-before-check.
- Ownership/no-leak reasoning (confirmed): the scalar reads touched by this task all run before any allocation (createShip()/new FFleet()/new FVehicle) in both loaders' loops, so a scalar-read failure at any of these points has no allocated sub-object to free -- returning nonzero is sufficient and no delete/cleanup was added or needed for these specific failure sites. The pre-existing createShip()==NULL guards and nested v->load()/f->load() failure paths (which DO delete a just-allocated, not-yet-owned sub-object before returning 1) are completely unchanged by this task.
- Callers: FPlayer::load() is invoked from FGame::load() (already FR-1-hardened to check FPlayer::load()'s return and delete the not-yet-pushed player before push_back on failure) and FFleet::load() is invoked both from FPlayer::load()'s own fleet loop (already checks f->load()!=0, deletes f, returns 1) and, indirectly, forms part of the wire data FGame::load() consumes -- so a new nonzero return from either loader for a purely-container-scalar truncation now reaches FGame::load()'s existing aggregate-abort/no-live-singleton/single-reportLoadError() path exactly the way an FF-1/FF-2 nested-load failure already does; no caller code needed to change.
- Doxygen: include/strategic/FPlayer.h's load() block and include/strategic/FFleet.h's load() block were both extended (not rewritten) with a new paragraph documenting the FF2-3/FR-D container-scalar return-checking, immediately after the existing FR-1 nested-load-return paragraph. @author lines were left as-is (already list 'Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)' from the prior FR-1 pass) since Claude Sonnet 5 was already present in the author list; @date Last Modified was already Jul 19, 2026 (today's date) from the FR-1 pass and remains Jul 19, 2026 for this pass.
- Scope discipline: only the four allowed files were touched (src/strategic/FPlayer.cpp, include/strategic/FPlayer.h, src/strategic/FFleet.cpp, include/strategic/FFleet.h). No test files were added or modified by the Implementer -- behavioral coverage is the Tester's responsibility per the task's explicit Tester-handoff note; do not add tester-owned files.

Expected validation failures carried forward:
- None
