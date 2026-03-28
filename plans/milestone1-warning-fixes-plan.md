# Milestone 1 Warning Fixes — Implementation Plan

## Problem Statement

The Reviewer report for `milestone1-wximage-removal` (at
`artifacts/gui_sep/milestone1-wximage-removal/milestone1-reviewer-report.md`) identified three
WARNING-level issues that partially undermine the milestone's clean-boundary goal.  This plan
addresses all three in dependency order without revisiting the already-passing items.

---

## Verified Context (Codebase Inspection Results)

### WARNING 1 — Strategic-layer wx boundary incomplete

- `include/strategic/FGame.h` line 15: `#include <wx/wx.h>` — pulls full wxWidgets into every
  translation unit that includes `FGame.h`.
- `include/strategic/FGame.h` lines 44, 59, 77: `wxWindow*` used in constructor, `create()`,
  and `init()` signatures.
- `include/strategic/FGame.h` lines 279, 281: `wxImage* m_tenday` and `wxImage* m_day` stored
  as members — the only wxImage state remaining in the strategic module.
- `src/strategic/FGame.cpp`: constructor loads tenday/day images; `drawTurnCounter()` and
  `draw()` use `m_parent` for DC drawing and modal dialogs.
- `src/strategic/FPlayer.cpp` line 13: `#include "gui/WXMapDisplay.h"` — the strategic module
  includes a GUI header.
- `src/strategic/FPlayer.cpp` line 140: `WXMapDisplay mapDisplay;` is declared inside
  `getFleet(double x, double y)` but **never used**; the method performs pure coordinate math.

**Minimum fix for the header boundary:** forward-declare `class wxWindow;` instead of
`#include <wx/wx.h>` in `FGame.h`; move `m_tenday`/`m_day` ownership into the `.cpp` file
using a private nested struct or by forwarding through `WXIconCache`; remove the dead
`WXMapDisplay` include and local variable from `FPlayer.cpp`.

### WARNING 2 — WXIconCache loads images relative to working directory

- `src/gui/WXIconCache.cpp::get()`: calls `wxImage(filename)` directly — path is
  relative to the process working directory.
- Old model-side code (`FVehicle.cpp`, `FFleet.cpp` before Milestone 1) called
  `FGameConfig::getBasePath() + iconName` which derives an absolute path from the
  executable location.
- `FGameConfig::getBasePath()` is in `include/core/FGameConfig.h` — already accessible
  from the `gui` module (the gui Makefile already lists `FGameConfig` as a dependency for
  several objects).
- All other wxImage loads in the codebase (`FBattleDisplay.cpp`, `FBattleBoard.cpp`,
  `FBattleScreen.cpp`, `FApp.cpp`, `FBattleSimApp.cpp`) already call
  `gc.getBasePath() + "icons/..."`.  Only `WXIconCache` is inconsistent.

**Fix:** In `WXIconCache::get()`, prepend `FGameConfig::getBasePath()` to the filename
before calling `wxImage(path)`, restoring the pre-milestone launch-path semantics.
Add `#include "core/FGameConfig.h"` to `WXIconCache.cpp`.

### WARNING 3 — Setter tests assert non-empty rather than exact filename

- `tests/ships/FVehicleTest.cpp` line 128-129:
  ```cpp
  m_v1->setIcon("icons/UPFDestroyer.png");
  CPPUNIT_ASSERT(!m_v1->getIconName().empty());
  ```
- `tests/ships/FVehicleTest.cpp` line 160: `CPPUNIT_ASSERT(!m_v1->getIconName().empty());`
  (after `setIcon` in the serialize setup).
- `tests/strategic/FFleetTest.cpp` lines 56-59:
  ```cpp
  m_f1->setIcon("icons/UPF.png");
  CPPUNIT_ASSERT( !m_f1->getIconName().empty() );
  m_f1->setIcon("icons/Sathar.png");
  CPPUNIT_ASSERT( !m_f1->getIconName().empty() );
  ```
