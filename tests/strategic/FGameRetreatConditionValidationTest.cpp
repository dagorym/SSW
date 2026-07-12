/**
 * @file FGameRetreatConditionValidationTest.cpp
 * @brief Implementation file for FGameRetreatConditionValidationTest
 */

#include "FGameRetreatConditionValidationTest.h"

#include <string>
#include <vector>

namespace FrontierTests {
using namespace Frontier;

namespace {

/**
 * @brief Mock IStrategicUI whose selectRetreatCondition() replays a scripted
 *        sequence of return values, one per call, holding the last value once
 *        the sequence is exhausted. Lets a test script an out-of-range value
 *        (for example an X-close/cancel sentinel) followed by a valid 1..5
 *        value to exercise FGame::init()'s re-prompt loop.
 */
class ScriptedRetreatConditionMockStrategicUI : public IStrategicUI {
public:
	explicit ScriptedRetreatConditionMockStrategicUI(const std::vector<int>& returnSequence) :
		m_returnSequence(returnSequence),
		m_nextIndex(0),
		selectRetreatConditionCalls(0),
		requestRedrawCalls(0),
		showRetreatConditionsCalls(0) {}

	void showMessage(const std::string& title, const std::string& body) {
		(void)title;
		(void)body;
	}

	void notifyFailedJump(const std::string& fleetName) {
		(void)fleetName;
	}

	void notifyVictory(int result) {
		(void)result;
	}

	int selectRetreatCondition() {
		selectRetreatConditionCalls++;
		size_t idx = (m_nextIndex < m_returnSequence.size()) ? m_nextIndex : (m_returnSequence.size() - 1);
		int value = m_returnSequence[idx];
		if (m_nextIndex < m_returnSequence.size()) {
			m_nextIndex++;
		}
		return value;
	}

	int runUPFUnattachedSetup(FPlayer* player, FMap* map) {
		(void)player;
		(void)map;
		return 0;
	}

	int runSatharFleetSetup(FPlayer* player, FMap* map, bool isInitialSetup) {
		(void)player;
		(void)map;
		(void)isInitialSetup;
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

	std::vector<int> m_returnSequence;
	size_t m_nextIndex;
	int selectRetreatConditionCalls;
	int requestRedrawCalls;
	int showRetreatConditionsCalls;
	std::string lastRetreatConditionsText;
};

}  // namespace

CPPUNIT_TEST_SUITE_REGISTRATION(FGameRetreatConditionValidationTest);

void FGameRetreatConditionValidationTest::setUp() {
}

void FGameRetreatConditionValidationTest::tearDown() {
}

void FGameRetreatConditionValidationTest::testInitRepromptsUntilValidRetreatConditionIsReturned() {
	// 0 and 6 are out-of-range; -1 stands in for an X-close/cancel sentinel
	// (e.g. wxID_CANCEL) that is likewise outside the valid 1..5 range. Only
	// the final scripted value (2) is valid.
	std::vector<int> sequence;
	sequence.push_back(0);
	sequence.push_back(6);
	sequence.push_back(-1);
	sequence.push_back(2);
	ScriptedRetreatConditionMockStrategicUI mock(sequence);

	FGame* game = &(FGame::create(&mock));
	CPPUNIT_ASSERT_EQUAL(0, game->init(NULL));

	// Proves re-prompting occurred: init() did not stop at the first
	// (invalid) return value.
	CPPUNIT_ASSERT(mock.selectRetreatConditionCalls > 1);
	CPPUNIT_ASSERT_EQUAL(4, mock.selectRetreatConditionCalls);

	// The stored condition must be the VALID one (2), observed through the
	// per-condition dispatch text of showRetreatConditions() rather than any
	// source inspection. Against the pre-fix unvalidated assignment, the
	// first (invalid) value would have been stored, m_satharRetreat would not
	// match any switch case, and this text would read
	// "Error:  No retreat condition selected" instead.
	game->showRetreatConditions();
	CPPUNIT_ASSERT_EQUAL(1, mock.showRetreatConditionsCalls);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("five heavy cruisers") != std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("Error") == std::string::npos);

