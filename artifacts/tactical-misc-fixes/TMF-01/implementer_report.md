# Implementer Report — TMF-01

**Status:** success
**Branch:** tmf-TMF-01-implementer-20260630
**Code commit hash:** ceb75e8

## Task Summary

Single-source the three PH_SEEKER_ACTIVATION instruction strings in `drawSeekerActivation()`
by extracting them into static `const wxString` constants in `FBattleDisplay`, mirroring the
SMRVI `SEEKER_DEPLOY_INSTRUCTION` pattern.

## Changed Files

- `include/tactical/FBattleDisplay.h`
- `src/tactical/FBattleDisplay.cpp`

## Implementation Details

**Header (`include/tactical/FBattleDisplay.h`):**
- Added three new static `const wxString` constants after the existing `SEEKER_DEPLOY_INSTRUCTION`:
  - `SEEKER_ACTIVATION_PHASE_INSTRUCTION` ("Seeker activation phase.")
  - `SEEKER_ACTIVATION_CLICK_INSTRUCTION` ("Click a seeker stack on the board to activate one seeker.")
  - `SEEKER_ACTIVATION_DEACTIVATE_INSTRUCTION` ("Click a row below to deactivate an activated seeker.")
- Updated `@date Last Modified` on the file header.
- Updated `@date Last Modified` and description on the `drawSeekerActivation()` doxygen block.

**Source (`src/tactical/FBattleDisplay.cpp`):**
- Added three constant definitions after the existing `SEEKER_DEPLOY_INSTRUCTION` definition.
- Replaced all six inline use sites in `drawSeekerActivation()`:
  - 3 `GetTextExtent(...)` calls in the `instrW` computation
  - 3 `DrawText(...)` calls in the drawing block
- Updated `@date Last Modified` on the file header.

## Validation Results

| Command | Result |
|---------|--------|
| `make` (repo root) | PASS |
| `make tactical-tests && ./tactical/TacticalTests` | 225/226 PASS (1 expected source-contract failure) |
| `xvfb-run -a ./GuiTests` | Not runnable — `xvfb-run` not available in environment |

**Expected source-contract failure (Tester must fix):**
```
FTacticalBattleDisplayFireFlowTest::testSeekerActivationPanelListsInstructionAndOneRowPerInactiveSeeker
  at FTacticalBattleDisplayFireFlowTest.cpp:243
  assertion: haystack.find("dc.DrawText(\"Seeker activation phase.\",leftOffset,getActionPromptLineY(0));")
```
This assertion checked for the old inline literal. The literal was intentionally replaced by
`SEEKER_ACTIVATION_PHASE_INSTRUCTION` as required by the approved task. This is an expected
consequence of the approved change, not an unintended regression. The Tester owns updating
the source-contract assertion to match the new constant-based pattern.

## Acceptance Criteria Status

1. Each of the three instruction strings is defined exactly once as a shared constant and
   used by both the measurement and drawing paths in drawSeekerActivation(). **SATISFIED**
2. No duplicated inline copy of any of the three literals remains in production code. **SATISFIED**
3. The seeker-activation panel renders identically (wording, layout, dynamic margin unchanged).
   **Structurally satisfied** — same string values, same layout logic. Behavioral GUI test
   not run (no display available); Tester must verify.

## Artifacts Written

- `artifacts/tactical-misc-fixes/TMF-01/implementer_report.md`
- `artifacts/tactical-misc-fixes/TMF-01/tester_prompt.txt`
- `artifacts/tactical-misc-fixes/TMF-01/implementer_result.json`
