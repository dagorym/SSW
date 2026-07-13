# Implementer Report

Status:
- success

Task summary:
- Resolve finding H7: render the entire tactical scene through a single buffered paint DC and eliminate throwaway wxClientDC usage in the paint path, in both FBattleBoard and FGamePanel.

Changed files:
- include/tactical/FBattleBoard.h
- src/tactical/FBattleBoard.cpp
- include/FGamePanel.h
- src/FGamePanel.cpp

Validation commands run:
- make -C <repo root> all_clean; make -C <repo root> -j4 -- PASS, no errors (SSW, BattleSim, SSWTests, TacticalTests, GuiTests all link)
- make -C <repo root> check -- SSWTests: 245/245 PASS. TacticalTests: 248/253 PASS (5 source-inspection failures in FTacticalBattleBoardRendererDelegationTest, see expected failures). make check stops at the first failing suite so GuiTests was run separately.
- cd tests/gui && make && xvfb-run -a ./GuiTests -- 79/80 PASS (1 failure, see expected failures)
- cd tests/tactical && make && ./TacticalTests -- 248/253 PASS (see expected failures)

Validation outcome:
- Build PASS (make -j4, exit 0, no errors touching FBattleBoard.cpp/.h or FGamePanel.cpp/.h). SSWTests: 245/245 PASS. GuiTests: 79/80 PASS -- the sole failure (testSeekerPathRendersInPHMoveWithMovementPath, tests/gui/TacticalGuiLiveTest.cpp) is an EXPECTED CONSEQUENCE of the approved H7 fix: it seeds a seeker at hex(5,7) (pixel center (350,381)) and asserts zero pixel diff in a 'path-only' band x=[344,356] y=[290,385] during PH_ATTACK_FIRE; that band overlaps the seeker icon's own ~50px bounding box, and the icon (correctly) now renders on the same offscreen wxMemoryDC in every phase (including PH_ATTACK_FIRE's committed-active-seeker branch) instead of silently leaking to a throwaway wxClientDC targeting the real window as it did pre-fix. The exact 390-pixel diff matches the icon/band overlap area, not a path line. TacticalTests: 248/253 PASS -- the 5 failures in FTacticalBattleBoardRendererDelegationTest are pure source-inspection (assertContains on function-body literals) checks for the pre-refactor exact signatures ('drawShips();', 'void FBattleBoard::drawShips()', 'drawCenteredOnHex(*m_seekerMissileIcon, *itr);') that the approved H7 acceptance criteria explicitly require changing to the dc-threaded forms. Per validation-triage these 6 total failures (5 tactical + 1 gui) are classified as expected consequences of the approved behavior change, not regressions; P4-2 allowed files exclude test files so they were not edited.

Implementation/code commit hash:
- b3ee8cee9a0933111016321faca17499e4dab728

Artifacts written:
- artifacts/phase4-tactical-gui-hybrid-cleanup/P4-2/implementer_report.md
- artifacts/phase4-tactical-gui-hybrid-cleanup/P4-2/tester_prompt.txt
- artifacts/phase4-tactical-gui-hybrid-cleanup/P4-2/implementer_result.json

Implementation context:
- FBattleBoard::onPaint (src/tactical/FBattleBoard.cpp) now constructs a wxAutoBufferedPaintDC dc(this) instead of wxPaintDC, and the constructor calls this->SetBackgroundStyle(wxBG_STYLE_PAINT) so the buffered DC is valid; #include <wx/dcbuffer.h> was added.
- drawCenteredOnHex(wxImage img, FPoint p, int rot) is now drawCenteredOnHex(wxDC &dc, wxImage img, FPoint p, int rot = 0); the internal wxClientDC dc(this) line was deleted and every call site now passes the caller's dc as the first argument. No wxClientDC remains anywhere in FBattleBoard.cpp.
- drawShips() is now drawShips(wxDC &dc); draw(wxDC &dc) calls drawShips(dc) and drawCenteredOnHex(dc, m_planetImages[planetChoice], planetPos) instead of the no-arg/no-dc forms, so the planet image and every ship icon render through the same DC as the rest of the scene.
- drawSeekerMissiles(wxDC &dc) previously had an unused '(void)dc;' and called drawCenteredOnHex(*m_seekerMissileIcon, ...) without a dc (which silently drew to a throwaway wxClientDC(this) targeting the real window, bypassing whatever DC draw() was actually called with). The '(void)dc;' line was removed and all 5 drawCenteredOnHex call sites inside drawSeekerMissiles now pass dc through explicitly (PH_SEEKER_ACTIVATION inactive+active branches, PH_ATTACK_FIRE pending+committed branches, and the default active-seeker branch).
- Net effect: onPaint(wxAutoBufferedPaintDC) -> draw(dc) -> {drawGrid(dc), drawCenteredOnHex(dc,...) for the planet, drawShips(dc) -> drawCenteredOnHex(dc,...) per ship, drawSeekerMissiles(dc) -> drawCenteredOnHex(dc,...) per seeker, drawRoute/drawSeekerPaths/drawSeekerMoveCountOverlay/drawTriggeredMineHexes/drawPlacementOrdnanceHexes/drawWeaponRange/drawTarget/drawMinedHexes(dc)} - every scene element renders through the single caller-supplied DC in one pass; ships and seekers no longer bypass the update-region clip/z-order via a separate wxClientDC.
- FGamePanel::onPaint (src/FGamePanel.cpp) now uses a single 'wxPaintDC dc(this);' on all platforms; the '#ifdef LINUX / wxClientDC / #else / wxPaintDC / #endif' branch and the trailing dead commented-out draw block (dc.SetBackground/Clear/SetPen/SetBrush/DrawRectangle) were both deleted. SetClientSize(m_parent->GetClientSize()) is retained.
- Doxygen: full header blocks added/updated on FBattleBoard::draw, ::onPaint, ::drawCenteredOnHex, ::drawShips, ::drawSeekerMissiles (Last Modified + @author, @param dc added where new), plus the FBattleBoard.h file-level and class-level headers. FGamePanel::onPaint doc comment and the FGamePanel class doc comment were updated (Last Modified + @author) to reflect the removed platform branch.
- A real behavioral consequence of this fix (not a defect): seeker icons now correctly render through the same DC as the rest of the scene in every phase where drawSeekerMissiles() draws them, including PH_ATTACK_FIRE's committed-active-seeker branch. Previously that branch's drawCenteredOnHex call silently drew to a throwaway wxClientDC targeting the real on-screen window, so an offscreen wxMemoryDC used by a test never saw the seeker icon there. See the GuiTests expected-failure note below.

