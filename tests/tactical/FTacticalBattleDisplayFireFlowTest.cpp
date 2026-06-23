/**
 * @file FTacticalBattleDisplayFireFlowTest.cpp
 * @brief Implementation file for the FTacticalBattleDisplayFireFlowTest class
 */

#include "FTacticalBattleDisplayFireFlowTest.h"

#include <fstream>
#include <iterator>
#include <vector>

#include "core/FHexMap.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"
#include "weapons/FSeekerMissileLauncher.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

class FFireFlowWeaponHarness : public FWeapon {
public:
FFireFlowWeaponHarness() {
m_name = "TST";
m_fullName = "Fire Flow Harness";
m_type = FWeapon::LC;
m_range = 9;
m_nDice = 1;
m_dMod = 0;
m_baseToHitProb = 100;
}

void aimAt(FVehicle * target) {
m_target = target;
m_targetRange = 1;
m_isHeadOn = false;
}
};

class FFireFlowShipHarness : public FVehicle {
public:
FFireFlowShipHarness() {
m_name = "FireFlowShip";
m_type = "FireFlowHarness";
}

void configureForOneShotKill() {
m_maxHP = 1;
m_currentHP = 1;
m_maxADF = 1;
m_currentADF = 1;
m_maxMR = 1;
m_currentMR = 1;
m_maxDCR = 1;
m_currentDCR = 1;
}

void addWeapon(FWeapon * weapon) {
m_weapons.push_back(weapon);
weapon->setParent(this);
}
};

struct FireFlowRuntimeFixture {
FTacticalGame game;
FleetList attackFleets;
FleetList defendFleets;
FFleet * attackFleet;
FFleet * defendFleet;
FFireFlowShipHarness * attacker;
FFireFlowShipHarness * defender;
FFireFlowWeaponHarness * weapon;
};

void destroyFixture(FireFlowRuntimeFixture & fixture) {
for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
delete *itr;
}
for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
delete *itr;
}
fixture.attackFleets.clear();
fixture.defendFleets.clear();
}

void setupOneKillScenario(FireFlowRuntimeFixture & fixture, int firePhase) {
fixture.attackFleet = new FFleet();
fixture.defendFleet = new FFleet();
fixture.attackFleets.push_back(fixture.attackFleet);
fixture.defendFleets.push_back(fixture.defendFleet);

fixture.attacker = new FFireFlowShipHarness();
fixture.defender = new FFireFlowShipHarness();
fixture.attacker->setName("Attacker");
fixture.defender->setName("Defender");
fixture.attacker->configureForOneShotKill();
fixture.defender->configureForOneShotKill();

fixture.weapon = new FFireFlowWeaponHarness();
fixture.weapon->aimAt(fixture.defender);
fixture.attacker->addWeapon(fixture.weapon);

fixture.attackFleet->addShip(fixture.attacker);
fixture.defendFleet->addShip(fixture.defender);

fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
fixture.game.setPhase(firePhase);
fixture.game.setActivePlayer(true);
}

class FSeekerFlowShipHarness : public FVehicle {
public:
FSeekerFlowShipHarness(const std::string & name) {
	m_name = name;
	m_type = "SeekerFlowHarness";
}

void configureForMovement(int speed, int adf, int heading) {
	m_maxHP = 10;
	m_currentHP = 10;
	m_maxADF = adf;
	m_currentADF = adf;
	m_maxMR = 1;
	m_currentMR = 1;
	m_maxDCR = 1;
	m_currentDCR = 1;
	m_speed = speed;
	m_heading = heading;
}

void addWeapon(FWeapon * weapon) {
	m_weapons.push_back(weapon);
	weapon->setParent(this);
}
};

struct SeekerFlowRuntimeFixture {
FTacticalGame game;
FleetList attackFleets;
FleetList defendFleets;
FFleet * attackFleet;
FFleet * defendFleet;
FSeekerFlowShipHarness * attacker;
FSeekerFlowShipHarness * defender;
FSeekerMissileLauncher * launcher;
FPoint attackerHex;
FPoint defenderHex;
};

void destroySeekerFixture(SeekerFlowRuntimeFixture & fixture) {
for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
	delete *itr;
}
for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
	delete *itr;
}
fixture.attackFleets.clear();
fixture.defendFleets.clear();
}

void placeShip(FTacticalGame & game, FVehicle * ship, const FPoint & hex, int heading, int state) {
game.setState(state);
game.setControlState(true);
game.setShip(ship);
CPPUNIT_ASSERT(game.placeShip(hex));
CPPUNIT_ASSERT(game.setShipPlacementHeading(heading));
}

void setupSeekerFlowRuntimeScenario(SeekerFlowRuntimeFixture & fixture) {
fixture.attackFleet = new FFleet();
fixture.defendFleet = new FFleet();
fixture.attackFleets.push_back(fixture.attackFleet);
fixture.defendFleets.push_back(fixture.defendFleet);

fixture.attacker = new FSeekerFlowShipHarness("SeekerAttacker");
fixture.defender = new FSeekerFlowShipHarness("SeekerDefender");
fixture.attacker->configureForMovement(2, 0, 0);
fixture.defender->configureForMovement(0, 0, 3);

fixture.launcher = new FSeekerMissileLauncher();
fixture.launcher->setMaxAmmo(3);
fixture.launcher->setCurrentAmmo(3);
fixture.attacker->addWeapon(fixture.launcher);

fixture.attackFleet->addShip(fixture.attacker);
fixture.defendFleet->addShip(fixture.defender);

fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);
fixture.attackerHex = FPoint(20, 20);
fixture.defenderHex = FPoint(10, 10);
placeShip(fixture.game, fixture.defender, fixture.defenderHex, 3, BS_SetupDefendFleet);
placeShip(fixture.game, fixture.attacker, fixture.attackerHex, 0, BS_SetupAttackFleet);
fixture.game.setState(BS_Battle);
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleDisplayFireFlowTest );

void FTacticalBattleDisplayFireFlowTest::setUp() {
}

void FTacticalBattleDisplayFireFlowTest::tearDown() {
}

std::string FTacticalBattleDisplayFireFlowTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalBattleDisplayFireFlowTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT(signaturePos != std::string::npos);

std::string::size_type bodyStart = source.find('{', signaturePos);
CPPUNIT_ASSERT(bodyStart != std::string::npos);

int depth = 1;
for (std::string::size_type i = bodyStart + 1; i < source.size(); i++) {
if (source[i] == '{') {
depth++;
} else if (source[i] == '}') {
depth--;
if (depth == 0) {
return source.substr(bodyStart + 1, i - bodyStart - 1);
}
}
}

CPPUNIT_FAIL("Function body not terminated");
return "";
}

void FTacticalBattleDisplayFireFlowTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalBattleDisplayFireFlowTest::assertNotContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected not to find '") + needle + "' in inspected source",
haystack.find(needle) == std::string::npos);
}

void FTacticalBattleDisplayFireFlowTest::assertBefore(const std::string & haystack, const std::string & first, const std::string & second) {
const std::string::size_type firstPos = haystack.find(first);
const std::string::size_type secondPos = haystack.find(second);
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find both source fragments in order: '") + first + "' before '" + second + "'",
firstPos != std::string::npos && secondPos != std::string::npos && firstPos < secondPos);
}

unsigned int FTacticalBattleDisplayFireFlowTest::countOccurrences(const std::string & haystack, const std::string & needle) {
unsigned int count = 0;
std::string::size_type pos = 0;
while ((pos = haystack.find(needle, pos)) != std::string::npos) {
++count;
pos += needle.size();
}
return count;
}

std::string stripCppComments(const std::string & source) {
	std::string result;
	result.reserve(source.size());
	bool inLineComment = false;
	bool inBlockComment = false;

	for (std::string::size_type i = 0; i < source.size(); ++i) {
		if (inLineComment) {
			if (source[i] == '\n') {
				inLineComment = false;
				result.push_back('\n');
			}
			continue;
		}
		if (inBlockComment) {
			if (source[i] == '*' && (i + 1) < source.size() && source[i + 1] == '/') {
				inBlockComment = false;
				++i;
			}
			continue;
		}
		if (source[i] == '/' && (i + 1) < source.size() && source[i + 1] == '/') {
			inLineComment = true;
			++i;
			continue;
		}
		if (source[i] == '/' && (i + 1) < source.size() && source[i + 1] == '*') {
			inBlockComment = true;
			++i;
			continue;
		}
		result.push_back(source[i]);
	}

	return result;
}

void FTacticalBattleDisplayFireFlowTest::testDrawAndOnPaintUseBattleScreenStateAccessors() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleDisplay::draw(wxDC &dc)");
const std::string onPaintBody = extractFunctionBody(source, "void FBattleDisplay::onPaint(wxPaintEvent & event)");
const std::string ctorBody = extractFunctionBody(
source,
"FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");

assertContains(drawBody, "switch (m_parent->getState())");
assertContains(drawBody, "if (m_parent->getControlState())");
assertContains(drawBody, "if (m_parent->getPhase()==PH_NONE)");
assertContains(drawBody, "drawPlaceMines(dc);");
assertContains(onPaintBody, "draw(dc);");
assertContains(ctorBody, "new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition");
assertContains(ctorBody, "wxSP_ARROW_KEYS | wxSP_VERTICAL");
assertContains(ctorBody, "m_spinCtrl1->SetMinSize(m_spinCtrl1->GetBestSize());");
assertContains(ctorBody, "SetSizer(rootSizer);");
assertNotContains(ctorBody, "wxSize( 50,-1 )");
assertNotContains(ctorBody, "wxPoint(leftOffset,3*BORDER+2)");
}

