# Implementer Report

Status:
- success

Task summary:
- Harden FPlanet::load() (src/strategic/FPlanet.cpp) against a corrupt/malicious save (reviewer follow-up FR-C). Previously load() called m_station = createShip(type) then m_station->load(is) with no NULL check -- an unknown station type left m_station NULL and triggered a NULL virtual-call crash -- and it unconditionally returned 0, neutralizing FSystem::load()'s existing if (p->load(is) != 0) abort check, leaving this depth entirely outside the aggregate-abort guarantee. Fixed by checking the return of every own read (m_ID, m_name, stationCount, and the station type tag when a station is present), NULL-checking createShip()'s result before calling m_station->load(is) (mirroring the createShip()==NULL abort pattern already used in FFleet/FPlayer::load()), checking the nested m_station->load(is) return, and returning nonzero on any failure instead of the previous unconditional return 0. This lets FSystem::load()'s existing check fire, propagating the abort through FMap::load() to FGame::load()'s aggregate-abort/no-live-singleton path.

Changed files:
- src/strategic/FPlanet.cpp
- include/strategic/FPlanet.h

Validation commands run:
- make -C tests/strategic
- make check

Validation outcome:
- make check is green: SSWTests 300/300 passed, TacticalTests 253/253 passed, GuiTests (xvfb-run) 85/85 passed (exit code 0). No regressions observed; no existing test needed updating. tests/strategic build (libstrategicTests.a) rebuilt clean with no warnings ahead of the full make check run.

Implementation/code commit hash:
- 7261e534d34ef130ba78accf2d6ba19a98164084

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-2/implementer_report.md
- artifacts/save-load-robustness-followups-2/FF2-2/tester_prompt.txt
- artifacts/save-load-robustness-followups-2/FF2-2/implementer_result.json

Implementation context:
- Fix site: FPlanet::load(std::istream&) in src/strategic/FPlanet.cpp (was lines 55-67, now longer). Field order on the wire (mirrors FPlanet::save()): m_ID (native int via read(), NOT fixed-width -- out of scope per task instructions, SF-enum-codes/native-scalar deferral applies) -> m_name (readString(), length-prefixed) -> stationCount (native int via read(); nonzero means a station follows) -> [if stationCount nonzero] station type tag (readString()) -> the station's own FVehicle::load(is) record.
- Exact new logic: `if (read(is,m_ID) != 0) return 1;` then `if (readString(is,m_name) != 0) return 1;` then `if (read(is,stationCount) != 0) return 1;` then, inside `if (stationCount){ ... }`: `if (readString(is,type) != 0) return 1;` then `m_station = createShip(type); if (m_station == NULL) return 1;` then `if (m_station->load(is) != 0) return 1;`. Success path (valid stationCount==0, or valid station whose nested load() returns 0) is unchanged and still ends with `return 0;`.
- Ownership/no-leak/no-double-free reasoning (confirmed correct): on createShip()==NULL, m_station stays NULL (createShip() returns NULL without allocating anything on that path) so there is nothing to free. On an m_station->load() failure, m_station is already a fully-owned member of `this` FPlanet (assigned via the same `m_station = createShip(type)` statement addStation() itself uses), so no leak occurs at the FPlanet::load() call site itself; the caller (FSystem::load(), src/strategic/FSystem.cpp:103-108) wraps FPlanet::load() as `FPlanet *p = new FPlanet; if (p->load(is) != 0){ delete p; return 1; }` -- deleting the not-yet-pushed `p` invokes ~FPlanet(), which already does `if(m_station!=NULL){ delete m_station; }` (src/strategic/FPlanet.cpp:31-33, unchanged), so the partially-loaded station is freed exactly once with no dangling pointer. This exactly mirrors the FFleet::load() ship-loop pattern (src/strategic/FFleet.cpp:191-206) that deletes the just-allocated-but-not-yet-owned FVehicle on its own load() failure before FPlanet ownership would apply -- FPlanet's case differs only in that m_station IS already an owned member by the time load() is called on it, so FPlanet::load() itself does not need its own explicit delete; ~FPlanet() covers it via the caller's delete-on-abort.
- FSystem::load() (src/strategic/FSystem.cpp:91-109) already contains the aggregate-abort check `if (p->load(is) != 0){ delete p; return 1; }` inside its per-planet load loop -- this task required no change to FSystem.cpp; the existing check was previously neutralized only because FPlanet::load() always returned 0 regardless of failure. FSystem::load()'s own nonzero return propagates up through FMap::load() (unchanged, not inspected in depth this pass but same aggregate-abort chain established by the P5-4/P5-5 hardening work) to FGame::load()'s existing per-system-load check and aggregate-abort/no-live-singleton path (reportLoadError(...) via the installed IStrategicUI, single call, no half-built game committed).
- Doxygen: include/strategic/FPlanet.h's load() block was rewritten to document the nonzero-on-failure contract for each own field, the createShip()==NULL guard, the nested m_station->load() check, and the resulting abort-propagation chain through FSystem::load() -> FMap::load() -> FGame::load(). @author is now 'Tom Stephens, Claude Sonnet 5 (medium)' (previous single-author 'Tom Stephens' preserved, new author appended per the comma-separated-list convention in AGENTS.md); @date Last Modified bumped to Jul 19, 2026 (today's date). FPlanet.cpp's file-header Doxygen block (top of file) was left unchanged -- only the function-level Doxygen in FPlanet.h was in scope per the task's allowed-files list ('Doxygen update for the new return contract' on include/strategic/FPlanet.h specifically).
- Scope discipline: only src/strategic/FPlanet.cpp and include/strategic/FPlanet.h were touched, matching the task's allowed-file list exactly. No test files were added or modified by the Implementer -- behavioral coverage (byte-patched unknown-station-type save, truncated-inside-station-record save, and a valid positive control covering both a planet with a station and a planet without one) is the Tester's responsibility per the task's explicit Tester-handoff note.
- No fixed-width readU32 conversion was made to m_ID/stationCount/the station type length prefix beyond what readString()/read() already did before this change (readString() already internally uses the fixed-width readU32 length prefix per FPObject.h; the task explicitly said do NOT convert m_ID or stationCount to readU32 -- SF-enum-codes/native-scalar-field conversions remain out of scope and deferred).

Expected validation failures carried forward:
- None
