/**
 * @file FGameSaveFormatTest.h
 * @brief Header file for the FGameSaveFormatTest class
 *
 * P5-5 acceptance coverage for FGame's versioned save-file header, the
 * aggregated FGame::load() error propagation added around the fixed-width
 * header/@c m_universe->load()/player @c load() calls, and the fixed-width
 * little-endian encoding of FGame's own counts/IDs
 * (@c m_currentPlayer / player count).
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 17, 2026
 */

#ifndef FGameSaveFormatTest_H_
#define FGameSaveFormatTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FGame.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Behavioral coverage for the P5-5 versioned save-file header and
 * FGame::load() error-aggregation contract.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 * @date Last Modified: Jul 17, 2026
 */
class FGameSaveFormatTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FGameSaveFormatTest );
	CPPUNIT_TEST( testSaveEmitsMagicAndVersionHeaderBeforeGameData );
	CPPUNIT_TEST( testFullRoundTripRestoresRoundCurrentPlayerRetreatConditionLossCountersMapAndPlayers );
	CPPUNIT_TEST( testLoadWrongMagicReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadUnsupportedVersionReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadTruncatedStreamReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadUnknownFactoryTypeReturnsNonzeroAndReportsExactlyOnce );
	CPPUNIT_TEST( testLoadFailureWithNoUIFallsBackToConsoleAndReturnsNonzero );
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

	/// With no IStrategicUI installed (NULL), a load() failure must still
	/// return nonzero and must not crash, exercising reportLoadError()'s
	/// console fallback branch.
	void testLoadFailureWithNoUIFallsBackToConsoleAndReturnsNonzero();
};

}

#endif /* FGameSaveFormatTest_H_ */
