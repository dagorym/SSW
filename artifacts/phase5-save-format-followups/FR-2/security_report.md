Security Review Report

Scope reviewed:
- FR-2 pass-2 remediation (closes deferred item SF-nullfmap-paint-guard) on branch phase5-FR-2-security-20260719, reviewing the second-pass diff that adds the residual NULL/half-built FMap guard to WXPlayerDisplay::drawFleets().
- In-scope changed files (pass-2): src/gui/WXPlayerDisplay.cpp (new FMap::hasMap() early-return guard + strategic/FMap.h include), include/gui/WXPlayerDisplay.h (doxygen), AGENTS.md (guard-coverage note), tests/gui/StrategicGuiLiveTest.{h,cpp} (new behavioral test testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash).
- Re-reviewed as retained context: pass-1 FMap::hasMap() predicate (src/strategic/FMap.cpp:36-38) and the WXMapDisplay::draw()/getScale() guards (src/gui/WXMapDisplay.cpp:34-36, 96-98), and the full WXGameDisplay::draw() paint chain (src/gui/WXGameDisplay.cpp).
- Full paint chain enumerated from WXGameDisplay::draw(): (1) WXMapDisplay::draw() -> internally getScale(); (2) per-player WXPlayerDisplay::drawFleets(); (3) drawTurnCounter().
- The FFleet/FPlayer nested-load changes present in the branch diff vs the plan base belong to FR-1 (already security-reviewed and passed) and are out of FR-2 scope; FMainFrame.cpp is not in the changed set.

Why specialist review was triggered:
- A corrupt or malicious save file can leave a live FGame whose FMap singleton was never built (m_map == NULL / half-built). A repaint reaching the strategic gui draw chain then dereferences a null reference, an availability / NULL-deref crash reachable in principle from untrusted input. FR-2's goal is to make the gui draw chain root-cause-complete against a NULL FMap rather than relying on implicit ordering invariants.
- Pass-1 returned CONDITIONAL PASS with one WARNING: the pass-1 guard covered only the WXMapDisplay path, while WXGameDisplay::draw() reaches FMap via a second, unguarded path -- WXPlayerDisplay::drawFleets(). This pass-2 review verifies that residual same-chain gap is closed.

Acceptance criteria / plan reference:
- plans/phase5-save-format-followups-plan.md (FR-2), which closes deferred item SF-nullfmap-paint-guard from doc/deferred-tasks.md.
- Pass-1 security finding (artifacts/phase5-save-format-followups/FR-2/history/pass-1/security_report.md) and the Verifier confirmation of the drawFleets WARNING that drove this remediation.

Findings

BLOCKING
- None

WARNING
- None

NOTE
- src/gui/WXPlayerDisplay.cpp:100 - After the FMap exists, drawFleets() dereferences map->getSystem(location)->getCoord(...) without a NULL check on the returned FSystem*; a corrupt save carrying an out-of-range fleet location ID would make getSystem() return NULL and crash here (same shape in WXMapDisplay for jump/system lists).
  This is a DIFFERENT failure mode than SF-nullfmap-paint-guard (a valid FMap that cannot resolve an untrusted ID, not a NULL FMap). It is pre-existing, not introduced or regressed by FR-2, and correctly outside FR-2's stated scope (NULL/half-built FMap). Flagged only so it can be tracked as a separate save-input-validation follow-up; it does not block this pass.
- src/gui/WXPlayerDisplay.cpp:42-48 - drawFleets() checks FMap::hasMap() and then calls mapDisplay.getScale(dc), which re-checks hasMap() internally. The double-check is redundant but harmless and safe (both are cheap, side-effect-free, single-threaded).
  No correctness or security impact; noted only for completeness. No action required.

Test sufficiency assessment:
- SUFFICIENT and behavioral. testWXPlayerDisplayDrawFleetsWithNoLiveFMapDoesNotCrash (tests/gui/StrategicGuiLiveTest.cpp) constructs a real (non-mock) FPlayer owning a real FFleet at a nonzero, non-transit getLocation() -- exactly the 'it's in a system' branch that previously bound `FMap *map = &(FMap::getMap())` against a NULL m_map and dereferenced it via map->getSystem(...) -- forces FMap::hasMap() false, and calls drawFleets() directly on an offscreen wxMemoryDC. Reaching the final assertions is the behavioral proof the guard short-circuits before any dereference; this test would crash against the unfixed code, satisfying the repo behavioral-verification mandate.
- The test is registered via CPPUNIT_TEST in StrategicGuiLiveTest.h and defensively tears down any process-static FMap a prior test may have left live.
- Verified locally: GUI suite builds clean and runs green under xvfb (OK (85 tests), up from 84), closing the pass-1 GAP note (fleet-bearing no-map coverage now exists). Sibling testGamePanelRepaintWithNoLiveFMapDoesNotCrash continues to cover the WXMapDisplay path.

Documentation / operational guidance assessment:
- SUFFICIENT and now accurate. The AGENTS.md null-FMap paint note is corrected to state the gui paint path reached from FGamePanel::onPaint() -> WXGameDisplay::draw() is root-cause-complete across BOTH FMap-touching call paths (WXMapDisplay::draw()/getScale() and WXPlayerDisplay::drawFleets()); the pass-1 OVERSTATEMENT is resolved.
- The WXPlayerDisplay.h doxygen accurately documents the hasMap() guard as additive defense-in-depth with no behavior change when a valid FMap exists, and names the guarded ordering invariant. The P5-5 FMainFrame::onOpen setGame-deferral mitigation description is explicitly preserved and unchanged, and doc/deferred-tasks.md correctly keeps SF-nullfmap-paint-guard removed (now fully closed).

Artifacts written:
- artifacts/phase5-save-format-followups/FR-2/security_report.md
- artifacts/phase5-save-format-followups/FR-2/security_result.json

Outcome:
- PASS