void FTacticalBattleDisplayFireFlowTest::testZoomImageLoadUsesResolveAssetPath() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string ctorBody = extractFunctionBody(
source,
"FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");

assertContains(ctorBody, "m_zoomImage.LoadFile(gc.resolveAssetPath(\"data/zoom.png\"));");
assertNotContains(ctorBody, "m_zoomImage.LoadFile(\"data/zoom.png\")");
assertNotContains(ctorBody, "gc.getBasePath() + \"data/zoom.png\"");
}

void FTacticalBattleDisplayFireFlowTest::testBattleBoardAndBattleScreenUseResolveAssetPathForTacticalImages() {
const std::string boardSource = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string screenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));

assertContains(boardSource, "m_maskingScreenIcon = new wxImage(gc.resolveAssetPath(\"icons/MaskingScreen.png\"));");
assertContains(boardSource, "m_seekerMissileIcon = new wxImage(gc.resolveAssetPath(\"icons/SeekerMissile.png\"));");
assertNotContains(boardSource, "gc.getBasePath() + \"icons/");
assertNotContains(boardSource, "../icons/");
assertNotContains(boardSource, "/home/");

assertContains(screenSource, "wxImage p0(gc.resolveAssetPath(\"icons/planet_01.png\"));");
assertContains(screenSource, "wxImage p1(gc.resolveAssetPath(\"icons/planet_02.png\"));");
assertContains(screenSource, "wxImage p2(gc.resolveAssetPath(\"icons/planet_03.png\"));");
assertNotContains(screenSource, "gc.getBasePath() + \"icons/");
assertNotContains(screenSource, "../icons/");
assertNotContains(screenSource, "/home/");
}

void FTacticalBattleDisplayFireFlowTest::testLegacyFireAllWeaponsHelperRemoved() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string header = readFile(repoFile("include/tactical/FBattleDisplay.h"));

assertNotContains(source, "void FBattleDisplay::fireAllWeapons(");
assertNotContains(header, "void fireAllWeapons(");
}

void FTacticalBattleDisplayFireFlowTest::testDefensiveFireDoneDelegatesToModelFirePhaseResolution() {
FireFlowRuntimeFixture fixture;
setupOneKillScenario(fixture, PH_DEFENSE_FIRE);

fixture.game.resolveCurrentFirePhase();

const std::vector<unsigned int> & destroyedShipIDs = fixture.game.getLastDestroyedShipIDs();
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), static_cast<unsigned int>(destroyedShipIDs.size()));
CPPUNIT_ASSERT_EQUAL(fixture.defender->getID(), destroyedShipIDs[0]);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

fixture.game.clearLastDestroyedShipIDs();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());

fixture.game.completeDefensiveFirePhase();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

destroyFixture(fixture);
}

void FTacticalBattleDisplayFireFlowTest::testOffensiveFireDoneDelegatesToModelFirePhaseResolution() {
FireFlowRuntimeFixture fixture;
setupOneKillScenario(fixture, PH_ATTACK_FIRE);

fixture.game.resolveCurrentFirePhase();

const std::vector<unsigned int> & destroyedShipIDs = fixture.game.getLastDestroyedShipIDs();
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), static_cast<unsigned int>(destroyedShipIDs.size()));
CPPUNIT_ASSERT_EQUAL(fixture.defender->getID(), destroyedShipIDs[0]);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

fixture.game.clearLastDestroyedShipIDs();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());

fixture.game.completeOffensiveFirePhase();
CPPUNIT_ASSERT(fixture.game.getLastDestroyedShipIDs().empty());
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), static_cast<unsigned int>(fixture.game.getShipList(fixture.game.getDefenderID()).size()));

destroyFixture(fixture);
}

void FTacticalBattleDisplayFireFlowTest::testDefensiveFireDoneDisablesAndHidesButtonBeforeResolution() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onDefensiveFireDone( wxCommandEvent& event )");

assertContains(body, "m_buttonDefensiveFireDone->Enable(false);");
assertContains(body, "m_buttonDefensiveFireDone->Hide();");
assertBefore(body, "m_buttonDefensiveFireDone->Enable(false);", "m_parent->resolveCurrentFirePhase()");
assertBefore(body, "m_buttonDefensiveFireDone->Hide();", "m_parent->resolveCurrentFirePhase()");
}

void FTacticalBattleDisplayFireFlowTest::testOffensiveFireDoneDisablesAndHidesButtonBeforeResolution() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onOffensiveFireDone( wxCommandEvent& event )");

assertContains(body, "m_buttonOffensiveFireDone->Enable(false);");
assertContains(body, "m_buttonOffensiveFireDone->Hide();");
assertBefore(body, "m_buttonOffensiveFireDone->Enable(false);", "m_parent->resolveCurrentFirePhase()");
assertBefore(body, "m_buttonOffensiveFireDone->Hide();", "m_parent->resolveCurrentFirePhase()");
}

void FTacticalBattleDisplayFireFlowTest::testWeaponSelectionDelegatesToBattleScreenModelApi() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::checkWeaponSelection(wxMouseEvent &event)");

assertContains(body, "m_parent->selectWeapon(i);");
assertContains(body, "m_parent->reDraw();");
assertNotContains(body, "m_parent->setWeapon(");
assertNotContains(body, "->setTarget(");
assertNotContains(body, "->fire(");
}

void FTacticalBattleDisplayFireFlowTest::testDefenseSelectionDelegatesToBattleScreenModelApi() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::checkDefenseSelection(wxMouseEvent &event)");

assertContains(body, "m_parent->selectDefense(i);");
assertContains(body, "m_parent->reDraw();");
assertNotContains(body, "m_parent->setDefense(");
assertNotContains(body, "->setCurrentDefense(");
}

void FTacticalBattleDisplayFireFlowTest::testMinePlacementFlowUsesModelMinePlacementApis() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string setSpeedBody = extractFunctionBody(source, "void FBattleDisplay::onSetSpeed( wxCommandEvent& event )");
const std::string doneBody = extractFunctionBody(source, "void FBattleDisplay::onMinePlacementDone( wxCommandEvent& event )");

assertContains(setSpeedBody, "if(!m_parent->beginMinePlacement())");
assertContains(doneBody, "m_parent->completeMinePlacement();");
}

void FTacticalBattleDisplayFireFlowTest::testMinePlacementDisplayUsesModelShipList() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string gameSource = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string placeMinesBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceMines(wxDC &dc)");
const std::string shipSelectionBody = extractFunctionBody(source, "void FBattleDisplay::checkShipSelection(wxMouseEvent &event)");
const std::string placeOrdnanceBody = extractFunctionBody(gameSource, "bool FTacticalGame::placeOrdnanceAtHex(const FPoint & hex)");
const std::string rebuildSourcesBody = extractFunctionBody(gameSource, "void FTacticalGame::rebuildDeployablePlacementSources()");
const std::string beginPlacementBody = extractFunctionBody(gameSource, "bool FTacticalGame::beginOrdnancePlacement()");

assertContains(placeMinesBody, "const std::vector<FTacticalDeploymentSource> & deployableSources = m_parent->getDeployablePlacementSources();");
assertContains(placeMinesBody, "const int selectedSourceIndex = m_parent->getSelectedPlacementSourceIndex();");
assertContains(placeMinesBody, "getDeploymentWeaponLabel(source.weaponType)");
assertContains(placeMinesBody, "os << \"Ammo: \" << weapon->getAmmo();");
assertContains(placeMinesBody, "m_shipSelectionSourceIndices.push_back(static_cast<int>(i));");
assertContains(shipSelectionBody, "m_parent->selectPlacementSourceByIndex(static_cast<unsigned int>(m_shipSelectionSourceIndices[i]));");
assertNotContains(shipSelectionBody, "m_parent->setWeapon(");

assertContains(placeOrdnanceBody, "if (removePlacedOrdnanceForSelection(hex, removed)) {");
assertContains(placeOrdnanceBody, "if (!restoreAmmoForSource(removed.source)) {");
assertContains(placeOrdnanceBody, "if (selectedSource.weaponType == FWeapon::M) {");
assertContains(placeOrdnanceBody, "if (selectedSource.weaponType == FWeapon::SM) {");
assertContains(rebuildSourcesBody, "if (weapon->getType() == FWeapon::M) {");
assertContains(rebuildSourcesBody, "m_shipsWithMines.push_back(*shipItr);");
assertContains(beginPlacementBody, "setState(BS_PlaceMines);");
}

