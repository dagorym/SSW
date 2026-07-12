# Implementer Report

Status:
- success

Task summary:
- Add a minimal deterministic RNG seed seam (reviewer follow-up F1) in include/Frontier.h, and switch FGame's constructor in src/strategic/FGame.cpp from raw srand(time(NULL)) to the new clock-based seam, so the flaky FFleetTest::testDecTransitTime (driven by irand(100) in FFleet::decTransitTime()) can be made deterministic by tests. This is the minimal seed seam, not the full injectable-RNG refactor.

Changed files:
- include/Frontier.h
- src/strategic/FGame.cpp

Validation commands run:
- make (repo root)
- cd tests && make && ./SSWTests

Validation outcome:
- Repo-root `make` completed with exit code 0 (builds SSW, BattleSim, and the tests/ tree, including tests/SSWTests). `./SSWTests` run from tests/ with cwd=tests: 243 run, 9 failures, 0 errors -- all 9 failures are pre-existing FGameHeaderDependencyTest source-inspection failures caused by worktree cwd path-candidate resolution (unrelated to this change, matches the task's documented pre-existing-failure note). FFleetTest (including testDecTransitTime) passed in this run. No new failures were introduced. A standalone sanity program (Frontier.h only, outside the test suite) confirmed seedRandomExplicit(N) followed by two independent irand(100) sequences of length 5 are byte-identical for the same seed, and differ for a different seed.

Implementation/code commit hash:
- ec821048

Artifacts written:
- artifacts/phase2-rules-correctness/P2-7/implementer_report.md
- artifacts/phase2-rules-correctness/P2-7/tester_prompt.txt
- artifacts/phase2-rules-correctness/P2-7/implementer_result.json

Implementation context:
- New seam lives in include/Frontier.h immediately after irand(): `inline void seedRandomExplicit(unsigned int seed)` calls srand(seed) directly (explicit-seed entry point for deterministic tests/replay); `inline void seedRandomFromClock()` calls srand((unsigned int)time(NULL)) (clock-based entry point for normal play). Both are thin wraps over the process-global srand()/rand() pair that irand() already uses -- irand() itself is byte-for-byte unchanged.
- include/Frontier.h now also includes <ctime> (in addition to the pre-existing <cstdlib>) to declare time().
- src/strategic/FGame::FGame(IStrategicUI * ui) in src/strategic/FGame.cpp now calls seedRandomFromClock() instead of the raw `srand(time(NULL))` it previously called; behavior for normal play is unchanged (still seeds from wall-clock time once per FGame construction), only the call site is now the shared named seam.
- FFleet::decTransitTime() (src/strategic/FFleet.cpp:98) is unmodified -- it still calls `irand(100) > getRJChance()` for the risk-jump roll. Determinism is obtained purely by seeding the shared process RNG via seedRandomExplicit(seed) before invoking decTransitTime() in a test, since irand() always draws from the same process-global rand() stream.
- Because seedRandomExplicit()/seedRandomFromClock() reseed the single process-global RNG, tests calling seedRandomExplicit() will affect any other irand()-consuming code called afterward in the same process (this is inherent to the minimal seam -- it is not a per-object/injectable RNG). Tests relying on determinism should seed immediately before the RNG-dependent call and should not assume isolation from other RNG consumers running interleaved in the same process.
- Constructing a fresh FGame (e.g. via FGame::create()/FGame::create(ui)) always reseeds the shared RNG from the clock, so any seed pinned by a test before constructing an FGame will be overwritten by that construction; seed after FGame construction (or avoid constructing FGame) if a test needs a pinned seed to survive.
- Doxygen headers were added/updated: full function-level blocks for the two new Frontier.h functions and for FGame::FGame(IStrategicUI*), plus updated file-level @author/@date Last Modified on both include/Frontier.h and src/strategic/FGame.cpp. FGame.h's one-line constructor comment was left untouched since include/strategic/FGame.h is outside this subtask's allowed-file list.

Expected validation failures carried forward:
- tests/strategic/FGameHeaderDependencyTest.cpp -- all 9 tests fail with 'Expected at least one candidate file path to be readable' (forced failure at line 21). These are pre-existing source-inspection tests whose candidate-path resolution is sensitive to the worktree's on-disk layout/cwd and are unrelated to this change; the task prompt documents this as an expected pre-existing worktree artifact. No FFleetTest, FGameTest, or other strategic test failed in this run.
