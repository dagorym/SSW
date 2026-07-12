/**
 * @file FGameTest.h
 * @brief Header file for the FGameTest class
 * @author Tom Stephens, Claude Sonnet 5 (medium)
 * @date Created: Jun 19, 2009
 * @date Last Modified: Jul 11, 2026
 */

#ifndef FGameTest_H_
#define FGameTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FGame.h"

namespace FrontierTests {
using namespace Frontier;

class FGameTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FGameTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST( testShowPlayers );
	CPPUNIT_TEST( testEndSatharTurn );
	CPPUNIT_TEST( testPlaceNova );
	CPPUNIT_TEST( testEndUPFTurn );
	CPPUNIT_TEST( testShowRetreatCondition );
	CPPUNIT_TEST( testGetPlayer );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testCleanUpShipsRemovesDestroyedShipAndReloadsSurvivor );
	CPPUNIT_TEST( testCleanUpShipsRemovesEmptiedFleetFromPlayerAndSystem );
	CPPUNIT_TEST( testCleanUpShipsUpdatesLossCountersWithoutInflation );
	CPPUNIT_TEST( testCheckForVictoryCondition4WinsWithStationsBelowThresholdAndResetsCounter );
	CPPUNIT_TEST( testCheckForVictoryCondition4NoWinAtThresholdButCounterStillResets );
	CPPUNIT_TEST( testCheckForVictoryCondition4RegressionDoesNotAwardVictoryWhenStationsAtFiveDespiteShipLossImbalance );
	CPPUNIT_TEST( testCheckForVictoryCondition5WinsWhenSatharLossesExceedUPFAndResetsCounters );
	CPPUNIT_TEST( testCheckForVictoryCondition5NoWinWhenSatharLossesDoNotExceedUPFButCountersStillReset );
	CPPUNIT_TEST( testCheckForVictoryCondition5RegressionAwardsVictoryWhenStationsAtFiveDespiteBeingAboveThreshold );
	CPPUNIT_TEST( testShowRetreatConditionsTextMatchesEvaluatedCaseNumber );
	CPPUNIT_TEST( testCleanUpShipsCountsSatharFightersTowardLostSatharShips );
	CPPUNIT_TEST( testCleanUpShipsExcludesSatharFightersFromTendaySatharCounter );
	CPPUNIT_TEST( testCleanUpShipsExcludesUPFFightersFromTendayUPFCounter );
	CPPUNIT_TEST( testCleanUpShipsExcludesSatharMilitiaShipsFromTendaySatharCounter );
	CPPUNIT_TEST( testCleanUpShipsExcludesUPFMilitiaShipsFromTendayUPFCounter );
	CPPUNIT_TEST( testCleanUpShipsCountsNonFighterNonMilitiaSatharShipsTowardTendaySatharCounter );
	CPPUNIT_TEST( testCleanUpShipsCountsNonFighterNonMilitiaUPFShipsTowardTendayUPFCounter );
	CPPUNIT_TEST( testCheckForVictoryCondition5FilteredSatharLossesExceedUPFDespiteFighterAndMilitiaNoise );
	CPPUNIT_TEST( testCheckForVictoryCondition5NoVictoryWhenFilteredSatharLossesDoNotExceedUPFDespiteFighterAndMilitiaNoise );
	CPPUNIT_TEST_SUITE_END();