void FTacticalBattleDisplayFireFlowTest::testSetupPlacementBoardUsesSourceSpecificOrdnanceRendering() {
const std::string boardSource = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string screenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string drawBody = extractFunctionBody(boardSource, "void FBattleBoard::draw(wxDC &dc)");
const std::string drawOrdnanceBody = extractFunctionBody(boardSource, "void FBattleBoard::drawPlacementOrdnanceHexes(wxDC &dc)");
const std::string colorBody = extractFunctionBody(boardSource, "wxColour FBattleBoard::getPlacementSourceColor(unsigned int shipID, int weaponIndex) const");
const std::string drawOrdnanceBodyWithoutComments = stripCppComments(drawOrdnanceBody);
const std::string colorBodyWithoutComments = stripCppComments(colorBody);

assertContains(drawBody, "if (m_parent->getState() == BS_PlaceMines || m_parent->getState() == BS_PlaceSeekers) {");
assertContains(drawBody, "drawPlacementOrdnanceHexes(dc);");
assertContains(drawOrdnanceBody, "const std::vector<FTacticalPlacedOrdnance> & placedOrdnance = m_parent->getPlacedOrdnance();");
assertContains(drawOrdnanceBody, "const std::vector<FTacticalDeploymentSource> & deployableSources = m_parent->getDeployablePlacementSources();");
assertContains(drawOrdnanceBody, "std::map<FPlacementSourceKey, unsigned int> sourceOrdinals;");
assertContains(drawOrdnanceBody, "m_placementSourceOrdinals.clear();");
assertContains(drawOrdnanceBody, "sourceOrdinals[key] = static_cast<unsigned int>(sourceOrdinals.size());");
assertContains(drawOrdnanceBody, "m_placementSourceOrdinals[sourceItr->first] = sourceItr->second;");
assertContains(drawOrdnanceBody, "getPlacementSourceColor(itr->source.shipID, itr->source.weaponIndex)");
assertContains(colorBody, "const unsigned int seedColorCount = sizeof(kPlacementColorHexes) / sizeof(kPlacementColorHexes[0]);");
assertContains(colorBody, "if (sourceOrdinal < seedColorCount) {");
assertContains(colorBody, "const unsigned int expandedOrdinal = sourceOrdinal - seedColorCount;");
assertContains(colorBody, "permutePlacementColorByte");
assertNotContains(colorBodyWithoutComments, "index %= colorCount;");
assertNotContains(colorBodyWithoutComments, "unsigned int index = shipID;");
assertNotContains(drawOrdnanceBodyWithoutComments, "index %= colorCount;");

assertContains(screenSource, "const std::vector<FTacticalPlacedOrdnance> & FBattleScreen::getPlacedOrdnance() const {");
assertContains(screenSource, "return m_tacticalGame->getPlacedOrdnance();");
assertContains(screenSource, "std::vector<FTacticalPlacedOrdnance> FBattleScreen::getPlacedOrdnanceAtHex(const FPoint & hex) const {");
assertContains(screenSource, "return m_tacticalGame->getPlacedOrdnanceAtHex(hex);");
assertContains(screenSource, "std::vector<FTacticalSeekerMissileState> FBattleScreen::getSeekerMissilesAtHex(");
assertContains(screenSource, "return m_tacticalGame->getSeekerMissilesAtHex(hex, activeOnly);");
}

void FTacticalBattleDisplayFireFlowTest::testDisplayClickFlowUsesModelForwardingApis() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string screenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string onLeftUpBody = extractFunctionBody(source, "void FBattleDisplay::onLeftUp(wxMouseEvent & event)");
const std::string pendingSelectionBody = extractFunctionBody(source, "bool FBattleDisplay::checkOffensiveSeekerPendingSelection(wxMouseEvent &event)");
const std::string handleHexClickBody = extractFunctionBody(screenSource, "bool FBattleScreen::handleHexClick(const FPoint & hex)");

assertContains(onLeftUpBody, "checkWeaponSelection(event);");
assertContains(onLeftUpBody, "checkDefenseSelection(event);");
assertContains(onLeftUpBody, "checkShipSelection(event);");
assertContains(onLeftUpBody, "if (m_parent->getPhase() == PH_ATTACK_FIRE && checkOffensiveSeekerPendingSelection(event)) {");
assertNotContains(onLeftUpBody, "m_parent->setWeapon(");
assertNotContains(onLeftUpBody, "m_parent->setDefense(");
assertNotContains(onLeftUpBody, "m_parent->assignTargetFromHex(");
assertNotContains(onLeftUpBody, "m_parent->placeMineAtHex(");
assertContains(pendingSelectionBody, "for (unsigned int i = 0; i < m_pendingSeekerRecallRegions.size(); ++i) {");
assertContains(pendingSelectionBody, "const wxPoint & pendingHex = m_pendingSeekerRecallHexes[i];");
assertContains(pendingSelectionBody, "return m_parent->recallSelectedOffensivePendingSeekerAtHex(FPoint(pendingHex.x, pendingHex.y));");
assertContains(handleHexClickBody, "const bool changed = m_tacticalGame->handleHexClick(hex);");
assertContains(handleHexClickBody, "if (changed) {");
assertContains(handleHexClickBody, "reDraw();");
assertContains(handleHexClickBody, "return changed;");
}

void FTacticalBattleDisplayFireFlowTest::testMoveDoneDelegatesToBattleScreenCompleteMovePhase() {
// AC: live wx move-done callback must route through the canonical completeMovePhase seam.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onMoveDone( wxCommandEvent& event )");

assertContains(body, "m_parent->completeMovePhase();");
assertNotContains(body, "m_parent->setPhase(PH_FINALIZE_MOVE);");
}

void FTacticalBattleDisplayFireFlowTest::testMoveDoneDisconnectsAndHidesMoveButtonAroundDelegation() {
// AC: move-done callback still handles UI button teardown while progressing phase flow.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::onMoveDone( wxCommandEvent& event )");

assertContains(body, "m_buttonMoveDone->Disconnect(");
assertContains(body, "m_buttonMoveDone->Hide();");
assertContains(body, "m_first=true;");
assertBefore(body, "m_parent->completeMovePhase();", "m_buttonMoveDone->Hide();");
}

void FTacticalBattleDisplayFireFlowTest::testMovePromptDifferentiatesStoppedFacingNormalAndNoShipCases() {
// AC: move-phase text guides stopped-ship facing, normal movement, and no-selection states.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::drawMoveShip(wxDC &dc)");

assertContains(body, "bool stoppedShipFacingSelection = false;");
assertContains(body, "stoppedShipFacingSelection = (turnData.speed == 0");
assertContains(body, "&& turnData.nMoved == 0");
assertContains(body, "&& m_parent->getShip()->getMR() > 0);");
assertContains(body, "if (stoppedShipFacingSelection) {");
assertContains(body, "Select a highlighted preview route to choose your starting facing.");
assertContains(body, "Continue a route, or click an adjacent hex then Movement Done to rotate in place.");
assertContains(body, "} else if (m_parent->getShip() != NULL && m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()) {");
assertContains(body, "Select route hexes to move the ship.");
assertContains(body, "Press the 'Movement Done' button when all ships have been assigned their movement instructions.");
assertContains(body, "} else {");
assertContains(body, "Please select a ship to move.");
assertContains(body, "drawWrappedActionPrompt(");
assertContains(body, "countWrappedActionPromptLines(");
assertContains(body, "reserveActionPromptLines(");
assertNotContains(body, "Select an adjacent hex to choose facing.");
assertNotContains(body, "Then move along a route, or press Movement Done to rotate in place.");
assertNotContains(body, "Press Movement Done when all ships finish movement.");
}

void FTacticalBattleDisplayFireFlowTest::testActionPromptSpacingContractConstantsAndHelpersDefined() {
// AC: spacing contract constants/helpers are explicitly declared for stable verification.
const std::string header = readFile(repoFile("include/tactical/FBattleDisplay.h"));
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string ctorBody = extractFunctionBody(
source,
"FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");
const std::string lineYBody = extractFunctionBody(source, "int FBattleDisplay::getActionPromptLineY(int lineIndex) const");
const std::string spacerBody = extractFunctionBody(source, "int FBattleDisplay::getActionButtonTopSpacerHeight() const");

assertContains(header, "static const int ACTION_PROMPT_TOP_MARGIN = 5;");
assertContains(header, "static const int ACTION_PROMPT_LINE_HEIGHT = 16;");
assertContains(header, "static const int ACTION_PROMPT_MAX_LINES = 3;");
assertContains(header, "static const int ACTION_PROMPT_BUTTON_GAP = 8;");
assertContains(header, "int getActionPromptLineY(int lineIndex) const;");
assertContains(header, "int getActionButtonTopSpacerHeight() const;");
assertContains(header, "void reserveActionPromptLines(");
assertContains(header, "int countWrappedActionPromptLines(");
assertContains(header, "void drawWrappedActionPrompt(");

assertContains(lineYBody, "return ACTION_PROMPT_TOP_MARGIN + (lineIndex * ACTION_PROMPT_LINE_HEIGHT);");
assertContains(spacerBody, "return getActionPromptLineY(ACTION_PROMPT_MAX_LINES) + ACTION_PROMPT_BUTTON_GAP;");

assertContains(ctorBody, "rootSizer->AddSpacer(getActionButtonTopSpacerHeight());");
assertBefore(ctorBody, "rootSizer->AddSpacer(getActionButtonTopSpacerHeight());", "rootSizer->Add(actionSizer, 0, wxTOP, BORDER);");
}

