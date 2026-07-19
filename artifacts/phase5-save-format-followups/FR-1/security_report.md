Security Review Report

Scope reviewed:
- FR-1 committed changes on branch phase5-FR-1-security-20260719 (commits 5da780fe implementer, 4e6b1185 tester, ef12f02b/127b1388 documenter), closing deferred item SF-nested-load-returns.
- src/strategic/FPlayer.cpp: FPlayer::load() nested-return checks in the m_unattached, fleet, and m_destroyed loops (delete-then-return-nonzero before any push_back).
- src/strategic/FFleet.cpp: FFleet::load() ship-loop nested-return check (same pattern).
- include/strategic/FPlayer.h, include/strategic/FFleet.h: Doxygen load() contract updates only.
- tests/strategic/FGameSaveFormatTest.{h,cpp}: new behavioral deep-truncation test testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce.
- doc/deferred-tasks.md (SF-nested-load-returns entry removal) and AGENTS.md save-format note sync (documentation only).
- Trust boundary reviewed: the binary save-file (.sav) loader consuming attacker-controllable input via FGame::load() -> FPlayer::load() -> FFleet::load()/FVehicle::load().

Why specialist review was triggered:
- Save files are untrusted, attacker-controllable input; a user may open a maliciously crafted or corrupt .sav.
- FR-1 hardens the untrusted-input trust boundary of the binary loader by propagating previously-discarded nested load() return codes, so a stream truncated deep inside a ship/fleet record can no longer bypass FGame::load()'s aggregate-abort guarantee and commit a logically-inconsistent game.
- The change adds new delete-then-return abort paths on the untrusted-input path, which must be confirmed free of double-free, dangling-container-entry, use-after-free, or leak on both success and abort paths.
- Coordinator inserted a specialist Security stage for this subtask because it modifies an untrusted-input parsing trust boundary.

Acceptance criteria / plan reference:
- plans/phase5-save-format-followups-plan.md (FR-1), closing deferred item SF-nested-load-returns from doc/deferred-tasks.md.
- Governing invariant from the completed Phase 5 save-format-hardening feature (plans/phase5-save-format-hardening-plan.md) and AGENTS.md Contributor Notes: FGame::load() must aggregate-abort (return nonzero, report via IStrategicUI::showMessage/reportLoadError, commit no live singleton) on any malformed input, with all reads bounded (readString capped at kMaxSerializedStringBytes, fixed-width readU32/read<T>, sticky failbit).

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/ships/FVehicle.cpp:215-302 - Pre-existing deeper gap (NOT introduced by FR-1, and outside FR-1's allowed file set): FVehicle::load() still does not check its own scalar-field read returns (read<T>/readU32/readString) nor the returns of nested weapon w->load()/defense d->load() calls; it only aborts nonzero via its pre-existing createWeapon()==NULL / createDefense()==NULL checks. A stream truncated strictly inside a vehicle's own scalar region reads defaults/garbage and still returns 0, so FFleet::load()/FPlayer::load()'s new v->load()!=0 checks will not fire for that specific truncation point.
  This is a robustness (not memory-safety) gap: all FVehicle reads remain bounded (readString capped at kMaxSerializedStringBytes; read<T>/readU32 fixed-width; sticky failbit), so no OOB read/over-allocation results, but a vehicle-internal truncation can still yield a logically-inconsistent ship without a load error. FR-1's allowed files (FPlayer.cpp/FFleet.cpp only) cannot close it. Recommend it be tracked as a follow-up deferred item (analogous to SF-enum-codes) so the aggregate-abort contract can eventually be made total. Non-blocking for FR-1.
- tests/strategic/FGameSaveFormatTest.cpp:313-368 - The single new behavioral test exercises FPlayer::load()'s fleet-loop propagation landing on FFleet::load()'s createShip("")==NULL abort (via a deterministic pre-fleet-ship truncation). The FPlayer m_unattached-loop, FPlayer m_destroyed-loop, and FFleet ship-loop v->load()!=0 branches are not each independently exercised behaviorally; they are the same three-line delete-then-return pattern and are covered by structural inspection.
  The chosen truncation point is the only one that deterministically yields a nonzero nested return without relying on FVehicle's unchecked internal reads (see the FVehicle note), so single-test behavioral coverage of the propagation contract is reasonable and correctly fails against the unfixed code. The remaining branches are memory-safety-verified by inspection. Non-blocking; recorded so the Verifier can weigh whether additional per-loop behavioral cases are desired. From a security standpoint the trust-boundary guarantee is adequately demonstrated end-to-end.

Test sufficiency assessment:
- Adequate for the security-sensitive behavior. testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce is fully behavioral: it builds a real valid FGame save via buildValidSaveBytes(), truncates the stream immediately before a fleet's first ship's type-tag, calls the real FGame::load(), and asserts the observed runtime outcome (rc != 0 and mock IStrategicUI showMessage called exactly once).
- The test would fail against the unfixed code: pre-FR-1, FPlayer::load()'s fleet loop discarded FFleet::load()'s nonzero return and returned 0, so FGame::load() would return 0 and commit a half-built fleet. It passes after the fix.
- Pre-existing negative coverage is preserved: wrong-magic, unsupported-version, header-truncation, unknown-factory-type, and no-UI console-fallback cases remain green (testLoadUnknownFactoryTypeReturnsNonzeroAndReportsExactlyOnce still exercises the unchanged createShip()==NULL abort).
- Independently rebuilt src/strategic and tests/strategic clean (exit 0) in this review; the full SSWTests suite run relies on the Tester's committed make check result (SSWTests 290/290 including the new test, TacticalTests 253/253, GuiTests 83/83, exit 0), which is consistent with the code-inspection analysis.
- Observation (non-blocking): the FPlayer m_unattached/m_destroyed loops and FFleet ship-loop v->load()!=0 branches are covered by structural inspection rather than an independent behavioral case each; the memory-safety of those identical paths is confirmed by code inspection.

Documentation / operational guidance assessment:
- Adequate. FPlayer.h and FFleet.h load() Doxygen blocks document the new nested-return propagation contract with @date Last Modified bumped to Jul 19, 2026 and Claude Sonnet 5 (medium) retained in @author.
- doc/deferred-tasks.md correctly removes the resolved SF-nested-load-returns entry (following the repository precedent of full removal for resolved items); the remaining SF-enum-codes and SF-nullfmap-paint-guard deferred entries are untouched, as expected for FR-1's scope.
- AGENTS.md Contributor Notes save-format paragraph was synced to describe the nested-load-return propagation; no edits to the non-volatile doc/rules/tactical_operations_manual.md.
- Recommend (non-blocking) recording the FVehicle::load() internal-read-checking gap as a tracked deferred item so the aggregate-abort contract's remaining hole is documented.

Artifacts written:
- artifacts/phase5-save-format-followups/FR-1/security_report.md
- artifacts/phase5-save-format-followups/FR-1/security_result.json

Outcome:
- PASS
