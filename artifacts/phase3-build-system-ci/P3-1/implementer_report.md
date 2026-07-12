# P3-1 Implementer Report — common.mk + Linux `-std=c++17` pin + Makefile drift fixes

**Subtask:** P3-1 (Phase 3 — Build-System Unification + Minimal CI)
**Role:** Implementer
**Branch:** `phase3-P3-1-implementer-20260712`
**Code commit:** `c88719a6ca39836c8ef8cba33dc093da3a611255`
**Date:** 2026-07-12

## Goal

Behavior-preserving build-config change: introduce a shared repo-root `common.mk`,
pin `-std=c++17` across all Linux Makefiles, and replace `src/tactical`'s
machine-specific hardcoded wx include path with `wx-config`-derived flags. No C++
source or header changed; every test suite produces the same results as before.

## What changed

### 1. New `common.mk` (repo root)

Holds the wx-**free** common base shared by the module Makefiles that include it:

- the `COVERAGE` / `COVERAGE_FLAGS` / `COVERAGE_PREP` opt-in block (`COVERAGE=1`),
- `.DEFAULT_GOAL := all`,
- base `CXXFLAGS = -I../../include -g -Wall -Woverloaded-virtual -DLINUX -std=c++17 $(COVERAGE_FLAGS)`,
- `DEPFLAGS = -MMD -MP`, `PICFLAGS = -fPIC -DPIC`,
- the `.SUFFIXES` list plus the `.c.o` / `.cpp.o` / `.cxx.o` / `.rsrc.r` implicit rules,
- the shared `objs` / `coverage-clean` / `clean` targets.

It **also** defines `WX_CXXFLAGS` **lazily** (recursive `=`, not `:=`):

```
WX_CXXFLAGS = $(shell wx-config --cxxflags) $(shell pkg-config --cflags gtk+-3.0)
```

**Rationale for lazy assignment:** GNU make only expands the `$(shell …)` calls
inside a recursively-expanded variable when that variable is actually referenced
by a rule that runs. The base `CXXFLAGS` in `common.mk` does **not** reference
`WX_CXXFLAGS`, so the five pure modules that include `common.mk` never expand it
and therefore never shell out to `wx-config`/`pkg-config`. Only `src/tactical`
references it (via `CXXFLAGS += $(WX_CXXFLAGS)`), reproducing the exact wx+gtk
include set that its old hardcoded path provided. A self-documenting header
comment in `common.mk` explains the role, what each including Makefile must still
define, and the lazy-`WX_CXXFLAGS` contract.

**Verification that WX_CXXFLAGS reproduces the old path:** `wx-config --cxxflags`
on this machine emits exactly
`-I/usr/local/lib/wx/include/gtk3-unicode-static-3.3 -I/usr/local/include/wx-3.3 -D_FILE_OFFSET_BITS=64 -D__WXGTK__ -pthread`,
matching the previous hardcoded literal, and `pkg-config --cflags gtk+-3.0` supplies
the gtk includes the old line already appended.

### 2. Converted to `include ../../common.mk`

Ten Makefiles now include `common.mk` and keep only their own `TARGET`, source
globs, the `all $(TARGET): … ar rcs` rule, and `-include $(DEPS)`:

- **6 non-GUI src modules:** `src/core`, `src/strategic`, `src/ships`,
  `src/weapons`, `src/defenses`, `src/tactical`.
- **4 simple test-lib modules:** `tests/core`, `tests/weapons`,
  `tests/strategic`, `tests/ships`.

Per-module preservation:

- The four `tests/*` libs still append `` `wx-config --cxxflags` `` and their
  `-lcppunit`/wx `LIBS`, and still use the `filter-out test_template.cpp` glob.
  `tests/core` still defines `-DCORE_TEST_REPO_ROOT=\"$(CURDIR)/../..\"`.
- `src/tactical` appends `CXXFLAGS += $(WX_CXXFLAGS)` and the hardcoded
  `/usr/local/lib/wx/include/gtk3-unicode-static-3.3 …` path is **removed
  entirely**.
- The five pure modules (`core`, `strategic`, `ships`, `weapons`, `defenses`)
  reference **neither** `wx-config` **nor** `WX_CXXFLAGS`.

### 3. Inline `-std=c++17` (no common.mk include)

Added strict `-std=c++17` (identical flag value everywhere) to the six Makefiles
that keep their current standalone form:

- `src/Makefile`, `src/gui/Makefile`, `src/battleSim/Makefile`,
- `tests/Makefile`, `tests/gui/Makefile`, `tests/tactical/Makefile`.

### 4. Drift fixes

- `src/defenses/Makefile`: the malformed `rm  -f*.gcov` is gone — that module now
  inherits the corrected `rm -f *.gcov` from `common.mk`'s shared `objs` target
  (the malformed literal no longer exists anywhere in the tree).
- `tests/core/Makefile`: the stale `-I../../../src/include` is removed.

### 5. Preserved mechanisms

