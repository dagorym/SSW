/**
 * @file WXTacticalUIAdapterTest.h
 * @brief Header file for WXTacticalUI adapter acceptance tests
 */

#ifndef WXTACTICALUIADAPTERTEST_H_
#define WXTACTICALUIADAPTERTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class WXTacticalUIAdapterTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( WXTacticalUIAdapterTest );
CPPUNIT_TEST( testHeaderDeclaresAdapterImplementingInterface );
CPPUNIT_TEST( testConstructorStoresParentAndRedrawUsesRefreshSafely );
CPPUNIT_TEST( testShowMessageMatchesRepositoryInformationalPattern );
CPPUNIT_TEST( testDamageSummaryAndICMSelectionWrapDialogsWithGuards );
CPPUNIT_TEST( testNotifyWinnerPreservesBattleScreenWinnerMessageSemantics );
CPPUNIT_TEST( testRuntimeTacticalCallersNotRewiredToAdapter );
CPPUNIT_TEST( testAdapterSourceCompilesStandalone );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);

public:
void setUp();
void tearDown();

void testHeaderDeclaresAdapterImplementingInterface();
void testConstructorStoresParentAndRedrawUsesRefreshSafely();
void testShowMessageMatchesRepositoryInformationalPattern();
void testDamageSummaryAndICMSelectionWrapDialogsWithGuards();
void testNotifyWinnerPreservesBattleScreenWinnerMessageSemantics();
void testRuntimeTacticalCallersNotRewiredToAdapter();
void testAdapterSourceCompilesStandalone();
};

} // namespace FrontierTests

#endif
