/**
 * @file FrontierTest.cpp
 * @brief Implementation file for the FrontierTest class
 *
 * Behavioral coverage for the minimal deterministic RNG seed seam (Phase 2
 * reviewer follow-up F1): seedRandomExplicit(seed) reseeds the shared
 * process RNG consumed by irand(), so a fixed seed reproduces the same
 * irand() sequence in-process, and different seeds produce different
 * sequences.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 */

#include "FrontierTest.h"

namespace FrontierTests {

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FrontierTest );

void FrontierTest::setUp(){
}

void FrontierTest::tearDown(){
}

/// AC: "Calling the explicit-seed function with a fixed value makes
/// subsequent irand() outputs reproducible (same seed -> same sequence
/// in-process)."
void FrontierTest::testSeedRandomExplicitReproducibleSameSeed(){
	const unsigned int seed = 1;

	seedRandomExplicit(seed);
	int firstDraws[5];
	for (int i = 0; i < 5; i++){
		firstDraws[i] = irand(100);
	}

	seedRandomExplicit(seed);
	int secondDraws[5];
	for (int i = 0; i < 5; i++){
		secondDraws[i] = irand(100);
	}

	for (int i = 0; i < 5; i++){
		CPPUNIT_ASSERT_EQUAL(firstDraws[i], secondDraws[i]);
	}
}

/// AC: "Calling the explicit-seed function with a fixed value makes
/// subsequent irand() outputs reproducible" -- the counterpart check that a
/// different seed produces a differing sequence, confirming the seed value
/// actually drives the observed irand() output rather than being ignored.
void FrontierTest::testSeedRandomExplicitDiffersAcrossSeeds(){
	seedRandomExplicit(1);
	int seedOneDraws[5];
	for (int i = 0; i < 5; i++){
		seedOneDraws[i] = irand(100);
	}

	seedRandomExplicit(2);
	int seedTwoDraws[5];
	for (int i = 0; i < 5; i++){
		seedTwoDraws[i] = irand(100);
	}

	bool differsSomewhere = false;
	for (int i = 0; i < 5; i++){
		if (seedOneDraws[i] != seedTwoDraws[i]){
			differsSomewhere = true;
			break;
		}
	}
	CPPUNIT_ASSERT( differsSomewhere );
}

}
