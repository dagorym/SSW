# Implementer Report

Status:
- success

Task summary:
- T2/T3: Resolve weapon to-hit against the target's most-effective OPERATING defense with the attracting-screen override, and restrict the masking-screen 'fired out of the screen' effect to lasers only.

Changed files:
- src/weapons/FWeapon.cpp
- src/ships/FVehicle.cpp
- include/ships/FVehicle.h

Validation commands run:
- make (repo root) -- clean build, exit 0
- make in tests/weapons and tests/ships -- module test libraries build clean (no standalone WeaponsTests/ShipsTests runner exists; both are linked into SSWTests)
- make in tests && run SSWTests from the tests/ directory -- OK (217 tests)

Validation outcome:
- PASS -- repo builds clean; full SSWTests suite OK (217 tests) when run from the tests/ directory. NOTE: SSWTests must be run with the tests/ directory as the working directory (e.g. env -C tests ./SSWTests); FGameHeaderDependencyTest reads source files via paths relative to cwd (../include/strategic/FGame.h) and reports 9 spurious 'candidate file path not readable' failures if launched from any other directory. Those 9 are a cwd artifact unrelated to this change (FGame is untouched), and all 217 pass from the correct directory.

Implementation/code commit hash:
- b65b1ffcca0466b99ba96804f3393e22f61b466a

Artifacts written:
- artifacts/phase2-rules-correctness/P2-2/implementer_report.md
- artifacts/phase2-rules-correctness/P2-2/tester_prompt.txt
- artifacts/phase2-rules-correctness/P2-2/implementer_result.json

Implementation context:
- New public method FVehicle::resolveToHitModifier(FWeapon::Weapon w) (include/ships/FVehicle.h + src/ships/FVehicle.cpp). It enumerates the target's OPERATING defenses via getDefense(i)/getDefenseCount() (NOT the ambiguous hasDefense() return): the reflective hull (FDefense::RH) if the ship owns one, plus the currently-raised getCurrentDefense(), deduplicated by pointer identity. It returns the MINIMUM getAttackModifier(w) among those, EXCEPT if an operating defense attracts w, in which case it returns that defense's modifier. Returns 0 if there are no operating defenses.
- Attract relations are encoded in an anonymous-namespace helper defenseAttractsWeapon(FDefense::Defense, FWeapon::Weapon) in src/ships/FVehicle.cpp: PS attracts EB; ES attracts PB; SS attracts T, SM, and M.
- FWeapon::fire() to-hit branch (src/weapons/FWeapon.cpp ~line 133): if the weapon is a laser (LC or LB) AND m_parent != NULL AND the attacker's current defense is MS, add m_parent->getCurrentDefense()->getAttackModifier(m_type) (laser fired out of the screen); ELSE add m_target->resolveToHitModifier(m_type). The 5%-floor, ICM, head-on (+10), range-dispersion (m_RD), and combat-control-damage (-10) adjustments are unchanged.
- Damage-halving block (FWeapon::fire) is unchanged in intent but now guards the m_parent MS check with (m_parent != NULL && ...) so a parentless mine/seeker does not deref. It still halves only for LC/LB when the target OR (non-null) attacker has MS raised.
- Reference modifiers (from the defense classes, for deriving exact Combat-Table percentages): FReflectiveHull::getAttackModifier returns 0 for all weapons; FProtonScreen: LB/LC=+15, EB=+10 (attract), PB=-35, DC=-10, else 0; FElectronScreen: PB=+10 (attract) etc.; FStasisScreen: T=+25, SM=+15, M=+20 (attract). Positive/attract modifiers are LARGER than RH's 0, so the attract override makes the target easier to hit -- this is the observable distinguishing correct behavior from most-effective-only.
- Tester MUST include a torpedo-fired-out-of-attacker-MS test that asserts the to-hit uses the TARGET's resolveToHitModifier (not the attacker's MS) and that damage is NOT halved; this test must fail against the pre-fix code. Behavioral tests must construct real FVehicle attackers/targets, raise the relevant defenses via setCurrentDefense(...), and assert observed toHitProbability and damageRolled/outcome from FWeapon::fire()'s FTacticalAttackResult (deriving exact percentages from the manual's Combat Table).

Expected validation failures carried forward:
- None
