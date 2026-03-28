# Milestone 1 — Remove wxImage from Model Classes: Implementation Plan

## Problem Statement

The SSW codebase mixes wxWidgets display types into model classes (`FVehicle`, `FFleet`,
`FPlayer`) and the shared header `Frontier.h`.  Milestone 1 draws the first clean boundary:
after these changes, the `ships` and `strategic` modules contain no wxWidgets types, and all
image loading is concentrated in a new `WXIconCache` singleton in the `gui` module.

---

## Verified Context (Codebase Inspection Results)

The following facts were confirmed by direct file inspection.  Discrepancies from the original
prompt are noted explicitly.

### Confirmed as stated in prompt

| Item | Status |
|------|--------|
| `FVehicle.h` has `wxImage * m_icon`, `getIcon()`, `setIcon()`, `m_iconName`, `#include "wx/wx.h"` | ✅ Confirmed |
| `FVehicle.cpp` constructor allocates `new wxImage(...)`, `setIcon()` calls `LoadFile`, `load()` also creates new wxImage | ✅ Confirmed |
| `FPlayer.h` has `wxImage m_fleetIcon`, `setFleetIcon()`, `m_iconName` | ✅ Confirmed |
| `FPlayer.cpp` `setFleetIcon()` and `load()` call `m_fleetIcon.LoadFile(...)` | ✅ Confirmed |
| `Frontier.h` has `#include "wxWidgets.h"` and `typedef std::vector<wxImage> ImageList` | ✅ Confirmed |
| `WXPlayerDisplay::drawFleets()` calls `getIcon()` | ✅ Confirmed — on **FFleet**, not FVehicle (see below) |
| `FBattleDisplay.cpp:315` calls `vehicle->getIcon()` | ✅ Confirmed |
| `FBattleBoard.cpp:381` calls `ships[k]->getIcon()` | ✅ Confirmed (not mentioned in prompt) |

### Corrections and additions

1. **FFleet also has `wxImage` coupling (not in prompt deliverables).**
   `include/strategic/FFleet.h` declares `wxImage * m_icon` (line 370) and
   `getIcon()` (line 310).  `src/strategic/FFleet.cpp` allocates this image in
   `setIcon()` and `load()`.  Critically, `WXPlayerDisplay::drawFleets()` calls
   `fleet->getIcon()` (on FFleet objects), not `vehicle->getIcon()`.
   FFleet **must** be addressed in Milestone 1 or the GUI caller update cannot compile.

2. **FVehicle has no public `getIconName()` method.**
   `m_iconName` is a protected member with no accessor.  A public getter must be
   added before GUI callers can switch to the WXIconCache pattern.

3. **FFleet has `getIconName()` commented out.**
   Line 312 of `FFleet.h` reads `// const std::string & getIconName() const ...`.
   This must be uncommented so WXPlayerDisplay can call `fleet->getIconName()`.

4. **Tests directly access protected `m_icon`.**
   `tests/ships/FVehicleTest.cpp` defines a `FVehicleDamageHarness` subclass that
   calls `delete m_icon; m_icon = NULL;` in its constructor.  Multiple test files
   also call `vehicle->getIcon() != NULL` as an assertion.  Both patterns break
   when `m_icon` and `getIcon()` are removed.

5. **`ImageList` is used in tactical-module files, not just gui.**
   `include/tactical/FBattleDisplay.h`, `include/tactical/FBattleBoard.h`, and
   `src/tactical/FBattleScreen.cpp` all use `ImageList` via `#include "Frontier.h"`.
   After `ImageList` moves to `GuiTypes.h`, all three files need a new include.

6. **FBattleBoard.cpp is a `getIcon()` caller (not in prompt).**
   `src/tactical/FBattleBoard.cpp:381` retrieves `icon = ships[k]->getIcon()` and
   uses the pointer through the draw loop.  It must be updated alongside
   FBattleDisplay.

---

## Files to Modify

