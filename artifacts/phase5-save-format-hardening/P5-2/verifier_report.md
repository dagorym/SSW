Verifier Report

Scope reviewed:
- Reviewed the combined P5-2 change (Implementer eb930e4b, Tester 46f81017, Documenter 3ee35c35, Security 8e07b1eb) diffed against the P5-1 Verifier-passed base 1bb99cec: include/ships/FVehicle.h, src/ships/FVehicle.cpp, tests/ships/FVehicleTest.cpp/.h, doc/deferred-tasks.md, doc/synthesized-roadmap.md.
- Confirmed the P5-2 hardening: FVehicle::save/load ID/owner/weapon-count/defense-count fields converted to the P5-1 fixed-width little-endian writeU32/readU32 helpers; a trailing active-defense index (H2) is written after the defense list and restored on load with the CRIT-3 empty/invalid-index fallback preserved; FVehicle::m_nextID is advanced past any loaded m_ID (H3); createWeapon(...)/createDefense(...) results are null-checked in load() and an unknown/corrupt wire type now aborts the load (return 1) instead of dereferencing NULL.
- Confirmed no changes were needed (or made) to FWeapon.h/.cpp or FDefense.h/.cpp: their save()/load() only persist m_type/m_maxAmmo/m_currentAmmo/m_isDamaged, none of which are counts/IDs, verified by direct inspection of src/weapons/FWeapon.cpp and src/defenses/FDefense.cpp.
- Reviewed the specialist Security stage's committed artifacts (security_report.md, security_result.json): outcome PASS, 0 blocking, 0 warning, 3 non-blocking NOTEs (bounded per-element loop iteration on adversarial counts, missing out-of-range-index negative test, and partial-object state on mid-load abort) -- all correctly scoped as improvements-over-prior-behavior and deferred to P5-5's aggregate-loader boundary, not blocking for P5-2.

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md, subtask P5-2 (lines 148-196): 'Vehicle / weapon / defense save-load hardening (H2, H3, factory null-checks)', Security review: required.
- Acceptance criteria verified 1:1 against the plan: weapons+defenses+state round trip; H2 active-defense restore with consistent MS turn-count/ammo; nonzero return with no NULL deref on unknown weapon/defense type; H3 next-ID advance strictly past loaded ID; fixed-width little-endian uint32 counts/IDs; clean wx-free build; deferred-tasks/roadmap H2/H3 status accuracy.

Convention files considered:
- AGENTS.md (Doxygen @author/@date policy verified compliant on FVehicle.h/.cpp: 'Tom Stephens, Claude Sonnet 5 (medium)' retained, Last Modified updated to Jul 17, 2026; Single-Source-of-Truth rule respected -- wire-format facts stay owned by FVehicle.h Doxygen, not duplicated into AGENTS.md/DesignNotes.md; module-boundary/wx-free rule for src/ships verified via build and Makefile grep).
- CLAUDE.md (pointer file, no independent facts).
- doc/deferred-tasks.md's own in-file convention ('An entry stays in this file until the deferred work is completed...') -- H2 entry outright removal matches this convention for a fully-closed item.
- doc/synthesized-roadmap.md's established resolved-row convention (RESOLVED marker + bracketed detail note) -- applied consistently to H2 (full) and H3 (partial, FVehicle-instance-only, with P5-3/P5-4 cross-reference).

Findings

BLOCKING
- None

WARNING
- None

NOTE
- None