- All assertions should use `CPPUNIT_ASSERT_EQUAL` with the exact expected string.

---

## Files to Modify

### Strategic module (WARNING 1)
- `include/strategic/FGame.h` — replace `#include <wx/wx.h>` with forward declaration;
  remove `wxImage*` members from header (move to `.cpp`)
- `src/strategic/FGame.cpp` — own `m_tenday`/`m_day` storage without exposing through header
  (e.g., anonymous struct, or route through `WXIconCache`)
- `src/strategic/FPlayer.cpp` — remove `#include "gui/WXMapDisplay.h"` and dead
  `WXMapDisplay mapDisplay;` local variable

### GUI module (WARNING 2)
- `src/gui/WXIconCache.cpp` — prepend `FGameConfig::getBasePath()` before loading

### Tests (WARNING 3)
- `tests/ships/FVehicleTest.cpp` — strengthen two `!empty()` assertions to exact-match
- `tests/strategic/FFleetTest.cpp` — strengthen two `!empty()` assertions to exact-match

---

## Overall Documentation Impact

- `AGENTS.md` contributor notes already document the icon-loading boundary; no change needed
  unless the `WXIconCache` contract (path relative vs. absolute) is explicitly described.
- No user guide update required.
- If `FGame.h` is restructured, Doxygen class docs may regenerate with minor differences;
  no manual doc edits required unless member descriptions are removed.

---

## Subtasks

### T1 — Remove dead `WXMapDisplay` dependency from `FPlayer.cpp`

**Scope:** Surgical removal of two lines in `src/strategic/FPlayer.cpp`.

**Files:** `src/strategic/FPlayer.cpp`

**Acceptance criteria:**
1. `src/strategic/FPlayer.cpp` no longer includes `gui/WXMapDisplay.h`.
2. `FPlayer::getFleet(double, double)` no longer declares a `WXMapDisplay mapDisplay` local variable.
3. `getFleet(double, double)` behaves identically (coordinate comparison logic unchanged).
4. Full test suite (`tests/SSWTests`) passes with no regressions.
5. Both executables (`SSW`, `BattleSim`) link successfully.

**Documentation Impact:** None expected.

**Dependencies:** None — can be implemented first or in parallel with T2.

---

### T2 — Forward-declare `wxWindow` in `FGame.h` and remove `wxImage` members from header

**Scope:** Restructure `include/strategic/FGame.h` so it does not include `<wx/wx.h>`.
Remove `wxImage* m_tenday` and `wxImage* m_day` from the header entirely; load those images
on demand in `drawTurnCounter()` through `WXIconCache`, consistent with the icon-loading
pattern established by Milestone 1.

**Files:**
- `include/strategic/FGame.h`
- `src/strategic/FGame.cpp`

**Approach:**
Replace `#include <wx/wx.h>` in `FGame.h` with the forward declaration `class wxWindow;`.
Remove the `wxImage* m_tenday` and `wxImage* m_day` member declarations from the header.
In `FGame.cpp`, delete the `new wxImage(...)` constructor calls and the `delete` destructor
calls for those members, and instead call `WXIconCache::instance().get(...)` directly inside
`drawTurnCounter()` wherever `m_tenday` and `m_day` were previously used.

**Acceptance criteria:**
1. `include/strategic/FGame.h` does not contain `#include <wx/wx.h>` or `#include <wx/...>`.
2. `include/strategic/FGame.h` still compiles when included by a translation unit that does
   **not** include wxWidgets (e.g., a pure strategic test file).
3. `wxWindow*` parameter in `FGame::create()` and `FGame::init()` is retained but uses only
   a forward declaration in the header.
4. `m_tenday` and `m_day` (or equivalent) are no longer declared in `FGame.h`.
5. `drawTurnCounter()` in `FGame.cpp` still renders correctly (tenday/day images loadable).
6. Full test suite passes with no regressions.
7. Both executables link successfully.

