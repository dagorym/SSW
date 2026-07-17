# Implementer Report

Status:
- success

Task summary:
- Harden the player/fleet save-load wire layer for the save-format hardening phase (P5-3): convert all counts/sizes and object IDs in FPlayer::save/load and FFleet::save/load to the P5-1 fixed-width little-endian writeU32/readU32 helpers; null-check createShip(...) so an unknown/corrupt ship type aborts the load (nonzero) instead of dereferencing NULL; serialize and restore FPlayer::m_destroyed (F2-serialization) with the same type-tag + createShip + FVehicle::load pattern and null-check as m_unattached/fleet ships, preserving FPlayer's sole-ownership contract; remove the legacy route-0 normalization in FFleet::load (H4) so a fleet on jump route ID 0 keeps its route; and advance FPlayer::m_nextID / FFleet::m_nextID past any loaded ID (H3) so newly constructed players/fleets never collide with a restored ID.

Changed files:
- include/strategic/FPlayer.h
- src/strategic/FPlayer.cpp
- include/strategic/FFleet.h
- src/strategic/FFleet.cpp
- doc/deferred-tasks.md

Validation commands run:
- make -C src/strategic
- make (top-level, full repo build: both executables + SSWTests + TacticalTests + GuiTests)

Validation outcome:
- src/strategic builds clean; FFleet.cpp and FPlayer.cpp compile with zero warnings (the only warning emitted by the strategic module is the pre-existing FJumpRoute.cpp int-to-pointer-cast warning, which is out of P5-3 scope and belongs to P5-4). Top-level `make` from the worktree root completed with exit code 0: all six non-GUI module libraries, gui, both executables (SSW, BattleSim), and all three test runners (SSWTests, TacticalTests, GuiTests) built. Confirmed src/strategic/Makefile remains wx-free (no wx-config / WX_CXXFLAGS references). No behavioral tests were added by this run since authoring new/updated tests is the Tester's responsibility.

Implementation/code commit hash:
- 5486b5a3

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-3/implementer_report.md
- artifacts/phase5-save-format-hardening/P5-3/tester_prompt.txt
- artifacts/phase5-save-format-hardening/P5-3/implementer_result.json

Implementation context:
- P5-1 (already merged) provides the wire helpers consumed here in include/core/FPObject.h: `const int writeU32(std::ostream&, uint32_t) const` and `int readU32(std::istream&, uint32_t&)` -- 4-byte explicit little-endian byte emission via shift/mask, returning 0 on success / nonzero on stream failure. P5-2 (already merged) hardened FVehicle::save/load the same way; FFleet/FPlayer ship lists delegate to that hardened FVehicle::load via createShip().
- FPlayer::save (src/strategic/FPlayer.cpp): m_ID and all three list counts (m_unattached.size(), m_fleets.size(), m_destroyed.size()) are now written via writeU32 (cast to uint32_t) instead of the old raw write(os, m_ID) / write(os, size()) (previously native unsigned int / size_t, non-portable width). Each ship's own save() writes its type string first, so m_destroyed serialization needs no separate type write -- it mirrors the m_unattached loop exactly.
- FPlayer::load: reads m_ID via readU32; immediately advances the static m_nextID with `if (m_ID >= m_nextID) { m_nextID = m_ID + 1; }` (H3). Counts read via readU32 into local uint32_t (uSize/fSize/dSize). Both ship loops (m_unattached and the new m_destroyed) readString the type tag, call createShip(type), and `if (v == NULL) return 1;` before any dereference (createShip returns NULL on an unknown type -- pre-existing factory behavior). The fleet loop is unchanged in structure (new FFleet + f->load) since createShip is not involved there. m_destroyed restoration preserves FPlayer's sole-ownership contract: the ships are owned only by m_destroyed and freed in ~FPlayer() (unchanged destructor).
- F2-serialization: FPlayer::m_destroyed is now written after the fleet list in save() and restored after the fleet loop in load(), using the identical type-tag + createShip() + v->load() + null-check pattern as m_unattached. The list round-trips but is intentionally not yet consumed by any rule (the optional Replacements rule that would read it does not exist yet) -- that is the intended, documented state. The doc/deferred-tasks.md F2-serialization entry was removed now that the persistence is implemented.
- FFleet::save (src/strategic/FFleet.cpp): m_ID, m_owner, m_location, m_destination, m_jumpRouteID, and m_ships.size() are now written via writeU32 (cast to uint32_t). The remaining scalar fields (m_inTransit bool, m_transitTime, m_jumpLength, m_speed unsigned ints, m_isMilitia/m_isHolding bools, m_pos[0]/m_pos[1]/m_dx/m_dy floats) keep the generic write<T> template per design decision #2 (only counts/sizes and object IDs get the fixed-width treatment; other multi-byte scalars remain portable across the two supported little-endian platforms).
- FFleet::load: reads m_ID via readU32; advances the static m_nextID with the H3 guard. Because FFleet::m_nextID is a signed int (not unsigned like the other classes), the guard compares and assigns through an unsigned view -- `if (m_ID >= (unsigned int)m_nextID) { m_nextID = (int)(m_ID + 1); }` -- to avoid a signed/unsigned comparison warning under -Wall while preserving the same 'strictly greater than any loaded ID' semantics. m_owner/m_location/m_destination/m_jumpRouteID are read via readU32 into local uint32_t and assigned back. The ship loop reads sCount via readU32 and null-checks createShip (return 1 on NULL) before dereference.
- H4 (route-0 removal): the line `if (m_jumpRouteID == 0) m_jumpRouteID = NO_ROUTE;` was removed from FFleet::load. Jump route ID 0 is a valid, distinct route ID (FJumpRoute IDs start at 0) and is now preserved exactly as saved. Only the distinguished NO_ROUTE sentinel value (static_cast<unsigned int>(-1)) means 'not on a route'. This is a deliberate save-format behavior change: there is no legacy read path (the clean break happens once, in this phase), so no legacy save relied on the old normalization.
- load() return-code contract for both classes: 0 on success, 1 (nonzero) on an unknown ship type. Consistent with the codebase-wide FPObject save/load boolean-style contract (0 = ok, nonzero = failure) and with P5-2's FVehicle::load. A truncated/short stream is not given a distinct code here; dedicated aggregate error-propagation and the truncation path are P5-5's concern (FGame::load boundary).
- Doxygen: include/strategic/FPlayer.h (class header, save()/load() blocks, addDestroyedShip() ownership contract, m_destroyed member comment) and include/strategic/FFleet.h (class header, save()/load() blocks) were updated with @date Last Modified: Jul 17, 2026 and Claude Sonnet 5 (medium) added to @author where not already present. Both .cpp file headers gained a Last Modified date and Claude Sonnet 5 (medium) in @author. The FPlayer/FFleet doc now describes the fixed-width ID/count fields, the H3 next-ID advance, the F2 m_destroyed persistence, the createShip null-check/abort contract, and (FFleet) the H4 route-0 preservation.

Expected validation failures carried forward:
- None