### Model / shared headers (ships + strategic modules)
- `include/ships/FVehicle.h`
- `src/ships/FVehicle.cpp`
- `include/strategic/FFleet.h`
- `src/strategic/FFleet.cpp`
- `include/strategic/FPlayer.h`
- `src/strategic/FPlayer.cpp`
- `include/Frontier.h`

### New files (gui module)
- `include/gui/GuiTypes.h` *(new)*
- `include/gui/WXIconCache.h` *(new)*
- `src/gui/WXIconCache.cpp` *(new)*

### GUI callers
- `src/gui/WXPlayerDisplay.cpp`
- `src/tactical/FBattleDisplay.cpp`
- `src/tactical/FBattleBoard.cpp`

### Tactical headers needing new include
- `include/tactical/FBattleDisplay.h`
- `include/tactical/FBattleBoard.h`
- `src/tactical/FBattleScreen.cpp`

### Tests
- `tests/ships/FVehicleTest.cpp` (FVehicleDamageHarness + getIcon assertions)
- `tests/ships/FTacticalAttackIntegrationTest.cpp` (same harness pattern)
- `tests/ships/FDestroyerTest.cpp`
- `tests/ships/FFortifiedStationTest.cpp`
- `tests/ships/FBattleshipTest.cpp`
- `tests/ships/FHvCruiserTest.cpp`
- `tests/ships/FMinelayerTest.cpp`
- `tests/ships/FLtCruiserTest.cpp`
- `tests/ships/FFrigateTest.cpp`
- `tests/ships/FFighterTest.cpp`
- `tests/ships/FAssaultScoutTest.cpp`

---

## Subtasks

### T1 — Add `getIconName()` public accessors to FVehicle and FFleet

**Depends on:** nothing  
**Blocks:** T4 (GUI caller update)

**Work:**
- In `include/ships/FVehicle.h`, add a public inline method:
  `const std::string & getIconName() const { return m_iconName; }`
- In `include/strategic/FFleet.h`, uncomment line 312:
  `const std::string & getIconName() const { return m_iconFile; }`

**Acceptance criteria:**
- `FVehicle::getIconName()` is declared public and returns `m_iconName` by const reference.
- `FFleet::getIconName()` is declared public and returns `m_iconFile` by const reference.
- No other files are changed in this subtask.
- The codebase compiles without errors or new warnings after this change.

---

### T2 — Remove wxImage from FVehicle

**Depends on:** T1  
**Blocks:** T5 (tests)

**Work:**
- `include/ships/FVehicle.h`:
  - Remove `#include "wx/wx.h"`.
  - Remove `wxImage * m_icon` from the protected section.
  - Remove `const wxImage * getIcon() const` declaration.
  - Keep `setIcon(std::string)` declaration; keep `m_iconName`.
- `src/ships/FVehicle.cpp`:
  - In the constructor: remove `m_icon = new wxImage(gc.getBasePath()+m_iconName);`
  - In the destructor: remove `delete m_icon;`
  - In `setIcon()`: remove `m_icon->LoadFile(...)`.  Keep `m_iconName = icon;`.
  - In `load()`: remove `delete m_icon; m_icon = new wxImage(...);`
  - Remove the `getIcon()` implementation if it has a body (it is inline in the header;
    confirm no separate `.cpp` definition exists).
  - Remove any wx-only `#include` that is no longer needed (FVehicle.cpp includes
    `Frontier.h` which still includes wx for now; only remove a direct wx include if
    one exists at the top of `FVehicle.cpp`).

**Acceptance criteria:**
- `FVehicle.h` contains no `#include "wx/wx.h"` and no `wxImage` symbol.
- `FVehicle.cpp` performs no `wxImage` allocation or loading.
- `setIcon(std::string)` stores only `m_iconName`; it does not touch any image object.
- The `ships` module builds cleanly (run `make` in `src/ships/`).

---

### T3 — Remove wxImage from FFleet

**Depends on:** T1  
**Blocks:** T4 (GUI caller update)

**Work:**
- `include/strategic/FFleet.h`:
  - Remove `wxImage * m_icon` from the private/protected section.
  - Remove `const wxImage * getIcon() const` declaration.
  - Keep `setIcon(std::string)` declaration; keep `m_iconFile`.
