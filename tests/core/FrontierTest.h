/**
 * @file FrontierTest.h
 * @brief Header file for the FrontierTest class
 *
 * Covers the minimal deterministic RNG seed seam added to Frontier.h
 * (Phase 2 reviewer follow-up F1): seedRandomExplicit(seed) must make
 * subsequent irand() output reproducible for a given seed, and differ
 * across different seeds.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 */
#ifndef FRONTIERTEST_H_
#define FRONTIERTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "Frontier.h"

namespace FrontierTests {

class FrontierTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FrontierTest );
	CPPUNIT_TEST( testSeedRandomExplicitReproducibleSameSeed );
	CPPUNIT_TEST( testSeedRandomExplicitDiffersAcrossSeeds );
	CPPUNIT_TEST_SUITE_END();

private:

public:
	void setUp();
	void tearDown();

	void testSeedRandomExplicitReproducibleSameSeed();
	void testSeedRandomExplicitDiffersAcrossSeeds();
};

}

#endif /* FRONTIERTEST_H_ */
