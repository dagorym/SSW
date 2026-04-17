Implementer Report - Subtask 1 (Startup splash/frame seam extraction)

Plan step status:
- Preflight scope check: completed
- Startup seam extraction for SSW app: completed
- Startup seam extraction for BattleSim app: completed
- Post-change validation: completed
- Code commit: completed

Task goal:
- Extract startup splash-and-initial-frame creation logic from `FApp::OnInit()` and `FBattleSimApp::OnInit()` into a small testable seam, preserving splash/frame concurrent startup behavior.

Allowed/modified files:
- include/gui/WXStartupLaunch.h (new helper seam)
- src/FApp.cpp
- src/FBattleSimApp.cpp

Acceptance criteria coverage:
- SSW startup path now delegates splash+frame creation to `createStartupSplashAndFrame(...)`.
- BattleSim startup path now delegates splash+frame creation to `createStartupSplashAndFrame(...)`.
- `FApp::OnInit()` and `FBattleSimApp::OnInit()` no longer duplicate inline splash/frame setup logic.
- Helper still creates splash first, then creates/shows frame and sets top window, preserving concurrent splash/frame behavior.

Validation commands run:
1) Baseline:
   - `cd tests/gui && make && xvfb-run -a ./GuiTests`
   - Outcome: failed because `xvfb-run` not installed in environment.
   - Fallback run: `cd tests/gui && ./GuiTests` -> passed (`OK (29 tests)`).
2) After implementation:
   - `cd src && make` -> passed.
   - `cd tests/gui && make && (xvfb-run -a ./GuiTests || ./GuiTests)` -> passed (`OK (29 tests)` via fallback).
3) Final re-validation after formatting adjustment:
   - `cd src && make` -> passed.
   - `cd tests/gui && make && (xvfb-run -a ./GuiTests || ./GuiTests)` -> passed (`OK (29 tests)` via fallback).

Implementation/code commit:
- `950b687e3c5a1176bfdb561f95f3ed5113455ef3`

Notes for Tester:
- Suggested test targets from plan: `tests/gui/StrategicGuiLiveTest.cpp`, `tests/gui/BattleSimGuiLiveTest.cpp`, `tests/gui/Makefile`.
- Shared artifact directory to reuse: `artifacts/startup-splash-frame-centering/subtask-1`.
