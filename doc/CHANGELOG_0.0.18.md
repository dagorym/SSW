# Version 0.0.18 Change Log

This change log summarizes the substantive repository changes made after version `0.0.17`, using commit `6d35096a2174b6655976ec74155ce45f69baaca5` from February 7, 2018 as the comparison base. Most of the user-visible work landed during the March-May 2026 development restart.

## User-Facing Changes

- Tactical combat now produces a much more informative damage summary dialog after fire resolution. The summary rolls damage up by ship, preserves weapon and defense hit identities, includes electrical fire and mine results, can show optional hit-detail lines, and now reports ADF and MR losses explicitly instead of collapsing them into vague effect text.
- Damage-resolution edge cases were tightened up to match the tactical rules more closely. Hull fallback handling was expanded for cases such as advanced damage hits, repeated damage-control repeat hits, and disastrous fire outcomes, reducing incorrect or misleading damage reporting.
- Stopped ships are easier to maneuver in tactical combat. The UI now supports stopped-ship facing preselection, free rotation before movement, preview-route rendering, and direct clicking on highlighted preview-route hexes to continue movement selection.
- Tactical station setup and movement behavior were corrected. Repositioning a station during setup no longer duplicates pending placement state, and station orbital movement now preserves the intended heading and speed when movement is finalized.
- Tactical screen layout and readability improved substantially. The movement and fire prompts no longer crowd or overlap action buttons, the lower panel reflows more reliably as the window changes size, ship-stat presentation is cleaner, and the tactical speed and ICM controls now size and render more consistently on wxGTK.
- Tactical dialogs are more polished and predictable. The damage summary dialog has a safer close path and better default focus behavior, while tactical, strategic, and BattleSim dialogs were reworked to fit and center correctly on first show.
- Startup and launch behavior is cleaner. The splash screen and initial frames are now centered consistently, and BattleSim launch dialogs and scenario flows received follow-up sizing and lifecycle fixes.
- Image and icon loading is more reliable across installation layouts. Shared asset-resolution logic now handles normalized paths and fallback lookup more consistently, which reduces missing-image failures in startup, tactical, and strategic displays.

## Development-Facing Changes

- The largest internal change was the tactical/strategic UI separation work. `IStrategicUI` and `ITacticalUI` were introduced as model-facing boundaries, with `WXStrategicUI` and `WXTacticalUI` acting as wx-backed adapters instead of letting core game logic depend directly on wx dialogs and windows.
- Tactical combat logic was heavily refactored around the additive `FTacticalGame` model surface. Tactical state ownership, reporting, selection, movement, fire progression, and cleanup seams were moved out of ad hoc wx-owned paths and into a more testable pure-C++ model layer.
- GUI dependencies were pushed out of non-GUI model code. wx image ownership was removed from `FVehicle`, `FFleet`, and `FPlayer`; `WXIconCache` and `GuiTypes.h` were added; and the non-GUI module Makefiles were cleaned up so they no longer actively depend on `wx-config`.
- Build and platform support were modernized. The Makefiles were refreshed, coverage instrumentation was gated behind `COVERAGE=1`, test build ordering was fixed, dedicated tactical and GUI runner targets were added, and Visual Studio 2022 project files were introduced or refreshed for current Windows builds.
- Automated validation expanded dramatically. The repository now has a dedicated GUI test harness and GUI runner, broader live-dialog and offscreen rendering coverage, far deeper tactical regression coverage, strategic mock-UI tests, core lifecycle checks, and additional weapon/ship integration tests.
- Several infrastructure defects that affected maintenance and correctness were addressed, including `FGameConfig` singleton hardening, a reusable asset-path resolver, `FFleet` sentinel normalization and non-copyable enforcement, tactical battery-range clamping fixes, and proper nonzero exit handling in `SSWTests`.
- Documentation was modernized alongside the code. The user guide and design notes were converted to Markdown and updated to reflect the current architecture and UI behavior, and repository process guidance was formalized through `AGENTS.md` and the `.myteam/` workflow files.