**Documentation Impact:** Doxygen member docs for removed `m_tenday`/`m_day` will disappear;
no manual update required.

**Dependencies:** T1 should complete first to keep strategic-module changes logically grouped,
but T2 is technically independent.

---

### T3 — Fix `WXIconCache` path resolution to use `FGameConfig::getBasePath()`

**Scope:** Update `src/gui/WXIconCache.cpp` so that icon filenames are resolved relative to
the executable's directory (via `FGameConfig::getBasePath()`) rather than the process working
directory, restoring pre-milestone runtime semantics.

**Files:** `src/gui/WXIconCache.cpp`

**Acceptance criteria:**
1. `WXIconCache::get(filename)` prepends `FGameConfig::getBasePath()` to `filename` before
   constructing `wxImage(path)`.
2. The resolved path matches the pattern used everywhere else in the codebase
   (e.g., `gc.getBasePath() + "icons/tenday.png"`).
3. `#include "core/FGameConfig.h"` is added to `WXIconCache.cpp`.
4. The cache key remains the original `filename` (not the resolved path) so callers that call
   `get()` with the same short name always hit the cache on the second call.
5. Full test suite passes with no regressions.
6. Both executables link successfully.

**Documentation Impact:** None for user guide.  If `WXIconCache.h` has a `@brief` describing
path semantics, update it to document the `getBasePath()` contract.

**Dependencies:** None — independent of T1 and T2, can run in parallel.

---

### T4 — Strengthen `setIcon` assertions in `FVehicleTest` and `FFleetTest`

**Scope:** Replace weak `!empty()` assertions with exact-string equality checks after
`setIcon()` calls in two test files.

**Files:**
- `tests/ships/FVehicleTest.cpp`
- `tests/strategic/FFleetTest.cpp`

**Specific changes:**

`tests/ships/FVehicleTest.cpp` (line 128-129):
```
// Before:
m_v1->setIcon("icons/UPFDestroyer.png");
CPPUNIT_ASSERT(!m_v1->getIconName().empty());

// After:
m_v1->setIcon("icons/UPFDestroyer.png");
CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFDestroyer.png"), m_v1->getIconName());
```

`tests/ships/FVehicleTest.cpp` (line ~160 — inside `testSerialize` setup):
```
// Before:
CPPUNIT_ASSERT( !m_v1->getIconName().empty());

// After:
CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFFrigate.png"), m_v1->getIconName());
```

`tests/strategic/FFleetTest.cpp` (lines 56-59):
```
// Before:
m_f1->setIcon("icons/UPF.png");
CPPUNIT_ASSERT( !m_f1->getIconName().empty() );
m_f1->setIcon("icons/Sathar.png");
CPPUNIT_ASSERT( !m_f1->getIconName().empty() );

// After:
m_f1->setIcon("icons/UPF.png");
CPPUNIT_ASSERT_EQUAL(std::string("icons/UPF.png"), m_f1->getIconName());
m_f1->setIcon("icons/Sathar.png");
CPPUNIT_ASSERT_EQUAL(std::string("icons/Sathar.png"), m_f1->getIconName());
```

**Acceptance criteria:**
1. All four `!empty()` assertions replaced with `CPPUNIT_ASSERT_EQUAL` exact-string checks.
2. Tests still pass (confirming `setIcon` stores the filename verbatim).
3. No other test logic is modified.
4. Full test suite passes with no regressions.

**Documentation Impact:** None.

**Dependencies:** None — independent of T1, T2, T3.  Can run in parallel.

---

## Dependency Ordering

```
T1 (remove WXMapDisplay from FPlayer) ──┐
                                         ├─ independent; run in parallel
T2 (clean FGame.h wx boundary)   ───────┤
                                         │
T3 (fix WXIconCache path)        ───────┤
                                         │
T4 (strengthen setter tests)     ───────┘
```

