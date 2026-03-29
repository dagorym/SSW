#ifndef FGameHeaderDependencyTest_H_
#define FGameHeaderDependencyTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FGameHeaderDependencyTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FGameHeaderDependencyTest);
	CPPUNIT_TEST(testHeaderDoesNotIncludeWxHeaders);
	CPPUNIT_TEST(testHeaderDoesNotExposeWxRenderOrMouseTypes);
	CPPUNIT_TEST(testHeaderUsesStrategicUIAndRemovesParentMember);
	CPPUNIT_TEST(testHeaderDoesNotDeclareStrategicDrawOrMouseHandlers);
	CPPUNIT_TEST(testSourceDoesNotUseDisplayPaintingHelpers);
	CPPUNIT_TEST(testSourceDoesNotImplementStrategicDrawOrMouseHandlers);
	CPPUNIT_TEST(testWXGameDisplayOwnsStrategicDrawEntryPoint);
	CPPUNIT_TEST(testSourceRemovesDirectStrategicDialogIncludes);
	CPPUNIT_TEST(testSourceRoutesStrategicInteractionsThroughInterface);
	CPPUNIT_TEST_SUITE_END();

public:
	void testHeaderDoesNotIncludeWxHeaders();
	void testHeaderDoesNotExposeWxRenderOrMouseTypes();
	void testHeaderUsesStrategicUIAndRemovesParentMember();
	void testHeaderDoesNotDeclareStrategicDrawOrMouseHandlers();
	void testSourceDoesNotUseDisplayPaintingHelpers();
	void testSourceDoesNotImplementStrategicDrawOrMouseHandlers();
	void testWXGameDisplayOwnsStrategicDrawEntryPoint();
	void testSourceRemovesDirectStrategicDialogIncludes();
	void testSourceRoutesStrategicInteractionsThroughInterface();
};

}

#endif /* FGameHeaderDependencyTest_H_ */
