/**
 * @file FGameSaveFormatTest.cpp
 * @brief Implementation file for the FGameSaveFormatTest class
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 17, 2026
 */

#include "FGameSaveFormatTest.h"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

#include "strategic/FFleet.h"
#include "strategic/FMap.h"
#include "strategic/FPlayer.h"
#include "strategic/FSystem.h"
#include "strategic/IStrategicUI.h"
#include "ships/FVehicle.h"

namespace FrontierTests {
using namespace Frontier;

CPPUNIT_TEST_SUITE_REGISTRATION( FGameSaveFormatTest );

namespace {

/**
 * @brief Minimal IStrategicUI that counts showMessage() calls and records
 * the most recent title/body, so a test can assert exactly one load-error
 * report was issued without depending on the exact wording.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 17, 2026
 */
class RecordingUI : public IStrategicUI {
public:
	RecordingUI() : showMessageCalls(0) {}

	void showMessage(const std::string& title, const std::string& body) {
		showMessageCalls++;
		lastTitle = title;
		lastBody = body;
	}
	void notifyFailedJump(const std::string&) {}
	void notifyVictory(int) {}
	int selectRetreatCondition() { return 3; }
	int runUPFUnattachedSetup(FPlayer*, FMap*) { return 0; }
	int runSatharFleetSetup(FPlayer*, FMap*, bool) { return 0; }
	void showSystemDialog(FSystem*, FMap*, FPlayer*) {}
	void showFleetDialog(FFleet*, FSystem*, FSystem*) {}
	void showRetreatConditions(const std::string&) {}
	int selectCombat(FSystem*, FleetList, FleetList, PlayerList*) { return 0; }
	void requestRedraw() {}

	int showMessageCalls;
	std::string lastTitle;
	std::string lastBody;
};

/**
 * @brief Builds a real, valid FGame::save() byte stream from a freshly
 * init()'d game, then tears the singleton back down so the caller starts
 * from a clean slate for the load() under test.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 17, 2026
 */
std::string buildValidSaveBytes(){
	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	CPPUNIT_ASSERT_EQUAL(0, g.init(NULL));
	std::ostringstream os;
	CPPUNIT_ASSERT_EQUAL(0, g.save(os));
	std::string bytes = os.str();
	delete &g;
	return bytes;
}

}  // namespace

void FGameSaveFormatTest::setUp(){
}

void FGameSaveFormatTest::tearDown(){
	// Defensively tear down whatever FGame singleton the last test left
	// live so the next test (in this fixture or any other) starts clean.
	delete &(FGame::create());
}

void FGameSaveFormatTest::testSaveEmitsMagicAndVersionHeaderBeforeGameData(){
	std::string bytes = buildValidSaveBytes();
	CPPUNIT_ASSERT(bytes.size() >= 8);

	// Decode the first 8 bytes as two explicit little-endian uint32_t
	// values (least-significant byte first), independent of host
	// endianness, matching writeU32()'s documented encoding.
	const unsigned char *raw = reinterpret_cast<const unsigned char *>(bytes.data());
	uint32_t magic = static_cast<uint32_t>(raw[0])
			| (static_cast<uint32_t>(raw[1]) << 8)
			| (static_cast<uint32_t>(raw[2]) << 16)
			| (static_cast<uint32_t>(raw[3]) << 24);
	uint32_t version = static_cast<uint32_t>(raw[4])
			| (static_cast<uint32_t>(raw[5]) << 8)
			| (static_cast<uint32_t>(raw[6]) << 16)
			| (static_cast<uint32_t>(raw[7]) << 24);

	CPPUNIT_ASSERT_EQUAL(kSaveMagic, magic);
	CPPUNIT_ASSERT_EQUAL(kSaveFormatVersion, version);
}

void FGameSaveFormatTest::testFullRoundTripRestoresRoundCurrentPlayerRetreatConditionLossCountersMapAndPlayers(){
	RecordingUI *ui1 = new RecordingUI();
	FGame *g1 = &(FGame::create(ui1));
	CPPUNIT_ASSERT_EQUAL(0, g1->init(NULL));

	FPlayer *upf = g1->getPlayer(1);
	FPlayer *sathar = g1->getPlayer(2);
	CPPUNIT_ASSERT(upf != NULL);
	CPPUNIT_ASSERT(sathar != NULL);

	FFleet *upfFleet = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(upfFleet != NULL);
	FSystem *sys = FMap::getMap().getSystem(upfFleet->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	// A co-located Sathar raider fleet with one doomed and one surviving
	// ship: triggers checkForCombat()/cleanUpShips() on the next
	// endUPFTurn(), which mutates the private loss counters and prunes the
	// doomed ship -- state with no public getter, verified below via a
	// byte-for-byte re-save comparison rather than a source-text check.
	FFleet *raiders = new FFleet;
	raiders->setName("P5-5 Roundtrip Raiders");
	raiders->setOwner(sathar->getID());
	raiders->setLocation(sys, false);
	FVehicle *doomed = createShip("Destroyer");
	doomed->setHP(0);
	FVehicle *survivor = createShip("Destroyer");
	unsigned int survivorID = survivor->getID();
	raiders->addShip(doomed);
	raiders->addShip(survivor);
	sathar->addFleet(raiders);
	sys->addFleet(raiders);

	// Drive the game to a state distinguishable from a fresh init(): after
	// init() the current player is already Sathar (players[1]) and round is
	// 0, so end exactly one Sathar turn, then one UPF turn (advances round
	// to 1, triggers cleanup, and leaves current player back on Sathar),
	// then one more Sathar turn so current player ends on UPF -- different
	// from the post-init() default on both round and current player.
	g1->endSatharTurn();
	g1->endUPFTurn();  // round -> 1; drives cleanUpShips()
	g1->endSatharTurn();  // current player -> UPF

	unsigned int roundBefore = g1->getRound();
	unsigned int currentPlayerBefore = g1->getCurrentPlayerID();
	size_t playerCountBefore = g1->getPlayers().size();
	std::string p1NameBefore = g1->getPlayers()[0]->getName();
	std::string p2NameBefore = g1->getPlayers()[1]->getName();
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), roundBefore);
	CPPUNIT_ASSERT_EQUAL(upf->getID(), currentPlayerBefore);