All four subtasks are independent and can be executed in parallel by separate implementers,
or sequentially in any order.  No subtask depends on completion of another.

---

## Artifact Directory

All artifacts produced by this plan's execution agents should be written under:

```
artifacts/gui_sep/milestone1-warning-fixes/
```

Create per-subtask directories `T1/`, `T2/`, `T3/`, `T4/` following the convention established
by the previous milestone run.

---

## Implementer Agent Prompts

### T1 Implementer Prompt

You are the implementer agent.

**Allowed files to change:**
- `src/strategic/FPlayer.cpp`

**Task:**
Remove the dead `WXMapDisplay` dependency from `FPlayer.cpp`.  Specifically:
1. Delete or comment out the `#include "gui/WXMapDisplay.h"` line near line 13.
2. Inside `FPlayer::getFleet(double x, double y)` (near line 140), remove the unused local
   variable declaration `WXMapDisplay mapDisplay;`.  Do not change any other logic in that
   method.
3. Verify the file compiles as part of the strategic module.

**Acceptance criteria:**
1. `src/strategic/FPlayer.cpp` no longer includes `gui/WXMapDisplay.h`.
2. `FPlayer::getFleet(double, double)` no longer declares a `WXMapDisplay mapDisplay` local
   variable.
3. `getFleet(double, double)` behaves identically (coordinate comparison logic unchanged).
4. Full test suite (`cd tests && make && ./SSWTests`) passes with no regressions.
5. Both executables (`SSW`, `BattleSim`) link successfully (`make` from repo root).

Write your implementer result JSON to `artifacts/gui_sep/milestone1-warning-fixes/T1/implementer_result.json`.

---

### T2 Implementer Prompt

You are the implementer agent.

**Allowed files to change:**
- `include/strategic/FGame.h`
- `src/strategic/FGame.cpp`

**Task:**
Remove `#include <wx/wx.h>` from `include/strategic/FGame.h` and remove the `wxImage*`
members (`m_tenday`, `m_day`) from the header so the strategic module header no longer
pulls in wxWidgets.

1. In `include/strategic/FGame.h`, replace `#include <wx/wx.h>` with the forward declaration:
   ```cpp
   class wxWindow;
   ```
2. Remove `wxImage * m_tenday;` and `wxImage * m_day;` from the private section of the header.
3. In `src/strategic/FGame.cpp`, remove the `new wxImage(...)` calls from the constructor and
   the corresponding `delete` calls from the destructor.
4. In `src/strategic/FGame.cpp`, inside `drawTurnCounter()`, replace each use of `m_tenday`
   and `m_day` with an on-demand call to `WXIconCache::instance().get(...)` passing the same
   path strings that were previously passed to `new wxImage(...)` in the constructor.
5. Add `#include "gui/WXIconCache.h"` to `FGame.cpp` if not already present.
6. Ensure `FGame.cpp` retains `#include <wx/wx.h>` (or the specific wx headers it needs)
   because it still uses `wxClientDC`, `wxMessageBox`, and other wx types in the `.cpp`.

**Acceptance criteria:**
1. `include/strategic/FGame.h` does not contain `#include <wx/wx.h>` or any `#include <wx/...>`.
2. `include/strategic/FGame.h` compiles when included by a translation unit that does not
   include wxWidgets (verify by building the strategic tests: `cd tests/strategic && make`).
3. `wxWindow*` parameter in `FGame::create()` and `FGame::init()` is retained and uses only a
   forward declaration in the header.
4. `m_tenday` and `m_day` are no longer declared in `FGame.h`.
5. `drawTurnCounter()` loads tenday and day images via `WXIconCache::instance().get(...)`.
6. Full test suite (`cd tests && make && ./SSWTests`) passes with no regressions.
7. Both executables link successfully (`make` from repo root).

Write your implementer result JSON to `artifacts/gui_sep/milestone1-warning-fixes/T2/implementer_result.json`.

---