- `src/strategic/FFleet.cpp`:
  - In the constructor: remove `m_icon = NULL;` (or whatever initialises the pointer).
  - In the destructor: remove `delete m_icon;`
  - In `setIcon()`: remove the `delete m_icon; m_icon = new wxImage(...)` block.
    Keep `m_iconFile = icon;`.
  - In `load()`: remove `m_icon = new wxImage(gc.getBasePath()+m_iconFile);`

**Acceptance criteria:**
- `FFleet.h` contains no `wxImage` symbol.
- `FFleet.cpp` performs no `wxImage` allocation or loading.
- `setIcon(std::string)` stores only `m_iconFile`; it does not touch any image object.
- The `strategic` module builds cleanly (run `make` in `src/strategic/`).

---

### T4 — Remove wxImage from FPlayer

**Depends on:** nothing (FPlayer.h does not directly include wx; wx enters transitively
through FFleet → FVehicle → wx/wx.h, which T2 removes; however T4 can be coded
independently and will compile once T2 is done)  
**Blocks:** nothing critical (FPlayer has no GUI callers that use its image directly)

**Work:**
- `include/strategic/FPlayer.h`:
  - Remove `wxImage m_fleetIcon` from the private section.
  - `getFleetIcon()` is already commented out; verify it remains commented or remove
    the dead comment block entirely.
  - Keep `setFleetIcon(std::string)` declaration and `m_iconName`.
- `src/strategic/FPlayer.cpp`:
  - In `setFleetIcon()`: remove `m_fleetIcon.LoadFile(...)`.
    Keep `m_iconName = file;` (verify this assignment exists; add it if missing).
  - In `load()`: remove `m_fleetIcon.LoadFile(gc.getBasePath()+m_iconName);`

**Acceptance criteria:**
- `FPlayer.h` contains no `wxImage` symbol.
- `FPlayer.cpp` performs no `wxImage` allocation or loading.
- `setFleetIcon(std::string)` stores only `m_iconName`.
- The `strategic` module builds cleanly (run `make` in `src/strategic/`).

---

### T5 — Create `GuiTypes.h` and clean `Frontier.h`

**Depends on:** nothing  
**Blocks:** T6 (WXIconCache), T7 (tactical include fixes)

**Work:**
- Create `include/gui/GuiTypes.h`:
  ```
  #ifndef _GUITYPES_H_
  #define _GUITYPES_H_

  #include <wx/wx.h>
  #include <vector>

  typedef std::vector<wxImage> ImageList;

  #endif //_GUITYPES_H_
  ```
- `include/Frontier.h`:
  - Remove `#include "wxWidgets.h"`.
  - Remove `typedef std::vector<wxImage> ImageList`.
  - Add a comment indicating `ImageList` is now in `include/gui/GuiTypes.h`.
- Update the three tactical files that depended on `ImageList` via `Frontier.h`:
  - `include/tactical/FBattleDisplay.h` — add `#include "gui/GuiTypes.h"`
  - `include/tactical/FBattleBoard.h` — add `#include "gui/GuiTypes.h"`
  - `src/tactical/FBattleScreen.cpp` — add `#include "gui/GuiTypes.h"` (or it will
    inherit through the headers above; confirm by inspection and add only if needed)

**Acceptance criteria:**
- `include/gui/GuiTypes.h` exists and defines `ImageList`.
- `Frontier.h` no longer includes `wxWidgets.h` and no longer defines `ImageList`.
- All files that previously used `ImageList` compile without errors.
- No file outside the `gui` or `tactical` display headers introduces a new wx include
  as a side-effect of this change.

---

### T6 — Create `WXIconCache` singleton

**Depends on:** T5 (GuiTypes.h for wxImage type; or directly include `<wx/wx.h>`)  
**Blocks:** T7 (GUI caller update)