void FTacticalBattleDisplayFireFlowTest::testActionPromptSpacingContractAppliedAcrossActionPhases() {
// AC: all action prompts route y positioning through helper and keep action row right of zoom column.
// SMF-03: pending seeker list is now a dedicated region left of ship-status, not inside drawCurrentShipStats.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string ctorBody = extractFunctionBody(
source,
"FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");
const std::string moveBody = extractFunctionBody(source, "void FBattleDisplay::drawMoveShip(wxDC &dc)");
const std::string defenseBody = extractFunctionBody(source, "void FBattleDisplay::drawDefensiveFire(wxDC &dc)");
const std::string attackBody = extractFunctionBody(source, "void FBattleDisplay::drawAttackFire(wxDC &dc)");
const std::string minesBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceMines(wxDC &dc)");
const std::string statsBody = extractFunctionBody(source, "void FBattleDisplay::drawCurrentShipStats(wxDC & dc)");
const std::string drawBody = extractFunctionBody(source, "void FBattleDisplay::draw(wxDC &dc)");

assertContains(ctorBody, "actionSizer->AddSpacer(leftOffset);");
assertBefore(ctorBody, "actionSizer->AddSpacer(leftOffset);", "actionSizer->Add(m_buttonMoveDone, 0, wxALIGN_CENTER_VERTICAL);");

assertContains(moveBody, "getActionPromptLineY(0)");
assertContains(moveBody, "countWrappedActionPromptLines(");
assertContains(moveBody, "drawWrappedActionPrompt(");
assertContains(moveBody, "reserveActionPromptLines(");
assertContains(moveBody, "Movement Done");

assertContains(defenseBody, "getActionPromptLineY(0)");
assertContains(defenseBody, "reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);");

assertContains(attackBody, "getActionPromptLineY(0)");
assertContains(attackBody, "reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);");
assertContains(attackBody, "if (m_parent->isOffensiveSeekerDeploymentMode()) {");
assertContains(attackBody, "Select legal path hexes to deploy seeker missiles.");

assertContains(minesBody, "getActionPromptLineY(0)");
assertContains(minesBody, "reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);");
assertContains(statsBody, "largestMarginWithStatsRoom");
// SMF-03: pending seeker rows moved out of drawCurrentShipStats into draw() left-of-stats region.
assertNotContains(statsBody, "drawOffensiveSeekerPendingRows(");
assertContains(drawBody, "PH_ATTACK_FIRE");
assertContains(drawBody, "drawOffensiveSeekerPendingRows(dc, leftOffset, pendingRegionTop, 10);");
}

void FTacticalBattleDisplayFireFlowTest::testActionButtonShowPathsRelayoutAfterVisibilityChange() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string moveBody = extractFunctionBody(source, "void FBattleDisplay::drawMoveShip(wxDC &dc)");
const std::string defenseBody = extractFunctionBody(source, "void FBattleDisplay::drawDefensiveFire(wxDC &dc)");
const std::string attackBody = extractFunctionBody(source, "void FBattleDisplay::drawAttackFire(wxDC &dc)");
const std::string minesBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceMines(wxDC &dc)");

assertContains(moveBody, "m_buttonMoveDone->Show();");
assertBefore(moveBody, "m_buttonMoveDone->Show();", "Layout();");
assertContains(defenseBody, "m_buttonDefensiveFireDone->Show();");
assertBefore(defenseBody, "m_buttonDefensiveFireDone->Show();", "Layout();");
assertContains(attackBody, "m_buttonOffensiveFireDone->Show();");
assertBefore(attackBody, "m_buttonOffensiveFireDone->Show();", "Layout();");
assertContains(minesBody, "m_buttonMinePlacementDone->Show();");
assertBefore(minesBody, "m_buttonMinePlacementDone->Show();", "Layout();");
}

void FTacticalBattleDisplayFireFlowTest::testActionButtonHidePathsRelayoutAfterVisibilityChange() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string moveBody = extractFunctionBody(source, "void FBattleDisplay::onMoveDone( wxCommandEvent& event )");
const std::string defenseBody = extractFunctionBody(source, "void FBattleDisplay::onDefensiveFireDone( wxCommandEvent& event )");
const std::string attackBody = extractFunctionBody(source, "void FBattleDisplay::onOffensiveFireDone( wxCommandEvent& event )");
const std::string minesBody = extractFunctionBody(source, "void FBattleDisplay::onMinePlacementDone( wxCommandEvent& event )");

assertContains(moveBody, "m_buttonMoveDone->Hide();");
assertBefore(moveBody, "m_buttonMoveDone->Hide();", "Layout();");
assertContains(defenseBody, "m_buttonDefensiveFireDone->Hide();");
assertBefore(defenseBody, "m_buttonDefensiveFireDone->Hide();", "Layout();");
assertContains(attackBody, "m_buttonOffensiveFireDone->Hide();");
assertBefore(attackBody, "m_buttonOffensiveFireDone->Hide();", "Layout();");
assertContains(minesBody, "m_buttonMinePlacementDone->Hide();");
assertBefore(minesBody, "m_buttonMinePlacementDone->Hide();", "Layout();");
}

void FTacticalBattleDisplayFireFlowTest::testSeekerActivationDrawAndClickFlowUseActivationPhaseRouting() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string screenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleDisplay::draw(wxDC &dc)");
const std::string onLeftUpBody = extractFunctionBody(source, "void FBattleDisplay::onLeftUp(wxMouseEvent & event)");
const std::string selectionBody = extractFunctionBody(source, "void FBattleDisplay::checkSeekerActivationSelection(wxMouseEvent &event)");
const std::string activateBody = extractFunctionBody(screenSource, "bool FBattleScreen::activateSelectedInactiveSeeker(unsigned int seekerID)");

assertContains(drawBody, "case PH_SEEKER_ACTIVATION:");
assertContains(drawBody, "drawSeekerActivation(dc);");
assertContains(onLeftUpBody, "if (m_parent->getPhase() == PH_SEEKER_ACTIVATION) {");
assertContains(onLeftUpBody, "checkSeekerActivationSelection(event);");
assertBefore(onLeftUpBody, "checkSeekerActivationSelection(event);", "checkWeaponSelection(event);");
assertContains(selectionBody, "for (unsigned int i = 0; i < m_seekerActivationRegions.size(); ++i) {");
assertContains(selectionBody, "if (!m_seekerActivationRegions[i].Contains(x,y)) {");
assertContains(selectionBody, "m_parent->deactivateActiveSeekerByID(m_seekerActivationSeekerIDs[i]);");
assertContains(selectionBody, "m_parent->reDraw();");
assertContains(activateBody, "if (changed) {");
assertContains(activateBody, "reDraw();");
}

void FTacticalBattleDisplayFireFlowTest::testSeekerActivationPanelListsInstructionAndOneRowPerInactiveSeeker() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleDisplay::drawSeekerActivation(wxDC &dc)");

assertContains(body, "dc.DrawText(\"Seeker activation phase.\",leftOffset,getActionPromptLineY(0));");
assertContains(body, "dc.DrawText(\"Click a seeker stack on the board to activate one seeker.\",leftOffset,getActionPromptLineY(1));");
assertContains(body, "dc.DrawText(\"Click a row below to deactivate an activated seeker.\",leftOffset,getActionPromptLineY(2));");
// SMF-05: "Activated seekers" block must start below the action-button row, not at BORDER
assertContains(body, "int y = getActionButtonRowBottom();");
assertNotContains(body, "int y = BORDER;");
// SMF-05: switched to this-phase accessor so only current-phase activations are listed
assertContains(body, "const std::vector<FTacticalSeekerMissileState> activated = m_parent->getActiveSeekersByMovingPlayerThisPhase();");
assertNotContains(body, "m_parent->getActiveSeekersByMovingPlayer();");
assertContains(body, "if (activated.empty()) {");
assertContains(body, "dc.DrawText(\"No seekers activated yet.\",lMargin,y);");
assertContains(body, "for (unsigned int i = 0; i < activated.size(); ++i) {");
assertContains(body, "os << \"Deactivate seeker #\" << seeker.seekerID");
assertContains(body, "m_seekerActivationRegions.push_back(wxRect(");
assertContains(body, "m_seekerActivationSeekerIDs.push_back(seeker.seekerID);");
}

void FTacticalBattleDisplayFireFlowTest::testSeekerActivationButtonUsesShowHideDisconnectAndRelayoutPattern() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string ctorBody = extractFunctionBody(
source,
"FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");
const std::string drawBody = extractFunctionBody(source, "void FBattleDisplay::drawSeekerActivation(wxDC &dc)");
const std::string doneBody = extractFunctionBody(source, "void FBattleDisplay::onSeekerActivationDone( wxCommandEvent& event )");

assertContains(ctorBody, "m_buttonSeekerActivationDone = new wxButton( this, wxID_ANY, wxT(\"Seeker Activation Done\"), wxDefaultPosition, wxDefaultSize, 0 );");
assertContains(ctorBody, "actionSizer->Add(m_buttonSeekerActivationDone, 0, wxALIGN_CENTER_VERTICAL);");
assertContains(ctorBody, "m_buttonSeekerActivationDone->Hide();");
assertContains(drawBody, "m_buttonSeekerActivationDone->Connect(");
assertContains(drawBody, "wxCommandEventHandler(FBattleDisplay::onSeekerActivationDone),");
assertContains(drawBody, "m_buttonSeekerActivationDone->Show();");
assertBefore(drawBody, "m_buttonSeekerActivationDone->Show();", "Layout();");
assertContains(doneBody, "m_buttonSeekerActivationDone->Disconnect(");
assertContains(doneBody, "m_buttonSeekerActivationDone->Hide();");
assertBefore(doneBody, "m_buttonSeekerActivationDone->Hide();", "Layout();");
assertContains(doneBody, "m_parent->completeSeekerActivationPhase();");
assertContains(doneBody, "m_first = true;");
}

