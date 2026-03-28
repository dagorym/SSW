#ifndef FGameHeaderDependencyTest_H_
#define FGameHeaderDependencyTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FGameHeaderDependencyTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FGameHeaderDependencyTest);
	CPPUNIT_TEST(testHeaderDoesNotIncludeWxHeaders);
	CPPUNIT_TEST(testHeaderUsesForwardDeclarationsForWxTypes);
	CPPUNIT_TEST(testHeaderUsesStrategicUIAndRemovesParentMember);
	CPPUNIT_TEST(testHeaderDoesNotDeclareDayAndTendayImages);
	CPPUNIT_TEST(testDrawTurnCounterUsesWXIconCacheForDayAndTenday);
	CPPUNIT_TEST(testSourceRemovesDirectStrategicDialogIncludes);
	CPPUNIT_TEST(testSourceRoutesStrategicInteractionsThroughInterface);
	CPPUNIT_TEST_SUITE_END();

public:
	void testHeaderDoesNotIncludeWxHeaders();
	void testHeaderUsesForwardDeclarationsForWxTypes();
	void testHeaderUsesStrategicUIAndRemovesParentMember();
	void testHeaderDoesNotDeclareDayAndTendayImages();
	void testDrawTurnCounterUsesWXIconCacheForDayAndTenday();
	void testSourceRemovesDirectStrategicDialogIncludes();
	void testSourceRoutesStrategicInteractionsThroughInterface();
};

}

#endif /* FGameHeaderDependencyTest_H_ */