**Work:**
- Create `include/gui/WXIconCache.h` defining a singleton class in `namespace Frontier`:
  - Private: `std::map<std::string, wxImage> m_cache` (or `unordered_map`).
  - Private default constructor.
  - Public: `static WXIconCache & instance()` returning the single static instance.
  - Public: `const wxImage & get(const std::string & filename)` — looks up
    `filename` in the cache; if absent, loads via `wxImage(filename)`, stores it,
    and returns a const reference to the cached copy.
  - No public destructor beyond the default (cache is process-lifetime).
- Create `src/gui/WXIconCache.cpp` implementing `instance()` and `get()`.
  - `get()` should handle the case where the image fails to load (e.g., log a warning
    and return a default-constructed wxImage rather than crashing).
  - `WXIconCache` is the **only** place in the codebase where `new wxImage(filePath)`
    (or `wxImage(filePath)`) is called for vehicle or fleet icons.
- Add `WXIconCache.cpp` to the gui module's `Makefile` so it is compiled and linked.

**Acceptance criteria:**
- `WXIconCache::instance().get("icons/UPFDestroyer.png")` returns a `const wxImage &`.
- A second call for the same filename returns the same cached object (no second disk read).
- An invalid path returns a valid (possibly empty) wxImage rather than a null pointer or crash.
- The gui module builds cleanly after adding the new source file.

---

### T7 — Update GUI callers to use WXIconCache

**Depends on:** T1, T2, T3, T6  
**Blocks:** T8 (build verification)

**Work:**
- `src/gui/WXPlayerDisplay.cpp`:
  - Add `#include "gui/WXIconCache.h"`.
  - Replace `(*itr)->getIcon()->Scale(...)` with:
    `WXIconCache::instance().get((*itr)->getIconName()).Scale(...)`.
  - The variable `b` receiving the bitmap should be updated accordingly.
- `src/tactical/FBattleDisplay.cpp`:
  - Add `#include "gui/WXIconCache.h"`.
  - Replace:
    ```cpp
    const wxImage *iptr = m_vList[i]->getIcon();
    wxImage im = iptr->Scale(ICON_SIZE, ICON_SIZE);
    ```
    with:
    ```cpp
    wxImage im = WXIconCache::instance().get(m_vList[i]->getIconName())
                                        .Scale(ICON_SIZE, ICON_SIZE);
    ```
- `src/tactical/FBattleBoard.cpp`:
  - Add `#include "gui/WXIconCache.h"`.
  - Replace:
    ```cpp
    const wxImage *icon;
    icon = m_hexData[i][j].ships[k]->getIcon();
    ```
    with:
    ```cpp
    const wxImage & iconRef =
        WXIconCache::instance().get(m_hexData[i][j].ships[k]->getIconName());
    ```
  - Update all subsequent uses of `icon` pointer to use `iconRef` (or `&iconRef`
    where a pointer is required — check usage in `drawCenteredOnHex`).

**Acceptance criteria:**
- No call to `getIcon()` on FVehicle or FFleet remains anywhere in `src/gui/` or
  `src/tactical/`.
- Each updated file compiles without errors.
- At runtime, the strategic map and tactical battle board display ship/fleet icons
  correctly (manual smoke test).

---

### T8 — Update tests that reference `getIcon()` or protected `m_icon`

**Depends on:** T2  
**Blocks:** T9 (run tests)

**Work:**
- `tests/ships/FVehicleTest.cpp`:
  - The `FVehicleDamageHarness` subclass calls `delete m_icon; m_icon = NULL;` in its
    constructor and sets `m_icon = NULL` in its destructor.  Remove both since `m_icon`
    no longer exists.
  - The same pattern likely appears in `FTacticalAttackIntegrationTest.cpp` — apply the
    same fix.
  - For every `CPPUNIT_ASSERT( m_v1->getIcon() != NULL )` assertion:
    - Replace with `CPPUNIT_ASSERT( !m_v1->getIconName().empty() )` to verify that the
      icon name was set (which is now the testable invariant).
  - For `m_v1->setIcon("icons/UPFDestroyer.png"); CPPUNIT_ASSERT(m_v1->getIcon() != NULL)`:
    - Replace the assertion with `CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFDestroyer.png"),
      m_v1->getIconName())`.
