Reviewer Report

Feature plan reviewed:
- plans/phase3-build-system-ci-plan.md (Phase 3 - Build-System Unification + Minimal CI), governing scope/acceptance: subtasks P3-1..P3-4, dependency/parallelization (S4), Documentation Impact (S5), Untouched Files/Constraints (S6), Completion Definition (S8).

Inputs reviewed:
- Delivered subtasks P3-1..P3-4 merged into coordination branch phase3 (reviewed from worktree branch phase3-reviewer-20260712, based on phase3 head 60ecb547).
- Per-subtask stage artifacts: artifacts/phase3-build-system-ci/P3-1|P3-2|P3-3|P3-4/ (implementer/tester/documenter/verifier reports + results); all four verifier verdicts PASS.
- Delivered build/CI/doc surface vs master: common.mk (new), the 6 non-GUI src Makefiles + 4 simple test-lib Makefiles converted to include ../../common.mk, inline -std=c++17 in the 6 wx-heavy/orchestration Makefiles, src/tactical wx-config-derived WX_CXXFLAGS, repo-root Makefile check target, vs2022/SSW.Common.props LanguageStandard pin, .github/workflows/ci.yml, AGENTS.md, doc/synthesized-roadmap.md, artifacts/VS_PROJECT_CHANGES.md.
- Cross-subtask integration validation run in this worktree: make all_clean -> make -> make check on the merged state.

Overall feature completeness:
- COMPLETE. The full Phase 3 feature is delivered and integrates correctly after merge. All four subtasks are present, consistent with the governing plan, and jointly build and pass.
- Cross-subtask integrated build: a full clean make all_clean then make succeeded with zero compile/link errors; every compile line carries strict -std=c++17. The five pure modules (core/strategic/ships/weapons/defenses) compile with no wx-config invocation; src/gui, src/battleSim, tests, and src/tactical carry wx flags as designed.
- Combined make check passed end-to-end and exited 0, running all three suites in sequence with the GUI suite under xvfb-run: SSWTests OK (245 tests), TacticalTests OK (253 tests), GuiTests OK (75 tests) - exactly the expected counts. The build refactor changed no test outcome.
- P3-1 (BS1+BS2-Linux): common.mk holds the wx-free shared base (COVERAGE block, .DEFAULT_GOAL, base CXXFLAGS with -std=c++17, DEPFLAGS, PICFLAGS, suffix implicit rules, shared objs/coverage-clean/clean targets) and a lazily-assigned wx-config-derived WX_CXXFLAGS the base CXXFLAGS never references. It is included by the 6 non-GUI src modules and 4 simple test-lib Makefiles. src/tactical references WX_CXXFLAGS and the hardcoded gtk3-unicode-static-3.3 path is gone. Drift fixed: defenses inherits the corrected rm -f *.gcov via common.mk; tests/core no longer carries -I../../../src/include. Coverage opt-in still works: make COVERAGE=1 emits -fprofile-arcs -ftest-coverage alongside -std=c++17.
- P3-2 (BS2-Windows): vs2022/SSW.Common.props adds exactly one <LanguageStandard>stdcpp17</LanguageStandard> to the shared ClCompile block; <ConformanceMode>false</ConformanceMode> is left unchanged and the C:\Users\steph26\repos\wxWidgets-3.3.1 install-path reference is untouched. MSVC compilation is not verifiable on Linux (structural only, as the plan anticipated).
- P3-3 (BS3): repo-root Makefile check target depends on all, then runs SSWTests, TacticalTests, and GuiTests (under xvfb-run -a) as sequential recipe lines with no leading '-' and no || true, so any suite failure propagates a non-zero exit. check is added to .PHONY.
- P3-4 (BS4): .github/workflows/ci.yml is valid YAML with push+pull_request(master)+workflow_dispatch triggers, a single ubuntu-24.04 job, permissions contents: read at workflow and job level, pinned third-party action SHAs (checkout v4.2.2, cache v4.2.0), no secrets, a wx 3.3.2 source build with actions/cache keyed on the version, and run steps that are command-identical to the local make and make check.
- Documentation (S5): AGENTS.md documents make check, common.mk, the -std=c++17 pin (both platforms), a Continuous Integration note, the src/tactical wx-config carve-out consistently in both the Module Boundary Rules and wxWidgets Integration sections, and refreshes general/Linux prose 3.3.1->3.3.2 while leaving the Windows install path at 3.3.1. doc/synthesized-roadmap.md item #3 is annotated RESOLVED. VS_PROJECT_CHANGES.md records the stdcpp17 pin and the intentional ConformanceMode=false.
- Constraints (S6): zero C++ source/header changes (git diff master..HEAD shows 0 .cpp/.h files); doc/rules/tactical_operations_manual.md and second-sathar-war-rules.md untouched; five pure modules wx-free; ConformanceMode still false; Windows install path unchanged. The build left no tracked file dirty (working tree clean).
- Completion Definition (S8): satisfied as a whole.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- .github/workflows/ci.yml - CI acceptance is structural + command-parity only; GitHub Actions cannot be executed from this environment.
  The workflow parses as valid YAML, carries correct triggers/permissions/pinned SHAs/no-secrets, and its make and make check steps are command-identical to the locally-verified build. This matches the plan's S2c expectation; a real green cloud run should still be observed on the first push/PR to master, and the from-source wx 3.3.2 build (first cold-cache run) may be slow.
- Makefile:24-27 - make check failure propagation was confirmed structurally here, not by a fresh forced-failure run in this review.
  The recipe uses sequential lines with no leading '-' and no || true, so make's default behavior guarantees a non-zero exit on the first failing suite; the successful path was observed to exit 0. The forced-failure/revert demonstration was already performed and recorded by P3-3's Tester/Verifier, so this is informational only.
- src/strategic/FJumpRoute.cpp:129,131 - Pre-existing -Wint-to-pointer-cast warnings persist in untouched source during the build.
  These warnings predate Phase 3 and live in C++ source the plan forbids editing (S6). They are correctly out of scope for this build/CI phase and do not affect the build result; noted only so they are not mistaken for a regression introduced here.

Missed functionality or edge cases:
- No missed functionality, integration gap, or unmet acceptance criterion was found. Every S8 completion item is satisfied and verified against the merged state.
- The one integration risk isolated per-subtask verification could not cover - that the merged combination of common.mk + make check + the CI-referenced commands works together - was explicitly re-validated here and passes (clean build + all three suites green + make check exit 0).
- Two items are structural-only by design, not gaps: MSVC C++17 compilation (P3-2) and the GitHub Actions cloud run (P3-4) cannot execute in this Linux environment; the plan (S2c) scopes their acceptance to XML/YAML correctness plus command-parity, which are met. No green cloud run is claimed.

Follow-up feature requests for planning:
- None

Artifacts written:
- artifacts/phase3-build-system-ci/reviewer_report.md
- artifacts/phase3-build-system-ci/reviewer_result.json

Final outcome:
- PASS
