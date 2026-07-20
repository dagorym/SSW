/**
 * @file FGameSaveFormatTest.h
 * @brief Header file for the FGameSaveFormatTest class
 *
 * P5-5 acceptance coverage for FGame's versioned save-file header, the
 * aggregated FGame::load() error propagation added around the fixed-width
 * header/@c m_universe->load()/player @c load() calls, and the fixed-width
 * little-endian encoding of FGame's own counts/IDs
 * (@c m_currentPlayer / player count). Also covers FR-1
 * (SF-nested-load-returns): a save stream truncated deep inside a fleet's
 * own ship record must still make @c FGame::load() return nonzero via
 * @c FPlayer::load()/@c FFleet::load()'s nested-return-checking fix. Also
 * covers FF-1 (SF-located-object-ids): a fleet whose location (system) ID or
 * jump-route ID does not resolve against the already-loaded FMap must abort
 * the load, while the documented sentinels (location 0, jump-route
 * FFleet::NO_ROUTE) remain accepted. Also covers FF-2/FF-3
 * (SF-vehicle-scalar-region / SF-container-propagation): a stream truncated
 * strictly inside a single vehicle's own scalar region -- deeper than the
 * type tag consumed by the container's factory call -- must still make
 * FGame::load() return nonzero, whether that vehicle sits in a fleet, in a
 * player's m_unattached list, or in a player's m_destroyed list. Also covers
 * FF2-1 (FR-A/FR-B): a fleet whose serialized destination (system) ID does
 * not resolve against the already-loaded FMap (and is not the
 * FFleet::NO_DESTINATION sentinel) must abort the load, and a fleet whose
 * serialized state is the illegal combination of getInTransit()==true with
 * location 0 must also abort the load, while a real in-transit fleet with a
 * resolvable destination continues to load cleanly.
 *
 * Also covers FF2-2 (FR-C) at the full-game aggregate level for the
 * unknown/corrupt station-type path: a save whose planet carries an
 * unknown station type tag must abort the load via FPlanet::load()'s
 * createShip()==NULL guard -- exercised through FMap::load()'s nested
 * FSystem::load()/FPlanet::load() chain, which runs before any player is
 * loaded, so no player is ever committed to the live FGame singleton
 * (previously a NULL virtual-call crash). The complementary
 * truncated-station-record FR-C scenario is covered as a fix-discriminating
 * behavioral test at the unit level in FPlanetTest, not here: at the
 * full-game level a stream truncated inside a station record already aborts
 * on the pre-FR-C code because FSystem::load()/FMap::load()'s own checked
 * reads hit the shortened stream upstream, so a full-game truncated test
 * cannot distinguish the FPlanet::load() fix from the unfixed code.
 *
 * @author Claude Sonnet 5 (medium), Claude Opus 4.8 (1M context) (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 19, 2026
 */

