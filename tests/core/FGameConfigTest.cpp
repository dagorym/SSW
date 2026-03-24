/**
 * @file FGameConfigTest.cpp
 * @brief Implementation file for the FGameConfigTest class
 * @author Tom Stephens
 * @date Created: Aug 2, 2009
 */

#include "FGameConfigTest.h"
#include <cstdio>
#include <fstream>

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FGameConfigTest );

void FGameConfigTest::setUp(){
	FGameConfig::reset();
	m_c1 = &(FGameConfig::create());
}

void FGameConfigTest::tearDown(){
	FGameConfig::reset();
	m_c1 = NULL;
}

void FGameConfigTest::testConstructor(){
	std::string basePath = m_c1->getBasePath();
	// Verify the base path is non-empty and has a trailing separator.
	// The exact path is runtime-derived from the executable location and
	// is not predictable across machines or worktrees.
	CPPUNIT_ASSERT(!basePath.empty());
	CPPUNIT_ASSERT(basePath[basePath.size() - 1] == '/');
}

void FGameConfigTest::testSerialize(){
	m_c1 = &(FGameConfig::getGameConfig());
	const char *filename = "FGameConfigTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m_c1->save(os);
	os.close();
	FGameConfig::reset();
	m_c1 = &(FGameConfig::create());
	std::ifstream is(filename,std::ios::binary);
	m_c1->load(is);
	is.close();
	std::remove(filename);
}

void FGameConfigTest::testGetGameConfigLazyCreatesSingleton(){
	// AC1: getGameConfig() must lazily initialize singleton when unset.
	FGameConfig::reset();

	FGameConfig &config = FGameConfig::getGameConfig();
	CPPUNIT_ASSERT(!config.getBasePath().empty());
}

void FGameConfigTest::testResetRecreatesSingletonAndMaintainsContract(){
	// AC2/AC3: lifecycle consistency via class contract and public reset().
	// create() and getGameConfig() return the same instance before reset.
	FGameConfig::reset();

	FGameConfig &firstCreate = FGameConfig::create();
	FGameConfig &firstGet = FGameConfig::getGameConfig();
	CPPUNIT_ASSERT(&firstCreate == &firstGet);

	// After reset(), getGameConfig() returns a functional singleton.
	// Note: pointer address comparison after delete is unreliable (allocator
	// may reuse memory), so we verify behavioral correctness instead.
	FGameConfig::reset();
	FGameConfig &secondGet = FGameConfig::getGameConfig();
	CPPUNIT_ASSERT(!secondGet.getBasePath().empty());

	// The new singleton is self-consistent: create() and getGameConfig()
	// still return the same instance after a reset/recreate cycle.
	FGameConfig &secondCreate = FGameConfig::create();
	CPPUNIT_ASSERT(&secondGet == &secondCreate);
}

void FGameConfigTest::testBasePathConsistentForProductionCallers(){
	// AC4: production callers still observe consistent base-path behavior.
	FGameConfig::reset();
	FGameConfig &created = FGameConfig::create();
	std::string createPath = created.getBasePath();

	FGameConfig &fetched = FGameConfig::getGameConfig();
	std::string getPath = fetched.getBasePath();

	CPPUNIT_ASSERT(createPath == getPath);
}

void FGameConfigTest::testCreateAndGetReturnExistingSingletonAcrossRepeatedCalls(){
	// Regression: repeated create()/getGameConfig() calls must not replace singleton.
	FGameConfig::reset();

	FGameConfig &initialCreate = FGameConfig::create();
	FGameConfig &secondCreate = FGameConfig::create();
	FGameConfig &firstGet = FGameConfig::getGameConfig();
	FGameConfig &secondGet = FGameConfig::getGameConfig();

	CPPUNIT_ASSERT(&initialCreate == &secondCreate);
	CPPUNIT_ASSERT(&initialCreate == &firstGet);
	CPPUNIT_ASSERT(&firstGet == &secondGet);
}

void FGameConfigTest::testMultipleResetAndRecreateCyclesRemainFunctional(){
	// Regression: repeated reset -> getGameConfig() cycles remain deterministic.
	FGameConfig::reset();

	std::string expectedPath;
	const int cycleCount = 5;
	for (int i = 0; i < cycleCount; i++){
		FGameConfig &config = FGameConfig::getGameConfig();
		const std::string currentPath = config.getBasePath();

		CPPUNIT_ASSERT(!currentPath.empty());
		if (i == 0){
			expectedPath = currentPath;
		}else{
			CPPUNIT_ASSERT(expectedPath == currentPath);
		}

		// Ensure create() in same cycle returns exact existing singleton.
		FGameConfig &created = FGameConfig::create();
		CPPUNIT_ASSERT(&config == &created);

		FGameConfig::reset();
	}
}

}
