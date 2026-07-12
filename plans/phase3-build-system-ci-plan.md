# Phase 3 — Build-System Unification + Minimal CI — Implementation Plan

**Author:** Planner (Claude Opus 4.8 (1M context), high)
**Date:** 2026-07-12
**Source roadmap:** `doc/synthesized-roadmap.md` Part III "Now" item #3 (line 700: "`make check` target + minimal CI; pin `-std=c++17` both platforms; common.mk"), grounded in §3.7 (build-system & test findings, lines 440–472) and Cross-Cutting initiative #7 (line 515).
**Scope chosen by user:** the roadmap "Now #3" build/CI hardening bundle only. No game behavior, no rules logic, no refactors of C++ sources. All material design decisions were resolved with the user during planning (see §2b).

**Output Artifact Path:** `plans/phase3-build-system-ci-plan.md` (this file, repository-root-relative).

---

## 1. Feature Restatement (engineering terms)

Four coupled build-infrastructure deliverables. None change compiled runtime behavior; all reduce build drift, pin the language standard, make the test suites runnable as one command, and add automated CI.

| ID | Deliverable | Primary surface |
|----|-------------|-----------------|
| BS1 | **`common.mk`** — a repository-root shared Make fragment that collapses the near-identical Makefiles and fixes their accumulated drift | new `common.mk` + module Makefiles |
| BS2 | **Pin `-std=c++17` on both platforms** — currently pinned nowhere (Linux rides GCC's `gnu++17` default; MSVC defaults to C++14) | `common.mk`/Makefiles (Linux) + `vs2022/SSW.Common.props` (Windows) |
| BS3 | **`make check`** — a repo-root target that builds and runs all three test runners so "run all tests" stops being a fiction | repo-root `Makefile` |
| BS4 | **Minimal CI** — a GitHub Actions workflow that builds and runs `make check` on Linux | new `.github/workflows/ci.yml` |

**Why these are one phase:** BS1 and BS2(Linux) both edit the same Makefiles and are applied and verified by a single clean build; BS3 and BS4 consume the result. They are decomposed below into four subtasks with (mostly) disjoint file ownership so three can run in parallel.

---

## 2. Confirmed Repository Facts

All verified directly against the working tree during planning:

- **No standard is pinned anywhere.** A repo-wide search for `-std=`, `-std=gnu++`, and `<LanguageStandard>` across `src/`, `tests/`, `Makefile`, and `vs2022/` returns **zero** hits. Linux therefore compiles at GCC's default (`gnu++17` on current GCC); MSVC defaults to C++14.
- **No CI exists.** `.github/` contains only `copilot-instructions.md`; there is no `.github/workflows/` directory.
- **The 6 non-GUI src module Makefiles are byte-near-identical.** `src/{core,strategic,ships,weapons,defenses}/Makefile` share `CXXFLAGS = -I../../include -g -Wall -Woverloaded-virtual -DLINUX $(COVERAGE_FLAGS)` (no `wx-config`), `DEPFLAGS = -MMD -MP`, `PICFLAGS`, the `COVERAGE`/`.DEFAULT_GOAL` block, the suffix-rule implicit rules, and `clean`/`coverage-clean`/`objs` targets. They differ only in `TARGET = lib<module>.a`.
- **`src/tactical/Makefile` is the one non-GUI module that needs wx include paths.** It adds `WX_CXXFLAGS ?= -I/usr/local/lib/wx/include/gtk3-unicode-static-3.3 -I/usr/local/include/wx-3.3 -D_FILE_OFFSET_BITS=64 -D__WXGTK__ -pthread $(shell pkg-config --cflags gtk+-3.0)` and appends it to `CXXFLAGS`. The hardcoded default is machine-specific and encodes build-config assumptions (the `-static-` config dir, the `3.3` version).
- **The installed wx is 3.3.2 and `wx-config` reports it correctly.** Verified live during planning: `wx-config --version` → `3.3.2`; `wx-config --selected-config` → `gtk3-unicode-static-3.3`; `wx-config --cxxflags` → `-I/usr/local/lib/wx/include/gtk3-unicode-static-3.3 -I/usr/local/include/wx-3.3 -D_FILE_OFFSET_BITS=64 -D__WXGTK__ -pthread` — i.e. **byte-identical to the tactical hardcode** (minus the separately-appended gtk `pkg-config` cflags). The 3.3.1→3.3.2 bump changed none of these paths (they are major.minor + build-config keyed, not patch keyed), so the Linux build is unaffected by the version bump either way — but `wx-config` also absorbs any future static→shared, 3.3→3.4, or prefix change that the hardcode would not.
- **`wx-config` usage is confined to gui-facing Makefiles:** `src/gui/Makefile`, `src/battleSim/Makefile`, `src/Makefile` (executables), `tests/Makefile` (SSWTests), and each `tests/<module>/Makefile`. AGENTS.md forbids active `wx-config` in the six non-GUI src module Makefiles only.
- **Windows edit point is single.** All **10** vcxproj under `vs2022/` import `SSW.Common.props` (confirmed by grep). `SSW.Common.props` has `<ConformanceMode>false</ConformanceMode>` (line 36) and **no** `<LanguageStandard>` element, so every project inherits MSVC's C++14 default.
- **Known Makefile drift to fix (from §3.7):**
  - `src/defenses/Makefile:64` has `rm  -f*.gcov` (missing space → a no-op that never deletes `.gcov` files).
  - `tests/core/Makefile:19` carries a stale, non-existent include path `-I../../../src/include` (resolves outside the repo).
  - `tests/gui/Makefile` and `tests/tactical/Makefile` use `.RECIPEPREFIX = >` (tab-free recipes) while all others use tab recipes — two recipe styles coexist.
- **The three runners that together cover the whole suite:**
  - `tests/SSWTests` — links `libcoreTests + libweaponsTests + libstrategicTests + libshipsTests` (core, weapons, strategic, ships). Built by `cd tests && make`.
  - `tests/tactical/TacticalTests` — the tactical fixtures (SSWTests registers only 4 of ~25). Built by `cd tests && make tactical-tests` (or `cd tests/tactical && make`).
  - `tests/gui/GuiTests` — the GUI fixtures; **needs an X display**. Built by `cd tests && make gui-tests`; run under `xvfb-run -a`.
  - `defenses` has **no** `tests/defenses` module (nothing to run there).
- **Build orchestration already in place:** repo-root `Makefile` `all` runs `$(MAKE) -C src all` then `$(MAKE) -C tests all SKIP_MODEL_DEPS=1`; `tests/Makefile` `all` depends on `gui-tests` and `tactical-tests`, so a single `make all` at repo root builds the SSW/BattleSim executables **and** all three test runners. The repo-root `Makefile` itself contains **no** compile commands / `CXXFLAGS` (pure delegation).
- **Build/test entry points (AGENTS.md):** repo-root `make`; `cd tests && make && ./SSWTests`; `cd tests/tactical && make && ./TacticalTests`; canonical headless GUI: `cd tests/gui && make && xvfb-run -a ./GuiTests`. Dependencies: wxWidgets **3.3.2** (installed) and CppUnit; `xvfb-run` for headless GUI. (AGENTS.md/CLAUDE.md prose still says "3.3.1" — a stale version string to refresh; see §5.)

## 2b. Design Decisions Resolved With User (binding)

1. **common.mk scope — "Moderate."** `common.mk` factors out the wx-**free** common base (include path, `-g`, `-Wall -Woverloaded-virtual -DLINUX`, the `COVERAGE`/`.DEFAULT_GOAL` block, `-std=c++17`, `DEPFLAGS`, `PICFLAGS`, the suffix implicit rules, and the shared `clean`/`coverage-clean`/`objs` targets). It is **included by** the 6 non-GUI src module Makefiles **and** the 4 simple module-test-lib Makefiles (`tests/{core,weapons,strategic,ships}/Makefile`). The wx-heavy / orchestration / runner Makefiles (`src/Makefile`, `src/gui`, `src/battleSim`, `tests/Makefile`, `tests/gui`, `tests/tactical`) keep their current form but receive the `-std=c++17` flag inline. wx users append their own `wx-config`/`WX_CXXFLAGS`; `common.mk` never contains `wx-config`, preserving the non-GUI-module boundary rule.
2. **`make check` composition — "All three under xvfb."** `make check` builds everything and runs `SSWTests`, `TacticalTests`, and `GuiTests`, with `GuiTests` wrapped in `xvfb-run -a`. It must propagate a non-zero exit if any suite fails.
3. **CI scope — "Linux, full check."** One Ubuntu GitHub Actions job: provision wxWidgets 3.3.2 + CppUnit + xvfb + GTK3 dev, build the repo, and run the full `make check` (including `GuiTests` under xvfb). No Windows job (Windows CI is a roadmap "revisit later" item).
4. **Linux dialect — "`-std=c++17` strict."** Pin strict `-std=c++17` on Linux to mirror the MSVC `stdcpp17` pin. If strict mode breaks the wx/GTK build, the Implementer must **STOP and consult the user** rather than silently downgrading to `-std=gnu++17`.
5. **Tactical wx flags — "`wx-config`-derived."** `common.mk` defines `WX_CXXFLAGS` by shelling out to `wx-config` (lazily) rather than hardcoding a path; `src/tactical` references that variable, so its wx include flags auto-adapt to the installed wx (version, prefix, static-vs-shared). This is an intentional, **user-approved deviation** from the current AGENTS.md/CLAUDE.md rule that forbids `wx-config` in the six non-GUI module Makefiles; Phase 3 therefore **amends that rule** to carve out `src/tactical`'s use of the shared `WX_CXXFLAGS` so policy and code stay consistent. The five genuinely-pure modules (`core/strategic/ships/weapons/defenses`) still never invoke `wx-config`.
6. **wxWidgets 3.3.2.** Target the installed wxWidgets **3.3.2** throughout (CI provisioning and doc prose); Linux include paths are unchanged from 3.3.1 (major.minor-keyed).

## 2c. Assumptions (labeled; not repository-confirmed)

- **(BS2-Windows minimal change)** The Windows pin adds `<LanguageStandard>stdcpp17</LanguageStandard>` to the shared `ClCompile` block in `SSW.Common.props` and **leaves `<ConformanceMode>false</ConformanceMode>` unchanged**. The roadmap asked only to pin the standard; flipping conformance mode is a stricter, separately-risky change with no ability to compile-test it from this Linux environment, so it is out of scope. (Documented, not silently dropped.)
- **(BS2-Windows unverifiable here)** The MSVC/vcxproj change **cannot be compiled or run from this Linux environment.** Its acceptance is therefore limited to XML correctness (the element is valid MSBuild, lands in the shared props inherited by all 10 projects, and does not affect the Linux build). Actual MSVC compilation is validated by the user on Windows or a future Windows CI job.
- **(BS1 `WX_CXXFLAGS` derivation — resolved, see §2b decision 5)** `common.mk` defines `WX_CXXFLAGS` **lazily** as `$(shell wx-config --cxxflags)` plus the gtk cflags the tactical build currently adds (`$(shell pkg-config --cflags gtk+-3.0)`), reproducing tactical's current effective wx+gtk include set with no regression. Because the assignment is lazy (`=`, not `:=`), `wx-config` is invoked **only** when `$(WX_CXXFLAGS)` is expanded — which only `src/tactical` (among the non-GUI modules) does. The five pure modules include `common.mk` but never reference `WX_CXXFLAGS`, so `wx-config` never runs for them and they remain genuinely wx-free at build time. `src/tactical/Makefile` itself contains no `wx-config` string (it lives in the shared fragment). `gui`/`battleSim`/tests may keep calling `wx-config` directly as they do today (no need to churn them onto the shared variable).
- **(BS4 wx 3.3.2 provisioning)** wxWidgets 3.3.2 is very new and likely not an Ubuntu apt package on GitHub runners, so CI will most likely **build wx 3.3.2 from source** (with an actions cache keyed on the wx version) before building SSW. The Implementer selects the concrete provisioning method (distro package if one exists on the chosen runner image and reports 3.3.x, else source build) and documents it; the acceptance below is expressed independent of which method is used.
- **(BS4 not runnable here)** GitHub Actions cannot be executed from this environment. The workflow's acceptance is structural (valid workflow YAML, correct triggers/steps) plus the fact that every command it runs (`make`, `make check`) is identical to the locally-verified commands.

---

## 3. Subtasks

Stable IDs `P3-1`…`P3-4`. **None require the specialist Security stage** (build configuration and CI wiring; no product trust boundary, no secret handling, no untrusted-input path). P3-4 carries build-only CI-hygiene acceptance criteria (minimal token permissions, pinned actions, no secrets) but does not need the Security specialist.

### P3-1 — `common.mk` + Linux `-std=c++17` pin + Makefile drift fixes (BS1 + BS2-Linux)
- **Allowed files (edit/create only these):**
  - `common.mk` (new, repo root)
  - `src/core/Makefile`, `src/strategic/Makefile`, `src/ships/Makefile`, `src/weapons/Makefile`, `src/defenses/Makefile`, `src/tactical/Makefile`
  - `src/gui/Makefile`, `src/battleSim/Makefile`, `src/Makefile`
  - `tests/core/Makefile`, `tests/weapons/Makefile`, `tests/strategic/Makefile`, `tests/ships/Makefile`
  - `tests/Makefile`, `tests/gui/Makefile`, `tests/tactical/Makefile`
  - **Not** the repo-root `Makefile` (owned by P3-3).
- **Task:**
  1. **Create `common.mk`** holding the wx-free common base: the `COVERAGE`/`COVERAGE_FLAGS`/`COVERAGE_PREP` block, `.DEFAULT_GOAL := all`, a base `CXXFLAGS` of `-I../../include -g -Wall -Woverloaded-virtual -DLINUX -std=c++17 $(COVERAGE_FLAGS)`, `DEPFLAGS = -MMD -MP`, `PICFLAGS`, the `.SUFFIXES` + `.c.o`/`.cpp.o`/`.cxx.o`/`.rsrc.r` implicit rules, and the shared `objs`/`coverage-clean`/`clean` targets. It **also defines** `WX_CXXFLAGS` **lazily** (recursive `=`, not `:=`) as `wx-config`-derived flags — `$(shell wx-config --cxxflags) $(shell pkg-config --cflags gtk+-3.0)` — reproducing `src/tactical`'s current effective wx+gtk include set. Lazy assignment ensures `wx-config` runs only when a Makefile actually expands `$(WX_CXXFLAGS)`. The **base `CXXFLAGS` must not reference `WX_CXXFLAGS`**, so the five pure modules that include `common.mk` never trigger `wx-config`.
  2. **Convert to `include ../../common.mk`:** the 6 non-GUI src module Makefiles and the 4 simple test-lib Makefiles (`tests/{core,weapons,strategic,ships}`). Each keeps only its own `TARGET`, `SRCS`/`OBJS`/`DEPS`, the `all $(TARGET): … ar rcs` rule, and `-include $(DEPS)`. Preserve each module's exact behavior: `tests/*` append `` `wx-config --cxxflags` `` and their `-D<MODULE>_TEST_REPO_ROOT=…` define and `-lcppunit`/wx libs, and use the `filter-out test_template.cpp` source glob. **`src/tactical` references the shared `WX_CXXFLAGS`** (`CXXFLAGS += $(WX_CXXFLAGS)`) instead of hardcoding a path — remove the machine-specific hardcoded include path entirely. The five pure modules (`core/strategic/ships/weapons/defenses`) reference neither `wx-config` nor `WX_CXXFLAGS`. **This `wx-config`-in-tactical derivation is a user-approved deviation from the current AGENTS.md/CLAUDE.md no-`wx-config`-in-the-six-modules rule; this subtask's Documenter stage amends that rule (see Documentation Impact). Proceed — do not treat it as a violation.**
  3. **Add `-std=c++17` inline** (no common.mk include) to the remaining compiling Makefiles that keep their form: `src/Makefile`, `src/gui/Makefile`, `src/battleSim/Makefile`, `tests/Makefile`, `tests/gui/Makefile`, `tests/tactical/Makefile`. The flag value must be identical everywhere (strict `-std=c++17`).
  4. **Fix drift:** correct `src/defenses/Makefile`'s `rm  -f*.gcov` → `rm -f *.gcov`; remove the stale `-I../../../src/include` from `tests/core/Makefile`.
  5. Preserve every existing target name, the `SKIP_MODEL_DEPS` mechanism, the `.RECIPEPREFIX = >` style in `tests/gui`/`tests/tactical`, coverage opt-in behavior, and the `-D*_TEST_REPO_ROOT` defines.
  6. **If strict `-std=c++17` breaks the wx/GTK build, STOP and consult the user** (per §2b decision 4); do not silently switch to `-std=gnu++17`.
- **Acceptance criteria (observable):**
  - `common.mk` exists at repo root and is `include`d by the 6 non-GUI src module Makefiles and the 4 simple test-lib Makefiles.
  - A clean `make all_clean` then `make` from the repo root builds `SSW`, `BattleSim`, and all three test runners with **no** errors, and every compile command shows `-std=c++17` (verifiable via `make -n` / `V=1`-style dry run or by inspecting a compile line).
  - `cd tests && make && ./SSWTests` passes; `cd tests/tactical && make && ./TacticalTests` passes; `cd tests/gui && make && xvfb-run -a ./GuiTests` passes — i.e. the refactor changed no test outcome.
  - `src/tactical`'s wx include flags derive from `wx-config` via `common.mk`'s `WX_CXXFLAGS` — the machine-specific hardcoded `/usr/local/lib/wx/include/gtk3-unicode-static-3.3` path no longer appears in `src/tactical/Makefile`.
  - The five pure modules stay wx-free: none of `src/{core,strategic,ships,weapons,defenses}/Makefile` contains `wx-config`, and none references `WX_CXXFLAGS` (so building them shells out to `wx-config` zero times — confirmable via a dry run).
  - `grep -n "gcov" src/defenses/Makefile` shows the corrected `rm -f *.gcov`; `tests/core/Makefile` no longer contains `../../../src/include`.
  - Coverage still opts in: `make COVERAGE=1 tests` compiles with `-fprofile-arcs -ftest-coverage`.
- **Documentation Impact:** Comments at the top of `common.mk` explaining its role, what each including Makefile must still define, and that `WX_CXXFLAGS` is `wx-config`-derived and lazy. No Doxygen (no C++ functions touched). **Required canonical-doc amendment (Documenter stage):** update the AGENTS.md/CLAUDE.md build-system + module-boundary text that forbids `wx-config` in the six non-GUI module Makefiles to carve out `src/tactical`'s use of the shared `WX_CXXFLAGS` (the five pure modules remain forbidden from `wx-config`); refresh the general/Linux "wxWidgets 3.3.1" prose to **3.3.2** (but leave the Windows `wxWidgets-3.3.1` install-path reference as-is — see §5); and note `common.mk` as the shared Make fragment. This amendment must land within this subtask so the documented policy matches the code before the Verifier/Reviewer pass. See §5.
- **Security review:** not required.
- **Dependencies:** none. Parallelizable with P3-2 and P3-3 (disjoint files). **Must land before P3-4 is validated** (CI builds via these Makefiles).

### P3-2 — Windows `-std=c++17` pin (BS2-Windows)
- **Allowed files:** `vs2022/SSW.Common.props`
- **Task:** Add `<LanguageStandard>stdcpp17</LanguageStandard>` to the shared `<ItemDefinitionGroup>/<ClCompile>` block in `SSW.Common.props` (the block already carrying `<ConformanceMode>false</ConformanceMode>`), so all 10 importing projects compile as C++17. **Leave `<ConformanceMode>false</ConformanceMode>` unchanged** (§2b assumption). Do not touch any individual `.vcxproj`, the Linux Makefiles, or any C++ source.
- **Acceptance criteria (observable):**
  - `SSW.Common.props` contains exactly one `<LanguageStandard>stdcpp17</LanguageStandard>` in the shared `ClCompile` item-definition block, and the file remains well-formed MSBuild XML (parses without error).
  - All 10 vcxproj still import `SSW.Common.props` (import lines unchanged), so every project inherits the pin.
  - The change is Windows-only: it introduces no new element outside `SSW.Common.props` and does not alter or break the Linux `make` build.
  - *(Note recorded in the artifact:* MSVC compilation is **not** verifiable in this Linux environment; the Implementer states this explicitly and does not claim a Windows build was run.)
- **Documentation Impact:** Optional one-line note in `artifacts/WXWIDGETS_UPGRADE_CHANGES.md` or `artifacts/VS_PROJECT_CHANGES.md` recording the standard pin (Documenter stage may place it); no source doc changes.
- **Security review:** not required.
- **Dependencies:** none. Fully parallelizable (disjoint file from every other subtask). Not required by P3-4 (CI is Linux-only).

### P3-3 — `make check` target (BS3)
- **Allowed files:** `Makefile` (repo root)
- **Task:** Add a `.PHONY` `check` target (and add `check` to the existing `.PHONY` line) to the repo-root `Makefile` that:
  1. Builds everything first (depend on / invoke the existing `all`, which already builds the SSW/BattleSim executables **and** all three test runners via `src` + `tests` with `SKIP_MODEL_DEPS=1`).
  2. Runs the three suites in sequence: `tests/SSWTests`, `tests/tactical/TacticalTests`, and `tests/gui/GuiTests` under `xvfb-run -a` (the Implementer chooses whether to `cd` into each runner's directory; if a runner is normally invoked from a specific directory, mirror that).
  3. **Propagates failure:** if any suite returns non-zero, `make check` must exit non-zero (no leading `-` on recipe lines, no `|| true`, no swallowing). The GUI step uses `xvfb-run -a` so it is headless-capable.
  - Keep the target self-contained in the repo-root `Makefile`; do not edit `tests/Makefile` (owned by P3-1) — drive the existing `tests` sub-targets instead.
- **Acceptance criteria (observable):**
  - From a clean tree, `make check` builds all three runners and executes all three suites, with the GUI suite running under `xvfb-run`.
  - When all suites pass, `make check` exits `0`.
  - When any single suite fails (demonstrable by temporarily forcing one assertion to fail, then reverting), `make check` exits non-zero — failure is not masked.
  - `make check` requires no arguments and needs only the documented deps (wxWidgets 3.3.2, CppUnit, xvfb) already required to build/test the repo.
- **Documentation Impact:** The new `make check` command is a canonical build/test entry point → the Documenter stage adds it to the AGENTS.md "Running Tests" section (see §5). No Doxygen.
- **Security review:** not required.
- **Dependencies:** none for editing (disjoint file — repo-root `Makefile` is touched by no other subtask). Parallelizable with P3-1/P3-2. **P3-4 consumes this target**, so P3-4 is validated after P3-3.

### P3-4 — Minimal GitHub Actions CI (BS4)
- **Allowed files:** `.github/workflows/ci.yml` (new). *(Create `.github/workflows/` as needed. If a small helper script is genuinely required for wx provisioning, it may live under `.github/` and must be listed in the Implementer's artifact; prefer inline workflow steps.)*
- **Task:** Add a minimal GitHub Actions workflow that, on `push` and `pull_request` to `master` (plus `workflow_dispatch`), runs one Ubuntu job that:
  1. Checks out the repo.
  2. Installs build dependencies: a C++ toolchain, CppUnit dev, GTK3 dev, `xvfb`, and **wxWidgets 3.3.2** — using a distro package if the chosen runner image provides 3.3.x, otherwise building wx 3.3.2 from source with an `actions/cache` step keyed on the wx version. Document the chosen approach in a workflow comment.
  3. Builds the repo (`make`).
  4. Runs `make check` (the P3-3 target; GUI suite runs under `xvfb-run`, already wired inside `check`).
  - **CI hygiene:** set job/workflow `permissions:` to the minimum needed (`contents: read`); pin any third-party actions to a specific version/tag (or commit SHA); use **no** repository secrets.
- **Acceptance criteria (observable):**
  - `.github/workflows/ci.yml` exists and is valid workflow YAML (parses; e.g. `python -c "import yaml,sys; yaml.safe_load(open('.github/workflows/ci.yml'))"` succeeds, or `actionlint` passes if available).
  - The workflow declares `push` + `pull_request` (targeting `master`) and `workflow_dispatch` triggers, a single Ubuntu job, and steps that install the deps (including a wx 3.3.2 provisioning step), run `make`, and run `make check`.
  - The workflow sets `permissions: contents: read`, pins third-party actions to a fixed ref, and references no secrets.
  - The build/test commands in the workflow are exactly the locally-verified `make` / `make check` commands (no CI-only divergent build path).
  - *(Note recorded in the artifact:* GitHub Actions cannot be executed from this environment; acceptance is structural + command-parity, not a green cloud run. The Implementer states this explicitly.)*
- **Documentation Impact:** The Documenter stage notes the new CI in AGENTS.md (a short "Continuous Integration" mention under Build System/Testing) and annotates the roadmap item (see §5). No Doxygen.
- **Security review:** not required (build-only CI, no secrets, read-only token). CI-hygiene criteria above are part of P3-4's own acceptance.
- **Dependencies:** **Runs last.** Logically depends on **P3-1** (the workflow builds via the unified Makefiles / pinned standard) and **P3-3** (`make check` must exist). No file collision with any subtask; the ordering is a build-integration dependency, not a shared-file lane. Independent of P3-2 (CI is Linux-only).

---

## 4. Dependency Ordering & Parallelization

- **Parallel group (disjoint files, no shared design):** `P3-1` (all Makefiles except repo-root), `P3-2` (`vs2022/SSW.Common.props`), and `P3-3` (repo-root `Makefile`) touch mutually disjoint files and may run concurrently.
- **`P3-4` runs last.** It creates a new file (`.github/workflows/ci.yml`) with no collision, but it *invokes* the products of P3-1 (unified build) and P3-3 (`make check`); validating it meaningfully requires both merged. Sequence P3-4 after P3-1 and P3-3.
- No cross-subtask design agreement is required beyond the settled §2b decisions. The only ordering constraint is the P3-4 build-integration dependency.

Execution shape: `{ P3-1 ∥ P3-2 ∥ P3-3 }` → `P3-4`.

---

## 5. Documentation Impact (overall)

- **`AGENTS.md` (canonical build/test doc — Documenter stage):** add `make check` to the "Running Tests" section as the one-command way to build+run all three suites (GUI under xvfb); note `common.mk` as the shared Make fragment in the "Build System / Linux / Make" section; note the pinned `-std=c++17` (both platforms); add a short "Continuous Integration" note pointing at `.github/workflows/ci.yml`; refresh the general/Linux "wxWidgets 3.3.1" version prose to **3.3.2** — but **leave the Windows install-path reference `C:\Users\steph26\repos\wxWidgets-3.3.1\` unchanged** (it tracks the actual Windows folder, which is still `-3.3.1` and the user updates separately; the `SSW.Common.props` `WxWidgetsRoot` default is likewise untouched by P3-2). **Amend the no-`wx-config` module-boundary rule** (in the Build System and Module Boundary/wxWidgets Integration text) to carve out `src/tactical`'s use of the shared `wx-config`-derived `WX_CXXFLAGS` — the five pure modules (`core/strategic/ships/weapons/defenses`) remain forbidden from `wx-config`; `src/tactical` uses the shared variable until `FBattleScreen`/`FBattleBoard`/`FBattleDisplay` are relocated to `src/gui` (the roadmap's long-term fix). This amendment is **owned by P3-1's Documenter stage** so policy and code land consistent. Per the single-source-of-truth rule these facts belong in AGENTS.md, not in entry-point pointer files. Keep edits additive and minimal.
- **`doc/synthesized-roadmap.md`:** annotate Part III "Now" item #3 (line 700) as **RESOLVED** (mirroring the C1–C6 / T1–T3 tags), noting the common.mk + `-std=c++17` (both platforms) + `make check` + CI deliverables, the `wx-config`-derived tactical flags that retire the hardcoded path (§3.5's "cheap win"), and the deliberate scope note that Windows/MSVC conformance-mode flip, the `FBattleScreen`/`Board`/`Display`→gui relocation, and CMake remain deferred.
- **`common.mk`:** self-documenting header comment (owned by P3-1).
- **Optional VS notes:** `artifacts/VS_PROJECT_CHANGES.md` / `artifacts/WXWIDGETS_UPGRADE_CHANGES.md` may record the `<LanguageStandard>` pin (Documenter stage).
- **No Doxygen changes:** no C++ functions or classes are added or modified in this phase, so the mandatory function-header convention does not apply to any subtask here.
- **Non-editable:** `doc/rules/tactical_operations_manual.md` and `doc/rules/second-sathar-war-rules.md` must not be touched.

## 6. Untouched Files / Constraints

- **Do not edit** any C++ source or header (`src/**/*.cpp`, `include/**/*.h`) — this is a pure build/CI phase.
- **Do not edit** `doc/rules/tactical_operations_manual.md` or `doc/rules/second-sathar-war-rules.md`.
- **Keep the five pure modules wx-free:** `src/{core,strategic,ships,weapons,defenses}/Makefile` must contain no `wx-config` and must not reference `WX_CXXFLAGS`, and `common.mk`'s base `CXXFLAGS` must not reference `WX_CXXFLAGS` (so including it never triggers `wx-config` for the pure modules). `src/tactical` is the approved exception — it references the shared `wx-config`-derived `WX_CXXFLAGS` (see §2b decision 5); its Makefile contains no literal `wx-config` string.
- **Do not flip** `<ConformanceMode>` on Windows (kept `false`; §2b assumption).
- **Do not change** any test's expected outcome; the build refactor must be behavior-preserving for every suite.
- **Do not introduce CMake** (roadmap defers it) or a Windows CI job (roadmap defers it).
- Preserve `SKIP_MODEL_DEPS`, the coverage opt-in (`COVERAGE=1`), the `.RECIPEPREFIX = >` style where it exists, and all existing target names.

---

## 7. Implementer Prompts (Coordinator-ready)

Each subtask's artifacts go under `artifacts/phase3-build-system-ci/<ID>/` (repository-root-relative). None require the Security stage.

### P3-1 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Unify the Linux build with a shared common.mk, pin -std=c++17 across all Linux Makefiles, and replace src/tactical's hardcoded wx include path with wx-config-derived flags (roadmap Phase 3: common.mk + pin -std=c++17). This is a behavior-preserving build-config change: no C++ source may change, and every test suite must produce the same results as before.

IMPORTANT (approved deviation): this subtask intentionally makes src/tactical obtain its wx include flags from wx-config (via a shared common.mk variable), which the CURRENT AGENTS.md/CLAUDE.md rule forbids for the six non-GUI module Makefiles. The user approved this. The same subtask's Documenter stage amends that rule to carve out src/tactical. Do NOT stop or treat this as a constraint violation — proceed.

Allowed files (edit/create only these):
- common.mk                              (new, repo root)
- src/core/Makefile, src/strategic/Makefile, src/ships/Makefile, src/weapons/Makefile, src/defenses/Makefile, src/tactical/Makefile
- src/gui/Makefile, src/battleSim/Makefile, src/Makefile
- tests/core/Makefile, tests/weapons/Makefile, tests/strategic/Makefile, tests/ships/Makefile
- tests/Makefile, tests/gui/Makefile, tests/tactical/Makefile
Do NOT edit the repo-root Makefile (that is a separate subtask).

Task:
1. Create common.mk at the repo root holding the wx-FREE common base shared by the module Makefiles:
   - the COVERAGE / COVERAGE_FLAGS / COVERAGE_PREP block (opt-in on COVERAGE=1),
   - .DEFAULT_GOAL := all,
   - a base CXXFLAGS = -I../../include -g -Wall -Woverloaded-virtual -DLINUX -std=c++17 $(COVERAGE_FLAGS),
   - DEPFLAGS = -MMD -MP, PICFLAGS = -fPIC -DPIC,
   - the .SUFFIXES list and the .c.o / .cpp.o / .cxx.o / .rsrc.r implicit rules,
   - the shared objs / coverage-clean / clean targets.
   common.mk must ALSO define WX_CXXFLAGS LAZILY (recursive `=`, NOT `:=`) as wx-config-derived flags:
       WX_CXXFLAGS = $(shell wx-config --cxxflags) $(shell pkg-config --cflags gtk+-3.0)
   This reproduces src/tactical's current effective wx+gtk include set. Because the assignment is lazy, wx-config runs ONLY when a Makefile expands $(WX_CXXFLAGS). The base CXXFLAGS in common.mk must NOT reference WX_CXXFLAGS, so pure modules that include common.mk never trigger wx-config.
2. Convert these to `include ../../common.mk`, keeping only their own TARGET, source globs, the `all $(TARGET): ... ar rcs` rule, and `-include $(DEPS)`:
   - the 6 non-GUI src modules (core, strategic, ships, weapons, defenses, tactical),
   - the 4 simple test-lib modules (tests/core, tests/weapons, tests/strategic, tests/ships).
   Preserve each Makefile's exact current behavior: tests/* must still append `wx-config --cxxflags`, their -D<MODULE>_TEST_REPO_ROOT define, cppunit/wx libs, and the `filter-out test_template.cpp` glob.
   - src/tactical: append the shared variable (`CXXFLAGS += $(WX_CXXFLAGS)`) and REMOVE the hardcoded `-I/usr/local/lib/wx/include/gtk3-unicode-static-3.3 ...` path entirely.
   - The five pure modules (core, strategic, ships, weapons, defenses): reference NEITHER wx-config NOR WX_CXXFLAGS.
3. Add -std=c++17 inline (no common.mk include) to the Makefiles that keep their current form: src/Makefile, src/gui/Makefile, src/battleSim/Makefile, tests/Makefile, tests/gui/Makefile, tests/tactical/Makefile. Use the identical flag everywhere.
4. Fix drift: change src/defenses/Makefile's `rm  -f*.gcov` to `rm -f *.gcov`; remove the stale `-I../../../src/include` from tests/core/Makefile.
5. Preserve all existing target names, the SKIP_MODEL_DEPS mechanism, the `.RECIPEPREFIX = >` style in tests/gui and tests/tactical, coverage opt-in, and the -D*_TEST_REPO_ROOT defines.

Constraint: if strict -std=c++17 breaks the wx/GTK build, STOP and report back to the user for a decision — do NOT silently switch to -std=gnu++17.

Acceptance criteria (implementation outcomes):
- common.mk exists and is included by the 6 non-GUI src modules and the 4 simple test-lib Makefiles.
- `make all_clean && make` from the repo root builds SSW, BattleSim, and all three test runners with no errors, and compile lines carry -std=c++17 (show via a dry run or a sample compile line).
- SSWTests, TacticalTests, and GuiTests (the last under xvfb-run -a) all still pass unchanged.
- src/tactical's wx flags come from common.mk's wx-config-derived WX_CXXFLAGS; the hardcoded /usr/local/lib/wx/include/gtk3-unicode-static-3.3 path is gone from src/tactical/Makefile.
- The five pure modules stay wx-free: no wx-config and no WX_CXXFLAGS reference in src/{core,strategic,ships,weapons,defenses}/Makefile (building them invokes wx-config zero times).
- src/defenses/Makefile shows `rm -f *.gcov`; tests/core/Makefile no longer references ../../../src/include.
- `make COVERAGE=1 tests` still compiles with -fprofile-arcs -ftest-coverage.

Validation:
- Clean rebuild: `make all_clean && make`.
- Suites: `cd tests && make && ./SSWTests`; `cd tests/tactical && make && ./TacticalTests`; `cd tests/gui && make && xvfb-run -a ./GuiTests`.
- Coverage opt-in smoke: `make COVERAGE=1 tests`.

Tester handoff:
- This is a build-config change with no C++ behavior delta; the "test" is that all three existing suites still build and pass under the refactored Makefiles (and coverage still opts in). No new CppUnit fixtures are expected. If any behavioral test is added, it belongs to the module whose Makefile changed, but the primary verification is a clean full-suite build+run.

Artifacts:
- Write your report/results under artifacts/phase3-build-system-ci/P3-1/ (repository-root-relative). Record the exact common.mk contents rationale, the list of converted vs inline-std Makefiles, confirmation that strict -std=c++17 compiled cleanly, that the tactical hardcoded wx path was replaced by wx-config-derived WX_CXXFLAGS, and an explicit note that the wx-config-in-tactical change is the user-approved deviation whose AGENTS.md/CLAUDE.md carve-out is handled by this subtask's Documenter stage (so the Verifier accepts it).

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P3-2 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Pin the Windows/MSVC C++ standard to C++17 (roadmap Phase 3: pin -std=c++17 both platforms). All 10 vs2022 projects import vs2022/SSW.Common.props, so this is a single-file change.

Allowed files (edit only this):
- vs2022/SSW.Common.props

Task:
- Add <LanguageStandard>stdcpp17</LanguageStandard> to the shared <ItemDefinitionGroup>/<ClCompile> block in SSW.Common.props (the same block that already contains <ConformanceMode>false</ConformanceMode>).
- Leave <ConformanceMode>false</ConformanceMode> unchanged (do not flip it).
- Do NOT edit any individual .vcxproj, any Linux Makefile, or any C++ source.

Acceptance criteria (implementation outcomes):
- SSW.Common.props contains exactly one <LanguageStandard>stdcpp17</LanguageStandard> in the shared ClCompile item-definition block, and the file remains well-formed XML (parses cleanly).
- All 10 vcxproj still import SSW.Common.props (unchanged import lines), so all inherit the pin.
- No change is made outside SSW.Common.props; the Linux `make` build is unaffected.

Validation:
- XML well-formedness, e.g.: `python -c "import xml.dom.minidom as m; m.parse('vs2022/SSW.Common.props')"`.
- Confirm the import fan-out: `grep -l "SSW.Common.props" vs2022/*.vcxproj` lists all 10 projects.
- NOTE: MSVC compilation cannot be run from this Linux environment. Do NOT claim a Windows build was executed; state this limitation explicitly in your artifact.

Tester handoff:
- No behavioral CppUnit coverage is possible for a Windows-only compiler-flag change from Linux. Verification is structural (valid XML in the shared props, inherited by all projects). Note this in the handoff.

Artifacts:
- Write your report/results under artifacts/phase3-build-system-ci/P3-2/ (repository-root-relative). Explicitly record that ConformanceMode was intentionally left false and that MSVC compilation was not verifiable here.

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P3-3 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Add a `make check` target that builds and runs all three test suites as one command (roadmap Phase 3: make check target). Today "run all tests" is a fiction — SSWTests covers only core/weapons/strategic/ships, the tactical fixtures live in TacticalTests, and the GUI fixtures in GuiTests.

Allowed files (edit only this):
- Makefile   (repo root)

Task:
- Add a `check` target (and add `check` to the repo-root Makefile's .PHONY list) that:
  1. Builds everything first by depending on / invoking the existing `all` target (which already builds SSW, BattleSim, and all three test runners via src + tests with SKIP_MODEL_DEPS=1).
  2. Runs the three suites in sequence: tests/SSWTests, tests/tactical/TacticalTests, and tests/gui/GuiTests wrapped in `xvfb-run -a`. You may cd into each runner's directory if that matches how it is normally invoked.
  3. Propagates failure: if any suite exits non-zero, `make check` must exit non-zero. Do NOT prefix recipe lines with `-`, and do NOT use `|| true` or any construct that masks a failing suite.
- Keep the target self-contained in the repo-root Makefile; drive the existing `tests` sub-targets rather than editing tests/Makefile.

Acceptance criteria (implementation outcomes):
- From a clean tree, `make check` builds all three runners and runs all three suites, with GuiTests under xvfb-run.
- When all suites pass, `make check` exits 0.
- When any one suite fails (demonstrate by temporarily breaking a single assertion, observe non-zero exit, then revert), `make check` exits non-zero — the failure is not masked.
- `make check` takes no arguments and needs only the already-required deps (wxWidgets 3.3.2, CppUnit, xvfb).

Validation:
- `make check` on a clean tree; confirm all three suites run and the GUI suite runs under xvfb.
- Confirm exit-code propagation with a temporary forced failure (then revert it).

Tester handoff:
- The deliverable is the target itself; verification is that `make check` builds+runs all three suites and returns a correct exit code (0 on success, non-zero on any suite failure). No new CppUnit fixtures are expected.

Artifacts:
- Write your report/results under artifacts/phase3-build-system-ci/P3-3/ (repository-root-relative). Record the observed run of all three suites and the exit-code-propagation check.

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

### P3-4 Implementer Prompt
```
Your role is 'implementer'. Your task is as follows:

Add a minimal GitHub Actions CI workflow that builds the repo and runs `make check` on Linux (roadmap Phase 3: minimal CI). This runs after the common.mk/-std pin (P3-1) and the `make check` target (P3-3) are in place.

Allowed files (create only these):
- .github/workflows/ci.yml   (new; create .github/workflows/ as needed)
(If a small wx-provisioning helper script is genuinely necessary, it may live under .github/ and must be listed in your artifact; prefer inline workflow steps.)

Task:
- Write a workflow triggered on `push` and `pull_request` targeting `master`, plus `workflow_dispatch`, with a single Ubuntu job that:
  1. Checks out the repository.
  2. Installs build dependencies: a C++ toolchain, CppUnit dev, GTK3 dev, xvfb, and wxWidgets 3.3.2. Use a distro package if the chosen runner image provides 3.3.x; otherwise build wx 3.3.2 from source and cache it with actions/cache keyed on the wx version. Document the chosen approach in a workflow comment.
  3. Builds the repo: `make`.
  4. Runs `make check` (the GUI suite runs under xvfb-run, already wired inside the check target).
- CI hygiene (required): set workflow/job `permissions:` to `contents: read`; pin any third-party action to a fixed version tag or commit SHA; use NO repository secrets.

Acceptance criteria (implementation outcomes):
- .github/workflows/ci.yml exists and is valid workflow YAML (e.g. `python -c "import yaml; yaml.safe_load(open('.github/workflows/ci.yml'))"` succeeds; or actionlint passes if available).
- The workflow declares push + pull_request (to master) + workflow_dispatch triggers, one Ubuntu job, and steps that install deps (including a wx 3.3.2 provisioning step), run `make`, and run `make check`.
- The workflow sets `permissions: contents: read`, pins third-party actions to a fixed ref, and references no secrets.
- The build/test commands match the locally-verified `make` and `make check` exactly (no CI-only divergent build path).

Validation:
- YAML parse / actionlint as above.
- Cross-check that the `make` and `make check` invocations equal the locally-working commands.
- NOTE: GitHub Actions cannot be run from this environment. Do NOT claim a green cloud run; state this limitation explicitly in your artifact, and confirm the workflow's build path is command-identical to the local build.

Tester handoff:
- CI cannot be executed locally; verification is structural (valid workflow YAML, correct triggers/steps/permissions) plus command-parity with the locally-verified make/make check. Note this in the handoff.

Artifacts:
- Write your report/results under artifacts/phase3-build-system-ci/P3-4/ (repository-root-relative). Record the wx 3.3.2 provisioning approach chosen, the YAML validation output, and the explicit note that the cloud run was not executed here.

Proceed past preflight and implement directly if no blocker is present.
Do not report success unless all required artifacts exist and all changes are committed.
```

---

## 8. Completion Definition

This plan is complete when P3-1…P3-4 are implemented, verified, documented, and merged back through the Coordinator workflow: `common.mk` exists and unifies the module Makefiles with a strict `-std=c++17` pin (Linux) that builds cleanly and leaves every test suite passing; `src/tactical`'s wx flags derive from `wx-config` (hardcoded path retired) while the five pure modules stay wx-free; `SSW.Common.props` pins `stdcpp17` for MSVC; `make check` builds and runs all three suites (GUI under xvfb) with correct exit-code propagation; `.github/workflows/ci.yml` provides a Linux CI job that builds and runs `make check` against wxWidgets 3.3.2; AGENTS.md documents `make check`/common.mk/the pinned standard/CI, carries the `src/tactical` `wx-config` carve-out, and reads 3.3.2; and `doc/synthesized-roadmap.md` Part III item #3 is annotated RESOLVED. No C++ source is changed and no test outcome changes. The final Reviewer pass runs after all subtasks merge.
