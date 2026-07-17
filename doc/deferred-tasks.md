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
