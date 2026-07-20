# Tester Report

Status:
- success

Task summary:
- FF2-2 (reviewer follow-up FR-C): harden FPlanet::load() against a corrupt/malicious save. The Implementer added return-checking to every own read (m_ID, m_name, stationCount, station type tag), a NULL-check of createShip()'s result before the nested m_station->load(is), and a check of that nested return, replacing the previous unconditional `return 0`. This subtask authored behavioral coverage validating that hardening at both the FPlanet unit level and the full-game FGame::load() aggregate-abort level.

Branch name:
- phase5-FF2-2-tester-20260719

Test commit hash:
- 20a3ebed055652f5998ed3d067d6b6da58230a03

Test files added or modified:
- tests/strategic/FPlanetTest.h
- tests/strategic/FPlanetTest.cpp
- tests/strategic/FGameSaveFormatTest.h
- tests/strategic/FGameSaveFormatTest.cpp

Commands run:
- make -C tests/strategic (compile the strategic test lib)
- make -C tests all (build SSWTests + GuiTests + TacticalTests)
- cd tests && ./SSWTests (fixed code: OK 309 tests)
- make -C <worktree> check (SSWTests OK 309, TacticalTests OK 253, GuiTests OK 85)
- Behavioral-mandate verification: reverted src/strategic/FPlanet.cpp to its pre-FF2-2 form (git show 7261e534^:src/strategic/FPlanet.cpp), rebuilt, and ran SSWTests

Pass/fail totals:
- GuiTests_failed: 0
- GuiTests_passed: 85
- SSWTests_failed: 0
- SSWTests_passed: 309
- TacticalTests_failed: 0
- TacticalTests_passed: 253
- new_tests_added: 9

Unmet acceptance criteria:
- None

Final test outcomes:
- make check green against the fixed code: SSWTests OK (309 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests).
- 9 new behavioral tests added: 8 in FPlanetTest (unit level) + 1 in FGameSaveFormatTest (full-game aggregate abort).
- AC met: FPlanet::load() returns nonzero on a failed own read (m_ID/m_name/stationCount/type) -- FPlanetTest::testLoadReturnsNonzeroOnTruncatedID / ...Name / ...StationCount / ...StationTypeTag.
- AC met: unknown station type (createShip()==NULL, no NULL virtual-call) returns nonzero and leaves m_station NULL -- FPlanetTest::testLoadReturnsNonzeroOnUnknownStationType (asserts rc!=0 AND getStation()==NULL).
- AC met: a failed nested m_station->load() returns nonzero -- FPlanetTest::testLoadReturnsNonzeroOnTruncatedStationRecord.
- AC met: a save whose planet carries an unknown/corrupt station type makes FGame::load() return nonzero, report via mock IStrategicUI exactly once, and commit no live singleton (getPlayers().size()==0) -- FGameSaveFormatTest::testLoadPlanetWithUnknownStationTypeReturnsNonzeroAndReportsExactlyOnce (previously a NULL virtual-call crash).
- AC met: a valid save (planets with and without stations) loads unchanged -- FPlanetTest::testLoadValidSaveWithStationReturnsZeroAndPreservesData / testLoadValidSaveWithoutStationReturnsZeroAndPreservesData, plus the pre-existing FGameSaveFormatTest valid-save round-trip tests still green.
- AC met: Doxygen already updated on FPlanet::load() by the Implementer (include/strategic/FPlanet.h) -- verified present; not a tester-owned file.
- Behavioral mandate confirmed against unfixed FPlanet.cpp: the full SSWTests run SEGFAULTS (exit 139) due to the NULL virtual-call in the unknown-station-type path (crash-class new tests fail/crash on unfixed); with the crash-inducing tests temporarily disabled, FPlanetTest::testLoadReturnsNonzeroOnTruncatedStationRecord FAILS on unfixed (FPlanet::load returns 0, assertion at FPlanetTest.cpp line 236) and passes on the fix. All new tests pass against the fix.
- TESTER FINDING (acceptance-criteria nuance): the acceptance criterion 'A save truncated strictly inside a planet's station record makes FGame::load() return nonzero (previously silent return 0)' holds at the FGame aggregate level on BOTH the fixed and unfixed code -- on the unfixed code FPlanet::load() swallows the truncated nested-load failure and returns 0, but the shortened stream is then caught by FSystem::load()'s and FMap::load()'s own already-checked reads upstream, so FGame::load() aborts nonzero regardless. A full-game truncated-station test therefore cannot discriminate the FPlanet::load() fix and would violate the 'each new test must fail against unfixed' mandate, so it was intentionally NOT added; the truncated-station-record behavior is instead covered by a fix-discriminating unit test (FPlanetTest::testLoadReturnsNonzeroOnTruncatedStationRecord). This deviation from the literal test-plan list is documented in an in-source NOTE in FGameSaveFormatTest.cpp and in the class doc.

Cleanup status:
- Restored src/strategic/FPlanet.cpp to the committed fixed version after behavioral-mandate verification (git status shows it unmodified).
- Temporary backups and captured run logs were written only under the session scratchpad (outside the worktree); no temporary byproducts remain in the worktree.
- Working tree clean after both commits.

Artifacts written:
- artifacts/save-load-robustness-followups-2/FF2-2/tester_report.md
- artifacts/save-load-robustness-followups-2/FF2-2/tester_result.json
- artifacts/save-load-robustness-followups-2/FF2-2/documenter_prompt.txt