Test sufficiency assessment:
- SUFFICIENT. All five new tests are genuine behavioral tests: they construct real FVehicle/FArmedStation objects (or hand-craft raw wire streams consumed by the real load() code path), execute save()/load() through std::stringstream, and assert on observed runtime state -- no test relies on source-text/source-structure inspection as its sole evidence for a behavioral claim.
- testSerializeRoundTripsWeaponsDefensesAndState exercises a 2-weapon/2-defense vehicle through a full save->load round trip and asserts weapon/defense type, ammo, and damage state plus HP/MR/ADF/DCR/heading/speed/owner/flags are preserved exactly.
- testLoadRestoresActiveDefenseSelectionAcrossSaveLoad raises Masking Screen via the real public setCurrentDefense(1) API (observing the real ammo-decrement/turn-count side effects), saves, reloads into a fresh FVehicle, and asserts getCurrentDefense()->getType()==MS with ammo/turn-count restored verbatim (not re-applied) -- directly covers H2 and distinguishes index-restore from re-invoking setCurrentDefense's side effects.
- testLoadAdvancesNextIDPastLoadedID hand-crafts a wire stream carrying m_ID=5,000,000, loads it, then default-constructs a fresh FVehicle and asserts its ID is strictly greater -- directly exercises the m_nextID advance guard end-to-end rather than asserting on source text.
- testLoadReturnsNonzeroOnUnknownWeaponType / testLoadReturnsNonzeroOnUnknownDefenseType each hand-craft a stream with an out-of-range enum tag (9999) for the weapon/defense factory, call the real FVehicle::load(), and assert rc != 0 with no crash -- directly exercises the createWeapon/createDefense NULL-check abort path on both loops.
- Fixed-width little-endian uint32 wire encoding is exercised behaviorally (not just structurally) by every one of the above tests via the shared writeU32/readU32 helpers already behaviorally proven in P5-1's FPObjectSerializationTest; no criterion in this subtask depends solely on source-inspection.
- Gap noted by the Security stage (non-blocking NOTE, concurred): no explicit adversarial out-of-range active-defense-index test (index >= size on a non-empty list) and no truncated-stream/oversized-count test at this layer; the index bound check is present and correct at FVehicle.cpp:293 and the truncated-stream case is explicitly plan-deferred to P5-5's aggregate-loader boundary. Does not block P5-2's stated scope.
- Full suite independently re-run from tests/ in this review: SSWTests 263/263, TacticalTests 253/253, GuiTests 81/81 (xvfb-run) via `make check` from the worktree root -- exit code 0, matching the Tester/Documenter/Security-reported baseline.

Documentation accuracy assessment:
- ACCURATE. include/ships/FVehicle.h's save()/load() Doxygen blocks (and class-level header) were reviewed line-by-line against the shipped src/ships/FVehicle.cpp behavior: they correctly describe the fixed-width uint32 ID/owner/count fields via writeU32/readU32, the trailing active-defense index written after the defense list and its restore-with-fallback semantics (H2, preserving the CRIT-3 safety fallback), the m_nextID advance guard (H3, exact guard form `if (m_ID >= m_nextID) m_nextID = m_ID + 1;` matches the code), and the createWeapon/createDefense NULL-check abort contract (nonzero return, no dereference). @author lists retain all prior authors (Tom Stephens, gpt-5.3-codex (medium), Claude Sonnet 5 (medium)) and @date Last Modified is updated to Jul 17, 2026 on every touched block, per AGENTS.md's Doxygen policy.
- doc/deferred-tasks.md: the H2 entry ('Persist which defense is active across save/load') was correctly removed outright (matching the file's own stated convention for fully-closed items), and the F2-serialization entry's stale 'same category of change as H2' cross-reference was corrected to reflect H2's resolution by P5-2 while F2/H3-adjacent work remains open for P5-3.
- doc/synthesized-roadmap.md: the H2 confirmed-defects row now carries a '-- ***RESOLVED*** (P5-2, ...)' marker plus a bracketed '[RESOLVED -- see P5-2: ...]' detail sentence citing the exact save()/load() mechanism and the behavioral test name -- consistent with the file's established resolved-row convention (C1/C2/C4/C5/C6 etc.). The H3 row is correctly left NOT marked as a header-level RESOLVED (since FFleet/FPlayer/FSystem/FJumpRoute instances of the same pattern remain open) but gains an accurate bracketed partial-resolution note scoped to 'FVehicle instance resolved... the FFleet/FPlayer/FSystem/FJumpRoute instances of this same pattern remain open, tracked by subtasks P5-3 and P5-4' -- this precisely satisfies the acceptance criterion that no stale entry misstate H2 or H3 as still fully open (H2 is not left open; H3 is not overclaimed as fully closed). The C3 row's H2 cross-reference was also updated in-place to note H2 'has since been resolved by P5-2', avoiding a stale forward-reference.
- No documentation asserts a control the code does not implement, and no wire-format fact was duplicated outside FVehicle.h's Doxygen (Single-Source-of-Truth rule respected, consistent with P5-1's precedent for FSaveFormat.h). doc/rules/tactical_operations_manual.md was not touched, correctly.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-2/verifier_report.md
- artifacts/phase5-save-format-hardening/P5-2/verifier_result.json

Verdict:
- PASS
