Reviewer Report

Feature plan reviewed:
- `plans/tactical-movement-prompt-overlap-replan-v2.md`

Inputs reviewed:
- Current branch tip `7d3a3e602bb0bd4796b6a4a86f4b9581b28adac5`
- Implementation commits/artifacts: `5c332730b252a1a07fb0309f691b1935ec1b2c8d`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/implementer_report.md`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/implementer_result.json`
- Tester remediation artifacts: `b6aa724`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/tester_report.md`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/tester_result.json`
- Documentation remediation artifacts: `edc27151ccaeb8e61537f93867bc0174b4927a73`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/documenter_report.md`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/documenter_result.json`
- Verifier remediation artifacts: `ea27bb20e70c4398e8d43635a0c3fd36a974d011`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/verifier_prompt.txt`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/verifier_report.md`, `artifacts/tactical-movement-prompt-overlap-plan/tmp-1/verifier_result.json`
- Reviewed source/tests/docs: `include/tactical/FBattleScreen.h`, `include/tactical/FBattleDisplay.h`, `src/tactical/FBattleScreen.cpp`, `src/tactical/FBattleDisplay.cpp`, `tests/gui/TacticalGuiLiveTest.cpp`, `tests/gui/TacticalGuiLiveTest.h`, `tests/tactical/FTacticalBattleDisplayFireFlowTest.cpp`, `tests/tactical/FTacticalBattleDisplayFireFlowTest.h`, `doc/UsersGuide.md`, `doc/DesignNotes.md`
- Validation rerun on this tip: `cd tests && make tactical-tests && ./tactical/TacticalTests` → `OK (152 tests)`; `cd tests/gui && make && xvfb-run -a ./GuiTests` → `OK (35 tests)`

Overall feature completeness:
- The delivered remediation satisfies the main shipped behavior in scope: `FBattleScreen` now defaults to `1200x900`, the lower panel starts from `120px`, the map floor stays at `60%`, open-dialog resize reflows immediately, shrink-back after width pressure removal is implemented and covered by live GUI restore coverage, and the reviewed diff stays within tactical layout behavior without changing combat rules or fire-resolution flow.
- Documentation matches the shipped resize/reflow and shrink-back behavior.
- One plan-level edge case remains: the implementation still retains a fixed three-line prompt reservation cap, so the feature does not fully match the governing plan's "no fixed prompt-line cap" layout contract.

Findings

BLOCKING
- None.

WARNING
- The governing plan's locked decision "There is no fixed prompt-line cap; layout decisions are geometry-driven" is not fully implemented. `include/tactical/FBattleDisplay.h` still defines `ACTION_PROMPT_MAX_LINES = 3`, `reserveActionPromptLines(...)` clamps to that maximum in `src/tactical/FBattleDisplay.cpp`, and both `getActionButtonTopSpacerHeight()` and `ensureLowerPanelLayoutState(...)` still size the button band / requested display height from that fixed cap. The current remediation therefore proves the tested narrow-and-restore path, but it still leaves an unreviewed narrower-width edge case where prompt wrapping can exceed the reserved three-line budget instead of growing fully from final rendered geometry as the plan required.

NOTE
- The shrink-back remediation itself is correctly evidenced in both source and tests: the retained-maximum height logic was removed from `FBattleDisplay::reflowLowerPanelLayout()` / `ensureLowerPanelLayoutState(...)` and from `FBattleScreen::applyLayoutPolicy()`, while `TacticalGuiLiveTest::testBattleDisplayLowerPanelLayoutStatePersistsAcrossPhaseAndGeometryChanges()` now proves the lower panel returns to its earlier height after width restoration.
- I found no evidence that the resize/reflow seam altered tactical combat rules, tactical state transitions, or fire-resolution behavior.
- `doc/UsersGuide.md` and `doc/DesignNotes.md` accurately describe the shipped resize/reflow and shrink-back behavior; no further documentation update is required for the current shipped implementation.

Explicit discussion of missed functionality or edge cases:
- The delivered work covers the remediated shrink-back bug and the deterministic selected-ship resize path that the tester/verifier focused on.
- However, the original plan also required prompt-driven layout without a fixed line cap. Because the current implementation still hard-caps prompt reservation at three lines, the feature-level contract is only partially complete for extreme constrained-width cases beyond the exercised live GUI scenario.

Follow-up feature requests for planning:
- Remove the fixed `ACTION_PROMPT_MAX_LINES` reservation cap from `FBattleDisplay` and make lower-panel prompt reservation, button spacing, and requested height derive entirely from the actual wrapped prompt geometry so very narrow selected-ship move layouts remain non-overlapping while still honoring the `120px` baseline and `60%` map-height floor.

Final outcome:
- CONDITIONAL PASS
