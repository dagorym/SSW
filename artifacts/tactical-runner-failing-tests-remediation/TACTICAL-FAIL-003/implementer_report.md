Plan step status:
- Preflight scope check: completed (task goal, allowed files, acceptance criteria, and validation commands confirmed).
- Implement incrementally: completed.
- Validate after each change: completed.
- Completion gate: completed with code commit and artifact generation.

Task summary:
- Replaced stale source-token assertions in `WXTacticalUIAdapterTest` with stable adapter checks that avoid implementation text inspection.
- Removed the stale runtime non-rewire assertion.
- Expanded `TacticalGuiLiveTest` to cover current `WXTacticalUI` modal behavior paths for ICM selection, message dialogs, and winner notifications.

Files changed (code commit):
- tests/tactical/WXTacticalUIAdapterTest.h
- tests/tactical/WXTacticalUIAdapterTest.cpp
- tests/gui/TacticalGuiLiveTest.cpp

Validation commands run:
1) cd tests/gui && make && xvfb-run -a ./GuiTests
   - First run: failed (missing modal OK button lookup in new winner/message assertions).
   - After fix: passed (33 tests).
2) cd tests && make gui-tests
   - Passed.
3) cd tests && make tactical-tests gui-tests && ./tactical/TacticalTests
   - Passed.
4) cd tests/tactical && make && ./TacticalTests
   - Passed (125 tests).

Validation outcome:
- Acceptance criteria for TACTICAL-FAIL-003 are implemented.
- Tactical runner failures caused by adapter source-token expectations are eliminated.
- GUI live coverage now validates active WXTacticalUI modal and winner behavior.
