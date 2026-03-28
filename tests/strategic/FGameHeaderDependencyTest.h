#ifndef FGameHeaderDependencyTest_H_
#define FGameHeaderDependencyTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FGameHeaderDependencyTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FGameHeaderDependencyTest);
	CPPUNIT_TEST(testHeaderDoesNotIncludeWxHeaders);
	CPPUNIT_TEST(testHeaderUsesForwardDeclarationsForWxTypes);
	CPPUNIT_TEST(testHeaderDoesNotDeclareDayAndTendayImages);
	CPPUNIT_TEST(testDrawTurnCounterUsesWXIconCacheForDayAndTenday);
	CPPUNIT_TEST_SUITE_END();

public:
	void testHeaderDoesNotIncludeWxHeaders();
	void testHeaderUsesForwardDeclarationsForWxTypes();
	void testHeaderDoesNotDeclareDayAndTendayImages();
	void testDrawTurnCounterUsesWXIconCacheForDayAndTenday();
};

}

#endif /* FGameHeaderDependencyTest_H_ */