private:
	FGame *m_g1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testInit();
	void testShowPlayers();
	void testPlaceNova();
	void testEndSatharTurn();
	void testEndUPFTurn();
	void testShowRetreatCondition();
	void testGetPlayer();
	void testSerialize();
	void testCleanUpShipsRemovesDestroyedShipAndReloadsSurvivor();
	void testCleanUpShipsRemovesEmptiedFleetFromPlayerAndSystem();
	void testCleanUpShipsUpdatesLossCountersWithoutInflation();

	/// C2 regression: case 4 (STATIONS) declares UPF victory at a tenday
	/// boundary when fewer than two stations were destroyed, and resets
	/// m_stationsDestroyed at the boundary.
	void testCheckForVictoryCondition4WinsWithStationsBelowThresholdAndResetsCounter();
	/// C2 regression: case 4 does not declare UPF victory when the station
	/// threshold is met/exceeded, but the counter still resets at the boundary.
	void testCheckForVictoryCondition4NoWinAtThresholdButCounterStillResets();
	/// C2 regression: with case 4 selected, 5 stations destroyed, and Sathar
	/// ship losses (5) exceeding UPF losses (0), the fixed code must NOT award
	/// UPF victory even though the pre-fix (swapped) code would have, since it
	/// evaluated the ship-loss condition for case 4 instead of the station
	/// condition.
	void testCheckForVictoryCondition4RegressionDoesNotAwardVictoryWhenStationsAtFiveDespiteShipLossImbalance();
	/// C2 regression: case 5 (SHIP-LOSS) declares UPF victory at a tenday
	/// boundary when Sathar tenday ship losses exceed UPF tenday ship losses,
	/// and resets both tenday loss counters at the boundary.
	void testCheckForVictoryCondition5WinsWhenSatharLossesExceedUPFAndResetsCounters();
	/// C2 regression: case 5 does not declare UPF victory when Sathar tenday
	/// losses do not exceed UPF's, but the counters still reset at the boundary.
	void testCheckForVictoryCondition5NoWinWhenSatharLossesDoNotExceedUPFButCountersStillReset();
	/// C2 regression: symmetric case for case 5 -- with 5 stations destroyed
	/// (at/above the case-4 threshold) but Sathar ship losses (5) exceeding
	/// UPF losses (0), the fixed code correctly awards UPF victory, whereas
	/// the pre-fix (swapped) code -- which evaluated the station condition for
	/// case 5 -- would have missed the win.
	void testCheckForVictoryCondition5RegressionAwardsVictoryWhenStationsAtFiveDespiteBeingAboveThreshold();
	/// C2 regression: showRetreatConditions() displayed text matches the
	/// counters actually evaluated for the corresponding case number (station
	/// count text for case 4, ship-loss text for case 5).
	void testShowRetreatConditionsTextMatchesEvaluatedCaseNumber();

	/// P2-3 (S2): destroyed Sathar Fighters count toward m_lostSatharShips
	/// (retreat condition 3, "40 ships, including fighters").
	void testCleanUpShipsCountsSatharFightersTowardLostSatharShips();
	/// P2-3 (S3): destroyed Sathar Fighters are excluded from
	/// m_lostTendaySathar (retreat condition 5 excludes fighters and militia).
	void testCleanUpShipsExcludesSatharFightersFromTendaySatharCounter();
	/// P2-3 (S3): destroyed UPF Fighters are excluded from m_lostTendayUPF
	/// (fighter exclusion applies to both sides).
	void testCleanUpShipsExcludesUPFFightersFromTendayUPFCounter();
	/// P2-3 (S3): destroyed non-fighter ships from a militia Sathar fleet are
	/// excluded from m_lostTendaySathar.
	void testCleanUpShipsExcludesSatharMilitiaShipsFromTendaySatharCounter();
	/// P2-3 (S3): destroyed non-fighter ships from a militia UPF fleet are
	/// excluded from m_lostTendayUPF (militia exclusion applies to both sides).
	void testCleanUpShipsExcludesUPFMilitiaShipsFromTendayUPFCounter();
	/// P2-3 (S3): destroyed non-fighter, non-militia Sathar ships ARE counted
	/// toward m_lostTendaySathar.
	void testCleanUpShipsCountsNonFighterNonMilitiaSatharShipsTowardTendaySatharCounter();
	/// P2-3 (S3): destroyed non-fighter, non-militia UPF ships ARE counted
	/// toward m_lostTendayUPF.
	void testCleanUpShipsCountsNonFighterNonMilitiaUPFShipsTowardTendayUPFCounter();
	/// P2-3 (S3): checkForVictory() condition-5 boundary declares a UPF
	/// victory when, after filtering out fighters and militia ships, Sathar
	/// tenday losses exceed UPF tenday losses, despite fighter/militia noise.
	void testCheckForVictoryCondition5FilteredSatharLossesExceedUPFDespiteFighterAndMilitiaNoise();
	/// P2-3 (S3): checkForVictory() condition-5 boundary does NOT declare a
	/// UPF victory when the filtered Sathar/UPF tenday loss counts are equal,
	/// even though the raw (unfiltered) Sathar total would exceed the UPF's --
	/// proving the filtering, not the raw counts, feeds the victory check.
	void testCheckForVictoryCondition5NoVictoryWhenFilteredSatharLossesDoNotExceedUPFDespiteFighterAndMilitiaNoise();
};

}

#endif /* FGameTest_H_ */