#ifndef FGameSaveFormatTest_H_
#define FGameSaveFormatTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FGame.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Behavioral coverage for the P5-5 versioned save-file header,
 * FGame::load() error-aggregation contract, and the FR-1
 * (SF-nested-load-returns) nested-return-checking fix in
 * FPlayer::load()/FFleet::load(). Also covers FF-1 (SF-located-object-ids):
 * out-of-range fleet location/jump-route IDs abort the load, while the
 * documented sentinels still load cleanly. Also covers FF-2/FF-3
 * (SF-vehicle-scalar-region / SF-container-propagation): a stream truncated
 * strictly inside a single vehicle's own scalar region must abort the load
 * regardless of which of the three ship containers (fleet, m_unattached,
 * m_destroyed) holds that vehicle. Also covers FF2-1 (FR-A/FR-B): an
 * out-of-range fleet destination ID and the illegal in-transit/location-0
 * state both abort the load, while a real in-transit fleet with a
 * resolvable destination still loads cleanly. Also covers FF2-2 (FR-C) at the
 * full-game level for the unknown/corrupt station-type path: a save whose
 * planet carries an unknown station type tag must abort the load via
 * FPlanet::load()'s createShip()==NULL guard (previously a NULL virtual-call
 * crash). The truncated-station-record FR-C scenario is a unit-level
 * discriminator in FPlanetTest (see the class doc above for why it is not a
 * full-game test).
 *
 * @author Claude Sonnet 5 (medium), Claude Opus 4.8 (1M context) (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 19, 2026
 */
class FGameSaveFormatTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FGameSaveFormatTest );
	CPPUNIT_TEST( testSaveEmitsMagicAndVersionHeaderBeforeGameData );
	CPPUNIT_TEST( testFullRoundTripRestoresRoundCurrentPlayerRetreatConditionLossCountersMapAndPlayers );
	CPPUNIT_TEST( testLoadWrongMagicReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadUnsupportedVersionReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadTruncatedStreamReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadUnknownFactoryTypeReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadFailureWithNoUIFallsBackToConsoleAndReturnsNonzero );
	CPPUNIT_TEST( testLoadFleetWithOutOfRangeLocationIdReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadFleetWithOutOfRangeJumpRouteIdReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadValidSaveWithSentinelLocationAndJumpRouteSucceeds );
	CPPUNIT_TEST( testLoadTruncatedInsideFleetShipScalarRegionReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadTruncatedInsideUnattachedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadTruncatedInsideDestroyedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadFleetWithOutOfRangeDestinationIdReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadFleetWithInTransitAndZeroLocationReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadValidInTransitFleetWithResolvableDestinationSucceeds );
	CPPUNIT_TEST( testLoadPlanetWithUnknownStationTypeReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadTruncatedInsidePlayerOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadTruncatedInsideFleetOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	/// The magic+version header is the first 8 bytes save() emits, decoded
	/// in explicit little-endian order and compared against kSaveMagic/
	/// kSaveFormatVersion.
	void testSaveEmitsMagicAndVersionHeaderBeforeGameData();

	/// A full init()->mutate->save()->load() round trip into a fresh FGame
	/// singleton restores round, current player, and player/fleet/ship
	/// identity, and a byte-for-byte re-save of the loaded game matches the
	/// original bytes exactly -- proving retreat condition, loss counters,
	/// and map data (fields with no public getters) round-trip faithfully.
	void testFullRoundTripRestoresRoundCurrentPlayerRetreatConditionLossCountersMapAndPlayers();

	/// A save file whose first 4 bytes do not equal kSaveMagic must be
	/// rejected: load() returns nonzero and reports exactly once via the
	/// installed IStrategicUI.
	void testLoadWrongMagicReturnsNonzeroAndReportsExactlyOnce();

	/// A save file with a correct magic tag but an unrecognized format
	/// version must be rejected: load() returns nonzero and reports exactly
	/// once via the installed IStrategicUI.
	void testLoadUnsupportedVersionReturnsNonzeroAndReportsExactlyOnce();

	/// A save file truncated partway through must be rejected: load()
	/// returns nonzero and reports exactly once via the installed
	/// IStrategicUI.
	void testLoadTruncatedStreamReturnsNonzeroAndReportsExactlyOnce();

	/// A save file whose ship-type tag names an unrecognized factory type
	/// must be rejected: load() returns nonzero and reports exactly once
	/// via the installed IStrategicUI, exercising the aggregated per-player
	/// load() failure path in FGame::load().
	void testLoadUnknownFactoryTypeReturnsNonzeroAndReportsExactlyOnce();

	/// FR-1 (SF-nested-load-returns): a save file truncated immediately
	/// before the first byte of a fleet's first ship's own record (i.e.
	/// after the fleet's own header fields and ship-count are fully present,
	/// but before any byte of that ship's type-tag string) must still be
	/// rejected: load() returns nonzero and reports exactly once via the
	/// installed IStrategicUI. This exercises FPlayer::load()'s fleet loop
	/// propagating FFleet::load()'s nonzero return (which itself aborts via
	/// its pre-existing createShip()==NULL check once the truncated
	/// type-tag read fails) instead of silently discarding it as before this
	/// fix.
	void testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce();

	/// With no IStrategicUI installed (NULL), a load() failure must still
	/// return nonzero and must not crash, exercising reportLoadError()'s
	/// console fallback branch.
	void testLoadFailureWithNoUIFallsBackToConsoleAndReturnsNonzero();

	/// FF-1 (SF-located-object-ids): a fleet whose serialized location
	/// (system) ID is nonzero but does not resolve against the
	/// already-loaded FMap must abort the load: load() returns nonzero,
	/// reports exactly once via the installed IStrategicUI, and no player is
	/// committed to the live FGame singleton.
	void testLoadFleetWithOutOfRangeLocationIdReturnsNonzeroAndReportsExactlyOnce();

	/// FF-1 (SF-located-object-ids): a fleet whose serialized jump-route ID
	/// is not the FFleet::NO_ROUTE sentinel but does not resolve against the
	/// already-loaded FMap must abort the load: load() returns nonzero,
	/// reports exactly once via the installed IStrategicUI, and no player is
	/// committed to the live FGame singleton.
	void testLoadFleetWithOutOfRangeJumpRouteIdReturnsNonzeroAndReportsExactlyOnce();

	/// Positive control for FF-1: a freshly init()'d game's save contains
	/// only the documented sentinels (location 0 for not-yet-placed fleets,
	/// FFleet::NO_ROUTE for fleets not on a jump route) and every placed
	/// fleet's location resolves against the loaded FMap, so a plain
	/// buildValidSaveBytes() -> load() round trip must succeed with no error
	/// reported.
	void testLoadValidSaveWithSentinelLocationAndJumpRouteSucceeds();

	/// FF-2/FF-3 (SF-vehicle-scalar-region / SF-container-propagation, site 1
	/// of 3: fleet). A save file truncated strictly inside a single-ship
	/// fleet's ship record -- after that ship's type tag is fully consumed by
	/// createShip() but partway through FVehicle::load()'s own leading m_ID
	/// read -- must be rejected: load() returns nonzero, reports exactly once
	/// via the installed IStrategicUI, and no player is committed to the live
	/// FGame singleton. A *single*-ship fleet is used deliberately: because
	/// the ship loop is the last thing FFleet::load() does, a one-ship fleet
	/// has no following ship whose missing type tag would trip the
	/// pre-existing createShip("")==NULL guard, so the abort depends solely on
	/// the FF-2 fix inside FVehicle::load() propagating through FFleet::load()'s
	/// ship loop and FPlayer::load()'s fleet loop. This is distinct from
	/// testLoadTruncatedInsideFleetShipRecordReturnsNonzeroAndReportsExactlyOnce
	/// above, which truncates before any byte of a multi-ship fleet's ship
	/// record and only exercises the pre-existing createShip()==NULL guard.
	void testLoadTruncatedInsideFleetShipScalarRegionReturnsNonzeroAndReportsExactlyOnce();

	/// FF-2/FF-3 (site 2 of 3: FPlayer::m_unattached). A save file truncated
	/// strictly inside an unattached ship's own scalar region must be
	/// rejected: load() returns nonzero, reports exactly once via the
	/// installed IStrategicUI, and no player is committed to the live FGame
	/// singleton. Exercises FPlayer::load()'s m_unattached loop propagating
	/// FVehicle::load()'s nonzero return.
	void testLoadTruncatedInsideUnattachedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce();

	/// FF-2/FF-3 (site 3 of 3: FPlayer::m_destroyed). A save file truncated
	/// strictly inside a destroyed ship's own scalar region must be
	/// rejected: load() returns nonzero, reports exactly once via the
	/// installed IStrategicUI, and no player is committed to the live FGame
	/// singleton. Exercises FPlayer::load()'s m_destroyed loop propagating
	/// FVehicle::load()'s nonzero return.
	void testLoadTruncatedInsideDestroyedShipScalarRegionReturnsNonzeroAndReportsExactlyOnce();

	/// FF2-1 (FR-A): a fleet whose serialized destination (system) ID is not
	/// the FFleet::NO_DESTINATION sentinel but does not resolve against the
	/// already-loaded FMap must abort the load: load() returns nonzero,
	/// reports exactly once via the installed IStrategicUI, and no player is
	/// committed to the live FGame singleton. The fleet's m_inTransit byte is
	/// also set so the corrupted fleet matches the acceptance-criteria
	/// scenario of an in-transit fleet with a bad destination, while its
	/// location field is left resolvable so only the FR-A check fires.
	void testLoadFleetWithOutOfRangeDestinationIdReturnsNonzeroAndReportsExactlyOnce();

	/// FF2-1 (FR-B): a fleet whose serialized state combines
	/// getInTransit()==true with location 0 is an illegal state (an
	/// in-transit fleet must have a real origin system) and must abort the
	/// load: load() returns nonzero, reports exactly once via the installed
	/// IStrategicUI, and no player is committed to the live FGame singleton.
	/// The fleet's destination field is left at the FFleet::NO_DESTINATION
	/// sentinel so only the FR-B check fires.
	void testLoadFleetWithInTransitAndZeroLocationReturnsNonzeroAndReportsExactlyOnce();

	/// Positive control for FF2-1: a fleet built via the normal in-model
	/// FFleet::setLocation(...) API with getInTransit()==true, a nonzero
	/// origin location, and a destination that resolves to a real,
	/// different loaded FSystem must still load cleanly -- load() returns 0
	/// and no error is reported -- proving the FR-A/FR-B checks do not
	/// falsely reject a legitimate in-transit fleet.
	void testLoadValidInTransitFleetWithResolvableDestinationSucceeds();

	/// FF2-2 (FR-C): a save whose planet's station carries an
	/// unknown/corrupt station type tag must abort the load: load() returns
	/// nonzero, reports exactly once via the installed IStrategicUI, and no
	/// player is committed to the live FGame singleton. Exercises
	/// FMap::load()'s nested FSystem::load()/FPlanet::load() chain (which
	/// runs before any player is loaded) propagating FPlanet::load()'s
	/// createShip()==NULL abort through FSystem::load()'s pre-existing
	/// `if (p->load(is) != 0)` check. Previously this scenario triggered a
	/// NULL virtual-call crash instead of a clean abort.
	void testLoadPlanetWithUnknownStationTypeReturnsNonzeroAndReportsExactlyOnce();

	/// FF2-3 (FR-D): a save truncated strictly inside UPF's (the first
	/// player processed) own scalar region -- after its m_ID is fully
	/// present, but partway through its m_name's own length-prefixed
	/// record -- must make FGame::load() return nonzero and report exactly
	/// once via the mock IStrategicUI, with no player ever committed to
	/// m_players. Before FF2-3, every one of FPlayer::load()'s own scalar
	/// reads below m_ID had its return value discarded, so this truncation
	/// point returned 0: two "successful" phantom players (both with
	/// default/empty state) were silently pushed onto the live singleton.
	void testLoadTruncatedInsidePlayerOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce();

	/// FF2-3 (FR-D): a save truncated strictly inside UPF's first fleet's
	/// ("Task Force Prenglar") own scalar region -- after the fleet's m_ID
	/// is fully present, but partway through its m_owner field -- must make
	/// FGame::load() return nonzero and report exactly once, with no player
	/// ever committed to m_players. Before FF2-3, every one of
	/// FFleet::load()'s own scalar reads below m_ID had its return value
	/// discarded, so this truncation point returned 0 for every remaining
	/// fleet in UPF's list (each with default/empty state), and the
	/// resulting phantom player was silently committed.
	void testLoadTruncatedInsideFleetOwnScalarRegionReturnsNonzeroAndReportsExactlyOnce();
};

}

#endif /* FGameSaveFormatTest_H_ */
