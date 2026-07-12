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

## H2 — Persist which defense is active across save/load

**Description:** A ship's currently-raised defense (`FVehicle::getCurrentDefense()`
/ the raised screen selected via combat) is not written by `FPlayer::save()`
or restored by `FPlayer::load()` (which flow through `FVehicle::save()`/`load()`).
After a save/reload cycle, the active-defense selection a ship had before the
save is lost, which can silently change to-hit resolution (see T2/T3, Phase 2
rules-correctness) for that ship after reload.

**Origin:** Identified during Phase 1 critical-defects work (C3) and
reconfirmed during Phase 2 rules-correctness planning (T2/T3 corrected the
in-memory to-hit resolution logic; H2 is the persistence gap for the same
piece of state).

**Why deferred:** Persisting the active defense requires adding a new field
to the ship save/load wire format (`FVehicle::save()`/`load()`, and by
extension every save file written after the change). That is a save-format
break and must be coordinated with the other format-affecting deferred items
rather than done piecemeal inside an unrelated correctness fix.

**Target phase:** Save-format hardening phase (roadmap `doc/synthesized-roadmap.md`
Part III "Next" item #5).

---

## F2-serialization — Serialize `FPlayer::m_destroyed`

**Description:** `FPlayer::m_destroyed` (the list of a player's destroyed
ships, populated via `addDestroyedShip()`) is not written by `FPlayer::save()`
or restored by `FPlayer::load()`. `FPlayer` now correctly owns and frees
these ships in memory (see `FPlayer::~FPlayer()`, fixed as part of this
plan's F2 item), but the list itself does not survive a save/reload cycle.
This list is intended to back the game's optional Replacements rule, which
needs to know which ships a player has lost in order to make them eligible
for replacement.

**Origin:** `artifacts/phase1-critical-defects/reviewer_report.md` follow-up
item F2/H2, refined during Phase 2 rules-correctness planning (subtask P2-5).

**Why deferred:** Serializing `m_destroyed` requires extending `FPlayer::save()`/
`load()`'s wire format with a new list, which is a save-format break — the
same category of change as H2, and deferred for the same reason: format
changes must be coordinated together rather than introduced incidentally by
a memory-ownership bugfix.

**Target phase:** Save-format hardening phase (roadmap `doc/synthesized-roadmap.md`
Part III "Next" item #5), alongside H2.
