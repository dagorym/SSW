/**
 * @file FAssaultCarrier.h
 * @brief Header file for the FAssaultCarrierTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#ifndef FAssaultCarrierTest_H_
#define FAssaultCarrierTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FAssaultCarrier.h"

namespace FrontierTests {
using namespace Frontier;

class FAssaultCarrierTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FAssaultCarrierTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST_SUITE_END();

private:
	FVehicle * m_v1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
};

}

#endif /* FAssaultCarrierTest_H_ */
