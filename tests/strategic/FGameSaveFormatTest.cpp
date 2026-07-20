/**
 * @file FGameSaveFormatTest.cpp
 * @brief Implementation file for the FGameSaveFormatTest class
 * @author Claude Sonnet 5 (medium), Claude Opus 4.8 (1M context) (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 19, 2026
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

/**
 * @brief Builds the exact wire-format encoding FPObject::writeString()
 * produces for a string: a 4-byte little-endian length prefix followed by
 * the raw bytes. Used to locate a specific vehicle's own name (or type tag)
 * field within a full-game save byte buffer via std::string::find(), the
 * same byte-surgery technique the pre-existing tests in this fixture use.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 19, 2026
 * @date Last Modified: Jul 19, 2026
 */
std::string lengthPrefixedTag(const std::string &s){
	std::string tag;
	uint32_t len = static_cast<uint32_t>(s.size());
	tag.push_back(static_cast<char>(len & 0xFF));
	tag.push_back(static_cast<char>((len >> 8) & 0xFF));
	tag.push_back(static_cast<char>((len >> 16) & 0xFF));
	tag.push_back(static_cast<char>((len >> 24) & 0xFF));
	tag += s;
	return tag;
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

void FGameSaveFormatTest::testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();

	// Locate UPF's "Task Force Prenglar" fleet by its own length-prefixed
	// name field (unique in the save; only this one fleet is named this),
	// using the same 4-byte little-endian length prefix + raw ASCII
	// encoding FPObject::writeString() produces.
	std::string fleetNameTag;
	fleetNameTag.push_back(static_cast<char>(19));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag += "Task Force Prenglar";
	size_t fleetNamePos = bytes.find(fleetNameTag);
	CPPUNIT_ASSERT(fleetNamePos != std::string::npos);

	// createTFPrenglar() (FGame.cpp) adds this fleet's ships in the order
	// 5x AssaultScout, 2x Minelayer, 3x Frigate, 2x Destroyer, 3x LtCruiser,
	// 1x Battleship, so the fleet's first ship's own FVehicle::save() writes
	// an "AssaultScout" type-tag first. Search for that tag starting from
	// fleetNamePos (not from the start of the stream) so the earlier
	// AssaultScout entries already written into UPF's m_unattached list
	// (which FPlayer::save() emits before m_fleets -- see
	// FGame::addUPFUnattached()) are not matched instead of this fleet's own
	// ship.
	std::string shipTypeTag;
	shipTypeTag.push_back(static_cast<char>(12));
	shipTypeTag.push_back(static_cast<char>(0));
	shipTypeTag.push_back(static_cast<char>(0));
	shipTypeTag.push_back(static_cast<char>(0));
	shipTypeTag += "AssaultScout";
	size_t shipTypePos = bytes.find(shipTypeTag, fleetNamePos);
	CPPUNIT_ASSERT(shipTypePos != std::string::npos);

	// Truncate the stream to end immediately before the first byte of that
	// ship's type-tag length prefix: the fleet's own header fields and its
	// ship-count (sCount) are fully present, but no byte of the first ship's
	// own record survives. FFleet::load()'s ship loop hits EOF on the
	// length-prefix read inside readString(), so createShip("") returns
	// NULL and FFleet::load()'s pre-existing `if (v == NULL) return 1;`
	// check fires; FPlayer::load()'s FR-1 fix
	// (`if (f->load(is) != 0){ delete f; return 1; }`) is what propagates
	// that failure up instead of silently discarding it (the pre-FR-1
	// behavior, which pushed the half-built fleet and returned 0).
	std::string truncated = bytes.substr(0, shipTypePos);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(truncated);
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

void FGameSaveFormatTest::testLoadFleetWithOutOfRangeLocationIdReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();

	// Locate UPF's "Task Force Prenglar" fleet by its own length-prefixed
	// name field, using the same 4-byte little-endian length prefix + raw
	// ASCII encoding FPObject::writeString() produces (see the sibling
	// truncated-ship-record test above for the identical lookup pattern).
	// This fleet is placed in the Prenglar system by createTFPrenglar(), so
	// its serialized m_location is a real, nonzero system ID before we
	// clobber it below.
	std::string fleetNameTag;
	fleetNameTag.push_back(static_cast<char>(19));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag += "Task Force Prenglar";
	size_t fleetNamePos = bytes.find(fleetNameTag);
	CPPUNIT_ASSERT(fleetNamePos != std::string::npos);

	// FFleet::save() writes, in order after the name: m_owner (4 bytes),
	// then m_location (4 bytes) -- the field under test here.
	size_t ownerPos = fleetNamePos + fleetNameTag.size();
	size_t locationPos = ownerPos + 4;
	CPPUNIT_ASSERT(locationPos + 4 <= bytes.size());

	// Sanity: confirm the byte range we are about to overwrite currently
	// decodes to a real, nonzero system ID (i.e. we found a placed fleet's
	// location field, not some unrelated bytes).
	const unsigned char *raw = reinterpret_cast<const unsigned char *>(bytes.data());
	uint32_t originalLocation = static_cast<uint32_t>(raw[locationPos])
			| (static_cast<uint32_t>(raw[locationPos + 1]) << 8)
			| (static_cast<uint32_t>(raw[locationPos + 2]) << 16)
			| (static_cast<uint32_t>(raw[locationPos + 3]) << 24);
	CPPUNIT_ASSERT(originalLocation != 0);

	// Overwrite the 4-byte m_location field with a little-endian encoding of
	// 0x0000BEEF, a value that is neither 0 (the "not yet in a system"
	// sentinel) nor any real loaded system ID (the loaded universe has only
	// a small, low-numbered set of systems).
	bytes[locationPos] = static_cast<char>(0xEF);
	bytes[locationPos + 1] = static_cast<char>(0xBE);
	bytes[locationPos + 2] = static_cast<char>(0x00);
	bytes[locationPos + 3] = static_cast<char>(0x00);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// UPF is the first player written to the stream (see FGame::getPlayers()
	// push order) and owns the corrupted fleet, so FGame::load()'s FF-1
	// validation aborts before any player is pushed onto m_players --
	// proving no live game state was committed.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

void FGameSaveFormatTest::testLoadFleetWithOutOfRangeJumpRouteIdReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();

	// Locate the same placed fleet as above; this time its m_location field
	// is left untouched (so the FF-1 location check passes) and only its
	// m_jumpRouteID field is corrupted.
	std::string fleetNameTag;
	fleetNameTag.push_back(static_cast<char>(19));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag += "Task Force Prenglar";
	size_t fleetNamePos = bytes.find(fleetNameTag);
	CPPUNIT_ASSERT(fleetNamePos != std::string::npos);

	// FFleet::save() field order after the name: m_owner (4), m_location
	// (4), m_inTransit (1), m_destination (4), m_transitTime (4),
	// m_jumpLength (4), m_speed (4), then m_jumpRouteID (4) -- the field
	// under test here, 17 bytes past the end of m_location.
	size_t ownerPos = fleetNamePos + fleetNameTag.size();
	size_t locationPos = ownerPos + 4;
	size_t jumpRoutePos = locationPos + 4 + 17;
	CPPUNIT_ASSERT(jumpRoutePos + 4 <= bytes.size());

	// Sanity: a freshly init()'d, placed, non-transiting fleet's
	// m_jumpRouteID is the FFleet::NO_ROUTE sentinel (all bits set) before we
	// clobber it below.
	const unsigned char *raw = reinterpret_cast<const unsigned char *>(bytes.data());
	uint32_t originalJumpRoute = static_cast<uint32_t>(raw[jumpRoutePos])
			| (static_cast<uint32_t>(raw[jumpRoutePos + 1]) << 8)
			| (static_cast<uint32_t>(raw[jumpRoutePos + 2]) << 16)
			| (static_cast<uint32_t>(raw[jumpRoutePos + 3]) << 24);
	CPPUNIT_ASSERT_EQUAL(FFleet::NO_ROUTE, originalJumpRoute);

	// Overwrite the 4-byte m_jumpRouteID field with a little-endian encoding
	// of 0x0000BEEF: neither FFleet::NO_ROUTE (0xFFFFFFFF, the "not on a
	// jump route" sentinel) nor any real loaded jump-route ID (route IDs are
	// small, sequentially assigned integers).
	bytes[jumpRoutePos] = static_cast<char>(0xEF);
	bytes[jumpRoutePos + 1] = static_cast<char>(0xBE);
	bytes[jumpRoutePos + 2] = static_cast<char>(0x00);
	bytes[jumpRoutePos + 3] = static_cast<char>(0x00);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// Same reasoning as the location-ID test above: UPF owns this fleet and
	// is the first player processed, so the FF-1 jump-route check aborts
	// before any player is committed.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

void FGameSaveFormatTest::testLoadValidSaveWithSentinelLocationAndJumpRouteSucceeds(){
	// A freshly init()'d game's save contains Strike Force Nova (location 0,
	// the "not yet in a system" sentinel) and every placed fleet on
	// FFleet::NO_ROUTE (the "not on a jump route" sentinel), all of which
	// FF-1's validation pass must accept unchanged.
	std::string bytes = buildValidSaveBytes();

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT_EQUAL(0, rc);
	CPPUNIT_ASSERT_EQUAL(0, ui.showMessageCalls);
	delete &g;
}

void FGameSaveFormatTest::testLoadTruncatedInsideFleetShipScalarRegionReturnsNonzeroAndReportsExactlyOnce(){
	// Build a real game and add a fleet containing exactly ONE uniquely-named
	// ship to UPF (the first player processed by FGame::load()). A
	// single-ship fleet is essential to isolating the FF-2 fix: the ship loop
	// is the last thing FFleet::load() does, so with only one ship there is
	// no *following* ship whose missing type tag would trip the pre-existing
	// createShip("")==NULL guard (the mechanism the sibling
	// testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce
	// relies on). The abort therefore depends solely on FVehicle::load()
	// itself returning nonzero when its own scalar read fails -- the FF-2
	// behavior. The fleet is placed in a real system so FF-1's location check
	// accepts it.
	RecordingUI buildUi;
	FGame &buildGame = FGame::create(&buildUi);
	CPPUNIT_ASSERT_EQUAL(0, buildGame.init(NULL));
	FPlayer *upf = buildGame.getPlayer(1);
	CPPUNIT_ASSERT(upf != NULL);
	FFleet *existing = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(existing != NULL);
	FSystem *sys = FMap::getMap().getSystem(existing->getLocation());
	CPPUNIT_ASSERT(sys != NULL);

	FFleet *soloFleet = new FFleet;
	soloFleet->setName("FF2 Solo Fleet Truncation Target");
	soloFleet->setOwner(upf->getID());
	soloFleet->setLocation(sys, false);
	FVehicle *fleetShip = createShip("Destroyer", "FF2 Fleet Ship Truncation Target");
	soloFleet->addShip(fleetShip);
	upf->addFleet(soloFleet);
	sys->addFleet(soloFleet);

	std::ostringstream os;
	CPPUNIT_ASSERT_EQUAL(0, buildGame.save(os));
	std::string bytes = os.str();
	delete &buildGame;

	// Locate the fleet's single ship by its own m_name field; the 4 bytes
	// immediately before that tag are the ship's m_ID field (FVehicle::save()
	// writes m_type, then m_ID, then m_name).
	std::string nameTag = lengthPrefixedTag("FF2 Fleet Ship Truncation Target");
	size_t namePos = bytes.find(nameTag);
	CPPUNIT_ASSERT(namePos != std::string::npos);
	CPPUNIT_ASSERT(namePos >= 4);
	size_t idFieldStart = namePos - 4;

	// Truncate so only 2 of m_ID's 4 bytes survive: the ship's type tag
	// (consumed by createShip() inside FFleet::load()'s ship loop) is fully
	// present, but FVehicle::load()'s own leading readU32(m_ID) call fails.
	std::string truncated = bytes.substr(0, idFieldStart + 2);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(truncated);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// UPF owns the corrupt fleet and is the first player processed, so the
	// aggregate-abort fires (via FVehicle::load() -> FFleet::load() ship loop
	// -> FPlayer::load() fleet loop -> FGame::load()) before any player is
	// pushed onto m_players.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

void FGameSaveFormatTest::testLoadTruncatedInsideUnattachedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce(){
	// Build a real game, then add one uniquely-named ship directly to UPF's
	// m_unattached list (distinct from the generically-named Destroyers
	// addUPFUnattached() already places there) so its own record can be
	// located unambiguously by name.
	RecordingUI buildUi;
	FGame &buildGame = FGame::create(&buildUi);
	CPPUNIT_ASSERT_EQUAL(0, buildGame.init(NULL));
	FPlayer *upf = buildGame.getPlayer(1);
	CPPUNIT_ASSERT(upf != NULL);
	FVehicle *unattachedShip = createShip("Destroyer", "FF2 Unattached Truncation Target");
	CPPUNIT_ASSERT_EQUAL(0, upf->addShip(unattachedShip));

	std::ostringstream os;
	CPPUNIT_ASSERT_EQUAL(0, buildGame.save(os));
	std::string bytes = os.str();
	delete &buildGame;

	// Locate the ship's own m_name field; FVehicle::save() writes m_type,
	// then m_ID (4 bytes), then m_name -- so the 4 bytes immediately before
	// this tag are that ship's m_ID field.
	std::string nameTag = lengthPrefixedTag("FF2 Unattached Truncation Target");
	size_t namePos = bytes.find(nameTag);
	CPPUNIT_ASSERT(namePos != std::string::npos);
	CPPUNIT_ASSERT(namePos >= 4);
	size_t idFieldStart = namePos - 4;

	// Truncate so only 2 of m_ID's 4 bytes survive: the type tag (consumed
	// by createShip() in FPlayer::load()'s m_unattached loop) is fully
	// present, but FVehicle::load()'s own leading readU32(m_ID) call fails.
	std::string truncated = bytes.substr(0, idFieldStart + 2);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(truncated);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// UPF's m_unattached loop runs before its fleet loop and before any
	// other player is processed, so the abort fires before any player is
	// pushed onto m_players.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

void FGameSaveFormatTest::testLoadTruncatedInsideDestroyedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce(){
	// Build a real game, then add one uniquely-named ship directly to UPF's
	// m_destroyed list via the public addDestroyedShip() API (see FPlayer.h:
	// "F2-serialization" -- m_destroyed is serialized with the same
	// type-tag + createShip() pattern as m_unattached and fleet ships).
	RecordingUI buildUi;
	FGame &buildGame = FGame::create(&buildUi);
	CPPUNIT_ASSERT_EQUAL(0, buildGame.init(NULL));
	FPlayer *upf = buildGame.getPlayer(1);
	CPPUNIT_ASSERT(upf != NULL);
	FVehicle *destroyedShip = createShip("Destroyer", "FF2 Destroyed Truncation Target");
	CPPUNIT_ASSERT_EQUAL(0, upf->addDestroyedShip(destroyedShip));

	std::ostringstream os;
	CPPUNIT_ASSERT_EQUAL(0, buildGame.save(os));
	std::string bytes = os.str();
	delete &buildGame;

	std::string nameTag = lengthPrefixedTag("FF2 Destroyed Truncation Target");
	size_t namePos = bytes.find(nameTag);
	CPPUNIT_ASSERT(namePos != std::string::npos);
	CPPUNIT_ASSERT(namePos >= 4);
	size_t idFieldStart = namePos - 4;

	// Truncate so only 2 of m_ID's 4 bytes survive, exactly as the
	// unattached-ship sibling test above.
	std::string truncated = bytes.substr(0, idFieldStart + 2);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(truncated);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// UPF's m_destroyed loop is the last thing FPlayer::load() does, but it
	// still runs before UPF itself is pushed onto m_players (see
	// FGame::load()), so the abort fires before any player is committed.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

void FGameSaveFormatTest::testLoadFleetWithOutOfRangeDestinationIdReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();

	// Locate UPF's "Task Force Prenglar" fleet by its own length-prefixed
	// name field, using the same lookup pattern as the sibling FF-1
	// location/jump-route tests above. This fleet is placed in the Prenglar
	// system, so its m_location field resolves and is left untouched.
	std::string fleetNameTag;
	fleetNameTag.push_back(static_cast<char>(19));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag += "Task Force Prenglar";
	size_t fleetNamePos = bytes.find(fleetNameTag);
	CPPUNIT_ASSERT(fleetNamePos != std::string::npos);

	// FFleet::save() field order after the name: m_owner (4), m_location
	// (4), m_inTransit (1), m_destination (4) -- the field under test here.
	size_t ownerPos = fleetNamePos + fleetNameTag.size();
	size_t locationPos = ownerPos + 4;
	size_t inTransitPos = locationPos + 4;
	size_t destinationPos = inTransitPos + 1;
	CPPUNIT_ASSERT(destinationPos + 4 <= bytes.size());

	// Sanity: a freshly init()'d, placed, non-transiting fleet's
	// m_destination is the FFleet::NO_DESTINATION sentinel (all bits set)
	// before we clobber it below.
	const unsigned char *raw = reinterpret_cast<const unsigned char *>(bytes.data());
	uint32_t originalDestination = static_cast<uint32_t>(raw[destinationPos])
			| (static_cast<uint32_t>(raw[destinationPos + 1]) << 8)
			| (static_cast<uint32_t>(raw[destinationPos + 2]) << 16)
			| (static_cast<uint32_t>(raw[destinationPos + 3]) << 24);
	CPPUNIT_ASSERT_EQUAL(FFleet::NO_DESTINATION, originalDestination);

	// Mark the fleet in-transit (matching the acceptance-criteria scenario
	// of an in-transit fleet carrying a bad destination) -- the FR-A check
	// itself does not depend on getInTransit(), but setting it here proves
	// the rejection also covers the described in-transit case, and the
	// fleet's location stays nonzero/resolvable so the FR-B check does not
	// also fire.
	bytes[inTransitPos] = static_cast<char>(1);

	// Overwrite the 4-byte m_destination field with a little-endian encoding
	// of 0x0000BEEF: neither FFleet::NO_DESTINATION (0xFFFFFFFF, the "no
	// destination set" sentinel) nor any real loaded system ID (the loaded
	// universe has only a small, low-numbered set of systems).
	bytes[destinationPos] = static_cast<char>(0xEF);
	bytes[destinationPos + 1] = static_cast<char>(0xBE);
	bytes[destinationPos + 2] = static_cast<char>(0x00);
	bytes[destinationPos + 3] = static_cast<char>(0x00);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// UPF owns this fleet and is the first player processed, so the FR-A
	// check aborts before any player is committed.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

void FGameSaveFormatTest::testLoadFleetWithInTransitAndZeroLocationReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();

	// Locate the same placed fleet as the sibling tests above; this time its
	// m_destination field is left untouched (at the FFleet::NO_DESTINATION
	// sentinel, so the FR-A check passes) and only m_location and
	// m_inTransit are corrupted to form the illegal FR-B combination.
	std::string fleetNameTag;
	fleetNameTag.push_back(static_cast<char>(19));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag.push_back(static_cast<char>(0));
	fleetNameTag += "Task Force Prenglar";
	size_t fleetNamePos = bytes.find(fleetNameTag);
	CPPUNIT_ASSERT(fleetNamePos != std::string::npos);

	size_t ownerPos = fleetNamePos + fleetNameTag.size();
	size_t locationPos = ownerPos + 4;
	size_t inTransitPos = locationPos + 4;
	CPPUNIT_ASSERT(inTransitPos + 1 <= bytes.size());

	// Sanity: this placed fleet's m_location is a real, nonzero system ID
	// and it is not in transit before we clobber both fields below.
	const unsigned char *raw = reinterpret_cast<const unsigned char *>(bytes.data());
	uint32_t originalLocation = static_cast<uint32_t>(raw[locationPos])
			| (static_cast<uint32_t>(raw[locationPos + 1]) << 8)
			| (static_cast<uint32_t>(raw[locationPos + 2]) << 16)
			| (static_cast<uint32_t>(raw[locationPos + 3]) << 24);
	CPPUNIT_ASSERT(originalLocation != 0);
	CPPUNIT_ASSERT_EQUAL(static_cast<char>(0), bytes[inTransitPos]);

	// Zero out m_location (the "not yet in a system" sentinel, which is only
	// legal for a fleet that is NOT in transit) and set m_inTransit to true,
	// forming the illegal state FR-B rejects.
	bytes[locationPos] = static_cast<char>(0);
	bytes[locationPos + 1] = static_cast<char>(0);
	bytes[locationPos + 2] = static_cast<char>(0);
	bytes[locationPos + 3] = static_cast<char>(0);
	bytes[inTransitPos] = static_cast<char>(1);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// UPF owns this fleet and is the first player processed, so the FR-B
	// check aborts before any player is committed.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

void FGameSaveFormatTest::testLoadValidInTransitFleetWithResolvableDestinationSucceeds(){
	// Build a real game and add a fleet whose in-model FFleet::setLocation(...)
	// call drives a genuine in-transit state: getInTransit()==true, a
	// nonzero origin location, and a destination that resolves to a
	// different real, loaded FSystem. This is the legitimate counterpart to
	// the two corrupt-save tests above and must load unchanged.
	RecordingUI buildUi;
	FGame &buildGame = FGame::create(&buildUi);
	CPPUNIT_ASSERT_EQUAL(0, buildGame.init(NULL));
	FPlayer *upf = buildGame.getPlayer(1);
	CPPUNIT_ASSERT(upf != NULL);
	FFleet *existing = upf->getFleet("Task Force Prenglar");
	CPPUNIT_ASSERT(existing != NULL);
	FSystem *origin = FMap::getMap().getSystem(existing->getLocation());
	CPPUNIT_ASSERT(origin != NULL);

	// Pick any other loaded system as the destination.
	FSystem *destination = NULL;
	const SystemList &systems = FMap::getMap().getSystemList();
	for (SystemList::const_iterator it = systems.begin(); it != systems.end(); it++){
		if ((*it)->getID() != origin->getID()){
			destination = *it;
			break;
		}
	}
	CPPUNIT_ASSERT(destination != NULL);

	FFleet *transiting = new FFleet;
	transiting->setName("FF2-1 In-Transit Positive Control");
	transiting->setOwner(upf->getID());
	transiting->setLocation(origin, true, 4, destination->getID(), 1, FFleet::NO_ROUTE);
	upf->addFleet(transiting);
	origin->addFleet(transiting);
	CPPUNIT_ASSERT(transiting->getInTransit());
	CPPUNIT_ASSERT(transiting->getLocation() != 0);
	CPPUNIT_ASSERT_EQUAL(destination->getID(), transiting->getDestination());

	std::ostringstream os;
	CPPUNIT_ASSERT_EQUAL(0, buildGame.save(os));
	std::string bytes = os.str();
	delete &buildGame;

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT_EQUAL(0, rc);
	CPPUNIT_ASSERT_EQUAL(0, ui.showMessageCalls);
	delete &g;
}

void FGameSaveFormatTest::testLoadPlanetWithUnknownStationTypeReturnsNonzeroAndReportsExactlyOnce(){
	std::string bytes = buildValidSaveBytes();

	// FGame::addStations() (FGame.cpp) names the Fortress placed on
	// Madderly's Star's planet Kdikit "Fortress Kdikit" -- a globally
	// unique ship name in a freshly init()'d game's save (the other three
	// Fortresses are named Redoubt/Gollwin/Pale). FVehicle::save() writes
	// m_type first (writeString), then a fixed-width 4-byte m_ID
	// (writeU32), then m_name -- so locating this station's own name tag
	// lets us walk back to the preceding type-tag field, which is exactly
	// the field FPlanet::load()'s readString(is,type) reads before calling
	// createShip(type).
	std::string nameTag = lengthPrefixedTag("Fortress Kdikit");
	size_t namePos = bytes.find(nameTag);
	CPPUNIT_ASSERT(namePos != std::string::npos);
	CPPUNIT_ASSERT(namePos >= 4);
	size_t idFieldStart = namePos - 4;

	std::string typeTag = lengthPrefixedTag("Fortress");
	CPPUNIT_ASSERT(idFieldStart >= typeTag.size());
	size_t typeTagPos = idFieldStart - typeTag.size();
	CPPUNIT_ASSERT(bytes.compare(typeTagPos, typeTag.size(), typeTag) == 0);

	// Overwrite just the 8 type-name bytes (leaving the 4-byte length
	// prefix untouched) with a same-length string createShip() does not
	// recognize, so FPlanet::load()'s `m_station = createShip(type)` comes
	// back NULL.
	const std::string bogusType = "NotAShip";
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), bogusType.size());
	bytes.replace(typeTagPos + 4, bogusType.size(), bogusType);

	RecordingUI ui;
	FGame &g = FGame::create(&ui);
	std::istringstream is(bytes);
	int rc = g.load(is);

	CPPUNIT_ASSERT(rc != 0);
	CPPUNIT_ASSERT_EQUAL(1, ui.showMessageCalls);
	// FMap::load() (nested FSystem::load()/FPlanet::load()) runs before any
	// player is loaded in FGame::load(), so the abort fires before any
	// player is pushed onto m_players.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), g.getPlayers().size());
	delete &g;
}

// NOTE (FF2-2/FR-C, tester finding): a full-game "truncated strictly inside a
// planet's station record" test is intentionally NOT included here because it
// cannot behaviorally discriminate the FPlanet::load() fix from the unfixed
// code -- both make FGame::load() return nonzero. On the unfixed code
// FPlanet::load() swallows the truncated station's nested load failure and
// returns 0, but the shortened stream is then caught by FSystem::load()'s and
// FMap::load()'s own already-checked reads upstream, so the aggregate load
// aborts nonzero regardless. The fix-discriminating truncated-station-record
// coverage therefore lives at the unit level in
// FPlanetTest::testLoadReturnsNonzeroOnTruncatedStationRecord, which calls
// FPlanet::load() in isolation and fails (return 0) on the unfixed code while
// passing (return nonzero) on the fix.

}
