### Implementer Report (SSW-SSMR-001)

#### Preflight restatement
- Goal: restore stopped-ship PH_MOVE preview completeness by including the current-heading forward route in the model-owned preview surface.
- Allowed files: `src/tactical/FTacticalGame.cpp` (`include/tactical/FTacticalGame.h` not needed).
- Acceptance criteria: include current-heading preview route when `speed==0`, `nMoved==0`, `MR>0`; keep reverse preview-hex heading lookup consistent; keep ordinary movement semantics unchanged; keep MR==0 preview disabled; preserve non-wx model boundary and preview-click continuation.
- Validation plan: `cd tests && make tactical-tests && ./tactical/TacticalTests` (from approved plan guidance).
- Tester guidance: focus on `tests/tactical/FTacticalMoveRouteSelectionTest.cpp`, `FTacticalGameMechanicsTest.cpp`, `FTacticalBattleBoardRendererDelegationTest.cpp`, and `FTacticalModelSelectionHexClickSurfaceTest.cpp`.

#### Plan step status
1. Preflight scope/constraints review: ✅ complete.
2. Implementation: ✅ complete (single localized edit in tactical model preview generation).
3. Validation: ⚠️ completed with one expected behavior-change failure.
4. Code commit then artifact commit flow: ✅ complete.

#### Files changed
- `src/tactical/FTacticalGame.cpp`

#### Implementation summary
- Updated `FTacticalGame::rebuildStoppedShipPreviewRoutes()` to iterate all six headings without skipping `turnData->curHeading`.
- This restores the missing current-heading forward preview route within the existing model-owned stopped-ship preview contract and automatically adds that route into the per-hex preview-heading lookup map.
- No changes were made to non-stopped movement logic, MR gating (`ship->getMR() > 0`), wx boundaries, or click-flow handling code.

#### Validation commands and outcomes
- `cd tests && make tactical-tests && ./tactical/TacticalTests` → ❌ 1 failure (expected consequence):
  - `FTacticalMoveRouteSelectionTest::testStoppedShipPreviewRoutesExposeLegalStartingFacings`
  - Expected `5` preview routes, actual `6`.

#### Validation triage classification
- Classification: **expected consequence of approved behavior change**.
- Evidence: approved task/plan explicitly requires adding current-heading stopped-ship preview route, increasing the route set from alternates-only to all legal starting facings.
- Tester follow-up: update/add tactical assertions to validate presence of current-heading preview route and heading lookup mapping while preserving MR==0 and ordinary movement behavior.

#### Acceptance criteria coverage
- ✅ Stopped-ship (`speed==0`, `nMoved==0`, `MR>0`) preview now includes current-heading route plus alternates.
- ✅ Preview hex→heading lookup now includes current-heading where that route traverses.
- ✅ No scope expansion to MR==0 preview behavior.
- ✅ Ordinary movement and non-wx tactical model boundary unchanged.
- ✅ Preview-click continuation path remains in existing normal route-selection flow.

#### Commits
- Implementation/code commit: `54988f8a407f2f967840604fb84895e512830ef0`
- Artifact commit: recorded separately in git history (not persisted in result JSON by contract).