	std::ostringstream os;
	CPPUNIT_ASSERT_EQUAL(0, g1->save(os));
	std::string originalBytes = os.str();

	delete g1;
	delete ui1;

	RecordingUI *ui2 = new RecordingUI();
	FGame *g2 = &(FGame::create(ui2));
	std::istringstream is(originalBytes);
	int rc = g2->load(is);

	CPPUNIT_ASSERT_EQUAL(0, rc);
	CPPUNIT_ASSERT_EQUAL(0, ui2->showMessageCalls);

	CPPUNIT_ASSERT_EQUAL(roundBefore, g2->getRound());
	CPPUNIT_ASSERT_EQUAL(currentPlayerBefore, g2->getCurrentPlayerID());
	CPPUNIT_ASSERT_EQUAL(playerCountBefore, g2->getPlayers().size());
	CPPUNIT_ASSERT_EQUAL(p1NameBefore, g2->getPlayers()[0]->getName());
	CPPUNIT_ASSERT_EQUAL(p2NameBefore, g2->getPlayers()[1]->getName());

	FPlayer *sathar2 = g2->getPlayer(2);
	CPPUNIT_ASSERT(sathar2 != NULL);
	FFleet *raiders2 = sathar2->getFleet("P5-5 Roundtrip Raiders");
	CPPUNIT_ASSERT(raiders2 != NULL);
	// The doomed ship was pruned by cleanUpShips() before the save above,
	// so exactly the surviving ship (same persisted ID) must come back.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), raiders2->getShipList().size());
	CPPUNIT_ASSERT_EQUAL(survivorID, raiders2->getShipList()[0]->getID());

	// Byte-for-byte proof that load() faithfully reconstructed every field
	// save() writes -- including the Sathar retreat condition and loss
	// counters, neither of which has a public getter -- since a faithful
	// save()/load() round trip must re-emit identical bytes for identical
	// game state.
	std::ostringstream os2;
	CPPUNIT_ASSERT_EQUAL(0, g2->save(os2));
	std::string reloadedBytes = os2.str();
	CPPUNIT_ASSERT(originalBytes == reloadedBytes);