void FTacticalBattleDisplayFireFlowTest::testOffensiveSeekerDeploymentRuntimeFlowSupportsPendingRecallAndCommit() {
	SeekerFlowRuntimeFixture fixture;
	setupSeekerFlowRuntimeScenario(fixture);

	const bool attackerSide = (fixture.attacker->getOwner() == fixture.game.getAttackerID());
	fixture.game.setPhase(PH_DEFENSE_FIRE);
	fixture.game.setMovingPlayer(!attackerSide);
	fixture.game.setActivePlayer(attackerSide);
	CPPUNIT_ASSERT(fixture.game.selectShipFromHex(fixture.attackerHex));
	CPPUNIT_ASSERT(!fixture.game.selectWeapon(0));

	fixture.game.setPhase(PH_ATTACK_FIRE);
	fixture.game.setMovingPlayer(attackerSide);
	fixture.game.setActivePlayer(attackerSide);
	CPPUNIT_ASSERT(fixture.game.selectShipFromHex(fixture.attackerHex));

	FTacticalTurnData * turnData = fixture.game.findTurnData(fixture.attacker->getID());
	CPPUNIT_ASSERT(turnData != NULL);
	const FPoint firstMoveHex = FHexMap::findNextHex(fixture.attackerHex, fixture.attacker->getHeading());
	const FPoint secondMoveHex = FHexMap::findNextHex(firstMoveHex, fixture.attacker->getHeading());
	turnData->path.clear();
	turnData->path.addPoint(fixture.attackerHex);
	turnData->path.addPoint(firstMoveHex);
	turnData->path.addPoint(secondMoveHex);
	turnData->startHeading = fixture.attacker->getHeading();
	turnData->curHeading = fixture.attacker->getHeading();
	turnData->finalHeading = fixture.attacker->getHeading();
	turnData->nMoved = 2;

	CPPUNIT_ASSERT(fixture.game.selectWeapon(0));
	CPPUNIT_ASSERT(fixture.game.isOffensiveSeekerDeploymentMode());
	CPPUNIT_ASSERT(fixture.game.getTargetHexes().find(fixture.attackerHex) != fixture.game.getTargetHexes().end());
	CPPUNIT_ASSERT(fixture.game.getTargetHexes().find(firstMoveHex) != fixture.game.getTargetHexes().end());
	CPPUNIT_ASSERT(fixture.game.getTargetHexes().find(secondMoveHex) != fixture.game.getTargetHexes().end());
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fixture.game.getTargetHexes().size());
	CPPUNIT_ASSERT(!fixture.game.assignTargetFromHex(fixture.defenderHex));
	CPPUNIT_ASSERT(!fixture.game.isHexDeployable(fixture.defenderHex));
	CPPUNIT_ASSERT(fixture.game.isHexDeployable(firstMoveHex));

	const size_t seekersAtHexBefore = fixture.game.getSeekerMissilesAtHex(firstMoveHex, false).size();
	CPPUNIT_ASSERT(fixture.game.handleHexClick(firstMoveHex));
	CPPUNIT_ASSERT_EQUAL(2, fixture.launcher->getAmmo());
	std::vector<FTacticalSeekerMissileState> seekersAtHex = fixture.game.getSeekerMissilesAtHex(firstMoveHex, false);
	CPPUNIT_ASSERT_EQUAL(seekersAtHexBefore + 1u, seekersAtHex.size());
	CPPUNIT_ASSERT(!seekersAtHex.back().active);
	std::vector<FTacticalPendingSeekerHexGroup> pendingGroups = fixture.game.getSelectedOffensivePendingSeekerHexGroups();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pendingGroups.size());
	CPPUNIT_ASSERT_EQUAL(1u, pendingGroups[0].count);

	CPPUNIT_ASSERT(fixture.game.handleHexClick(firstMoveHex));
	CPPUNIT_ASSERT_EQUAL(1, fixture.launcher->getAmmo());
	seekersAtHex = fixture.game.getSeekerMissilesAtHex(firstMoveHex, false);
	CPPUNIT_ASSERT_EQUAL(seekersAtHexBefore + 2u, seekersAtHex.size());
	pendingGroups = fixture.game.getSelectedOffensivePendingSeekerHexGroups();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pendingGroups.size());
	CPPUNIT_ASSERT_EQUAL(2u, pendingGroups[0].count);

	CPPUNIT_ASSERT(fixture.game.recallSelectedOffensivePendingSeekerAtHex(firstMoveHex));
	CPPUNIT_ASSERT_EQUAL(2, fixture.launcher->getAmmo());
	seekersAtHex = fixture.game.getSeekerMissilesAtHex(firstMoveHex, false);
	CPPUNIT_ASSERT_EQUAL(seekersAtHexBefore + 1u, seekersAtHex.size());
	pendingGroups = fixture.game.getSelectedOffensivePendingSeekerHexGroups();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pendingGroups.size());
	CPPUNIT_ASSERT_EQUAL(1u, pendingGroups[0].count);

	fixture.game.setWeapon(NULL);
	CPPUNIT_ASSERT(fixture.game.selectWeapon(0));
	pendingGroups = fixture.game.getSelectedOffensivePendingSeekerHexGroups();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pendingGroups.size());
	CPPUNIT_ASSERT_EQUAL(1u, pendingGroups[0].count);

	fixture.game.completeOffensiveFirePhase();
	CPPUNIT_ASSERT(!fixture.game.isOffensiveSeekerDeploymentMode());
	CPPUNIT_ASSERT(fixture.game.getSelectedOffensivePendingSeekerHexGroups().empty());
	CPPUNIT_ASSERT(!fixture.game.recallSelectedOffensivePendingSeekerAtHex(firstMoveHex));
	seekersAtHex = fixture.game.getSeekerMissilesAtHex(firstMoveHex, false);
	CPPUNIT_ASSERT_EQUAL(seekersAtHexBefore + 1u, seekersAtHex.size());
	CPPUNIT_ASSERT(!seekersAtHex.back().active);

	destroySeekerFixture(fixture);
}

void FTacticalBattleDisplayFireFlowTest::testLowerPanelLayoutStateDefinesSharedPromptStatsAndHeightFields() {
const std::string header = readFile(repoFile("include/tactical/FBattleDisplay.h"));
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string ctorBody = extractFunctionBody(
source,
"FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");

assertContains(header, "enum LowerPanelLayoutMode {");
assertContains(header, "LOWER_PANEL_LAYOUT_RIGHT_SPLIT");
assertContains(header, "LOWER_PANEL_LAYOUT_STACKED");
assertContains(header, "struct LowerPanelLayoutState {");
assertContains(header, "LowerPanelLayoutMode mode;");
assertContains(header, "int shipStatsLeftMargin;");
assertContains(header, "int shipStatsTop;");
assertContains(header, "int reservedPromptLines;");
assertContains(header, "int requestedDisplayHeight;");
assertContains(header, "bool initialized;");
assertContains(header, "LowerPanelLayoutState m_lowerPanelLayoutState;");
assertContains(header, "void ensureLowerPanelLayoutState(int panelWidth, int panelHeight);");
assertContains(header, "void applyRequestedDisplayHeight();");

assertContains(ctorBody, "m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_RIGHT_SPLIT;");
assertContains(ctorBody, "m_lowerPanelLayoutState.shipStatsLeftMargin = 300;");
assertContains(ctorBody, "m_lowerPanelLayoutState.shipStatsTop = BORDER;");
assertContains(ctorBody, "m_lowerPanelLayoutState.reservedPromptLines = ACTION_PROMPT_MAX_LINES;");
assertContains(ctorBody, "m_lowerPanelLayoutState.requestedDisplayHeight = 120;");
assertContains(ctorBody, "m_lowerPanelLayoutState.initialized = false;");
assertContains(source, "m_lowerPanelLayoutState.reservedPromptLines = lineCount;");
}

void FTacticalBattleDisplayFireFlowTest::testLowerPanelLayoutStatePersistsSplitUntilGeometryInvalidatesIt() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string stateBody = extractFunctionBody(
source,
"void FBattleDisplay::ensureLowerPanelLayoutState(int panelWidth, int panelHeight)");

assertContains(stateBody, "if (m_lowerPanelLayoutState.initialized){");
assertContains(stateBody, "if (m_lowerPanelLayoutState.mode == LOWER_PANEL_LAYOUT_RIGHT_SPLIT){");
assertContains(stateBody, "keepCurrentState = splitCanFit");
assertContains(stateBody, "m_lowerPanelLayoutState.shipStatsLeftMargin >= minStatsLeftMargin");
assertContains(stateBody, "m_lowerPanelLayoutState.shipStatsLeftMargin <= largestMarginWithStatsRoom;");
assertContains(stateBody, "const bool splitCanFitWithPadding = splitCanFit");
assertContains(stateBody, "largestMarginWithStatsRoom >= (minStatsLeftMargin + BORDER);");
assertContains(stateBody, "} else {");
assertContains(stateBody, "keepCurrentState = m_lowerPanelLayoutState.shipStatsTop >= stackedTop;");
assertContains(stateBody, "&& !splitCanFitWithPadding");
assertContains(stateBody, "if (!keepCurrentState){");
assertContains(stateBody, "if (splitCanFit){");
assertContains(stateBody, "if (splitCanFitWithPadding){");
assertContains(stateBody, "m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_RIGHT_SPLIT;");
assertContains(stateBody, "} else {");
assertContains(stateBody, "m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_STACKED;");
assertContains(stateBody, "m_lowerPanelLayoutState.shipStatsLeftMargin = leftOffset;");
assertContains(stateBody, "m_lowerPanelLayoutState.shipStatsTop = stackedTop;");
assertContains(stateBody, "m_lowerPanelLayoutState.initialized = true;");

