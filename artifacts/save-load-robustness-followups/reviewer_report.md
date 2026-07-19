Reviewer Report

Feature plan reviewed:
- plans/save-load-robustness-followups-plan.md (Save-Load Robustness Follow-Ups; FF-1, FF-2 with FF-3 folded into FF-2 as expanded Tester coverage).
- Governing base: coordination branch `phase5` (final feature diff 879248eb..HEAD).

Inputs reviewed:
- Plan Sections 1-8 (feature restatement, D1 design decision, confirmed repo facts, IN/OUT scope, per-subtask tasks + acceptance criteria + Tester guidance, documentation impact, dependency ordering, Coordinator-ready prompts).
- FF-1 code: src/strategic/FGame.cpp (post-player-load fleet located-object-ID validation loop), include/strategic/FGame.h (Doxygen). Diff base 375ff9b0.
- FF-1 tests: tests/strategic/FGameSaveFormatTest.{h,cpp} (out-of-range location ID, out-of-range jump-route ID, sentinel positive control). Diff base 4f78c7a0.
- FF-2 code: src/ships/FVehicle.cpp (return-checking on every readU32/readString/read<T> and every nested w->load()/d->load()), include/ships/FVehicle.h (Doxygen). Diff base da076ba2.
- FF-2 tests: tests/ships/FVehicleTest.{h,cpp} (unit-level in-vehicle scalar-region truncation) + tests/strategic/FGameSaveFormatTest.{h,cpp} (three full-game truncation tests covering all four FR-1 propagation sites). Diff base 82993ccc.
- Documentation: AGENTS.md save-format Contributor Notes + Testing/FGameSaveFormatTest bullet (FF-1 and FF-2 addenda). Diff bases 4b073be6, 4ba19678.
- Per-subtask artifacts (implementer/tester/documenter/security/verifier reports + results) under artifacts/save-load-robustness-followups/FF-1/ and FF-2/.
- Sentinel/interface cross-check: include/strategic/FFleet.h (NO_ROUTE, getJumpRoute, getLocation), include/strategic/FMap.h (getSystem(id), getJumpRoute(id)).

Independent spot-check performed this session (read-only build/run):
- Built the full tests tree (`make -C tests`) — clean link of SSWTests and TacticalTests.
- Ran SSWTests from tests/ — OK (297 tests), including all FF-1 and FF-2 behavioral cases. No failures.
- (Both subtask Verifiers had independently re-confirmed the behavioral-test mandate by reverting the fix, observing the new tests fail against unfixed code, then restoring and re-confirming green make check across SSWTests/TacticalTests/GuiTests.)

Overall feature completeness:
- COMPLETE for the plan's declared scope. Both IN-scope subtasks are delivered, merged, and green.
  - FF-1: FGame::load() now validates, for every fleet of every successfully-loaded player (before wiring fleets into system lists), the fleet location (system) ID (exempting the documented `0` "not yet in a system" sentinel) via m_universe->getSystem(id), and the jump-route ID (exempting FFleet::NO_ROUTE) via m_universe->getJumpRoute(id). The first unresolved reference deletes the not-yet-owned FPlayer, calls reportLoadError(...), and returns 1 — reusing the existing aggregate-abort / IStrategicUI::showMessage() / no-live-singleton path. Sentinel values were confirmed against FFleet.h rather than assumed (matches plan Section 5 instruction). Per design decision D1, no gui file was modified — verified: the feature diff touches no src/gui or include/gui file.
  - FF-2: FVehicle::load() now checks the return of every scalar read (readU32/readString/read<T>) and every nested weapon/defense load(); a failing read/load aborts nonzero with correct cleanup (a just-allocated weapon/defense that fails its own load is deleted before return; already-appended entries stay owned by the vehicle and are freed by ~FVehicle when the FR-1-hardened caller deletes the not-yet-committed vehicle). The createWeapon()/createDefense()==NULL unknown-type abort and the H2 active-defense fallback are preserved unchanged. Change is confined to the two allowed files (src/ships/FVehicle.cpp, include/ships/FVehicle.h).
  - FF-3: delivered as expanded Tester coverage inside FF-2 exactly as the plan directs — three full-game truncation tests exercise all four FR-1 propagation sites (single-ship fleet → FFleet ship-loop + FPlayer fleet-loop; m_unattached; m_destroyed), each asserting rc != 0, exactly one showMessage(), and zero players committed; a fourth unit-level FVehicleTest case asserts the in-vehicle abort plus non-dangling m_currentDefense.