	delete g2;
	delete ui2;
}

void FGameSaveFormatTest::testLoadWrongMagicReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();
	CPPUNIT_ASSERT(bytes.size() >= 4);
	// Flip every bit of the 4-byte magic tag so it cannot coincidentally
	// still equal kSaveMagic.
	for (int i = 0; i < 4; i++){
		bytes[i] = static_cast<char>(~bytes[i]);
	}

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	delete &g;
}

void FGameSaveFormatTest::testLoadUnsupportedVersionReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();
	CPPUNIT_ASSERT(bytes.size() >= 8);
	// Leave the magic tag (first 4 bytes) untouched; corrupt only the
	// version field (next 4 bytes) to a value that cannot equal
	// kSaveFormatVersion (1).
	bytes[4] = static_cast<char>(0xFF);
	bytes[5] = static_cast<char>(0xFF);
	bytes[6] = static_cast<char>(0xFF);
	bytes[7] = static_cast<char>(0xFF);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	delete &g;
}

void FGameSaveFormatTest::testLoadTruncatedStreamReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();
	CPPUNIT_ASSERT(bytes.size() > 16);
	// Cut the stream immediately after the 8-byte magic+version header, so
	// magic/version themselves validate but the very next read (m_gui, one
	// of FGame::load()'s own explicitly-checked header-field reads) finds
	// no bytes left and fails. A deeper cut point risks landing inside
	// FPlayer::load()'s fleet loop, which does not check FFleet::load()'s
	// return value (a pre-existing gap outside this subtask's file list) --
	// truncating right after the header instead exercises only the checked
	// reads this subtask (P5-5) is responsible for.
	std::string truncated = bytes.substr(0, 8);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(truncated);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	delete &g;
}

void FGameSaveFormatTest::testLoadUnknownFactoryTypeReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();

	// Build the exact wire-format encoding of a "Destroyer" ship-type tag:
	// a 4-byte little-endian length (9) followed by the raw ASCII bytes,
	// matching FPObject::writeString()'s encoding, which every
	// FVehicle::save() call uses to write m_type first. A default-init()'d
	// game's UPF player m_unattached list always includes real Destroyers
	// (see FGame::addUPFUnattached()), so this exact byte pattern is
	// guaranteed present, and FPlayer::load()'s m_unattached loop already
	// checks createShip()'s result and returns nonzero on an unrecognized
	// type (see FPlayer::load()).
	std::string lengthPrefixedType;
	lengthPrefixedType.push_back(static_cast<char>(9));
	lengthPrefixedType.push_back(static_cast<char>(0));
	lengthPrefixedType.push_back(static_cast<char>(0));
	lengthPrefixedType.push_back(static_cast<char>(0));
	lengthPrefixedType += "Destroyer";

	size_t pos = bytes.find(lengthPrefixedType);
	CPPUNIT_ASSERT(pos != std::string::npos);

	// Overwrite just the 9 type-name bytes (leaving the 4-byte length
	// prefix untouched) with a same-length string that is not one of
	// createShip()'s recognized type tags.
	const std::string bogusType = "NotAShip1";
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), bogusType.size());
	bytes.replace(pos + 4, bogusType.size(), bogusType);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	delete &g;
}

void FGameSaveFormatTest::testLoadFailureWithNoUIFallsBackToConsoleAndReturnsNonzero(){
	std::string bytes = buildValidSaveBytes();
	CPPUNIT_ASSERT(bytes.size() >= 4);
	for (int i = 0; i < 4; i++){
		bytes[i] = static_cast<char>(~bytes[i]);
	}

	FGame &g = FGame::create();  // no IStrategicUI installed (NULL)

	std::ostringstream capturedOut;
	std::streambuf *originalCoutBuf = std::cout.rdbuf(capturedOut.rdbuf());
	std::istringstream is(bytes);
	int rc = g.load(is);
	std::cout.rdbuf(originalCoutBuf);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT(!capturedOut.str().empty());
	delete &g;
}

}
