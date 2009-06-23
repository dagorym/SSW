/**
 * @file FAssaultScoutTest.h
 * @brief Header file for the FAssaultScoutTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#ifndef FAssaultScoutTest_H_
#define FAssaultScoutTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FAssaultScout.h"

namespace FrontierTests {
using namespace Frontier;

class FAssaultScoutTest : public CppUnit::TestFixture, public Frontier::FPObject{
	CPPUNIT_TEST_SUITE( FAssaultScoutTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testLoader );
	CPPUNIT_TEST( testReload );
	CPPUNIT_TEST_SUITE_END();

private:
	FAssaultScout *m_v1;
	const virtual int save(std::ostream &os) const;
	virtual int load(std::istream &is);

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testSerialize();
	void testLoader();
	void testReload();
};

}

#endif /* FAssaultScoutTest_H_ */
