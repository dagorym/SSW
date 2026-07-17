Security Review Report

Scope reviewed:
- Specialist security review of subtask P5-2 (Vehicle / weapon / defense save-load hardening) of the phase5-save-format-hardening plan. Reviewed as committed on branch phase5-P5-2-security-20260717 (documenter chain complete); no implementation or test code was modified.
- Primary review surface: the untrusted-input wire layer include/ships/FVehicle.h and src/ships/FVehicle.cpp (FVehicle::save / FVehicle::load), which parse attacker-controllable ship data (weapon list, defense list, IDs, owner, counts, active-defense index, per-object state).
- Supporting surface: the createWeapon(...) / createDefense(...) factories (src/weapons/FWeapon.cpp, src/defenses/FDefense.cpp) that return NULL on an unknown type tag; the readU32/writeU32/read/write helpers (include/core/FPObject.h) and save-format constants (include/core/FSaveFormat.h) from P5-1; the FWeapon/FDefense save/load paths (confirming the deliberate no-change decision); and behavioral coverage in tests/ships/FVehicleTest.h/.cpp.
- Verification performed: built the ships test library and the top-level SSWTests runner via the documented `all` target and executed the suite; all 263 tests pass from the tests/ working directory (the 9 transient failures seen when run from a foreign cwd are all cwd-dependent FGameHeaderDependencyTest source-inspection tests unrelated to P5-2, and pass when run from tests/).

Why specialist review was triggered:
- Loading a save file is an UNTRUSTED-INPUT trust boundary (plan Section 7). The plan marks P5-2 `Security review: required` because the hardening IS the security fix.
- The pre-hardening FVehicle::load dereferenced the factory result unconditionally: an unknown/corrupt weapon or defense type tag on the wire produced a NULL-pointer dereference crash (adversarial-input DoS).
- The prior wire layer wrote counts/IDs as native size_t (non-portable, and on 64-bit up to 2^64), and reset the active defense to the base on every load (H2), and did not advance the static ID counter past a restored ID (H3, post-load ID reuse/collision risk).

Acceptance criteria / plan reference:
- plans/phase5-save-format-hardening-plan.md, subtask P5-2 (lines 148-196) and Security Notes Section 7 (untrusted-input focus: oversized lengths, truncated streams, unknown type tags, out-of-range IDs, integer-boundary counts).
- Acceptance criteria evaluated: weapon/defense round-trip; non-default active-defense restore (H2) with consistent masking-screen/turn-count state; FVehicle::load returns nonzero without NULL deref on unknown weapon OR defense type; post-load new-vehicle ID strictly greater than every loaded ID (H3); fixed-width little-endian uint32 counts/IDs; clean build with ship/weapon/defense modules wx-free.
- H2 resolution and partial-H3 documentation cross-checked against doc/deferred-tasks.md and doc/synthesized-roadmap.md.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/ships/FVehicle.cpp:247-259, 269-280 - The per-element weapon and defense rebuild loops do not check the stream state after read(is,type)/read(is,defType) or after the per-object w->load(is)/d->load(is); on a truncated stream carrying an adversarially large uint32 count they can iterate up to `count` times.
  Bounded, not infinite: the loop is capped by the uint32 count (<= 2^32) and the factory null-check aborts immediately whenever the (indeterminate, post-EOF) type tag maps to an invalid enum. The residual window is only when the garbage tag maps to a valid enum, allowing up to ~2^32 small allocations before termination (OOM DoS). This is PRE-EXISTING behavior that P5-2 materially IMPROVED (count is now bounded to uint32 vs native size_t, and the null-check adds an early-abort path). The plan explicitly delegates aggregate stream-exhaustion abort to the container / FGame::load boundary and subtask P5-5. Recommendation: P5-5 should verify stream state and/or impose a sane per-container element cap; ideally these per-element loops would also propagate read()/load() failure to abort early (defense-in-depth). Not blocking for P5-2's stated scope.
