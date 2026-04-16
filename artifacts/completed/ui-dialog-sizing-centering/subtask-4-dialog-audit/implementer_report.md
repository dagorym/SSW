Implementer report

Plan step status:
- Preflight scope check: completed (allowed files + AC + validation command confirmed)
- Audit + remediation: completed for SatharRetreatGUI, CombatLocationGUI, TwoPlanetsGUI, SelectResolutionGUI
- Regression coverage: completed in tests/gui/StrategicGuiLiveTest.cpp
- Validation: completed
- Commit handling: code commit completed; artifact commit pending

Files changed:
- src/gui/SatharRetreatGUI.cpp
- src/gui/CombatLocationGUI.cpp
- src/gui/TwoPlanetsGUI.cpp
- src/gui/SelectResolutionGUI.cpp
- tests/gui/StrategicGuiLiveTest.cpp

Implementation summary:
- Replaced legacy SetSizer()+Layout()+Fit() calls in the four remediated strategic dialogs with SetSizerAndFit().
- Locked minimum size to first-show computed size via SetMinSize(GetSize()).
- Centered each remediated dialog with Centre(wxBOTH) for consistent first-show placement.
- Added deterministic live GUI assertions that key action buttons remain fully inside client area for:
  - SatharRetreatGUI (Done button)
  - CombatLocationGUI (Around the Planet, In Deep Space)
  - TwoPlanetsGUI (both planet-selection buttons)
  - SelectResolutionGUI (To the Battle Board!, Enter Combat Results)

Validation commands run:
- Baseline: cd tests/gui && make && xvfb-run -a ./GuiTests (failed: xvfb-run missing in environment)
- Baseline fallback: cd tests/gui && ./GuiTests (passed: OK 27 tests)
- Post-change: cd tests/gui && make && ./GuiTests (passed: OK 27 tests)

Validation outcome:
- PASS (no regressions in GUI suite)
