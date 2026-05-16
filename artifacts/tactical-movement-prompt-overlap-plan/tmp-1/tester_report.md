# Tester Report: Tactical movement prompt overlap remediation

## Scope
Validated tactical movement-phase layout remediation for:
- wrapped movement instructions vs action-button row spacing,
- selected-ship stats separation,
- updated movement reminder wording,
- dynamic margin / helper exposure for regression locking.

## Assumptions
- Used provided test targets as canonical commands: `cd tests && make tactical-tests && ./tactical/TacticalTests` and `cd tests/gui && make && xvfb-run -a ./GuiTests`.
- Implementer-modified surface is limited to `include/tactical/FBattleDisplay.h` and `src/tactical/FBattleDisplay.cpp`.

## Test changes
- `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`
  - Added source-contract checks for wrapped movement prompt flow (`drawWrappedActionPrompt`, `countWrappedActionPromptLines`, `reserveActionPromptLines`), quoted `Movement Done` reminder text, non-movement reservation reset, and available-width fallback marker `largestMarginWithStatsRoom`.
- `tests/gui/TacticalGuiLiveTest.cpp`
  - Added GUI fixture source-lock assertions for quoted movement reminder, prompt wrapping helpers, dynamic prompt reservation, and stats-margin fallback token.

## Commands run
1. `cd tests && make tactical-tests && ./tactical/TacticalTests` (baseline) — PASS (`OK (143 tests)`)
2. `cd tests/gui && make && xvfb-run -a ./GuiTests` (baseline) — PASS (`OK (33 tests)`)
3. `cd tests && make tactical-tests && ./tactical/TacticalTests` (after test updates) — FAIL (3 failures)
4. `cd tests/gui && make && xvfb-run -a ./GuiTests` (after test updates) — FAIL (1 failure)

## Failure evidence
- Tactical failure: expected movement reminder text
  - `Press the 'Movement Done' button when all ships have been assigned their movement instructions.`
  - actual source still contains shorter legacy reminder.
- Tactical failure: expected helper declarations/usages not present
  - `reserveActionPromptLines(...)`
  - `countWrappedActionPromptLines(...)`
- GUI failure mirrors missing quoted reminder token in `src/tactical/FBattleDisplay.cpp`.

## Acceptance criteria status
1. Movement prompt and selected-ship stats do not overlap in selected-ship movement state: **not met** (new dynamic reservation/margin evidence absent).
2. Revised third instruction line with quoted `Movement Done` label: **not met**.
3. Narrow-width readability via wrapping/constrained rendering: **not met** (`drawWrappedActionPrompt`/`countWrappedActionPromptLines` absent).
4. Dynamic margin derived from available/prompt width constraints: **not met** (`largestMarginWithStatsRoom` branch absent).
5. Explicit helper/constants for downstream regression coverage: **partially met** (existing fixed spacing constants exist; new wrapping/reservation helpers absent).

## Commits
- Test changes commit: `2239e3544516fba1e5b161ae092476156ff474a9`
- Artifact commit: pending at time of this report write.

## Cleanup
- No temporary non-handoff byproducts created.