const std::string statsBody = extractFunctionBody(source, "void FBattleDisplay::drawCurrentShipStats(wxDC & dc)");
assertContains(statsBody, "ensureLowerPanelLayoutState(panelWidth, panelHeight);");
assertContains(statsBody, "if (m_lowerPanelLayoutState.mode == LOWER_PANEL_LAYOUT_RIGHT_SPLIT");
assertContains(statsBody, "&& largestMarginWithStatsRoom >= leftOffset + ACTION_PROMPT_MIN_WIDTH");
assertContains(statsBody, "largestMarginWithStatsRoom < leftOffset + ACTION_PROMPT_MIN_WIDTH + BORDER");
assertContains(statsBody, "m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_STACKED;");
assertContains(statsBody, "m_lowerPanelLayoutState.shipStatsTop = getActionButtonRowBottom() + ACTION_PROMPT_BUTTON_GAP;");
}

void FTacticalBattleDisplayFireFlowTest::testRequestedDisplayHeightFlowsFromLayoutStateIntoMinSize() {
	const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
	const std::string stateBody = extractFunctionBody(
		source,
		"void FBattleDisplay::ensureLowerPanelLayoutState(int panelWidth, int panelHeight)");
	const std::string applyBody = extractFunctionBody(source, "void FBattleDisplay::applyRequestedDisplayHeight()");
	const std::string statsBody = extractFunctionBody(source, "void FBattleDisplay::drawCurrentShipStats(wxDC & dc)");
	const std::string reflowBody = extractFunctionBody(source, "void FBattleDisplay::reflowLowerPanelLayout()");

	assertContains(stateBody, "int requestedHeight = getActionPromptLineY(ACTION_PROMPT_MAX_LINES) + ACTION_PROMPT_LINE_HEIGHT + ACTION_PROMPT_BUTTON_GAP + BORDER;");
	assertContains(stateBody, "const int statsBottom = m_lowerPanelLayoutState.shipStatsTop + statsHeight + BORDER;");
	assertContains(stateBody, "if (statsBottom > requestedHeight){");
	assertContains(stateBody, "requestedHeight = statsBottom;");
	assertNotContains(stateBody, "if (panelHeight > requestedHeight){");
	assertNotContains(stateBody, "requestedHeight = panelHeight;");
	assertContains(stateBody, "m_lowerPanelLayoutState.requestedDisplayHeight = requestedHeight;");

	assertContains(applyBody, "int requestedHeight = m_lowerPanelLayoutState.requestedDisplayHeight;");
	assertContains(applyBody, "if (requestedHeight < 120){");
	assertContains(applyBody, "if (GetMinSize().GetHeight() != requestedHeight){");
	assertContains(applyBody, "SetMinSize(wxSize(-1, requestedHeight));");
	assertNotContains(reflowBody, "previousMinHeight");

	assertContains(statsBody, "applyRequestedDisplayHeight();");
	assertBefore(statsBody, "ensureLowerPanelLayoutState(panelWidth, panelHeight);", "applyRequestedDisplayHeight();");
}

void FTacticalBattleDisplayFireFlowTest::testBattleScreenDefaultsTo1200x900InConstructorSignature() {
const std::string header = readFile(repoFile("include/tactical/FBattleScreen.h"));
assertContains(header, "const wxSize& size = wxSize( 1200,900 )");
}

void FTacticalBattleDisplayFireFlowTest::testBattleScreenLayoutPolicyUsesBaselineAndMapFloorConstants() {
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
assertContains(source, "const int BATTLE_SCREEN_BASE_DISPLAY_HEIGHT = 120;");
assertContains(source, "const int BATTLE_SCREEN_MAP_MIN_PERCENT = 60;");
assertContains(source, "m_displayRequestedMinHeight = BATTLE_SCREEN_BASE_DISPLAY_HEIGHT;");
}

void FTacticalBattleDisplayFireFlowTest::testBattleScreenLayoutPolicyAppliesDisplayRequestAndMapFloorClamp() {
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleScreen::applyLayoutPolicy()");

	assertContains(body, "const int mapMinHeight = (clientHeight * BATTLE_SCREEN_MAP_MIN_PERCENT) / 100;");
	assertContains(body, "const int currentDisplayRequest = m_display->GetMinSize().GetHeight();");
	assertContains(body, "m_displayRequestedMinHeight = currentDisplayRequest;");
	assertContains(body, "if (m_displayRequestedMinHeight < BATTLE_SCREEN_BASE_DISPLAY_HEIGHT) {");
	assertContains(body, "m_displayRequestedMinHeight = BATTLE_SCREEN_BASE_DISPLAY_HEIGHT;");
	assertNotContains(body, "if (currentDisplayRequest > m_displayRequestedMinHeight) {");
	assertContains(body, "if (desiredDisplayHeight < BATTLE_SCREEN_BASE_DISPLAY_HEIGHT) {");
	assertContains(body, "desiredDisplayHeight = BATTLE_SCREEN_BASE_DISPLAY_HEIGHT;");
	assertContains(body, "const int maxDisplayHeight = clientHeight - mapMinHeight;");
	assertContains(body, "if (maxDisplayHeight >= 0 && desiredDisplayHeight > maxDisplayHeight) {");
assertContains(body, "desiredDisplayHeight = maxDisplayHeight;");
assertContains(body, "m_map->SetMinSize(wxSize(-1, mapMinHeight));");
assertContains(body, "m_display->SetMinSize(wxSize(-1, desiredDisplayHeight));");
assertNotContains(body, "m_tacticalGame");
}

void FTacticalBattleDisplayFireFlowTest::testBattleScreenResizeHandlerReflowsLowerPanelBeforeLayoutPolicy() {
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleScreen::onSize(wxSizeEvent & event)");

assertContains(body, "if (m_display != NULL) {");
assertContains(body, "m_display->reflowLowerPanelLayout();");
assertContains(body, "applyLayoutPolicy();");
assertBefore(body, "m_display->reflowLowerPanelLayout();", "applyLayoutPolicy();");
assertContains(body, "event.Skip();");
assertNotContains(body, "m_tacticalGame");
assertNotContains(body, "setPhase(");
assertNotContains(body, "resolveCurrentFirePhase");
}

void FTacticalBattleDisplayFireFlowTest::testLowerPanelReflowPathRecomputesMovePromptReservationFromCurrentGeometry() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string reflowBody = extractFunctionBody(source, "void FBattleDisplay::reflowLowerPanelLayout()");
const std::string refreshBody = extractFunctionBody(source, "void FBattleDisplay::refreshMovePromptReservation(wxDC &dc, int panelWidth, int panelHeight)");
const std::string maxWidthBody = extractFunctionBody(source, "int FBattleDisplay::getCurrentPromptMaxWidth(int panelWidth) const");

assertContains(reflowBody, "if (m_inResizeReflow){");
assertContains(reflowBody, "m_inResizeReflow = true;");
assertContains(reflowBody, "if (panelWidth > 0 && panelHeight > 0) {");
assertContains(reflowBody, "wxClientDC dc(this);");
assertContains(reflowBody, "if (m_parent->getState() == BS_Battle && m_parent->getPhase() == PH_MOVE) {");
assertContains(reflowBody, "refreshMovePromptReservation(dc, panelWidth, panelHeight);");
assertContains(reflowBody, "ensureLowerPanelLayoutState(panelWidth, panelHeight);");
assertContains(reflowBody, "applyRequestedDisplayHeight();");
assertContains(reflowBody, "m_inResizeReflow = false;");
assertNotContains(reflowBody, "resolveCurrentFirePhase");
assertNotContains(reflowBody, "setPhase(");

assertContains(refreshBody, "ensureLowerPanelLayoutState(panelWidth, panelHeight);");
assertContains(refreshBody, "int promptMaxWidth = getCurrentPromptMaxWidth(panelWidth);");
assertContains(refreshBody, "buildMovePromptText(turnPrompt, detailPromptOne, detailPromptTwo);");
assertContains(refreshBody, "countWrappedActionPromptLines(dc, turnPrompt, promptMaxWidth);");
assertContains(refreshBody, "reserveActionPromptLines(promptLineCount);");
CPPUNIT_ASSERT(countOccurrences(refreshBody, "ensureLowerPanelLayoutState(panelWidth, panelHeight);") >= 3);
CPPUNIT_ASSERT(countOccurrences(refreshBody, "reserveActionPromptLines(promptLineCount);") >= 2);

assertContains(maxWidthBody, "if (promptMaxWidth < 120){");
assertContains(maxWidthBody, "promptMaxWidth = 120;");
}

