Security Review Report

Scope reviewed:
- Specialist security review of subtask FF-2 (folding FF-3) of the Save-Load Robustness Follow-Ups plan (plans/save-load-robustness-followups-plan.md).
- Committed diff on branch phase5-FF-2-security-20260719 relative to coordination base branch phase5.
- Production change: src/ships/FVehicle.cpp FVehicle::load() -- now checks the return of every scalar read (readU32/readString/read<T>) and every nested weapon/defense load(), aborting nonzero on any failure.
- Doxygen: include/ships/FVehicle.h.
- Tests: tests/ships/FVehicleTest.{h,cpp} (in-vehicle scalar-region truncation) and tests/strategic/FGameSaveFormatTest.{h,cpp} (three end-to-end FGame::load() truncation tests: fleet ship-loop, m_unattached, m_destroyed).
- Docs: AGENTS.md contributor-notes addendum.
- Trust boundary reviewed: parsing of untrusted on-disk save files (corrupt or maliciously crafted) crossing into the FVehicle::load() path.

Why specialist review was triggered:
- Save files are untrusted input crossing a trust boundary into the load path; a corrupt or adversarial file must not be able to commit a half-built object graph as the live game singleton or trigger memory-safety faults.
- Before FF-2, FVehicle::load() discarded the return value of every scalar read and each nested weapon/defense load(), returning nonzero only on the createWeapon()/createDefense()==NULL factory paths. A stream truncated strictly inside a single vehicle's own scalar/weapon/defense region therefore returned 0 (silent success on exhausted/garbage input), so the FR-1 per-container aggregate-abort guarantee did not reach that depth.
- Plan marks FF-2 'Security review: required' (untrusted-save-input parsing).
- Review focus per coordinator: memory safety on every added abort path (no leak / no double-free / no dangling m_currentDefense), completeness of read-return checking, correct propagation through FFleet::load()/FPlayer::load() to FGame::load(), integer/overflow/resource-exhaustion on count reads, and that the success + unknown-type paths are unchanged.

Acceptance criteria / plan reference:
- plans/save-load-robustness-followups-plan.md -- FF-2 (Deepen the load aggregate-abort guarantee into FVehicle::load(); FF-3 folded).
- Acceptance: every scalar read and nested weapon/defense load() in FVehicle::load() has its return checked; a nonzero return aborts FVehicle::load() nonzero; a stream truncated strictly inside a vehicle's own scalar/weapon/defense region propagates to FGame::load() abort; no leak/dangle/double-free on abort; success path and createWeapon()/createDefense()==NULL unknown-type abort unchanged.
- Read-helper contract confirmed: include/core/FPObject.h -- read<T> returns 1 on is.fail(); src/core/FPObject.cpp readU32 returns 1 on is.fail(); readString validates decoded length against kMaxSerializedStringBytes before allocating and returns 1 on failure.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/strategic/FFleet.cpp / src/strategic/FPlayer.cpp:147/156/158 (FFleet), 133+ (FPlayer) - FPlayer::load() and FFleet::load() still discard the return of their own container-level scalar reads (fleet/player m_ID, m_name, m_owner, m_location, jump-route ID, etc.).
  A stream truncated strictly inside a fleet's or player's OWN scalar region (not inside a vehicle record) is the same silent-success class of hole that FF-2 closes one level down. This is explicitly OUT of FF-2's scope (FF-2 targets FVehicle::load() only; the plan sequences FR-1 nested-return checking separately) and is not a blocker for FF-2, but is a candidate for a future follow-up to fully close the aggregate-abort guarantee at the container-scalar depth.
- include/core/FPObject.h / src/ships/FVehicle.cpp:196-200 / read(is,type), read(is,m_onFire) etc. - read<T>() copies raw bytes into bool / enum (FWeapon::Weapon, FDefense::Defense) lvalues; an adversarial byte pattern can yield an out-of-range enum ordinal or a non-0/1 bool.
  Out-of-range enum ordinals are already neutralized downstream (createWeapon()/createDefense() return NULL -> abort). A non-canonical bool is pre-existing behavior not introduced or worsened by FF-2 (FF-2 strictly improves safety by aborting on read failure). No exploitable path found; noted only for completeness and as pre-existing hardening surface tracked outside FF-2 (cf. SF-enum-codes deferred item).

Test sufficiency assessment:
- Sufficient and behavioral. Four new behavioral tests exercise the real load path against crafted/truncated byte streams and assert observed runtime outcomes (return codes, showMessage call counts, committed-player count, non-dangling defense), not source-text shape.
- tests/ships/FVehicleTest.cpp::testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion -- unit-level: truncates after m_ID/m_name but before m_iconName; asserts load() returns nonzero AND getCurrentDefense() is non-NULL and of type NONE (proves no dangling m_currentDefense and the object is safe to query/destruct after a failed load).
- tests/strategic/FGameSaveFormatTest.cpp -- three end-to-end FGame::load() tests truncate 2 bytes into a target ship's m_ID (type tag fully present so the pre-existing createShip()==NULL guard cannot be what fires), driving the FF-2 nonzero FVehicle::load() through each FR-1 propagation site: fleet ship-loop (single-ship fleet, which also chains through FPlayer's fleet loop), m_unattached loop, and m_destroyed loop. Each asserts rc!=0, exactly one IStrategicUI::showMessage, and zero players committed to the live singleton.
- The single-ship-fleet design deliberately isolates the FF-2 fix from the pre-existing createShip()==NULL guard, so a regression that reverted FF-2 would fail these tests. All four FR-1 propagation sites (FFleet ship-loop, FPlayer fleet-loop, m_unattached, m_destroyed) are covered.
- Verified by build+run: full SSWTests suite builds clean and reports OK (297 tests) when run from tests/. (The only failures observed were cwd-relative source-inspection tests that look up source files by relative path and are unrelated to FF-2; they pass from the correct working directory.)

Documentation / operational guidance assessment:
- Sufficient. include/ships/FVehicle.h Doxygen on load() accurately describes the FF-2 return-checking discipline, the delete-just-allocated-weapon/defense-before-return ownership rule, the reliance on the FR-1-hardened caller to delete the not-yet-committed vehicle (so ~FVehicle frees already-appended entries), and that the success path and unknown-type abort are unchanged.
- In-code comments at each abort site correctly state why no leak/dangle/double-free occurs (entry not yet appended when deleted; already-appended entries owned by the vehicle; m_currentDefense left NULL).
- AGENTS.md addendum documents that FVehicle::load()'s aggregate-abort now reaches inside a single vehicle's own scalar/weapon/defense region and that FGameSaveFormatTest/FVehicleTest are the confirming coverage.

Artifacts written:
- artifacts/save-load-robustness-followups/FF-2/security_report.md
- artifacts/save-load-robustness-followups/FF-2/security_result.json

Outcome:
- PASS