`SKIP_MODEL_DEPS`, the `.RECIPEPREFIX = >` style in `tests/gui` and
`tests/tactical`, the coverage opt-in, all existing target names, and every
`-D*_TEST_REPO_ROOT` define are intact.

## Converted vs. inline-std Makefiles (summary)

| Makefile | Treatment | wx flags |
|---|---|---|
| `common.mk` (new) | — | defines lazy `WX_CXXFLAGS`; base CXXFLAGS is wx-free |
| `src/core` | include common.mk | none |
| `src/strategic` | include common.mk | none |
| `src/ships` | include common.mk | none |
| `src/weapons` | include common.mk | none |
| `src/defenses` | include common.mk | none |
| `src/tactical` | include common.mk | `CXXFLAGS += $(WX_CXXFLAGS)` (approved exception) |
| `tests/core` | include common.mk | `+= \`wx-config --cxxflags\`` |
| `tests/weapons` | include common.mk | `+= \`wx-config --cxxflags\`` |
| `tests/strategic` | include common.mk | `+= \`wx-config --cxxflags\`` |
| `tests/ships` | include common.mk | `+= \`wx-config --cxxflags\`` |
| `src/Makefile` | inline `-std=c++17` | unchanged `\`wx-config --cxxflags\`` |
| `src/gui/Makefile` | inline `-std=c++17` | unchanged `\`wx-config --cxxflags\`` |
| `src/battleSim/Makefile` | inline `-std=c++17` | unchanged `\`wx-config --cxxflags\`` |
| `tests/Makefile` | inline `-std=c++17` | unchanged `\`wx-config --cxxflags\`` |
| `tests/gui/Makefile` | inline `-std=c++17` | unchanged `\`wx-config --cxxflags\`` `\`pkg-config --cflags gtk+-3.0\`` |
| `tests/tactical/Makefile` | inline `-std=c++17` | unchanged `\`wx-config --cxxflags\`` |

## Validation (all run in the foreground)

- **Clean full build:** `make all_clean` then `make` from the repo root — exit 0.
  Builds `SSW`, `BattleSim`, and all three test runners with no errors. Sample
  compile line confirming the pin (a pure module):
  `g++ -c -I../../include -g -Wall -Woverloaded-virtual -DLINUX -std=c++17  -MMD -MP -fPIC -DPIC -o FGameConfig.o FGameConfig.cpp`
- **`SSWTests`:** `OK (245 tests)`.
- **`TacticalTests`:** `OK (253 tests)` (`cd tests/tactical && make && ./TacticalTests`).
- **`GuiTests`:** `OK (75 tests)` (`cd tests/gui && make && xvfb-run -a ./GuiTests`).
- **Strict `-std=c++17` compiled cleanly** — no wx/GTK build breakage; the STOP
  constraint (do not silently fall back to `-std=gnu++17`) was never triggered.
- **`src/tactical` wx flags now derive from `wx-config`** via `common.mk`'s
  `WX_CXXFLAGS`. A sample tactical compile line shows the identical effective
  include set (`-I/usr/local/lib/wx/include/gtk3-unicode-static-3.3 …` plus the
  gtk-3.0 includes) plus `-std=c++17`. The hardcoded path string no longer
  appears in `src/tactical/Makefile`.
- **Five pure modules stay wx-free:** `grep -l "wx-config\|WX_CXXFLAGS"` over
  `src/{core,strategic,ships,weapons,defenses}/Makefile` returns nothing; a
  `make -n` compile line for `src/core/FPoint.o` has no `wx-config` invocation.
- **Drift fixes confirmed:** the malformed `rm  -f*.gcov` and the stale
  `src/include` reference are both absent from the entire `src`/`tests` tree.
- **Coverage opt-in still works:** `make -n COVERAGE=1` for a `common.mk`-including
  test object yields
  `g++ -c -I../../include -g -Wall -Woverloaded-virtual -DLINUX -std=c++17 -fprofile-arcs -ftest-coverage \`wx-config --cxxflags\` -MMD -MP -fPIC -DPIC -o FWeaponTest.o FWeaponTest.cpp`,
  and a full `make COVERAGE=1 tests` compiles and links with
  `-fprofile-arcs -ftest-coverage`.

## Approved deviation (for the Verifier)

Making `src/tactical` obtain its wx include flags from `wx-config` (via the shared
`common.mk` `WX_CXXFLAGS` variable) is a **user-approved deviation** from the
CURRENT AGENTS.md/CLAUDE.md rule that forbids `wx-config` in the six non-GUI
module Makefiles. The user approved this during planning (plan §2b decision 5).
The AGENTS.md/CLAUDE.md carve-out — permitting `src/tactical`'s use of the shared
`wx-config`-derived `WX_CXXFLAGS` while the five pure modules remain forbidden
from `wx-config` — is owned by **this subtask's Documenter stage**, so policy and
code land consistent before the Verifier/Reviewer pass. The five pure modules
were kept strictly wx-free, matching the unchanged part of the rule.

## Scope

Only the 16 approved Makefiles were modified and `common.mk` was created. No C++
source, header, `vs2022/*`, repo-root `Makefile`, or rules document was touched.