	delete game;
}

void FGameRetreatConditionValidationTest::testInitAcceptsValidRetreatConditionWithoutExtraPrompts() {
	std::vector<int> sequence;
	sequence.push_back(4);  // valid on the first call
	ScriptedRetreatConditionMockStrategicUI mock(sequence);

	FGame* game = &(FGame::create(&mock));
	CPPUNIT_ASSERT_EQUAL(0, game->init(NULL));

	// Exactly one call: a valid first response must not trigger any
	// re-prompting.
	CPPUNIT_ASSERT_EQUAL(1, mock.selectRetreatConditionCalls);

	game->showRetreatConditions();
	CPPUNIT_ASSERT_EQUAL(1, mock.showRetreatConditionsCalls);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("destroying at least two") != std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("Error") == std::string::npos);

	delete game;
}

void FGameRetreatConditionValidationTest::testInitStoredConditionDrivesCorrectVictoryDispatch() {
	std::vector<int> sequence;
	sequence.push_back(5);  // valid on the first call
	ScriptedRetreatConditionMockStrategicUI mock(sequence);

	FGame* game = &(FGame::create(&mock));
	CPPUNIT_ASSERT_EQUAL(0, game->init(NULL));
	CPPUNIT_ASSERT_EQUAL(1, mock.selectRetreatConditionCalls);

	// Behavioral confirmation the stored condition is a real 1..5 value
	// dispatched to its own switch case (not the invalid/default branch)
	// via the observable per-condition dispatch text.
	game->showRetreatConditions();
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find(
		"Fighters and Militia ships are not counted") != std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("Error") == std::string::npos);

	// checkForVictory() is private (not test-accessible without a public
	// accessor or deep combat-state setup to trigger an actual UPF-victory
	// branch), so the per-condition dispatch text above -- routed through the
	// same m_satharRetreat switch that drives checkForVictory()'s victory
	// switch -- is the feasible behavioral proof that init() stored a real,
	// dispatchable 1..5 condition rather than an unvalidated value that would
	// silently fall through to checkForVictory()'s default (UPFVictory =
	// false forever) branch.

	delete game;
}

void FGameRetreatConditionValidationTest::testInitBoundsRepromptLoopAndLeavesRetreatConditionUnset() {
	// A single-element sequence of an out-of-range X-close/cancel sentinel
	// (-1, standing in for wxID_CANCEL) replays forever once the sequence is
	// exhausted (see ScriptedRetreatConditionMockStrategicUI::selectRetreatCondition()),
	// modeling a degenerate UI -- e.g. a headless WXStrategicUI -- that never
	// returns a valid 1..5 value.
	std::vector<int> sequence;
	sequence.push_back(-1);
	ScriptedRetreatConditionMockStrategicUI mock(sequence);

	FGame* game = &(FGame::create(&mock));

	// If the re-prompt loop is unbounded, this call hangs; a bound proves
	// init() terminates and returns normally in the same test run.
	CPPUNIT_ASSERT_EQUAL(0, game->init(NULL));

	// The loop must be bounded to exactly the documented cap
	// (kMaxRetreatConditionPrompts = 1000 in FGame::init()), not merely
	// "eventually" stop for some other reason.
	CPPUNIT_ASSERT_EQUAL(1000, mock.selectRetreatConditionCalls);

	// The stored condition must NOT be the bogus out-of-range sentinel:
	// showRetreatConditions() must fall through to the safe default/unset
	// "Error" branch rather than dispatching any valid 1..5 condition's
	// text, proving m_satharRetreat was left at its prior/default value
	// instead of being set to an invalid value.
	game->showRetreatConditions();
	CPPUNIT_ASSERT_EQUAL(1, mock.showRetreatConditionsCalls);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("Error") != std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("two assault carriers") == std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("five heavy cruisers") == std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("40 ships") == std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("tenday passes") == std::string::npos);
	CPPUNIT_ASSERT(mock.lastRetreatConditionsText.find("Fighters and Militia ships are not counted") == std::string::npos);

	delete game;
}

}
