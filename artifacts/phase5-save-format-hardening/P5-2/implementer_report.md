# Implementer Report

Status:
- success

Task summary:
- Harden the vehicle/weapon/defense save-load wire layer for the save-format hardening phase (P5-2): convert FVehicle::save/load counts and the vehicle/owner IDs to the P5-1 fixed-width little-endian writeU32/readU32 helpers, null-check createWeapon(...)/createDefense(...) so an unknown/corrupt wire type aborts the load (nonzero) instead of dereferencing NULL, persist and restore the active defense selection across save/load (H2), and advance FVehicle::m_nextID past any loaded ID so newly constructed vehicles never collide with a restored ID (H3).

Changed files:
- include/ships/FVehicle.h
- src/ships/FVehicle.cpp
- doc/deferred-tasks.md

Validation commands run:
- make -C src/ships
- make (top-level, full repo build: both executables + SSWTests + TacticalTests + GuiTests)
- tests/SSWTests (run from tests/ so FGameHeaderDependencyTest's relative-path lookups resolve)

Validation outcome:
- src/ships builds clean with zero warnings. Top-level `make` from the worktree root completed with exit code 0 (all six non-GUI module libraries, gui, both executables, and all three test runners built). `tests/SSWTests` run from the tests/ directory: 258/258 tests pass, 0 failures, 0 errors (pre-existing suite, exercised as a regression check; no FVehicle-specific behavioral tests were added by this run since new/updated tests are the Tester's responsibility). Confirmed src/ships, src/weapons, and src/defenses Makefiles remain wx-free (no wx-config/WX_CXXFLAGS references).

Implementation/code commit hash:
- eb930e4b

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-2/implementer_report.md
- artifacts/phase5-save-format-hardening/P5-2/tester_prompt.txt
- artifacts/phase5-save-format-hardening/P5-2/implementer_result.json

Implementation context:
- include/core/FPObject.h / include/core/FSaveFormat.h (from P5-1, already merged) provide the helpers consumed here: `const int writeU32(std::ostream&, uint32_t) const` / `int readU32(std::istream&, uint32_t&)` -- 4-byte explicit little-endian, return 0 on success / nonzero on stream failure.
- FVehicle::save(std::ostream&) const (src/ships/FVehicle.cpp) now writes m_ID and m_owner via writeU32 (cast from unsigned int to uint32_t), and writes the weapon-list size and defense-list size via writeU32 instead of the old raw write(os, m_weapons.size()) / write(os, m_defenses.size()) (previously size_t, non-portable width). All other scalar fields (m_currentMR/ADF/HP/DCR, m_speed, m_heading, m_maskingScreenTurnCount, the three bool flags, m_navError) are unchanged -- they use the generic write<T> template and are not counts/IDs per the phase's design decision #2 (only counts/sizes and object IDs get the fixed-width treatment).
- H2 (active defense persistence): after the defense-save loop, save() now writes a trailing uint32_t = the index of m_currentDefense within m_defenses (0 if not found, matching the existing default). load() reads that index after fully rebuilding m_defenses from the stream and, when the index is < m_defenses.size(), sets m_currentDefense = m_defenses[index] directly (NOT via setCurrentDefense(i), which has side effects -- ammo decrement and resetting m_maskingScreenTurnCount to 5 -- that would double-apply state already restored verbatim from the stream). Falls back to m_defenses[0] when the index is out of range, or fabricates a fresh FNone when m_defenses is empty, preserving the existing CRIT-3 fallback behavior exactly.
- H3 (ID-counter restore): immediately after readU32 populates m_ID in load(), `if (m_ID >= m_nextID) { m_nextID = m_ID + 1; }` advances the static FVehicle::m_nextID counter so the NEXT default-constructed FVehicle (any concrete ship subclass -- none of them override the constructor's ID assignment) gets an ID strictly greater than every loaded ID. Note FVehicle::~FVehicle() resets m_nextID to 0 when m_classCount drops to 0 -- that reset-on-empty behavior is unchanged; the ID-collision guard only fires on the next load() call for IDs, not the constructor path, so it composes safely with that reset.
- Factory null-checks: createWeapon(FWeapon::Weapon) (src/weapons/FWeapon.cpp) and createDefense(FDefense::Defense) (src/defenses/FDefense.cpp) both already return NULL on an unrecognized enum value (pre-existing factory behavior, unchanged). FVehicle::load() previously dereferenced the result unconditionally (w->setParent(this); w->load(is); / d->load(is);) which would segfault on a corrupt/adversarial type tag. load() now checks `if (w == NULL) return 1;` and `if (d == NULL) return 1;` immediately after each factory call, before any dereference. On the defense path, m_currentDefense is explicitly set to NULL right after the old defense objects are deleted (before the rebuild loop) so an early-return failure leaves it NULL rather than dangling at freed memory.
- FVehicle::load() return-code contract: 0 on success, 1 (nonzero) on an unknown weapon or defense type. It does NOT distinguish stream-exhaustion/read failures from a bad type tag with a different code -- both the P5-1 helpers' internal failure and the type-tag NULL-check collapse to the same success/failure boolean-style contract used throughout this codebase (see FPObject::save/load doc contract: 0 = ok, nonzero = failure). A stream that runs out of bytes mid-object will generally also fail because reads on a failed stream keep failing/return sentinel values through the helper chain, but no dedicated 'truncated stream' test is required by this subtask's acceptance criteria -- that is P5-5's aggregate error-propagation concern.
- No changes were made to FWeapon.h/.cpp or FDefense.h/.cpp: FWeapon has an m_ID field and static m_nextID counter but FWeapon::save()/load() never write or read m_ID at all (a fresh weapon ID is always assigned by the constructor when createWeapon() runs during FVehicle::load()); FDefense has no m_ID field at all. The only fields either save()/load() touch are m_type (enum, deferred SF-enum-codes, unchanged), m_maxAmmo/m_currentAmmo (int, not size_t, no width portability issue), and m_isDamaged (bool). This was confirmed by grep across src/weapons/*.cpp and src/defenses/*.cpp -- only the base FWeapon/FDefense classes implement save()/load(); no concrete weapon or defense subclass overrides them.
- doc/deferred-tasks.md: the H2 entry ('Persist which defense is active across save/load') has been removed from the deferred list now that H2 is implemented, per the Documentation Impact note in the P5-2 plan section.
- Doxygen: include/ships/FVehicle.h save()/load() doc blocks and the class-level header were updated (@date Last Modified: Jul 17, 2026; Claude Sonnet 5 (medium) already present in @author) to describe the fixed-width ID/count fields, the H2 active-defense index field and restore behavior, the H3 next-ID advance, and the factory null-check/abort contract. src/ships/FVehicle.cpp file header gained a Last Modified date and Claude Sonnet 5 (medium) in @author.

Expected validation failures carried forward:
- None