- Apply the same `getIcon()` → `getIconName()` assertion replacement to all other
  affected test files listed in the "Files to Modify" section above
  (`FDestroyerTest.cpp`, `FFortifiedStationTest.cpp`, etc.).

**Acceptance criteria:**
- No test file references `getIcon()` or `m_icon` directly.
- All replaced assertions test a meaningful model property (icon name is set and correct).
- The `tests/ships/` module builds cleanly (`make` in `tests/ships/` or `tests/`).

---

### T9 — Build and run tests to confirm no regressions

**Depends on:** T2, T3, T4, T5, T6, T7, T8  
**Blocks:** nothing (milestone complete)

**Work:**
- From the repo root, run `make all_clean && make` and confirm both `SSW` and
  `BattleSim` link successfully.
- `cd tests && make && ./SSWTests`
- All previously passing tests must continue to pass.
- Zero new compiler warnings related to the changed files.

**Acceptance criteria:**
- `make` exits 0 with both executables produced.
- `./SSWTests` reports zero failures and zero errors.
- The ships module, strategic module, and gui module each show no wx-related includes
  outside the gui module (verified with `grep -r "wx/wx.h" include/ships include/strategic`
  returning empty).

---

## Dependency Ordering

```
T1 (add getIconName accessors)
  ├── T2 (remove wxImage from FVehicle)
  │     └── T8 (update tests)
  └── T3 (remove wxImage from FFleet)

T4 (remove wxImage from FPlayer)   [independent; best done with T2/T3]

T5 (GuiTypes.h + Frontier.h)
  └── T6 (WXIconCache)
        └── T7 (update GUI callers)  ← also depends on T1, T2, T3

T9 (build + test) ← depends on all of T2–T8
```

**Parallelism opportunities:**
- T1 and T5 can start simultaneously.
- T2, T3, T4 can all be coded in parallel once T1 is merged.
- T6 can begin as soon as T5 is done, independent of T2–T4.
- T7 requires T1 + T2 + T3 + T6 before it will compile cleanly.
- T8 can be worked alongside T2 (same author, same PR is fine).

---

## Implementer Agent Prompts

---

### Prompt for T1

```
You are the implementer agent.

Files you are allowed to change:
- include/ships/FVehicle.h
- include/strategic/FFleet.h

Task:
Add a public getIconName() accessor to both FVehicle and FFleet.

1. In include/ships/FVehicle.h, add to the public section (near the other getters,
   after getIcon() or where appropriate):
     const std::string & getIconName() const { return m_iconName; }

2. In include/strategic/FFleet.h, uncomment line 312 (the existing commented-out
   getIconName method):
     // const std::string & getIconName() const { return m_iconFile; }
   becomes:
     const std::string & getIconName() const { return m_iconFile; }

Do not change any other code, rename anything, or alter any other method.

Acceptance criteria:
- FVehicle::getIconName() is declared public and returns m_iconName by const reference.
- FFleet::getIconName() is declared public and returns m_iconFile by const reference.
- The codebase compiles without new errors or warnings after this change alone.
```

---

### Prompt for T2

```
You are the implementer agent.

Files you are allowed to change:
- include/ships/FVehicle.h
- src/ships/FVehicle.cpp

Prerequisite: T1 (getIconName accessor) must already be merged.

Task:
Remove all wxImage usage from FVehicle.

In include/ships/FVehicle.h:
- Remove the line: #include "wx/wx.h"
- Remove the declaration: const wxImage * getIcon() const { return m_icon; }
- Remove the protected member: wxImage * m_icon;
- Keep setIcon(std::string), m_iconName, and the new getIconName() unchanged.

In src/ships/FVehicle.cpp:
- In the constructor: remove the line that does m_icon = new wxImage(...)
- In the destructor: remove delete m_icon;
- In setIcon(): remove m_icon->LoadFile(...) (or any image load); keep m_iconName = icon;
- In load(): remove the lines that delete and re-allocate m_icon from the file path.
- Do not remove readString(is, m_iconName) — that must remain.
- If there is any remaining wx-specific #include in FVehicle.cpp that is no longer
  needed (check: FVehicle.cpp currently includes Frontier.h not wx directly), remove it
  only if it is exclusively used for wxImage.

Acceptance criteria:
- FVehicle.h contains no wxImage symbol and no wx include.
- FVehicle.cpp performs no wxImage allocation or image file loading.
- setIcon(std::string) stores only m_iconName and does nothing else.
- `make` in src/ships/ exits 0 with no new errors.
```

