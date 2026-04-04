Implementation Summary
- Updated `src/tactical/FTacticalGame.cpp` to use a shared path-heading progression helper for both forward-fire highlight generation and moving-player target validation.
- Added runtime tactical regression fixture `tests/tactical/FTacticalForwardFireFinalOrientationTest.{h,cpp}` covering final-turn endpoint orientation, straight-line forward-fire behavior, and non-forward (battery) behavior.

Files Changed
- src/tactical/FTacticalGame.cpp
- tests/tactical/FTacticalForwardFireFinalOrientationTest.h
- tests/tactical/FTacticalForwardFireFinalOrientationTest.cpp

Validation Commands and Outcomes
1. `cd tests/tactical && make && ./TacticalTests` (baseline before edits): PASS (75 tests)
2. `cd tests/tactical && make && ./TacticalTests` (after implementation edits): PASS (75 tests)
3. Standalone runtime fixture validation:
   - Compile and run `FTacticalForwardFireFinalOrientationTest` against `FTacticalGame.o` and model libs: PASS (3 tests)
4. `cd tests/tactical && make && ./TacticalTests` (final regression re-run): PASS (75 tests)

Implementation Commit
- b8335b9d25a25cb1523bddfebb015115013daa63