### T3 Implementer Prompt

You are the implementer agent.

**Allowed files to change:**
- `src/gui/WXIconCache.cpp`
- `include/gui/WXIconCache.h` (only if updating `@brief` path-semantics documentation)

**Task:**
Fix `WXIconCache::get()` to resolve icon filenames using `FGameConfig::getBasePath()`,
restoring the path-resolution semantics that existed before Milestone 1.

1. Add `#include "core/FGameConfig.h"` to `src/gui/WXIconCache.cpp`.
2. In `WXIconCache::get(const std::string & filename)`, before constructing `wxImage(...)`,
   resolve the full path:
   ```cpp
   FGameConfig & gc = FGameConfig::getGameConfig();
   std::string fullPath = gc.getBasePath() + filename;
   ```
3. Use `fullPath` when constructing `wxImage` and in the warning log message.
4. Keep the cache key as the original `filename` (not `fullPath`) so callers using the same
   short name always hit the cache on a second call.
5. If `include/gui/WXIconCache.h` has a `@brief` or parameter description for `get()` that
   mentions path semantics, update it to say: "Path is relative to the executable base path
   as returned by `FGameConfig::getBasePath()`."

**Acceptance criteria:**
1. `WXIconCache::get()` prepends `FGameConfig::getBasePath()` before loading the image.
2. The resolved path pattern matches `gc.getBasePath() + filename`, consistent with all other
   icon/image loads in the codebase.
3. The cache key remains the short `filename`.
4. Full test suite passes with no regressions.
5. Both executables link successfully.

Write your implementer result JSON to `artifacts/gui_sep/milestone1-warning-fixes/T3/implementer_result.json`.

---

### T4 Implementer Prompt

You are the implementer agent.

**Allowed files to change:**
- `tests/ships/FVehicleTest.cpp`
- `tests/strategic/FFleetTest.cpp`

**Task:**
Strengthen the `setIcon`/`getIconName` assertions in both test files to verify the exact
stored filename rather than just non-emptiness.

**Changes to make:**

In `tests/ships/FVehicleTest.cpp`:

1. After `m_v1->setIcon("icons/UPFDestroyer.png");` (line ~128), replace:
   ```cpp
   CPPUNIT_ASSERT(!m_v1->getIconName().empty());
   ```
   with:
   ```cpp
   CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFDestroyer.png"), m_v1->getIconName());
   ```

2. Find the assertion `CPPUNIT_ASSERT( !m_v1->getIconName().empty());` that appears inside
   `testSerialize` after `setIcon("icons/UPFFrigate.png")` is called (line ~160).  Replace it
   with:
   ```cpp
   CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFFrigate.png"), m_v1->getIconName());
   ```

In `tests/strategic/FFleetTest.cpp`:

3. After `m_f1->setIcon("icons/UPF.png");` (line ~56), replace:
   ```cpp
   CPPUNIT_ASSERT( !m_f1->getIconName().empty() );
   ```
   with:
   ```cpp
   CPPUNIT_ASSERT_EQUAL(std::string("icons/UPF.png"), m_f1->getIconName());
   ```

4. After `m_f1->setIcon("icons/Sathar.png");` (line ~58), replace:
   ```cpp
   CPPUNIT_ASSERT( !m_f1->getIconName().empty() );
   ```
   with:
   ```cpp
   CPPUNIT_ASSERT_EQUAL(std::string("icons/Sathar.png"), m_f1->getIconName());
   ```

**Acceptance criteria:**
1. All four `!empty()` assertions are replaced with exact-string `CPPUNIT_ASSERT_EQUAL` checks.
2. Tests still pass (confirms `setIcon` stores the filename verbatim without modification).
3. No other test logic is changed.
4. Full test suite (`cd tests && make && ./SSWTests`) passes with no regressions.

Write your implementer result JSON to `artifacts/gui_sep/milestone1-warning-fixes/T4/implementer_result.json`.
