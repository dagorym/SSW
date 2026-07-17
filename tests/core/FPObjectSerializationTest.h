/**
 * @file FPObjectSerializationTest.h
 * @brief Header file for the FPObjectSerializationTest class
 *
 * Behavioral coverage for the P5-1 core wire-format serialization
 * primitives: @c FPObject::writeU32/readU32, the hardened
 * @c writeString/readString pair, the generic @c write<T>/read<T>
 * templates, and the named save-format constants in
 * @c include/core/FSaveFormat.h. Because these helpers are protected
 * members of the abstract @c FPObject class, a small test-local concrete
 * subclass (@c FPObjectSerializationHarness) forwards them as public
 * methods so the fixture can exercise them directly against
 * @c std::stringstream buffers.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 */

#ifndef FPObjectSerializationTest_H_
#define FPObjectSerializationTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "core/FPObject.h"
#include "core/FSaveFormat.h"

#include <cstdint>
#include <string>

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief test-local concrete FPObject exposing protected serialization
 *        helpers as public forwarders
 *
 * @c save()/load() are trivial no-op implementations that only exist to
 * satisfy the abstract base so this harness can be instantiated; the
 * fixture never calls them. The remaining methods are one-line forwarders
 * to the protected members under test.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 */
class FPObjectSerializationHarness : public FPObject {
public:
	const virtual int save(std::ostream &os) const { (void)os; return 0; }
	virtual int load(std::istream &is) { (void)is; return 0; }

	int callWriteU32(std::ostream &os, uint32_t v) const { return writeU32(os, v); }
	int callReadU32(std::istream &is, uint32_t &v) { return readU32(is, v); }
	int callWriteString(std::ostream &os, const std::string &s) const { return writeString(os, s); }
	int callReadString(std::istream &is, std::string &s) { return readString(is, s); }
	template<typename T> int callWrite(std::ostream &os, const T &d) const { return write(os, d); }
	template<typename T> int callRead(std::istream &is, T &d) { return read(is, d); }
};

class FPObjectSerializationTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FPObjectSerializationTest );
	CPPUNIT_TEST( testWriteU32ReadU32RoundTrip );
	CPPUNIT_TEST( testWriteU32LittleEndianByteLayout );
	CPPUNIT_TEST( testWriteU32MagicByteLayout );
	CPPUNIT_TEST( testWriteStringReadStringRoundTripNormal );
	CPPUNIT_TEST( testWriteStringReadStringRoundTripEmbeddedNul );
	CPPUNIT_TEST( testWriteStringReadStringRoundTripEmpty );
	CPPUNIT_TEST( testReadStringRejectsOversizeLengthNoPayloadRead );
	CPPUNIT_TEST( testReadStringBoundaryExactCapAllowed );
	CPPUNIT_TEST( testReadStringBoundaryOneOverCapRejected );
	CPPUNIT_TEST( testWriteStringOversizeGuardRejectsWrite );
	CPPUNIT_TEST( testWriteReadTemplateRoundTrip );
	CPPUNIT_TEST( testFailedStreamReturnsNonzeroForAllHelpers );
	CPPUNIT_TEST( testConstantsHaveExpectedValues );
	CPPUNIT_TEST_SUITE_END();

private:
	FPObjectSerializationHarness *m_h;

public:
	void setUp();
	void tearDown();

	void testWriteU32ReadU32RoundTrip();
	void testWriteU32LittleEndianByteLayout();
	void testWriteU32MagicByteLayout();
	void testWriteStringReadStringRoundTripNormal();
	void testWriteStringReadStringRoundTripEmbeddedNul();
	void testWriteStringReadStringRoundTripEmpty();
	void testReadStringRejectsOversizeLengthNoPayloadRead();
	void testReadStringBoundaryExactCapAllowed();
	void testReadStringBoundaryOneOverCapRejected();
	void testWriteStringOversizeGuardRejectsWrite();
	void testWriteReadTemplateRoundTrip();
	void testFailedStreamReturnsNonzeroForAllHelpers();
	void testConstantsHaveExpectedValues();
};

}

#endif /* FPObjectSerializationTest_H_ */
