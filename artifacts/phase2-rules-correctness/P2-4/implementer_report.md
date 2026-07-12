# P2-4 Implementer Report (pass-2 remediation) — Bound the Retreat-Condition Re-Prompt Loop

## Status
COMPLETE

## Context
This is a Verifier-driven remediation pass. The pass-1 Verifier returned
**PASS** for P2-4 with one non-blocking **WARNING**:

> The retreat-condition re-prompt loop in `FGame::init()`
> (`src/strategic/FGame.cpp` ~lines 113-118) is
> `while (retreatCondition < 1 || retreatCondition > 5) { retreatCondition =
> m_ui->selectRetreatCondition(); }` inside the `if (m_ui != NULL)` guard. If a
> non-null but degenerate UI (e.g. a headless `WXStrategicUI` when
> `wxTheApp == NULL`) returns `wxID_CANCEL` on EVERY call, this loop spins
> forever. The plan's P2-4 requirement explicitly says "do not loop forever".
> The null-`m_ui` case is already guarded, but the always-invalid-return case
> is not.

Pass-1's fix (commit `5745906f`) correctly handled the `m_ui == NULL` case
(loop is skipped entirely) and the normal interactive case (re-prompt until
valid), but the `while` loop itself had no upper bound: a UI that always
returns an out-of-range value would spin forever.

## Fix
Added a bounded safety cap to the re-prompt loop in `FGame::init()`
(`src/strategic/FGame.cpp`). The loop now tracks an attempt counter and stops
after `kMaxRetreatConditionPrompts` (1000) calls even if a valid value is
never returned. `m_satharRetreat` is now only assigned when the final
`retreatCondition` is actually in the valid `1..5` range; if the cap is
exhausted without ever seeing a valid value, `m_satharRetreat` is left
untouched at its prior/default value (`-1` from the constructor in the
normal startup path) rather than being set to a bogus out-of-range value.

### `src/strategic/FGame.cpp` — `FGame::init(wxWindow*)`
Before (post pass-1):
```cpp
if (m_ui != NULL){
    int retreatCondition = m_ui->selectRetreatCondition();
    while (retreatCondition < 1 || retreatCondition > 5) {
        retreatCondition = m_ui->selectRetreatCondition();
    }
    m_satharRetreat = retreatCondition;
}
```
After (pass-2):
```cpp
if (m_ui != NULL){
    // Bounded re-prompt cap: a normal interactive UI returns a valid 1..5
    // value well within this many attempts. The cap exists to guard
    // against a degenerate (but non-NULL) UI -- e.g. a headless
    // WXStrategicUI when wxTheApp == NULL -- that returns an invalid
    // value (such as wxID_CANCEL) on every single call; without a cap
    // that case would spin forever. If the cap is exhausted without ever
    // seeing a valid value, m_satharRetreat is left untouched (at its
    // prior/default value) rather than being set to a bogus,
    // out-of-range value.
    const int kMaxRetreatConditionPrompts = 1000;
    int retreatCondition = m_ui->selectRetreatCondition();
    int retreatConditionAttempts = 1;
    while ((retreatCondition < 1 || retreatCondition > 5) &&
            retreatConditionAttempts < kMaxRetreatConditionPrompts) {
        retreatCondition = m_ui->selectRetreatCondition();
        retreatConditionAttempts++;
    }
    if (retreatCondition >= 1 && retreatCondition <= 5) {
        m_satharRetreat = retreatCondition;
    }
}
```

Behavior preserved exactly for the two normal-case scenarios:
- **Valid on first call**: `retreatConditionAttempts` starts at 1, the `while`
  condition is false immediately (no extra call), and the valid value is
  stored — still exactly one `selectRetreatCondition()` call.
- **Invalid then eventually valid**: the loop keeps re-invoking the UI (as
  before) until a valid value is returned (well under the 1000-attempt cap
  for any realistic interactive or scripted-sequence case), then stores it.

New behavior for the degenerate case:
- **Always invalid**: after 1000 calls, the loop exits with `retreatCondition`
  still out of range. The `if (retreatCondition >= 1 && retreatCondition <=
  5)` guard is false, so `m_satharRetreat` is never written — it stays at
  whatever it already was (the constructor default `-1` in the normal
  startup path), never a bogus value like `wxID_CANCEL`. `init()` returns 0
  and completes in bounded time instead of hanging.

### `include/strategic/FGame.h` — `FGame::init(...)` Doxygen header
Expanded the description to document the bounded-cap fallback behavior
(degenerate-UI hang guard, "leave `m_satharRetreat` at its prior/default value
rather than a bogus one"). `@author` already included `Claude Sonnet 5
(medium)` from pass-1 (unchanged — no new author to add for this pass).
`@date Last Modified` remains `Jul 11, 2026` (today's date; pass-1 was also
committed today).

## Scope
Only the two approved files were modified, matching the P2-4 allowed-file
list:
- `src/strategic/FGame.cpp`
- `include/strategic/FGame.h`

`checkForVictory()` switch bodies (C2) and the retreat-condition display text
were left untouched. No test files were added or modified by the Implementer
(test authorship is explicitly the Tester's responsibility per the
remediation preamble).

## Validation
- `make` (repo root): clean build, exit 0.
- `cd tests && make && ./SSWTests` (run with cwd=`tests/`): **OK (229 tests)**
  — all tests pass, no failures. This includes the three pre-existing
  `FGameRetreatConditionValidationTest` behavioral tests from pass-1
  (`testInitRepromptsUntilValidRetreatConditionIsReturned`,
  `testInitAcceptsValidRetreatConditionWithoutExtraPrompts`,
  `testInitStoredConditionDrivesCorrectVictoryDispatch`), which all remain
  green against the bounded-loop change — none of them approach the
  1000-attempt cap, so their call-count and stored-value assertions are
  unaffected. No `FGameHeaderDependencyTest` cwd-artifact failures were
  observed in this run (consistent with pass-1's note that they are
  environment-dependent, not a regression from this change).

## Acceptance Criteria Assessment (pass-2 remediation criteria)
- A mock returning an invalid value first, then a valid one: `init()` still
  re-prompts and stores the VALID value (pass-1 test 1, unchanged and green).
- A mock returning a valid `1..5` immediately: stored unchanged with exactly
  one call (pass-1 test 2, unchanged and green).
- A degenerate mock that ALWAYS returns an invalid value: `init()` now
  terminates after the bounded 1000-attempt cap and does **not** store an
  out-of-range value into `m_satharRetreat` (new behavior added this pass;
  behavioral proof is the Tester's responsibility per the handoff below).
- After setup, `checkForVictory()` always evaluates a real `1..5` condition or
  the safe default — never a bogus stored value (unchanged from pass-1,
  reinforced by the `if (retreatCondition >= 1 && retreatCondition <= 5)`
  guard added this pass).
- Repo builds clean; full suite passes: OK (229 tests), no new failures.

## Code Commit
`30b16cc09f5e41e526b2fbe28f235702ff9c5c1c`

(Verify via `git -C <worktree> show --stat 30b16cc0` — this hash is the
implementation/code commit only; the subsequent artifact commit is not
recorded here per commit-flow rules.)