---

### Prompt for T3

```
You are the implementer agent.

Files you are allowed to change:
- include/strategic/FFleet.h
- src/strategic/FFleet.cpp

Prerequisite: T1 (getIconName accessor) must already be merged.

Task:
Remove all wxImage usage from FFleet.

In include/strategic/FFleet.h:
- Remove: const wxImage * getIcon() const { return m_icon; }
- Remove: wxImage * m_icon; from the private/protected member list.
- Keep setIcon(std::string), m_iconFile, and the newly uncommented getIconName().

In src/strategic/FFleet.cpp:
- In the constructor: remove m_icon = NULL; (or similar initialization of m_icon).
- In the destructor: remove delete m_icon;
- In setIcon(): remove the delete m_icon and m_icon = new wxImage(...) lines.
  Keep m_iconFile = icon;
- In load(): remove m_icon = new wxImage(gc.getBasePath()+m_iconFile);
  Keep readString(is, m_iconFile).

Acceptance criteria:
- FFleet.h contains no wxImage symbol.
- FFleet.cpp performs no wxImage allocation or image file loading.
- setIcon(std::string) stores only m_iconFile and does nothing else.
- `make` in src/strategic/ exits 0 with no new errors.
```

---

### Prompt for T4

```
You are the implementer agent.

Files you are allowed to change:
- include/strategic/FPlayer.h
- src/strategic/FPlayer.cpp

Task:
Remove all wxImage usage from FPlayer.

In include/strategic/FPlayer.h:
- Remove: wxImage m_fleetIcon; from the private member list.
- The method getFleetIcon() is already commented out — leave that comment in place
  or remove the dead comment block cleanly; do not re-introduce a live getFleetIcon().
- Keep setFleetIcon(std::string) declaration and m_iconName.

In src/strategic/FPlayer.cpp:
- In setFleetIcon(): remove m_fleetIcon.LoadFile(gc.getBasePath()+m_iconName);
  Ensure m_iconName = file; is present (add it if missing).
- In load(): remove m_fleetIcon.LoadFile(gc.getBasePath()+m_iconName);
  The readString call that populates m_iconName must remain.
- If the wxImage type is only referenced for m_fleetIcon and there is no other wx
  usage in FPlayer.cpp, remove the wx include if one exists directly in that file
  (FPlayer.h does not directly include wx; double-check FPlayer.cpp's includes).

Acceptance criteria:
- FPlayer.h contains no wxImage symbol.
- FPlayer.cpp performs no wxImage LoadFile or construction for the fleet icon.
- setFleetIcon(std::string) updates m_iconName and does nothing else.
- `make` in src/strategic/ exits 0 with no new errors.
```

---

### Prompt for T5

```
You are the implementer agent.

Files you are allowed to change (create or modify):
- include/gui/GuiTypes.h  [NEW FILE]
- include/Frontier.h
- include/tactical/FBattleDisplay.h
- include/tactical/FBattleBoard.h
- src/tactical/FBattleScreen.cpp

Task:
Move ImageList out of Frontier.h into a new GuiTypes.h.

1. Create include/gui/GuiTypes.h with these contents:
   - An include guard (#ifndef _GUITYPES_H_ / #define / #endif).
   - #include <wx/wx.h>
   - #include <vector>
   - typedef std::vector<wxImage> ImageList;

2. In include/Frontier.h:
   - Remove the line: #include "wxWidgets.h"
   - Remove the line: typedef std::vector<wxImage> ImageList;
   - Do not change any other content (enums, inline functions, etc.).

3. In include/tactical/FBattleDisplay.h:
   - Add: #include "gui/GuiTypes.h"

4. In include/tactical/FBattleBoard.h:
   - Add: #include "gui/GuiTypes.h"

5. In src/tactical/FBattleScreen.cpp:
   - Inspect whether ImageList is already inherited via the headers above or whether
     a direct include is needed.  Add #include "gui/GuiTypes.h" only if a direct
     include is required for it to compile.

Acceptance criteria:
- include/gui/GuiTypes.h exists and correctly typedefs ImageList.
- Frontier.h no longer includes wxWidgets.h and no longer defines ImageList.
- All files that use ImageList compile without errors.
- No source file outside include/gui/ or include/tactical/ gains a new wx dependency
  as a side-effect of this change.
- `make` at repo root exits 0.
```

