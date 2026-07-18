# Deferred Tasks

This is the standing list of intentionally-deferred technical items for the
SSW codebase. Each entry records a known, agreed-upon gap between current
behavior and the target behavior, why the fix is deferred rather than done
immediately, and which future phase should pick it up. Planners should append
new entries here rather than letting deferred items live only in a single
plan document or reviewer report.

An entry stays in this file until the deferred work is completed (at which
point it should be removed or marked resolved by the plan that closes it).

---

## SF-enum-codes — Serialize weapon/defense enums as explicit stable codes

**Description:** Weapon and defense type enums are serialized by their raw
ordinal value (`FWeapon::Weapon` / `FDefense::Defense` written directly via the
`write<T>` helper in `FVehicle::save()`/`load()` and the weapon/defense
`save()`/`load()` paths). Inserting a new value in the middle of either enum
silently renumbers the existing values, so an old save decodes to the wrong
weapon/defense type. The fix is to serialize a stable integer code per type
(decoupled from enum declaration order) via an explicit code<->type mapping,
so enum insertions no longer corrupt saved ships.

**Origin:** `doc/synthesized-roadmap.md` §3.3 item (3), surfaced during
Save-format hardening (roadmap Part III item #5) planning and deliberately
scoped out of that change.

**Why deferred:** Stable type codes belong to the broader data-driven-stats
initiative (roadmap Part III item #5's ships/weapons/defenses table + factory
registries + stable serialization codes). Pulling the code<->type mapping into
the format-hardening change would drag in the weapons/defenses factory redesign.
The versioned header added by the save-format hardening phase is precisely what
lets stable codes be introduced later behind a normal version bump without an
additional out-of-band compatibility break.

**Target phase:** Data-driven ship/weapon/defense stat tables + factory
registries (roadmap `doc/synthesized-roadmap.md` Part III "Next" item #5,
data-driven-stats initiative / Cross-Cutting Initiative #5).

---

## SF-fhexmap — Implement `FHexMap::save()`/`load()` (tactical occupancy persistence)

**Description:** `FHexMap::save()` and `load()` are inline no-op stubs
(`include/core/FHexMap.h:171-172`, `{return 0;}`). A caller persisting a
tactical battle's hex occupancy map silently loses all occupancy data and is
told the operation succeeded. The strategic save path (`FGame::save()`/`load()`)
does not currently traverse `FHexMap`, so this is latent rather than an active
data-loss bug today.

**Origin:** `doc/synthesized-roadmap.md` §3.1, surfaced during Save-format
hardening (roadmap Part III item #5) planning and scoped out of that change.

**Why deferred:** `FHexMap` occupancy persistence is only meaningful once
tactical battles are themselves saveable, which roadmap §6.2 item 12 ties to the
later `FTacticalGame` decomposition/serialization work. Implementing it during
strategic save-format hardening would add tactical-persistence surface with no
caller and no way to exercise it end-to-end.

**Target phase:** Tactical save/load, gated behind `FTacticalGame`
decomposition/serialization (roadmap `doc/synthesized-roadmap.md` §6.2 item 12).

---

## SF-nested-load-returns — `FPlayer::load()`/`FFleet::load()` ignore nested sub-object load() return codes

**Description:** `FPlayer::load()` (`src/strategic/FPlayer.cpp`) discards the
return value of `v->load()` in both its `m_unattached` loop and its
`m_destroyed` loop, and of `f->load()` in its fleet loop; `FFleet::load()`
(`src/strategic/FFleet.cpp`) likewise discards the return value of `v->load()`
for each ship it loads. A save stream truncated deep inside fleet/ship data
makes the nested `FFleet`/`FVehicle::load()` call return nonzero, but the
discarding caller still returns 0, so `FGame::load()`'s per-player check
(`src/strategic/FGame.cpp`) observes success even though the underlying data
is incomplete. The aggregate-abort guarantee added by the save-format
hardening work (P5-3 magic/version header plus `FGame::load()`'s per-sub-load
return checks) is bypassed for this class of deep-truncation input, silently
producing a logically-inconsistent game (partial fleet, fields left at
constructor defaults) instead of the intended load-error path.

**Origin:** Surfaced by the P5-5 Security review (pass 1, Finding #1) during
save-format hardening. Not a memory-safety issue -- all reads remain bounded
(`readString` capped at `kMaxSerializedStringBytes`, `readU32`/`read<T>` fixed
width, sticky stream failbit) -- but it is a save-load robustness/hardening
gap: the top-level abort contract `FGame::load()` provides is only as strong
as its weakest nested `load()` caller.

**Why deferred:** The fix (checking and propagating `v->load()`/`f->load()`
return codes in `FPlayer::load()`'s three loops and `v->load()` in
`FFleet::load()`) touches files outside P5-5's allowed scope (P5-3-scope
files: `src/strategic/FPlayer.cpp`, `src/strategic/FFleet.cpp`). Fixing it
requires reopening the P5-3 subtask rather than extending P5-5's
GUI-load-error-UX scope.

**Target phase:** P5-3 remediation (save-format hardening,
`src/strategic/FPlayer.cpp`/`src/strategic/FFleet.cpp` load-path hardening).

---

## SF-nullfmap-paint-guard — GUI paint path lacks a NULL-`FMap` guard

**Description:** The GUI paint path `FGamePanel::onPaint()` ->
`WXGameDisplay::draw()` -> `WXMapDisplay::getScale()` -> `FMap::getMaxSize()`
has no guard against a NULL or partially-built `FMap` singleton. P5-5 pass 2
mitigated the one reachable trigger inside `FMainFrame::onOpen()` by deferring
`m_drawingPanel->setGame(m_game)` until after `FGame::load()` succeeds (see
`src/FMainFrame.cpp`), so a failed/corrupt load can no longer deliver a
spontaneous repaint of a `FGamePanel` associated with a game whose `FMap` was
never assigned. That is a call-site mitigation, not the root-cause fix: it
only prevents `onOpen()`'s own load-error nested modal loop from reaching the
unguarded paint path. It does not add a NULL-`FMap` guard to the paint path
itself, so any other path that could show a live `FGamePanel` backed by a
NULL or partially-built `FMap` remains unguarded and can still crash.

**Origin:** Surfaced by the P5-5 Security review (pass 1, Finding #2) during
save-format hardening; the in-scope mitigation (setGame deferral) was
implemented and verified in P5-5 pass 2.

**Why deferred:** The canonical fix -- a NULL-`FMap` guard in the gui draw
path (`FGamePanel::onPaint()` / `WXGameDisplay::draw()` /
`WXMapDisplay::getScale()`) -- lives in the gui module, outside P5-5's
allowed files, and is a root-cause hardening change rather than a
load-error-UX behavior belonging to the save-format hardening phase.

**Target phase:** gui-module follow-up (add a NULL/invalid-`FMap` guard to
`WXMapDisplay::getScale()` or an earlier point in the
`FGamePanel`/`WXGameDisplay` draw chain, tolerant of a `FGamePanel` shown
before or during an unloaded/partially-loaded game).
