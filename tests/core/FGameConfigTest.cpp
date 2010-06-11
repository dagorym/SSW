/**
 * @file FGameConfigTest.cpp
 * @brief Implementation file for the FGameConfigTest class
 * @author Tom Stephens
 * @date Created: Aug 2, 2009
 */

#include "FGameConfigTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FGameConfigTest );

void FGameConfigTest::setUp(){
	m_c1 = &(FGameConfig::create());
}

void FGameConfigTest::tearDown(){
	delete m_c1;
}

void FGameConfigTest::testConstructor(){
	std::string basePath = m_c1->getBasePath();
	CPPUNIT_ASSERT (basePath == "/home/tstephen/Development/SSW/");
}

void FGameConfigTest::testSerialize(){
	m_c1 = &(FGameConfig::getGameConfig());
	std::ofstream os("test",std::ios::binary);
	m_c1->save(os);
	os.close();
	delete m_c1;
	m_c1 = &(FGameConfig::create());
	std::ifstream is("test",std::ios::binary);
	m_c1->load(is);
	is.close();
//	delete c;
}

}