- tests/ships/FVehicleTest.cpp:722-759 - There is behavioral coverage for the H2 in-range active-defense restore and for the empty-list CRIT-3 fallback (testLoadReassignsCurrentDefenseToLiveDefenseAfterMultiDefenseReload), but no explicit negative test that feeds an OUT-OF-RANGE currentDefenseIndex (index >= size on a non-empty defense list) and asserts the safe fallback to m_defenses[0] rather than an out-of-bounds select.
  The bound check at FVehicle.cpp:293 (currentDefenseIndex < m_defenses.size(), unsigned compare so negative is impossible) is present and correct, so this is a coverage-completeness nit rather than a defect. Adding an adversarial out-of-range-index test would lock the CRIT-3+H2 fallback against future regressions on the most direct attacker-controllable index field.
- src/ships/FVehicle.cpp:251-255, 268, 273-277 - On a mid-load abort (return 1) the partially-populated FVehicle is left inconsistent: m_currentDefense may be NULL (nulled at line 268 before the defense rebuild) and the weapon/defense lists partial. This is memory-safe (NULL is a deterministic-crash-safe sentinel, and m_currentDefense is a non-owning alias into m_defenses so there is no double-free), but relies on the caller discarding the object on nonzero return.
  Within P5-2 scope this is correct and is a strict improvement over the prior dangling-pointer (use-after-free) UB. The safety guarantee depends on the aggregate loader (FGame::load / P5-5) aborting the whole load and discarding the half-built object on any nonzero propagation. Recommendation: confirm P5-5 enforces discard-on-failure so a partially-loaded vehicle is never handed to live game code.

Test sufficiency assessment:
- SUFFICIENT for P5-2's security-relevant behaviors. The two adversarial-input controls are covered by end-to-end behavioral tests that hand-craft a corrupt stream and assert observed runtime outcome: testLoadReturnsNonzeroOnUnknownWeaponType and testLoadReturnsNonzeroOnUnknownDefenseType construct a stream with an out-of-range type tag (9999), call FVehicle::load, and assert rc != 0 with no crash - directly exercising the createWeapon/createDefense NULL-check abort on BOTH loops.
- H2 active-defense restore is covered behaviorally by testLoadRestoresActiveDefenseSelectionAcrossSaveLoad (raises Masking Screen via the real API, saves, reloads into a fresh FVehicle, asserts getCurrentDefense()->getType()==MS and that ammo/turn-count are restored verbatim rather than re-applied). The CRIT-3 live-pointer fallback is covered by testLoadReassignsCurrentDefenseToLiveDefenseAfterMultiDefenseReload and testLoadedVehicleSurvivesWeaponFireViaCurrentDefense.
- H3 ID-counter restore is covered behaviorally by testLoadAdvancesNextIDPastLoadedID (loads a vehicle with a large ID, then asserts a freshly-constructed vehicle receives a strictly-greater ID). Round-trip fidelity is covered by testSerialize and testSerializeRoundTripsWeaponsDefensesAndState.
- Gap (non-blocking, NOTE): no explicit out-of-range active-defense-index negative test and no truncated-stream/oversized-count test at this layer (the latter is deferred to the P5-5 aggregate-abort boundary). Full suite executed: 263/263 pass from tests/.

Documentation / operational guidance assessment:
- SUFFICIENT. The FVehicle::save and FVehicle::load Doxygen headers (include/ships/FVehicle.h) accurately document the fixed-width uint32 IDs/counts, the trailing active-defense index (H2), the m_nextID advance guard (H3), the createWeapon/createDefense NULL-check abort semantics, and the CRIT-3 safe fallback for an out-of-range index. Inline comments in src/ships/FVehicle.cpp explain the early m_currentDefense=NULL clearing before the defense rebuild and the index validation.
- doc/deferred-tasks.md and doc/synthesized-roadmap.md record H2 as resolved by P5-2 and correctly reflect the partial/remaining H3 scope (fleet/player ID-counter work carried into P5-3). No documentation asserts a control that the code does not implement.
- Recommendation (non-blocking): note in the aggregate-loader (P5-5) documentation the loader-contract dependency that a nonzero FVehicle::load return requires the caller to discard the partially-built object.

Artifacts written:
- artifacts/phase5-save-format-hardening/P5-2/security_report.md
- artifacts/phase5-save-format-hardening/P5-2/security_result.json

Outcome:
- PASS