Cross-subtask integration:
- FF-1 and FF-2 harden the same untrusted-save trust boundary at FGame::load() at two different depths (fleet-reference resolution vs. in-vehicle scalar/nested-load truncation) and are cleanly orthogonal — disjoint source files, no functional dependency, sequenced only for the shared AGENTS.md edit. Together with the pre-existing FR-1 per-container propagation, they give a consistent aggregate-abort story for the parts of the format they claim to cover: a truncated/corrupt stream inside a vehicle record (FF-2) now propagates through FFleet::load()/FPlayer::load() to FGame::load(), and a structurally-valid-but-unresolvable fleet reference (FF-1) is rejected at the same seam with the same reporting/no-singleton guarantee. The abort-path memory-safety invariant (FF-1 validation must run before fleet-to-system wiring and before m_players.push_back) is documented by both the FF-1 Security and Verifier reports and is correctly implemented.
- AGENTS.md now describes the combined FR-1/FF-1/FF-2 behavior accurately and non-redundantly: the FF-1 sentence records load-time rejection of out-of-range fleet located-object references (with the D1 gui-left-unmodified decision), and the FF-2 sentence records the aggregate-abort guarantee reaching inside FVehicle::load() and propagating through all four FR-1 sites, without duplicating the header-level Doxygen. The Testing bullet is extended with the FF-2/FF-3 container coverage. No doc/deferred-tasks.md change, as the plan specifies.

Documentation obligations (plan Section 6):
- MET. Doxygen updated on FGame::load() (FGame.h) and FVehicle::load() (FVehicle.h), with @date Last Modified bumped to Jul 19, 2026 and implementing model added to @author; new test methods carry per-function Doxygen. AGENTS.md addenda present and accurate for both subtasks. doc/deferred-tasks.md correctly untouched. The tactical_operations_manual.md non-negotiable constraint is untouched.

