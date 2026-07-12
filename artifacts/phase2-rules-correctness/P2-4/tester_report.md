# Tester Report — P2-4 (pass-2 remediation)

## Task
Pass-2 remediation testing for P2-4: validate the **bounded** retreat-condition
re-prompt loop the Implementer added to `FGame::init()`
(`src/strategic/FGame.cpp`). Pass-1 fixed `init()` to validate
`IStrategicUI::selectRetreatCondition()` against the valid 1..5 range and
re-prompt on out-of-range values; the pass-1 Verifier returned PASS with one
non-blocking WARNING that a non-NULL but degenerate UI (e.g. a headless
`WXStrategicUI` when `wxTheApp == NULL`) returning an invalid value (such as
`wxID_CANCEL`) on every call would spin the loop forever. Pass-2 adds a
documented cap (`kMaxRetreatConditionPrompts = 1000`) and only assigns
`m_satharRetreat` when the final value is actually in 1..5, so an exhausted cap
leaves `m_satharRetreat` at its prior/default value rather than storing a bogus
out-of-range value.

- Implementer code commit under test: `30b16cc09f5e41e526b2fbe28f235702ff9c5c1c`
- Branch: `p2rc-P2-4-tester-20260711`

## Implementation surface under test
- `src/strategic/FGame.cpp` — `FGame::init(wxWindow*)` bounded re-prompt loop.
- `include/strategic/FGame.h` — `FGame::init` Doxygen header only (no behavior).

## Test scope decision
The pass-1 fixture `FGameRetreatConditionValidationTest` and its scripted mock
`ScriptedRetreatConditionMockStrategicUI` already exist and are registered in
`tests/SSWTests.cpp`. Per the task, the 3 existing pass-1 test methods were left
**unmodified**, and **one** new behavioral test was ADDED to the same fixture.
No new mock class was needed: the existing scripted mock holds its last scripted
value once the sequence is exhausted, so a single-element invalid sequence (-1,
standing in for `wxID_CANCEL`) already yields "always returns invalid" behavior.
`tests/SSWTests.cpp` already registers the fixture, so no registration change was
required.

## New test added
`FGameRetreatConditionValidationTest::testInitBoundsRepromptLoopAndLeavesRetreatConditionUnset`
(added to `tests/strategic/FGameRetreatConditionValidationTest.cpp` and its
declaration + `CPPUNIT_TEST(...)` entry in the `.h`). It is a fully behavioral
test — it constructs a real `FGame`, installs a real mock `IStrategicUI` via the
`FGame::create(IStrategicUI*)` seam, runs `init()`, and asserts on observed
runtime state. It covers the pass-2 acceptance criteria:

- **(a) Termination / bounded loop**: `init(NULL)` returns `0` (the test itself
  completing proves the loop did not hang), and `selectRetreatCondition()` was
  invoked **exactly 1000 times** — `CPPUNIT_ASSERT_EQUAL(1000, mock.selectRetreatConditionCalls)`.
  The exact-count assertion distinguishes a bounded cap from an unspecified
  "eventually gives up"; against a pre-fix unbounded loop this test would hang.
- **(b) Cap count**: the 1000 assertion above is the direct observation of the
  documented `kMaxRetreatConditionPrompts` cap.
- **(c) No bogus stored value**: `showRetreatConditions()` dispatches the safe
  default branch — the emitted text contains `"Error"` (i.e. "Error:  No
  retreat condition selected") and contains **none** of the five valid
  condition texts ("two assault carriers", "five heavy cruisers", "40 ships",
  "tenday passes", "Fighters and Militia ships are not counted"). Paired with the
  exact 1000-call assertion, this proves `m_satharRetreat` was left at its
  unset/default value and was never set to the invalid sentinel.

All assertions are behavioral (runtime state / emitted dispatch text); no
source-string inspection is used.

## Test results
- Build (repo root `make`): clean, exit 0.
- Test build (`cd tests && make`): clean.
- Suite run (`./SSWTests`, cwd = `tests/`): **OK (230 tests)** — all green, zero
  failures. Baseline for this pass was 229 tests; the new test brings the count
  to 230.
- The 3 existing pass-1 tests
  (`testInitRepromptsUntilValidRetreatConditionIsReturned`,
  `testInitAcceptsValidRetreatConditionWithoutExtraPrompts`,
  `testInitStoredConditionDrivesCorrectVictoryDispatch`) remain unchanged and
  green.
- No NEW failures. In particular `FGameHeaderDependencyTest` (noted as prone to
  cwd-artifact source-path failures in unusual worktree cwds) did not fail —
  the run was executed with cwd = `tests/` and the suite reported a clean
  `OK (230 tests)`.
- The new test completes effectively instantly (1000 mock calls with no real UI
  work); no special timeout handling was required.

| Metric | Value |
|---|---|
| Tests in suite | 230 (was 229) |
| Passed | 230 |
| Failed | 0 |
| New tests added | 1 |
| Existing tests modified | 0 |

## Commit
- Test commit: `76aa9dedfec2e7fbe82aab3ce4d0e25a093ad7e6`
- Files changed by this commit:
  - `tests/strategic/FGameRetreatConditionValidationTest.cpp`
  - `tests/strategic/FGameRetreatConditionValidationTest.h`

## Cleanup
A temporary run helper (`run_tests.sh`) was written only under the session
scratchpad directory (outside the repository/worktree); no temporary byproducts
remain in the worktree.

## Outcome
PASS — the pass-2 bounded-loop remediation is behaviorally covered and all tests
are green. Handing off to the Documenter.
