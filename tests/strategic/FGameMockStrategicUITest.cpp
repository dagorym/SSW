/**
 * @file FGameMockStrategicUITest.cpp
 * @brief Implementation file for FGameMockStrategicUITest
 */

#include "FGameMockStrategicUITest.h"

#include <string>

namespace FrontierTests {
using namespace Frontier;

namespace {

class MockStrategicUI : public IStrategicUI {
public:
	MockStrategicUI() :
		showMessageCalls(0),
		selectRetreatConditionCalls(0),
		runUPFUnattachedSetupCalls(0),
		runSatharFleetSetupCalls(0),
		requestRedrawCalls(0),
		showRetreatConditionsCalls(0),
		lastSatharInitialSetup(false),
		lastUPFPlayer(NULL),
		lastUPFMap(NULL),
		lastSatharPlayer(NULL),
		lastSatharMap(NULL) {}

	void showMessage(const std::string& title, const std::string& body) {
		showMessageCalls++;
		lastMessageTitle = title;
		lastMessageBody = body;
	}

	void notifyFailedJump(const std::string& fleetName) {
		(void)fleetName;
	}

	void notifyVictory(int result) {
		(void)result;
	}

	int selectRetreatCondition() {
		selectRetreatConditionCalls++;
		return 3;
	}

	int runUPFUnattachedSetup(FPlayer* player, FMap* map) {
		runUPFUnattachedSetupCalls++;
		lastUPFPlayer = player;
		lastUPFMap = map;
		return 0;
	}

	int runSatharFleetSetup(FPlayer* player, FMap* map, bool isInitialSetup) {
		runSatharFleetSetupCalls++;
		lastSatharPlayer = player;
		lastSatharMap = map;
		lastSatharInitialSetup = isInitialSetup;
		return 0;
	}

	void showSystemDialog(FSystem* sys, FMap* map, FPlayer* player) {
		(void)sys;
		(void)map;
		(void)player;
	}

	void showFleetDialog(FFleet* fleet, FSystem* location, FSystem* destination) {
		(void)fleet;
		(void)location;
		(void)destination;
	}

	void showRetreatConditions(const std::string& text) {
		showRetreatConditionsCalls++;
		lastRetreatConditionsText = text;
	}

	int selectCombat(FSystem* sys,
					 FleetList defenders,
					 FleetList attackers,
					 PlayerList* players) {
		(void)sys;
		(void)defenders;
		(void)attackers;
		(void)players;
		return 0;
	}

	void requestRedraw() {
		requestRedrawCalls++;
	}

	int showMessageCalls;
	int selectRetreatConditionCalls;
	int runUPFUnattachedSetupCalls;
	int runSatharFleetSetupCalls;
	int requestRedrawCalls;
	int showRetreatConditionsCalls;
	bool lastSatharInitialSetup;
	FPlayer* lastUPFPlayer;
	FMap* lastUPFMap;
	FPlayer* lastSatharPlayer;
	FMap* lastSatharMap;
	std::string lastMessageTitle;
	std::string lastMessageBody;
	std::string lastRetreatConditionsText;
};

}  // namespace

CPPUNIT_TEST_SUITE_REGISTRATION(FGameMockStrategicUITest);

void FGameMockStrategicUITest::setUp() {
	m_ui = new MockStrategicUI();
	m_game = &(FGame::create(m_ui));
}

void FGameMockStrategicUITest::tearDown() {
	delete m_game;
	delete m_ui;
}

void FGameMockStrategicUITest::testInitWithMockStrategicUI() {
	MockStrategicUI *ui = static_cast<MockStrategicUI*>(m_ui);

	CPPUNIT_ASSERT_EQUAL(0, m_game->init(NULL));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), m_game->getPlayers().size());
	CPPUNIT_ASSERT_EQUAL(std::string("UPF"), m_game->getPlayers()[0]->getName());
	CPPUNIT_ASSERT_EQUAL(std::string("Sathar"), m_game->getPlayers()[1]->getName());
	CPPUNIT_ASSERT(!m_game->isUPFTurn());
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), m_game->getRound());
	CPPUNIT_ASSERT_EQUAL(m_game->getPlayers()[1]->getID(), m_game->getCurrentPlayerID());

	CPPUNIT_ASSERT_EQUAL(1, ui->requestRedrawCalls);
	CPPUNIT_ASSERT_EQUAL(1, ui->selectRetreatConditionCalls);
	CPPUNIT_ASSERT_EQUAL(1, ui->runUPFUnattachedSetupCalls);
	CPPUNIT_ASSERT_EQUAL(1, ui->runSatharFleetSetupCalls);
	CPPUNIT_ASSERT(ui->lastSatharInitialSetup);
	CPPUNIT_ASSERT(ui->lastUPFPlayer != NULL);
	CPPUNIT_ASSERT(ui->lastSatharPlayer != NULL);
	CPPUNIT_ASSERT(ui->lastUPFMap != NULL);
	CPPUNIT_ASSERT(ui->lastSatharMap != NULL);
	CPPUNIT_ASSERT_EQUAL(ui->lastUPFMap, ui->lastSatharMap);
	CPPUNIT_ASSERT_EQUAL(std::string("UPF"), ui->lastUPFPlayer->getName());
	CPPUNIT_ASSERT_EQUAL(std::string("Sathar"), ui->lastSatharPlayer->getName());

	m_game->showPlayers();
	CPPUNIT_ASSERT_EQUAL(1, ui->showMessageCalls);
	CPPUNIT_ASSERT_EQUAL(std::string("Player List"), ui->lastMessageTitle);
	CPPUNIT_ASSERT(ui->lastMessageBody.find("UPF") != std::string::npos);
	CPPUNIT_ASSERT(ui->lastMessageBody.find("Sathar") != std::string::npos);

	m_game->showRetreatConditions();
	CPPUNIT_ASSERT_EQUAL(1, ui->showRetreatConditionsCalls);
	CPPUNIT_ASSERT(ui->lastRetreatConditionsText.find("40 ships") != std::string::npos);
}

}
