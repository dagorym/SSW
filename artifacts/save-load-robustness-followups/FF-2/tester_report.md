# Tester Report

Status:
- success

Task summary:
- Validated reviewer follow-up FF-2 (and folded-in FF-3): FVehicle::load() now checks the return of every scalar read and every nested weapon/defense load() call, so a save stream truncated strictly inside a single vehicle's own scalar/weapon/defense region aborts the load (nonzero), and that nonzero return propagates through the FR-1-hardened FFleet::load()/FPlayer::load() up to FGame::load(). Added four behavioral tests: one unit-level FVehicleTest case proving FVehicle::load() aborts on an in-vehicle scalar-region truncation (with getCurrentDefense() left non-dangling for leak/dangle safety), and three full-game FGameSaveFormatTest cases exercising all four FR-1 propagation sites individually (single-ship fleet -> FFleet ship-loop + FPlayer fleet-loop; FPlayer m_unattached loop; FPlayer m_destroyed loop). Each full-game case asserts FGame::load() returns nonzero, RecordingUI::showMessage() is called exactly once, and no player is committed to the live singleton. All four new tests were confirmed to FAIL against the pre-FF-2 FVehicle.cpp (load returned 0 / no abort) and PASS against the shipped fix.

Branch name:
- phase5-FF-2-tester-20260719

Test commit hash:
- 82993ccc

Test files added or modified:
- tests/ships/FVehicleTest.h
- tests/ships/FVehicleTest.cpp
- tests/strategic/FGameSaveFormatTest.h
- tests/strategic/FGameSaveFormatTest.cpp

Commands run:
- make -C tests/ships (clean build of libshipsTests.a, no warnings)
- make -C tests/strategic (clean build of libstrategicTests.a, no warnings)
- make -C tests (top-level build of SSWTests + GuiTests + TacticalTests)
- SSWTests (run from tests/) -> OK (297 tests)
- make check (worktree root) -> exit 0; SSWTests OK (297), TacticalTests OK (253), GuiTests OK (85)
- Behavioral-mandate check: temporarily replaced src/ships/FVehicle.cpp with the pre-FF-2 version (git show da076ba2^:src/ships/FVehicle.cpp), rebuilt src/ships + SSWTests, re-ran -> all four new tests FAILED (load returned 0 / no abort); restored FVehicle.cpp via git checkout (git status shows it unmodified) and rebuilt

Pass/fail totals:
- GuiTests: 85 run / 0 failures
- SSWTests: 297 run / 0 failures (293 baseline + 4 new: 1 FVehicleTest + 3 FGameSaveFormatTest)
- TacticalTests: 253 run / 0 failures

Unmet acceptance criteria:
- None

Final test outcomes:
- make check (worktree root) exit 0: SSWTests OK (297 tests), TacticalTests OK (253 tests), GuiTests OK (85 tests) -- all green, no regressions vs. the pre-FF-2 baseline (293/253/85); the +4 in SSWTests are the four new behavioral cases.
- testLoadReturnsNonzeroWhenTruncatedInsideOwnScalarRegion (tests/ships/FVehicleTest): saves a synthetic vehicle stream (type tag + m_ID + m_name), truncates it before m_iconName's own record can be read, strips the type string the caller (not load()) consumes, calls FVehicle::load() and asserts rc != 0; then asserts getCurrentDefense() != NULL and its type is FDefense::NONE, proving the constructor's default defense is still live (non-dangling) after a failed load.
- testLoadTruncatedInsideFleetShipScalarRegionReturnsNonzeroAndReportsExactlyOnce (tests/strategic/FGameSaveFormatTest, FR-1 site 1: FFleet ship-loop + FPlayer fleet-loop): builds a real init()'d game, adds a fleet with exactly ONE uniquely-named ship to UPF (a single-ship fleet is required so no following ship's missing type tag can trip the pre-existing createShip("")==NULL guard), saves, locates that ship by name, truncates 2 bytes into its m_ID field, and asserts FGame::load() rc != 0, showMessageCalls == 1, getPlayers().size() == 0.
- testLoadTruncatedInsideUnattachedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce (FR-1 site 2: FPlayer m_unattached loop): adds a uniquely-named ship to UPF's m_unattached via addShip(), saves, truncates 2 bytes into that ship's m_ID field, asserts the same abort outcome.
- testLoadTruncatedInsideDestroyedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce (FR-1 site 3: FPlayer m_destroyed loop): adds a uniquely-named ship to UPF's m_destroyed via addDestroyedShip(), saves, truncates 2 bytes into that ship's m_ID field, asserts the same abort outcome.
- Behavioral-test mandate satisfied: with src/ships/FVehicle.cpp reverted to its pre-FF-2 form, all four new tests failed (FGame::load()/FVehicle::load() returned 0 instead of nonzero because every downstream read silently succeeded on the exhausted stream); with the shipped fix all four pass. FVehicle.cpp was restored verbatim afterward (git status confirms only the four test files changed and they are now committed).
- Note: the ships and strategic test modules build only static libraries (libshipsTests.a / libstrategicTests.a) -- there is no standalone ./ShipsTests or ./StrategicTests binary in this repo (unlike the tactical/weapons/gui modules). Those module tests execute via the top-level tests/SSWTests runner, which links libshipsTests.a and libstrategicTests.a and reports OK (297 tests). The tester_prompt's suggested `./ShipsTests`/`./StrategicTests` commands therefore have no matching target; SSWTests is the equivalent execution path.
- Note: run from a working directory other than tests/, the standalone SSWTests binary reports pre-existing FGameHeaderDependencyTest source-inspection failures (those tests read repo source files by relative path). Run from tests/ (as make check does), the full suite is OK (297 tests); this is an unrelated pre-existing CWD artifact.

Cleanup status:
- All temporary byproducts (helper run scripts, the fixed-FVehicle.cpp backup, the pre-FF-2 revert, and the make check log) live in the session scratchpad outside the worktree and are not committed.
- src/ships/FVehicle.cpp was restored verbatim via git checkout after the temporary behavioral-mandate revert; git status shows it unmodified.
- Worktree working tree is clean; the four test files are committed in test commit 82993ccc and the tester artifacts are committed in a separate second commit.

Artifacts written:
- artifacts/save-load-robustness-followups/FF-2/tester_report.md
- artifacts/save-load-robustness-followups/FF-2/tester_result.json
- artifacts/save-load-robustness-followups/FF-2/documenter_prompt.txt