Security obligations:
- MET for delivered scope. Both subtasks correctly received specialist Security review as the plan required (both "Security review: required"): FF-1 CONDITIONAL PASS (no BLOCKING; two WARNINGs and one NOTE, all confirmed pre-existing and outside FF-1's allowed files/scope), FF-2 PASS (no BLOCKING/WARNING; two informational NOTEs outside FF-2 scope). No feature-level security gap remains *within* the delivered scope: the two references FF-1 committed to validate are validated, and every read/nested-load FF-2 committed to check is checked. The residuals below are all in code paths the plan explicitly excluded.

Test sufficiency at the feature level:
- SUFFICIENT and behavioral. All new tests construct real save byte-streams, corrupt/truncate a precise field at a justified offset, drive the real FGame::load()/FVehicle::load() path, and assert observed runtime outcomes (return code, mock IStrategicUI showMessage count, committed-player/live-singleton state, non-dangling getCurrentDefense()) — no source-inspection assertions. Both Verifiers independently confirmed the fails-against-unfixed / passes-against-fixed mandate. Positive-control coverage (sentinel location 0 + NO_ROUTE still loads clean) guards against false rejection.

Findings

BLOCKING
- None.

WARNING
- None against the plan as delivered. (All substantive robustness residuals are pre-existing and plan-excluded; consolidated below as Planner-ready follow-up requests rather than blockers, per the review charter.)

NOTE
- Wording precision (already flagged by FF-1 Security and Verifier, non-blocking): the FF-1 in-code comment / Doxygen / AGENTS.md sentence open with "located-object reference IDs" before enumerating only the two fields actually validated (location, jump-route). Not materially inaccurate once read in full — the very next clauses name exactly the two fields — and the plan (Section 4) is precise. If a future follow-up adds m_destination validation, tighten this phrasing at the same time. No action required for this plan.
- Load-safety ordering invariant (documented, non-blocking): FF-1's abort path (`delete p` on the not-yet-owned FPlayer) is safe only because validation runs before fleet-to-system wiring and before m_players.push_back(p). Future edits to FGame::load() must preserve this ordering to avoid dangling FSystem fleet-list pointers or a stale m_players entry. Correctly implemented as delivered.

Out-of-scope residuals confirmed genuinely outside this plan's scope:
- The FF-1 Security WARNINGs (m_destination not validated; location==0 + inTransit illegal-state) target FGame::moveFleets(), which is not in FF-1's allowed files and which the plan's Section 4/5 explicitly scopes to only the location and jump-route references. Confirmed pre-existing (moveFleets was not modified).
- The FF-2 Verifier WARNING (FPlanet::load() NULL-deref on unknown station type + unconditional return 0) is in src/strategic/FPlanet.cpp, outside FF-2's allowed files (src/ships/FVehicle.cpp, include/ships/FVehicle.h). Confirmed pre-existing.
- The FF-2 Security NOTEs (FPlayer::load()/FFleet::load() container-level scalar reads still unchecked; read<T> into bool/enum lvalues may decode non-canonical values) are one level up from FF-2's target and, for the enum case, already tracked as SF-enum-codes. Confirmed out of FF-2 scope.
All four are recorded as accepted-by-user future follow-ups; none blocks this plan.

Follow-up feature requests for planning (Planner-ready):
- FR-A (robustness / trust-boundary; HIGH): Validate a fleet's m_destination (system) reference ID at load time in FGame::load(), mirroring FF-1's existing NO_ROUTE-style sentinel-exempt pattern with a NO_DESTINATION exemption, and reject an out-of-range non-sentinel m_destination via the same reportLoadError()/no-live-singleton abort. Rationale: a crafted in-transit save with an out-of-range m_destination currently passes load and reaches an unguarded m_universe->getSystem(getDestination())->addFleet(f) NULL dereference in FGame::moveFleets() on the next turn advancement (same crash class FF-1 targets, different field/path). Add behavioral coverage that such a save is rejected at load and that load+advance-turn on it does not crash.
- FR-B (robustness / trust-boundary; MEDIUM-HIGH, pairs with FR-A): Reject the illegal in-transit + location-0 fleet state at load (or null-guard the moveFleets() getSystem() sinks). Rationale: FF-1 (correctly) exempts location==0 as the "not yet in a system" sentinel, but a crafted save can set m_inTransit=true with m_location=0, which passes FF-1 and then reaches an unguarded getSystem(0)->removeFleet(...) NULL dereference in moveFleets() (getSystem(0) is always NULL since system IDs start at 1). Add a behavioral test for the crafted illegal-state save.
- FR-C (robustness / trust-boundary; HIGH): Harden FPlanet::load() (src/strategic/FPlanet.cpp) to (a) NULL-check createShip(type) before calling m_station->load(is), and (b) check its own read/nested-load returns and return nonzero on failure instead of unconditionally returning 0. Rationale: an unrecognized station type on a corrupt save leaves m_station NULL and triggers a NULL virtual-call crash; and because FPlanet::load() can never return nonzero, FSystem::load()'s existing `if (p->load(is) != 0)` check is neutralized, so this depth is entirely outside the aggregate-abort guarantee. More severe than the silent-continue class FF-2 fixed (unguarded NULL deref vs. silent success). Add behavioral coverage for an unknown/truncated station record aborting FGame::load().
- FR-D (robustness / trust-boundary; MEDIUM): Extend the aggregate-abort guarantee to the container-level scalar reads in FPlayer::load() and FFleet::load() (their own m_ID/m_name/m_owner/m_location/jump-route-ID/count reads), mirroring FF-2's per-read return-checking. Rationale: a stream truncated strictly inside a fleet's or player's own scalar region (not inside a vehicle record) is the same silent-success hole FF-2 closed one level down; closing it completes the container-scalar depth of the guarantee. Add behavioral coverage for each container-scalar truncation point.
- (Informational, lower priority) Non-canonical bool/enum decode from read<T> is already tracked as the deferred SF-enum-codes item; no new request needed beyond noting FR-C/FR-D would benefit from a stable-code encoding effort there.

Final outcome:
- PASS