---

### Prompt for T6

```
You are the implementer agent.

Files you are allowed to create or change:
- include/gui/WXIconCache.h  [NEW FILE]
- src/gui/WXIconCache.cpp  [NEW FILE]
- src/gui/Makefile  (add WXIconCache.cpp to the build)

Prerequisite: T5 (GuiTypes.h) must already exist.

Task:
Create a WXIconCache singleton in the gui module.

WXIconCache is the single authoritative location in the codebase where wxImage
objects for vehicle and fleet icons are constructed.

Requirements:
- Declare and implement the class in namespace Frontier.
- Private constructor (singleton — prevent direct instantiation).
- Private: std::map<std::string, wxImage> m_cache;
- Public static: WXIconCache & instance() — returns the single static instance.
- Public: const wxImage & get(const std::string & filename)
    - If filename is already in m_cache, return the cached wxImage by const ref.
    - If not, construct wxImage(filename), insert it into m_cache, return const ref.
    - If the loaded wxImage is not ok (IsOk() == false), log a std::cerr warning
      and still insert the (empty) image; do not throw or crash.
- WXIconCache must not be copyable or assignable (delete copy constructor and
  assignment operator).

Update src/gui/Makefile to include WXIconCache.cpp in the SOURCES or equivalent
variable so it is compiled into the gui static library.

Acceptance criteria:
- WXIconCache::instance().get("icons/UPFDestroyer.png") returns a const wxImage &.
- A second call with the same path does not reload from disk (cache hit).
- An invalid path returns a valid wxImage reference (IsOk() == false) without crashing.
- The gui module builds cleanly: `make` in src/gui/ exits 0.
```

---

### Prompt for T7

```
You are the implementer agent.

Files you are allowed to change:
- src/gui/WXPlayerDisplay.cpp
- src/tactical/FBattleDisplay.cpp
- src/tactical/FBattleBoard.cpp

Prerequisites: T1, T2, T3, T6 must all be merged first.

Task:
Replace all calls to getIcon() on FVehicle and FFleet objects with
WXIconCache::instance().get(object->getIconName()).

1. src/gui/WXPlayerDisplay.cpp:
   - Add: #include "gui/WXIconCache.h"
   - In drawFleets(), find: wxBitmap b((*itr)->getIcon()->Scale((int)scale,(int)scale));
   - Replace with:
       wxBitmap b(WXIconCache::instance().get((*itr)->getIconName())
                               .Scale((int)scale, (int)scale));

2. src/tactical/FBattleDisplay.cpp:
   - Add: #include "gui/WXIconCache.h"
   - Find:
       const wxImage *iptr = m_vList[i]->getIcon();
       wxImage im = iptr->Scale(ICON_SIZE, ICON_SIZE);
   - Replace with:
       wxImage im = WXIconCache::instance().get(m_vList[i]->getIconName())
                                           .Scale(ICON_SIZE, ICON_SIZE);

3. src/tactical/FBattleBoard.cpp:
   - Add: #include "gui/WXIconCache.h"
   - Find (around line 379-382):
       const wxImage *icon;
       icon = m_hexData[i][j].ships[k]->getIcon();
   - Replace with:
       const wxImage & iconRef =
           WXIconCache::instance().get(m_hexData[i][j].ships[k]->getIconName());
   - Update all subsequent uses of the `icon` pointer variable within that scope
     to use iconRef (use &iconRef wherever a pointer is required).

Acceptance criteria:
- grep -r "getIcon()" src/gui/ src/tactical/ returns no results (ignoring comments).
- All three files compile without errors.
- `make` at repo root exits 0.
```

