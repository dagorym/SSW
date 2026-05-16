Implementer Report — layout-update-implementer-20260516

Plan step status:
- Preflight scope check: completed
- Implementation: completed
- Validation: completed with pre-existing unrelated failures
- Completion gate prep: completed

Summary of implementation:
- Updated `FBattleScreen` default size from `750x550` to `1200x900`.
- Added explicit tactical screen layout policy constants in `FBattleScreen.cpp` for a `120px` baseline lower panel and `60%` minimum map-height floor.
- Added `applyLayoutPolicy()` and resize handling (`onSize`) so the parent layout allocates the lower `FBattleDisplay` row at `120px` when possible, honors larger display-height requests when observed, and clamps lower-panel height so the map remains at least 60% of client height.
- Kept tactical rules/state behavior unchanged (layout-only edits in `FBattleScreen`).

Files changed:
- include/tactical/FBattleScreen.h
- src/tactical/FBattleScreen.cpp

Validation commands run:
1) `cd tests && make tactical-tests && ./tactical/TacticalTests` (baseline)
   - Outcome: FAIL (pre-existing)
   - Details: 3 failures in `FTacticalBattleDisplayFireFlowTest` expecting missing `FBattleDisplay.cpp` source tokens (`Movement Done` prompt text and prompt-spacing helper tokens). Unrelated to FBattleScreen layout scope.
2) `cd tests/gui && make && xvfb-run -a ./GuiTests` (baseline)
   - Outcome: FAIL (pre-existing)
   - Details: at least one pre-existing source-token failure in `TacticalGuiLiveTest::testTacticalActionButtonsStayBelowPromptReservationAcrossPhases` expecting the same `FBattleDisplay.cpp` movement prompt token.
3) `cd tests && make tactical-tests && ./tactical/TacticalTests` (post-change)
   - Outcome: FAIL (same pre-existing failures)
   - Details: unchanged 3 failures in `FTacticalBattleDisplayFireFlowTest`, all in `FBattleDisplay.cpp` token assertions.
4) `cd tests/gui && make && xvfb-run -a ./GuiTests` (post-change)
   - Outcome: FAIL (same pre-existing failure)
   - Details: unchanged `TacticalGuiLiveTest` source-token failure against `FBattleDisplay.cpp` movement prompt text.

Validation triage classification:
- Classification: expected pre-existing failure, not a regression from this implementation.
- Evidence: failures target source-token expectations in `src/tactical/FBattleDisplay.cpp`; this task changed only `include/tactical/FBattleScreen.h` and `src/tactical/FBattleScreen.cpp`.
- Tester follow-up: confirm layout acceptance criteria in FBattleScreen while treating existing FBattleDisplay token failures as baseline unless behavior outside approved scope changed.

Implementation/code commit:
- 4d8cb29beb1bd94fc84cd394bb9686adcb845007

Artifacts written:
- artifacts/tactical-movement-prompt-overlap-plan/tmp-1/implementer_report.md
- artifacts/tactical-movement-prompt-overlap-plan/tmp-1/tester_prompt.txt
- artifacts/tactical-movement-prompt-overlap-plan/tmp-1/implementer_result.json
