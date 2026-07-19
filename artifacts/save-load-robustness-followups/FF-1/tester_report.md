# Tester Report

Status:
- success

Task summary:
- Validated FF-1 (reviewer follow-up / SF-located-object-ids): FGame::load() now rejects, at load time, a save whose fleet carries an out-of-range located-object reference ID (a location/system ID or jump-route ID that does not resolve against the already-loaded FMap), aborting via the existing reportLoadError()->IStrategicUI::showMessage() aggregate-abort path (the freshly-allocated FPlayer is deleted and load() returns nonzero, committing no live singleton). Added three behavioral tests to the existing tests/strategic/FGameSaveFormatTest suite: an out-of-range location-ID abort, an out-of-range jump-route-ID abort, and a positive control proving a valid save with the documented sentinels (location 0, jump-route FFleet::NO_ROUTE) still loads cleanly. Both negative tests were confirmed to fail against the unfixed code (FF-1 block temporarily disabled -> load returns 0) and pass against the shipped fix; the positive control passes in both states.

Branch name:
- phase5-FF-1-tester-20260719

Test commit hash:
- 4f78c7a0

Test files added or modified:
- tests/strategic/FGameSaveFormatTest.h
- tests/strategic/FGameSaveFormatTest.cpp

Commands run:
- make -C tests/strategic (clean build of libstrategicTests.a, no warnings)
- make -C tests (top-level build of SSWTests + GuiTests + TacticalTests)
- ./SSWTests (run from tests/) -> OK (293 tests)
- ./TacticalTests (run from tests/tactical/) -> OK (253 tests)
- make check (top-level, from worktree root) -> exit 0; SSWTests OK (293), TacticalTests OK (253), GuiTests OK (85)
- Behavioral-mandate check: temporarily disabled the FF-1 validation block in src/strategic/FGame.cpp, rebuilt src/strategic + SSWTests, re-ran -> the two out-of-range tests FAILED (load returned 0); restored FGame.cpp verbatim (git status shows it unmodified) and rebuilt

Pass/fail totals:
- GuiTests: 85 run / 0 failures
- SSWTests: 293 run / 0 failures (290 baseline + 3 new)
- TacticalTests: 253 run / 0 failures

Unmet acceptance criteria:
- None

Final test outcomes:
- make check (top-level) exit 0: SSWTests OK (293 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests) -- all green, no regressions vs. the pre-FF-1 baseline (290/253/85); the +3 in SSWTests are the three new FGameSaveFormatTest cases.
- testLoadFleetWithOutOfRangeLocationIdReturnsNonzeroAndReportsExactlyOnce is fully behavioral: it builds a real valid FGame save via buildValidSaveBytes(), locates UPF's placed 'Task Force Prenglar' fleet by its length-prefixed name tag, asserts the serialized m_location currently decodes to a real nonzero system ID, then byte-patches those 4 bytes to little-endian 0x0000BEEF (not 0, not any loaded system ID), calls the real FGame::load(), and asserts rc != 0, mock IStrategicUI showMessageCalls == 1, and getPlayers().size() == 0 (no live game committed).
- testLoadFleetWithOutOfRangeJumpRouteIdReturnsNonzeroAndReportsExactlyOnce leaves m_location valid so the location check passes, asserts the serialized m_jumpRouteID currently equals FFleet::NO_ROUTE, then byte-patches it (17 bytes past m_location, per FFleet::save()'s field order) to little-endian 0x0000BEEF (not NO_ROUTE, not any loaded route ID), and asserts the same abort outcome (rc != 0, one report, zero players committed).
- testLoadValidSaveWithSentinelLocationAndJumpRouteSucceeds is the positive control: an unmodified buildValidSaveBytes() -> load() round trip (Strike Force Nova at location 0, all placed fleets on NO_ROUTE) asserts rc == 0 and showMessageCalls == 0, proving the sentinels remain exempt from FF-1's resolution check.
- Behavioral-test mandate satisfied: with the FF-1 validation block in FGame::load() temporarily disabled, exactly the two out-of-range tests failed (load returned 0 instead of nonzero) while the positive control still passed; with the shipped fix all three pass. FGame.cpp was restored verbatim afterward (git status confirms only the two test files changed).
- Note: the standalone SSWTests binary also reports 9 FGameHeaderDependencyTest source-inspection failures when the binary is invoked with a working directory other than tests/ (those tests read repository source files by relative path). Run from tests/ (as make check does), the full suite is OK (293 tests); these 9 are a pre-existing CWD artifact unrelated to FF-1 and not present under make check.

Cleanup status:
- All temporary byproducts (helper run scripts, the FGame.cpp behavioral-mandate backup) live in the session scratchpad outside the worktree and are not committed. The worktree working tree is clean except for the committed test files; src/strategic/FGame.cpp was restored verbatim after the temporary behavioral-mandate revert (git status shows it unmodified).

Artifacts written:
- artifacts/save-load-robustness-followups/FF-1/tester_report.md
- artifacts/save-load-robustness-followups/FF-1/tester_result.json
- artifacts/save-load-robustness-followups/FF-1/documenter_prompt.txt