---

### Prompt for T8

```
You are the implementer agent.

Files you are allowed to change:
- tests/ships/FVehicleTest.cpp
- tests/ships/FTacticalAttackIntegrationTest.cpp
- tests/ships/FDestroyerTest.cpp
- tests/ships/FFortifiedStationTest.cpp
- tests/ships/FBattleshipTest.cpp
- tests/ships/FHvCruiserTest.cpp
- tests/ships/FMinelayerTest.cpp
- tests/ships/FLtCruiserTest.cpp
- tests/ships/FFrigateTest.cpp
- tests/ships/FFighterTest.cpp
- tests/ships/FAssaultScoutTest.cpp

Prerequisite: T2 (wxImage removed from FVehicle) must be merged.

Task:
Update all tests that reference getIcon() or the protected m_icon member.

1. In FVehicleTest.cpp, the inner class FVehicleDamageHarness has:
     delete m_icon;
     m_icon = NULL;
   in its constructor, and m_icon = NULL in its destructor.
   Remove both lines since m_icon no longer exists in FVehicle.
   Apply the same fix in FTacticalAttackIntegrationTest.cpp if the same pattern appears.

2. For every assertion of the form:
     CPPUNIT_ASSERT( someVehicle->getIcon() != NULL );
   Replace with:
     CPPUNIT_ASSERT( !someVehicle->getIconName().empty() );

3. For any assertion that tests the result of setIcon() via getIcon():
     m_v1->setIcon("icons/UPFDestroyer.png");
     CPPUNIT_ASSERT(m_v1->getIcon() != NULL);
   Replace the assertion with:
     CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFDestroyer.png"), m_v1->getIconName());

Acceptance criteria:
- No test file references getIcon() or m_icon directly.
- All replacement assertions test a meaningful model property.
- `make` in tests/ exits 0 with no new errors.
- `./SSWTests` reports zero failures and zero errors.
```

---

### Prompt for T9

```
You are the implementer agent.

Files you are allowed to change: none (verification only).

Prerequisites: T2, T3, T4, T5, T6, T7, T8 must all be merged.

Task:
Build the full project and run the complete test suite to confirm no regressions.

Steps:
1. From the repo root: make all_clean && make
   Both SSW and BattleSim executables must be produced with exit code 0.
2. cd tests && make && ./SSWTests
   All tests must pass with zero failures and zero errors.
3. Verify no wx includes have leaked into non-gui modules:
     grep -r "wx/wx.h" include/ships/ include/strategic/
   This must return no results.
4. Report the full test output.

Acceptance criteria:
- make exits 0; both executables present.
- ./SSWTests exits 0 with zero failures.
- grep for wx/wx.h in ships and strategic headers returns empty.
```

---

## Risk Summary

| Risk | Likelihood | Mitigation |
|------|-----------|------------|
| FFleet not in original deliverables but required | High (already identified) | T3 explicitly addresses it |
| Tests break on m_icon access | High (already identified) | T8 explicitly addresses it |
| ImageList consumers in tactical module | Medium (already identified) | T5 explicitly updates those headers |
| FVehicle.cpp wx dependency via Frontier.h | Low | Frontier.h loses its wx include in T5; FVehicle.cpp only needs it for wxImage which is removed in T2 |
| WXIconCache base path for icon loading | Low | Callers pass the full filename as stored in m_iconName (check if FGameConfig path prefix is needed — FVehicle.cpp previously used `gc.getBasePath()+m_iconName`; WXIconCache::get() may need to apply the same prefix) |

> **Note on base path:** The existing code loads images with `gc.getBasePath() + m_iconName`
> (FGameConfig prefix + relative path).  `WXIconCache::get()` must apply the same prefix.
> The implementer for T6 should inspect `FGameConfig` to determine how the path is
> constructed and replicate the same logic in `WXIconCache::get()`, or accept a
> fully-qualified path from callers and have T7 pass `gc.getBasePath() + vehicle->getIconName()`.
