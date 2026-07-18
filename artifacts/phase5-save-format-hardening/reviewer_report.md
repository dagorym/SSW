# Reviewer Report — Phase 5: Save-Format Hardening

**Reviewer:** claude-opus-4-8[1m] (reviewer role)
**Review type:** Final, read-only, feature-level review across all five subtasks
**Coordination base branch:** `phase5`
**Date:** Jul 18, 2026

---

## Feature plan reviewed

- `plans/phase5-save-format-hardening-plan.md` (authoritative scope/acceptance
  source). Feature = one coordinated rebuild of the SSW binary serialization
  format so saves are versioned, portable, and robust against corrupt/malicious
  input (roadmap Part III "Next" item #5). Bundles H1, H2, H3, H4, the
  `writeString` NUL bug, the `FJumpRoute`/`FMap` 16-bit pointer-mask, factory
  null-deref, and F2-serialization.

## Inputs reviewed

- Governing plan (Sections 2, 4, 5, 6, 7) plus all five Coordinator-ready
  Implementer prompts.
- Per-subtask artifacts P5-1..P5-5 (implementer / tester / documenter / security /
  verifier reports + result.json; P5-4 and P5-5 `history/pass-1/` remediation
  cycles). All five subtasks: **Verifier PASS**, **Security PASS** (P5-5 Security
  upgraded from a pass-1 CONDITIONAL PASS after the setGame-deferral mitigation).
- Merged source/test/doc diff `master...phase5` (merge-base `33f0cb4b`), read
  directly at file level, not via verdict labels:
  - `include/core/FSaveFormat.h`, `include/core/FPObject.h`, `src/core/FPObject.cpp`
  - `src/ships/FVehicle.cpp` (+ header)
  - `src/strategic/FPlayer.cpp`, `FFleet.cpp`, `FMap.cpp`, `FSystem.cpp`,
    `FJumpRoute.cpp`, `FGame.cpp` (+ headers)
  - `src/FMainFrame.cpp` (+ header)
  - `doc/deferred-tasks.md`, `doc/synthesized-roadmap.md`, `AGENTS.md`,
    `doc/UsersGuide.md`
  - Behavioral tests: `tests/core/FPObjectSerializationTest.*`,
    `tests/ships/FVehicleTest.*`, `tests/strategic/FPlayerTest.*`,
    `FFleetTest.*`, `FMapTest.*`, `FSystemTest.*`, `FJumpRouteTest.*`,
    `FGameSaveFormatTest.*`, `tests/gui/StrategicGuiLiveTest.*`
- Independent build/test spot-check: `make check` from a fresh tree —
  **TacticalTests 253 OK**, **GuiTests 83 OK**; `SSWTests` **289 OK** when run
  from `tests/` (the single apparent failure was a cwd-sensitive source-contract
  test, `FGameHeaderDependencyTest`, that resolves candidate paths relative to
  its working directory — it passes under `make check`/from `tests/`). Counts
  match the reported final state exactly.

---

## Overall feature completeness

The delivered feature matches the governing plan. Every Section 4 IN-scope item is
implemented and backed by behavioral coverage; the four binding Section 2 design
decisions hold consistently across all subtasks (verified at file level, not by
trusting subtask verdicts); the Section 6 documentation obligations all landed and
are internally consistent; and the cross-subtask integration (full-game round-trip
through `FGame::save/load`) is real and green.

### Section 2 design decisions — hold consistently across subtasks

1. **Clean break / no legacy read path.** `FGame::load` reads `kSaveMagic` then
   `kSaveFormatVersion` first and rejects on mismatch/unsupported before
   interpreting any game data. The legacy `if (m_jumpRouteID == 0) m_jumpRouteID
   = NO_ROUTE;` normalization is removed in `FFleet::load` (H4). Confirmed.
2. **Fixed-width little-endian wire ints.** `writeU32`/`readU32` do explicit
   byte-shift/mask emission (not raw struct copy). All counts and object IDs
   route through them in `FPObject`, `FVehicle`, `FPlayer`, `FFleet`, `FMap`,
   `FSystem`, `FJumpRoute`, and `FGame` (incl. `m_currentPlayer`,
   `m_players.size()`, and `FJumpRoute` start/end system IDs). Single-byte and
   other multi-byte scalars keep `write<T>`/`read<T>` (now with a post-I/O
   stream-state check). Consistent everywhere I inspected.
3. **Robust helpers + 64 KB cap + factory null-checks.** `readString` validates
   the decoded length against `kMaxSerializedStringBytes` (named constant, 65536)
   **before** allocating; `writeString` uses `os.write(s.data(), s.size())`
   (embedded-NUL safe) and oversize-guards. `createWeapon`/`createDefense`
   (`FVehicle::load`) and `createShip` (`FPlayer::load` ×2, `FFleet::load`) are
   null-checked and abort with nonzero on unknown type. Consistent.
4. **Loader error UX.** `FGame::reportLoadError(...)` routes through
   `m_ui->showMessage(...)` when a UI is installed and falls back to `std::cout`
   otherwise; `FGame::load` returns nonzero on any failure and
   `FMainFrame::onOpen` tears the freshly-created game down via the existing
   `resetGame()` path (which `delete`s the game, NULLs the singleton pointer, and
   leaves all game-dependent menu items disabled). Confirmed.

### Section 4 scope (IN) — each delivered and behaviorally covered

- **H1 format core** — `FSaveFormat.h` constants + hardened `FPObject` helpers
  (P5-1) and the `FGame` magic/version header (P5-5). Covered by
  `FPObjectSerializationTest` (embedded-NUL round-trip, 64 KB no-alloc rejection,
  LE byte layout, failed-stream error codes) and `FGameSaveFormatTest`.
- **H2 active-defense persistence** — `FVehicle::save` writes the index of
  `m_currentDefense` within `m_defenses`; `load` restores it, validating the
  index against the rebuilt list and preserving the CRIT-3 empty/invalid
  fallback. Covered (`FVehicleTest::testLoadRestoresActiveDefenseSelection...`).
- **H3 ID-counter restore** across all five classes — guard applied in each
  `load()`, correctly respecting the per-class increment convention (`FSystem`
  uses `>` for its pre-increment `++m_nextID`; the others use `>=` for
  post/`+1` conventions; `FFleet` handles its signed `m_nextID` via an unsigned
  view). Covered in each of the five `*Test` fixtures.
- **H4 / route-0 removal** — done in `FFleet::load`; covered
  (`FFleetTest::testJumpRouteZeroPreservedAcrossSaveLoad`).
- **`FJumpRoute`/`FMap` ID pointer-mask fix** — real `uint32_t`
  `m_startSystemID`/`m_endSystemID` fields (exposed via getters), resolved in
  `FMap::load` via `getSystem(id)` with a NULL-guard that aborts on an
  out-of-range/unknown system; the `(FSystem*)` cast and `& 0x0000FFFFL` mask are
  gone. Covered (>65535 round-trip + absent-system nonzero return in `FMapTest`).
- **`writeString` NUL fix** — `os.write(s.data(), s.size())`. Covered.
- **F2-serialization (`m_destroyed`)** — persisted/restored with the same
  type-tag + `createShip` + `load` + null-check pattern as `m_unattached`,
  preserving sole-ownership. Covered (`FPlayerTest`).

### Section 7 security — trust boundary enforced consistently

The untrusted-input hardening the plan calls out is enforced at the
memory-safety level across every parsing subtask: oversized string lengths
(64 KB cap, no pre-validation alloc), truncated streams (fixed-width reads +
sticky failbit checks), unknown type tags (factory null-checks → nonzero abort),
out-of-range system IDs (`FMap::load` NULL-guard → nonzero abort), and
integer-boundary counts (all counts are fixed-width `uint32_t`). The removed
16-bit truncation eliminates wrong-object resolution. No half-built `FGame` is
committed as the live singleton on a rejected load.

---

## Findings

### BLOCKING

None.

### WARNING

- **W1 — `SF-nested-load-returns` (known, tracked deferral; correctly scoped,
  acceptable — not a blocking feature-level gap).** I independently confirmed at
  file level that `FFleet::load` discards each `v->load()` return, and
  `FPlayer::load` discards `v->load()` (both the `m_unattached` and `m_destroyed`
  loops) and `f->load()`. A stream truncated *deep inside* a ship/fleet record can
  make the nested `load()` return nonzero while the discarding caller still
  returns 0, so `FGame::load`'s per-player abort check is bypassed for that class
  of input and a logically-inconsistent game (partial fleet, constructor-default
  fields) is committed instead of the intended load-error path. This is a
  data-integrity/robustness gap, **not** memory-safety: all reads remain bounded
  (`readString` cap, fixed-width `readU32`/`read<T>`, sticky failbit) and the
  unknown-*ship-type* case is still caught by the `createShip` null-check. It is
  correctly recorded in `doc/deferred-tasks.md` and is out of P5-5's allowed-file
  scope (the fix lives in the P5-3 files). Acceptable as a deferral; carried
  forward as follow-up FR-1.

### NOTE

- **N1 — `SF-nullfmap-paint-guard` (known, tracked deferral; reachable trigger
  closed).** The gui paint chain (`FGamePanel::onPaint` → `WXGameDisplay::draw` →
  `WXMapDisplay::getScale` → `FMap::getMaxSize`) has no NULL-`FMap` guard. P5-5's
  user-approved, in-scope `onOpen` mitigation (defer `m_drawingPanel->setGame`
  until after `FGame::load` succeeds) closes the one reachable crash trigger, and
  `StrategicGuiLiveTest::testMainFrameOnOpenFailedLoadWithFrameShownSurvivesForcedRepaint`
  behaviorally proves it. The root-cause gui-module guard (defense-in-depth for
  any hypothetical other path) is correctly deferred and recorded. Acceptable;
  carried forward as follow-up FR-2.
- **N2 — enum-ordinal portability (`SF-enum-codes`) remains out of scope, as
  planned.** Weapon/defense `m_type` enums still serialize by raw ordinal. This is
  within design decision 2's stated "two supported little-endian platforms" scope
  and is explicitly deferred and retained in `doc/deferred-tasks.md`. Not a gap.
- **N3 — `SF-fhexmap` (tactical `FHexMap::save/load` stub) correctly remains out
  of scope and retained in `doc/deferred-tasks.md`.** Not a gap.

### Documentation obligations (Section 6) — all landed, consistent

- Doxygen headers updated on every touched `save`/`load` and the `FPObject`
  helper surface, with `@date Last Modified` bumped and the implementing model
  added to `@author`.
- `doc/deferred-tasks.md`: `H2` and `F2-serialization` removed as resolved;
  `SF-enum-codes` and `SF-fhexmap` retained; the two new follow-ups
  (`SF-nested-load-returns`, `SF-nullfmap-paint-guard`) recorded with accurate
  origin, scope-rationale, and target-phase.
- `doc/synthesized-roadmap.md`: H1/H2/H3/H4, the Part III item #5, and the
  pointer-mask / `m_destroyed` review lines annotated `***RESOLVED***`
  consistently with prior phases.
- `AGENTS.md`: concise save-format note added (constants, `writeU32`/`readU32`,
  capped `writeString`/`readString`, magic+version header, aggregate error
  contract) without restating low-level format details in a way that breaks the
  single-source rule.
- `doc/UsersGuide.md`: the one-time compatibility-break + on-screen-error /
  return-to-no-game-state note added to the File→Open entry.

### Cross-subtask integration

The whole hardened format funnels through `FGame::save/load`; the per-module wire
changes compose into a full-game round-trip (round, current player, retreat
condition, loss counters, map, and all players with fleets/ships), verified by
`FGameSaveFormatTest` and the green `make check`. The four malformed-input
rejection paths (wrong magic, unsupported version, truncated stream, unknown
factory type) each return nonzero and report via a mock `IStrategicUI`. The
deferred `SF-enum-codes` and `SF-fhexmap` items remain correctly out of scope.

---

## Follow-up feature requests for planning

- **FR-1 (from W1 / `SF-nested-load-returns`):** Harden the player/fleet load
  path so the top-level aggregate-abort guarantee is not bypassed by
  deep-truncation input. In `src/strategic/FPlayer.cpp` check and propagate the
  return of `v->load()` in both the `m_unattached` and `m_destroyed` loops and of
  `f->load()` in the fleet loop; in `src/strategic/FFleet.cpp` check and propagate
  the return of `v->load()` per ship. On any nonzero nested return, delete the
  just-created object and return nonzero so `FGame::load`'s per-player check and
  loader error UX fire. Add a behavioral test that truncates a stream inside a
  fleet's ship record and asserts `FGame::load` returns nonzero, reports via the
  mock `IStrategicUI`, and leaves no live game singleton. (Scope: P5-3 files;
  data-integrity/robustness, not memory-safety.)

- **FR-2 (from N1 / `SF-nullfmap-paint-guard`):** Add a root-cause NULL/invalid-
  `FMap` guard in the gui draw chain (`WXMapDisplay::getScale` or an earlier point
  in `FGamePanel::onPaint` / `WXGameDisplay::draw`) so a `FGamePanel` shown before
  or during an unloaded/partially-loaded game cannot dereference a NULL/half-built
  `FMap`, independent of the `onOpen` call-site mitigation. Add a gui behavioral
  test that forces a repaint with the panel's game/map unset and asserts no crash.
  (Scope: gui module; defense-in-depth beyond the already-closed `onOpen`
  trigger.)

---

## Final outcome

**PASS.**

The Phase 5 save-format-hardening feature is complete and matches the governing
plan: all Section 4 IN-scope items are delivered with behavioral coverage, the
four Section 2 design decisions hold consistently across subtasks, the untrusted-
input trust boundary is enforced at the memory-safety level throughout, the
Section 6 documentation obligations are satisfied and consistent, and the full-
feature `make check` is green (SSWTests 289, TacticalTests 253, GuiTests 83). The
two open items (`SF-nested-load-returns`, `SF-nullfmap-paint-guard`) are known,
correctly-scoped, and tracked deferrals — neither rises to a blocking feature-
level gap — and are handed off to the Planner as FR-1 and FR-2.