void FTacticalBattleDisplayFireFlowTest::testMovePromptConstrainedWidthSelectionPathUsesDeterministicHelpers() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleDisplay::drawMoveShip(wxDC &dc)");
const std::string builderBody = extractFunctionBody(source, "void FBattleDisplay::buildMovePromptText(wxString & turnPrompt, wxString & detailPromptOne, wxString & detailPromptTwo) const");
const std::string header = readFile(repoFile("include/tactical/FBattleDisplay.h"));

assertContains(drawBody, "refreshMovePromptReservation(dc, panelWidth, panelHeight);");
assertContains(drawBody, "int promptMaxWidth = getCurrentPromptMaxWidth(panelWidth);");
assertContains(drawBody, "wxString turnPrompt;");
assertContains(drawBody, "wxString detailPromptOne;");
assertContains(drawBody, "wxString detailPromptTwo;");
assertBefore(drawBody, "refreshMovePromptReservation(dc, panelWidth, panelHeight);", "drawWrappedActionPrompt(");
assertContains(drawBody, "Select a highlighted preview route to choose your starting facing.");
assertContains(drawBody, "Press the 'Movement Done' button when all ships have been assigned their movement instructions.");

assertContains(builderBody, "if (stoppedShipFacingSelection) {");
assertContains(builderBody, "Select a highlighted preview route to choose your starting facing.");
assertContains(builderBody, "Continue a route, or click an adjacent hex then Movement Done to rotate in place.");
assertContains(builderBody, "} else if (m_parent->getShip() != NULL && m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()) {");
assertContains(builderBody, "Select route hexes to move the ship.");
assertContains(builderBody, "Press the 'Movement Done' button when all ships have been assigned their movement instructions.");
assertContains(builderBody, "} else {");
assertContains(builderBody, "Please select a ship to move.");

assertContains(header, "void buildMovePromptText(wxString & turnPrompt, wxString & detailPromptOne, wxString & detailPromptTwo) const;");
assertContains(header, "void refreshMovePromptReservation(wxDC &dc, int panelWidth, int panelHeight);");
assertContains(header, "int getCurrentPromptMaxWidth(int panelWidth) const;");
}

void FTacticalBattleDisplayFireFlowTest::testShipStatsMeasurementAndSplitEligibilityUseContentBasedSizing() {
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string measureBody = extractFunctionBody(
source,
"FBattleDisplay::ShipStatsLayoutRequirements FBattleDisplay::measureShipStatsLayoutRequirements(wxDC &dc) const");
const std::string stateBody = extractFunctionBody(
source,
"void FBattleDisplay::ensureLowerPanelLayoutState(int panelWidth, int panelHeight)");

assertContains(measureBody, "dc.GetTextExtent(selectedShip->getName())");
assertContains(measureBody, "for (unsigned int i = 0; i < selectedShip->getWeaponCount(); i++)");
assertContains(measureBody, "for (unsigned int i = 0; i < selectedShip->getDefenseCount(); i++)");
assertContains(measureBody, "otherStatusWidth += dc.GetTextExtent(\"none\").GetWidth();");
assertContains(measureBody, "requirements.width = rightEdge + BORDER;");
assertContains(measureBody, "requirements.height = bottomEdge + BORDER;");
assertContains(measureBody, "if (requirements.width < SHIP_STATS_MIN_WIDTH)");

assertContains(stateBody, "const ShipStatsLayoutRequirements shipStatsRequirements = measureShipStatsLayoutRequirements(dc);");
assertContains(stateBody, "const int largestMarginWithStatsRoom = panelWidth - shipStatsRequirements.width - BORDER;");
assertContains(stateBody, "const bool splitCanFit = largestMarginWithStatsRoom >= minStatsLeftMargin;");
assertContains(stateBody, "const int stackedTop = buttonRowBottom + ACTION_PROMPT_BUTTON_GAP;");
assertContains(stateBody, "const int statsHeight = shipStatsRequirements.height;");
assertContains(stateBody, "const int statsBottom = m_lowerPanelLayoutState.shipStatsTop + statsHeight + BORDER;");
assertContains(stateBody, "if (splitCanFit){");
assertContains(stateBody, "m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_RIGHT_SPLIT;");
assertContains(stateBody, "m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_STACKED;");
}

void FTacticalBattleDisplayFireFlowTest::testPlaceMinesSourceListStartsAtActionButtonRowBottom() {
// AC: SMRIV-01 -- drawPlaceMines() anchors the source-selection rows to the top of the
// lower panel (right column at lMargin=310, starting at getActionPromptLineY(0)) and wraps
// the instruction text via drawWrappedActionPrompt() in the left column.
// AC: The placement source rows remain clickable (hit regions still align with drawn text).
// AC: No regression to other lower-panel layouts that share the same spacer logic.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string minesBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceMines(wxDC &dc)");
const std::string moveBody = extractFunctionBody(source, "void FBattleDisplay::drawMoveShip(wxDC &dc)");
const std::string defenseBody = extractFunctionBody(source, "void FBattleDisplay::drawDefensiveFire(wxDC &dc)");
const std::string attackBody = extractFunctionBody(source, "void FBattleDisplay::drawAttackFire(wxDC &dc)");

// AC #1: Source list y-start is getActionPromptLineY(0), anchored to the top of the lower
// panel (right column), not getActionButtonRowBottom() as in the old layout.
assertContains(minesBody, "int y = getActionPromptLineY(0);");
assertNotContains(minesBody, "int y = BORDER;");
assertNotContains(minesBody, "int y = getActionButtonRowBottom();");

// AC #2: Instruction text is wrapped via drawWrappedActionPrompt() in the left column.
assertContains(minesBody, "drawWrappedActionPrompt(dc,");

// AC #3: Hit regions use the same y variable as the drawn text rows, preserving click alignment.
// The region push occurs after DrawText at the same y offset in the loop body.
assertContains(minesBody, "m_shipNameRegions.push_back(wxRect(lMargin,y,");

// AC #4: Other draw phases are not affected by the SMRIV-01 change.
// drawMoveShip, drawDefensiveFire, and drawAttackFire must not reference getActionButtonRowBottom()
// as the y initializer for their own content (they use the action-prompt line helpers instead).
assertNotContains(moveBody, "int y = getActionButtonRowBottom();");
assertNotContains(defenseBody, "int y = getActionButtonRowBottom();");
assertNotContains(attackBody, "int y = getActionButtonRowBottom();");
}

void FTacticalBattleDisplayFireFlowTest::testTwoPhaseSeekerDeploymentDrawAndClickDispatching() {
// AC: SMF-02 -- draw() dispatches BS_PlaceSeekers to drawPlaceSeekers(); onLeftUp()
// dispatches BS_PlaceSeekers to checkShipSelection(); constructor creates, adds to
// actionSizer, and hides m_buttonSeekerPlacementDone at startup.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleDisplay::draw(wxDC &dc)");
const std::string onLeftUpBody = extractFunctionBody(source, "void FBattleDisplay::onLeftUp(wxMouseEvent & event)");
const std::string ctorBody = extractFunctionBody(
	source,
	"FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");

// draw() must dispatch BS_PlaceSeekers to drawPlaceSeekers(dc).
assertContains(drawBody, "case BS_PlaceSeekers:");
assertContains(drawBody, "drawPlaceSeekers(dc);");

// onLeftUp() must dispatch BS_PlaceSeekers to checkShipSelection().
assertContains(onLeftUpBody, "case BS_PlaceSeekers:");
assertContains(onLeftUpBody, "checkShipSelection(event);");

// Constructor must create m_buttonSeekerPlacementDone with the fixed label,
// add it to the actionSizer, and hide it at startup.
assertContains(ctorBody, "m_buttonSeekerPlacementDone = new wxButton( this, wxID_ANY, wxT(\"Seeker Placement Done\")");
assertContains(ctorBody, "actionSizer->Add(m_buttonSeekerPlacementDone,");
assertContains(ctorBody, "m_buttonSeekerPlacementDone->Hide();");
}

void FTacticalBattleDisplayFireFlowTest::testDrawPlaceSeekersUsesSeekerSpecificPromptsAndSMFilter() {
// AC: SMF-02 -- drawPlaceSeekers() uses the seeker-specific prompt text, SM-only filter,
// completeSeekerPlacement() delegation, and the standard show/disconnect/hide button lifecycle.
// SMRIV-02: The source-list y-start is now getActionPromptLineY(0) (three-column layout,
// matching drawPlaceMines()), not getActionButtonRowBottom() as in the pre-SMRIV-02 layout.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string seekersBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceSeekers(wxDC &dc)");
const std::string onDoneBody = extractFunctionBody(source, "void FBattleDisplay::onSeekerPlacementDone( wxCommandEvent& event )");

// Seeker-specific prompt text (exact string from AC).
assertContains(seekersBody,
	"The defending player may now place seeker missiles before the attacker sets up their ships.");

// Seeker-specific selection line (exact string from AC).
assertContains(seekersBody, "Select a source row to place seeker missiles.");

// SM-only filter: rows whose weaponType is not SM must be skipped.
assertContains(seekersBody, "if (source.weaponType != FWeapon::SM)");

// SMRIV-02: Source list y-start uses getActionPromptLineY(0) (three-column layout,
// anchored to the top of the lower panel), not getActionButtonRowBottom().
assertContains(seekersBody, "int y = getActionPromptLineY(0);");
assertNotContains(seekersBody, "int y = getActionButtonRowBottom();");

// Show m_buttonSeekerPlacementDone on first draw.
assertContains(seekersBody, "m_buttonSeekerPlacementDone->Show();");

// onSeekerPlacementDone must disconnect, hide, call completeSeekerPlacement(), and reset m_first.
assertContains(onDoneBody, "m_buttonSeekerPlacementDone->Disconnect(");
assertContains(onDoneBody, "m_buttonSeekerPlacementDone->Hide();");
assertContains(onDoneBody, "m_parent->completeSeekerPlacement();");
assertContains(onDoneBody, "m_first = true;");
}

