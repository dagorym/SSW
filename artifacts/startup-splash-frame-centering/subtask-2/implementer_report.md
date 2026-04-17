Implementer Report — subtask 2 (startup splash/frame centering)

Agent activation
- Requested agent: implementer
- Repository-local definition found: no
- Shared definition found: yes (`/home/tstephen/repos/agents/agents/implementer.md`)
- Precedence decision: shared definition used because no repository-local implementer profile exists.
- Workflow obligations followed:
  - stay within allowed file scope plus required artifacts,
  - run preflight + incremental edits + validation,
  - produce two commits (code then artifacts),
  - include Tester handoff prompt and machine-readable result.

Plan step status
1) Preflight scope check — complete
- Goal: center startup splash and initial frame for SSW + BattleSim, preserve simultaneous splash+frame with splash on top, remove fixed SSW startup position, restore full splash default semantics.
- Allowed files used: `src/FApp.cpp`, `src/FMainFrame.cpp`, and extracted helper `include/gui/WXStartupLaunch.h`.
- Tester locations supplied by planner: `tests/gui/StrategicGuiLiveTest.cpp`, `tests/gui/BattleSimGuiLiveTest.cpp`.
- Validation command from planner: `cd tests/gui && make && xvfb-run -a ./GuiTests`.

2) Implementation — complete
- `src/FApp.cpp`
  - replaced legacy fixed startup position `wxPoint(50, 50)` with `wxDefaultPosition`.
- `src/FMainFrame.cpp`
  - added `CentreOnScreen(wxBOTH);` so the initial SSW frame is deterministically centered.
- `include/gui/WXStartupLaunch.h`
  - splash style now includes `wxFRAME_NO_TASKBAR` (`wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP`) to preserve wx splash default top-level/taskbar semantics.
  - centered created frame before show (`frame->CentreOnScreen(wxBOTH);`) for deterministic startup placement in both apps.

3) Validation — complete
Commands run:
- Baseline attempt: `cd tests/gui && make && xvfb-run -a ./GuiTests`
  - result: failed because `xvfb-run` is unavailable in this environment (`command not found`).
- Effective baseline/verification fallback:
  - `cd tests/gui && Xvfb :99 -screen 0 1400x900x24 >/dev/null 2>&1 & echo $!`
  - `cd tests/gui && make && DISPLAY=:99 ./GuiTests`
  - result: pass (`OK (29 tests)`).

Acceptance criteria mapping
- SSW splash centered on screen: yes (`wxSPLASH_CENTRE_ON_SCREEN`).
- SSW initial `FMainFrame` centered on screen: yes (`CentreOnScreen(wxBOTH)` plus helper centering).
- BattleSim splash centered on screen: yes (`wxSPLASH_CENTRE_ON_SCREEN`).
- BattleSim initial `BattleSimFrame` centered on screen: yes (helper centering + existing frame centering).
- Splash and frame may coexist with splash on top: preserved (splash created before frame show; splash remains `wxSTAY_ON_TOP`).
- `FMainFrame` no longer relies on old fixed startup position: yes (`wxPoint(50,50)` removed from startup path).
- Splash style no longer strips expected default splash behavior: yes (`wxFRAME_NO_TASKBAR` restored).
- Deterministic startup placement in both apps: yes (explicit frame centering in shared startup helper).

Files changed
- `include/gui/WXStartupLaunch.h`
- `src/FApp.cpp`
- `src/FMainFrame.cpp`

Implementation commit
- `a5b065e39b0670d2997c48162e03877d92ef2a65`
