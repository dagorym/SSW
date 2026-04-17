/**
 * @file FGameConfigTest.h
 * @brief Header file for the FGameConfigTest class
 * @author Tom Stephens
 * @date Created: Aug 2, 2009
 */

#ifndef FGameConfigTest_H_
#define FGameConfigTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "core/FGameConfig.h"

namespace FrontierTests {
using namespace Frontier;

class FGameConfigTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FGameConfigTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testGetGameConfigLazyCreatesSingleton );
	CPPUNIT_TEST( testResetRecreatesSingletonAndMaintainsContract );
	CPPUNIT_TEST( testBasePathConsistentForProductionCallers );
	CPPUNIT_TEST( testCreateAndGetReturnExistingSingletonAcrossRepeatedCalls );
	CPPUNIT_TEST( testMultipleResetAndRecreateCyclesRemainFunctional );
	CPPUNIT_TEST( testResolveAssetPathReturnsUnchangedForEmptyAndAbsolutePaths );
	CPPUNIT_TEST( testResolveAssetPathFindsRepoAssetWhenRunningFromTests );
	CPPUNIT_TEST( testResolveAssetPathNormalizesLeadingDotSlashAndSeparators );
	CPPUNIT_TEST_SUITE_END();

private:
	FGameConfig *m_c1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testSerialize();
	void testGetGameConfigLazyCreatesSingleton();
	void testResetRecreatesSingletonAndMaintainsContract();
	void testBasePathConsistentForProductionCallers();
	void testCreateAndGetReturnExistingSingletonAcrossRepeatedCalls();
	void testMultipleResetAndRecreateCyclesRemainFunctional();
	void testResolveAssetPathReturnsUnchangedForEmptyAndAbsolutePaths();
	void testResolveAssetPathFindsRepoAssetWhenRunningFromTests();
	void testResolveAssetPathNormalizesLeadingDotSlashAndSeparators();
};

}

#endif /* FGameConfigTest_H_ */