void FTacticalBattleDisplayFireFlowTest::testMinePhaseUsesExactPromptTextAndMFilter() {
// AC: SMF-02 -- drawPlaceMines() uses the mine-specific prompt text, M-only filter,
// and no combined "Weapon Placement Done" label path remains anywhere in the file.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string minesBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceMines(wxDC &dc)");

// Mine-specific prompt text (exact string from AC).
assertContains(minesBody,
	"The defending player may now place mines before the attacker sets up their ships.");

// Mine-specific selection line (exact string from AC).
assertContains(minesBody, "Select a source row to place mines.");

// M-only filter: rows whose weaponType is not M must be skipped.
assertContains(minesBody, "if (source.weaponType != FWeapon::M)");

// No combined "Weapon Placement Done" label must exist anywhere in the source.
assertNotContains(source, "Weapon Placement Done");

// Mine done button uses fixed label "Mine Placement Done" set in constructor,
// confirmed via constructor; the drawPlaceMines path shows m_buttonMinePlacementDone.
assertContains(minesBody, "m_buttonMinePlacementDone->Show();");
}

void FTacticalBattleDisplayFireFlowTest::testGetActionButtonRowBottomIncludesSeekerPlacementDoneButton() {
// AC: SMF-02 -- getActionButtonRowBottom() scans m_buttonSeekerPlacementDone
// in its shown-button array so the seeker placement button bottom contributes
// to the source-list y-start just as the mine placement button does.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string rowBottomBody = extractFunctionBody(source, "int FBattleDisplay::getActionButtonRowBottom() const");

// m_buttonSeekerPlacementDone must appear in the scan array inside getActionButtonRowBottom().
assertContains(rowBottomBody, "m_buttonSeekerPlacementDone,");
}

void FTacticalBattleDisplayFireFlowTest::testDrawPlaceMinesExpandsPanelHeightWhenRowsExceedMinimum() {
// AC: SMFR-01 -- Source contract: drawPlaceMines() uses the variable name mineListBottom = y + BORDER,
// compares it against requestedDisplayHeight, and calls applyRequestedDisplayHeight() when it exceeds.
// This mirrors the pattern used by drawOffensiveSeekerPendingRows().
//
// NOTE: This is a source-contract (structural) test only. It verifies that the code is shaped
// correctly but does NOT prove the runtime behavior works. The authoritative behavioral coverage
// for this criterion is testOrdnancePlacementAndActivationPanelHeightAutoExpands in
// tests/gui/TacticalGuiLiveTest.cpp. Removing that behavioral test would leave only this
// source-inspection check, which violates AGENTS.md policy (behavioral tests are mandatory).
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string minesBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceMines(wxDC &dc)");

// Variable name and computation must match the spec exactly.
assertContains(minesBody, "const int mineListBottom = y + BORDER;");

// Expansion guard: must compare against requestedDisplayHeight.
assertContains(minesBody, "if (mineListBottom > m_lowerPanelLayoutState.requestedDisplayHeight)");

// Must update requestedDisplayHeight and call applyRequestedDisplayHeight() inside the guard.
assertContains(minesBody, "m_lowerPanelLayoutState.requestedDisplayHeight = mineListBottom;");
assertContains(minesBody, "applyRequestedDisplayHeight();");
}

void FTacticalBattleDisplayFireFlowTest::testDrawPlaceSeekersExpandsPanelHeightWhenRowsExceedMinimum() {
// AC: SMFR-01 -- Source contract: drawPlaceSeekers() uses the variable name seekerListBottom = y + BORDER,
// compares it against requestedDisplayHeight, and calls applyRequestedDisplayHeight() when it exceeds.
//
// NOTE: This is a source-contract (structural) test only. It verifies that the code is shaped
// correctly but does NOT prove the runtime behavior works. The authoritative behavioral coverage
// for this criterion is testOrdnancePlacementAndActivationPanelHeightAutoExpands in
// tests/gui/TacticalGuiLiveTest.cpp. Removing that behavioral test would leave only this
// source-inspection check, which violates AGENTS.md policy (behavioral tests are mandatory).
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string seekersBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceSeekers(wxDC &dc)");

// Variable name and computation must match the spec exactly.
assertContains(seekersBody, "const int seekerListBottom = y + BORDER;");

// Expansion guard: must compare against requestedDisplayHeight.
assertContains(seekersBody, "if (seekerListBottom > m_lowerPanelLayoutState.requestedDisplayHeight)");

// Must update requestedDisplayHeight and call applyRequestedDisplayHeight() inside the guard.
assertContains(seekersBody, "m_lowerPanelLayoutState.requestedDisplayHeight = seekerListBottom;");
assertContains(seekersBody, "applyRequestedDisplayHeight();");
}

void FTacticalBattleDisplayFireFlowTest::testDrawSeekerActivationExpandsPanelHeightWhenRowsExceedMinimum() {
// AC: SMFR-01 -- Source contract: drawSeekerActivation() uses the variable name activationListBottom = y + BORDER,
// compares it against requestedDisplayHeight, and calls applyRequestedDisplayHeight() when it exceeds.
//
// NOTE: This is a source-contract (structural) test only. It verifies that the code is shaped
// correctly but does NOT prove the runtime behavior works. The authoritative behavioral coverage
// for this criterion is testOrdnancePlacementAndActivationPanelHeightAutoExpands in
// tests/gui/TacticalGuiLiveTest.cpp. Removing that behavioral test would leave only this
// source-inspection check, which violates AGENTS.md policy (behavioral tests are mandatory).
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string activationBody = extractFunctionBody(source, "void FBattleDisplay::drawSeekerActivation(wxDC &dc)");

// Variable name and computation must match the spec exactly.
assertContains(activationBody, "const int activationListBottom = y + BORDER;");

// Expansion guard: must compare against requestedDisplayHeight.
assertContains(activationBody, "if (activationListBottom > m_lowerPanelLayoutState.requestedDisplayHeight)");

// Must update requestedDisplayHeight and call applyRequestedDisplayHeight() inside the guard.
assertContains(activationBody, "m_lowerPanelLayoutState.requestedDisplayHeight = activationListBottom;");
assertContains(activationBody, "applyRequestedDisplayHeight();");
}

void FTacticalBattleDisplayFireFlowTest::testDrawPlaceSeekersThreeColumnLayoutMatchesMinePhasePattern() {
// AC: SMRIV-02 -- drawPlaceSeekers() uses a three-column layout that mirrors drawPlaceMines():
//   Left column:   instruction text wrapped by drawWrappedActionPrompt() in the left column.
//   Middle column: source-selection rows at lMargin=310, anchored at getActionPromptLineY(0).
//   Right column:  recall list at recallMargin=620, anchored at getActionPromptLineY(0).
// Both columns are independent; source rows use wxRect(lMargin,y,...); recall rows use
// wxRect(recallMargin,cy,...) with cy reset to getActionPromptLineY(0) for the right column.
//
// NOTE: This is a source-contract (structural) test. It verifies that the code is shaped
// correctly but does NOT prove the runtime column positions or click-region alignment.
// The authoritative behavioral coverage is
// testPlaceSeekersThreeColumnLayoutColumnPositionsAndClickRegions in TacticalGuiLiveTest.cpp.
const std::string source = readFile(repoFile("src/tactical/FBattleDisplay.cpp"));
const std::string seekersBody = extractFunctionBody(source, "void FBattleDisplay::drawPlaceSeekers(wxDC &dc)");

// Left column: instruction text wrapped via drawWrappedActionPrompt().
assertContains(seekersBody, "drawWrappedActionPrompt(dc,");

// Middle column: lMargin=310, y anchored at getActionPromptLineY(0).
assertContains(seekersBody, "int lMargin = 310;");
assertContains(seekersBody, "int y = getActionPromptLineY(0);");

// Right column: recallMargin=620, cy anchored at getActionPromptLineY(0).
assertContains(seekersBody, "const int recallMargin = 620;");
assertContains(seekersBody, "int cy = getActionPromptLineY(0);");

// Source-row hit regions must use wxRect(lMargin,y,...).
assertContains(seekersBody, "m_shipNameRegions.push_back(wxRect(lMargin,y,");

// Recall-row hit regions must use wxRect(recallMargin,cy,...).
assertContains(seekersBody, "m_preGameSeekerRecallRegions.push_back(wxRect(recallMargin, cy,");

// Recall column header text drawn at recallMargin.
assertContains(seekersBody, "dc.DrawText(\"Placed seekers (click to recall):\", recallMargin,");

// Right column is independent of middle: recallMargin > lMargin ensures no overlap.
// Verified structurally by confirming both margins are defined in the same function.
assertBefore(seekersBody, "int lMargin = 310;", "const int recallMargin = 620;");
}

}