Expected validation failures carried forward:
- tests/tactical/FTacticalBattleBoardRendererDelegationTest::testDrawUsesBattleScreenModelAccessors - asserts body contains literal 'drawShips();'. H7 changed the call to 'drawShips(dc);'. Tester must update the literal to match the new signature (this remains a source-contract test; the assertion is only checking delegation wiring, not device-context behavior).
- tests/tactical/FTacticalBattleBoardRendererDelegationTest::testOverlayDrawingUsesModelStateAccessors - asserts extractFunctionBody(source, 'void FBattleBoard::drawShips()') finds a matching signature. H7 changed the signature to 'void FBattleBoard::drawShips(wxDC &dc)'. Tester must update the signature literal used to extract the function body.
- tests/tactical/FTacticalBattleBoardRendererDelegationTest::testDrawShipsUsesTemporaryFacingForStoppedSelectedMover - same 'void FBattleBoard::drawShips()' signature-extraction failure as above, plus asserts body contains 'drawCenteredOnHex(*icon,hex,getRenderedHeadingForShip(m_parent, *itr));' which is now 'drawCenteredOnHex(dc, *icon,hex,getRenderedHeadingForShip(m_parent, *itr));'. Tester must update both literals.
- tests/tactical/FTacticalBattleBoardRendererDelegationTest::testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules - asserts body contains 'drawCenteredOnHex(*m_seekerMissileIcon, *itr);' and 'drawCenteredOnHex(*m_seekerMissileIcon, itr->hex, itr->heading);'. Both now have 'dc, ' prepended as the first argument. Tester must update these literals (and the analogous ones checked by the sibling test below).
- tests/tactical/FTacticalBattleBoardRendererDelegationTest::testDrawSeekerMissilesPendingOffensiveFireBranch - asserts body contains 'drawCenteredOnHex(*m_seekerMissileIcon, *itr);' in the PH_ATTACK_FIRE branch, now 'drawCenteredOnHex(dc, *m_seekerMissileIcon, *itr);'. Tester must update the literal.
- tests/gui/TacticalGuiLiveTest::testSeekerPathRendersInPHMoveWithMovementPath (SMFR-05 AC1 gate) - fails with Expected: 0, Actual: 390 on the PH_ATTACK_FIRE path-band pixel-diff assertion. Root cause: the test's 'path-only' pixel band (x=[344,356], y=[290,385]) overlaps the bounding box of the seeker icon drawn at hex(5,7) (pixel center (350,381)); pre-fix, that icon's drawCenteredOnHex call used an internal wxClientDC(this) that drew to the real window and never touched the offscreen wxMemoryDC bitmap the test compares, so the band appeared path-only by accident of the bug. Post-fix, the icon correctly renders on the same passed-in dc in every phase drawSeekerMissiles is called (including PH_ATTACK_FIRE's committed-active-seeker branch), so the band now also picks up the icon's pixels. This is exactly the acceptance criterion 'Ships, seeker missiles, and centered images render at correct hex positions and rotations' being satisfied where it previously was not. Tester should narrow the pixel band to exclude the seeker icon's bounding box (icon is ~m_size square, i.e. ~50px, centered on hex (5,7) at (350,381)), or change the assertion to check specifically for cyan (#00CCCC) path pixels rather than any pixel diff, so the test isolates drawSeekerPaths() output from drawSeekerMissiles() output. Do not restore any wxClientDC bypass to make this test pass as originally written.
