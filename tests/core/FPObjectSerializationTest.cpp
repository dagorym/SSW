/**
 * @file FPObjectSerializationTest.cpp
 * @brief Implementation file for the FPObjectSerializationTest class
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 17, 2026
 */

#include "FPObjectSerializationTest.h"

#include <sstream>

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FPObjectSerializationTest );

void FPObjectSerializationTest::setUp(){
	m_h = new FPObjectSerializationHarness;
}

void FPObjectSerializationTest::tearDown(){
	delete m_h;
	m_h = NULL;
}

void FPObjectSerializationTest::testWriteU32ReadU32RoundTrip(){
	uint32_t values[] = { 0u, 1u, 0xFFu, 0x100u, 0x12345678u, 0xFFFFFFFFu };
	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i){
		std::stringstream ss;
		int wrc = m_h->callWriteU32(ss, values[i]);
		CPPUNIT_ASSERT_EQUAL( 0, wrc );

		uint32_t readBack = 0xDEADBEEFu;
		int rrc = m_h->callReadU32(ss, readBack);
		CPPUNIT_ASSERT_EQUAL( 0, rrc );
		CPPUNIT_ASSERT_EQUAL( values[i], readBack );
	}
}

void FPObjectSerializationTest::testWriteU32LittleEndianByteLayout(){
	std::stringstream ss;
	int rc = m_h->callWriteU32(ss, 0x12345678u);
	CPPUNIT_ASSERT_EQUAL( 0, rc );

	std::string raw = ss.str();
	CPPUNIT_ASSERT_EQUAL( (size_t)4, raw.size() );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)0x78, (unsigned char)raw[0] );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)0x56, (unsigned char)raw[1] );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)0x34, (unsigned char)raw[2] );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)0x12, (unsigned char)raw[3] );
}

void FPObjectSerializationTest::testWriteU32MagicByteLayout(){
	// kSaveMagic is chosen so its little-endian byte order spells the ASCII
	// bytes 'S','S','W','1'; verify that observed byte layout directly
	// rather than trusting the header comment.
	std::stringstream ss;
	int rc = m_h->callWriteU32(ss, kSaveMagic);
	CPPUNIT_ASSERT_EQUAL( 0, rc );

	std::string raw = ss.str();
	CPPUNIT_ASSERT_EQUAL( (size_t)4, raw.size() );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)'S', (unsigned char)raw[0] );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)'S', (unsigned char)raw[1] );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)'W', (unsigned char)raw[2] );
	CPPUNIT_ASSERT_EQUAL( (unsigned char)'1', (unsigned char)raw[3] );
}

void FPObjectSerializationTest::testWriteStringReadStringRoundTripNormal(){
	std::stringstream ss;
	std::string original = "hello world";
	int wrc = m_h->callWriteString(ss, original);
	CPPUNIT_ASSERT_EQUAL( 0, wrc );

	std::string readBack;
	int rrc = m_h->callReadString(ss, readBack);
	CPPUNIT_ASSERT_EQUAL( 0, rrc );
	CPPUNIT_ASSERT_EQUAL( original, readBack );
	CPPUNIT_ASSERT_EQUAL( original.size(), readBack.size() );
}

void FPObjectSerializationTest::testWriteStringReadStringRoundTripEmbeddedNul(){
	// Construct a 5-byte string with an embedded NUL; a c_str()-based writer
	// would truncate this at index 2, so the round trip must reproduce all 5
	// bytes exactly to prove NUL-safety.
	std::string original("ab\0cd", 5);
	CPPUNIT_ASSERT_EQUAL( (size_t)5, original.size() );

	std::stringstream ss;
	int wrc = m_h->callWriteString(ss, original);
	CPPUNIT_ASSERT_EQUAL( 0, wrc );

	std::string readBack;
	int rrc = m_h->callReadString(ss, readBack);
	CPPUNIT_ASSERT_EQUAL( 0, rrc );
	CPPUNIT_ASSERT_EQUAL( (size_t)5, readBack.size() );
	CPPUNIT_ASSERT_EQUAL( original, readBack );
	CPPUNIT_ASSERT_EQUAL( (char)'a', readBack[0] );
	CPPUNIT_ASSERT_EQUAL( (char)'b', readBack[1] );
	CPPUNIT_ASSERT_EQUAL( (char)'\0', readBack[2] );
	CPPUNIT_ASSERT_EQUAL( (char)'c', readBack[3] );
	CPPUNIT_ASSERT_EQUAL( (char)'d', readBack[4] );
}

void FPObjectSerializationTest::testWriteStringReadStringRoundTripEmpty(){
	std::stringstream ss;
	int wrc = m_h->callWriteString(ss, std::string(""));
	CPPUNIT_ASSERT_EQUAL( 0, wrc );

	std::string readBack = "not-empty";
	int rrc = m_h->callReadString(ss, readBack);
	CPPUNIT_ASSERT_EQUAL( 0, rrc );
	CPPUNIT_ASSERT_EQUAL( (size_t)0, readBack.size() );
}

void FPObjectSerializationTest::testReadStringRejectsOversizeLengthNoPayloadRead(){
	// Hand-craft a stream whose length prefix declares 131072 bytes
	// (> kMaxSerializedStringBytes) but which is followed by only 10 bytes
	// of payload. If readString validated the cap before attempting to read
	// the payload, those 10 trailing bytes are left completely untouched;
	// if it instead tried to allocate/read the declared length first, the
	// stream's read position would advance past the length prefix as the
	// implementation consumed whatever payload bytes were available. This
	// makes the "no allocation / no payload read on the bad path" claim
	// directly observable rather than merely inferred from a return code.
	std::ostringstream headerStream;
	int headerRc = m_h->callWriteU32(headerStream, 131072u); // 0x00020000
	CPPUNIT_ASSERT_EQUAL( 0, headerRc );

	std::string raw = headerStream.str() + std::string("1234567890");
	CPPUNIT_ASSERT_EQUAL( (size_t)14, raw.size() );

	std::istringstream is(raw);
	std::string out;
	int rc = m_h->callReadString(is, out);
	CPPUNIT_ASSERT( rc != 0 );

	// Only the 4-byte length prefix should have been consumed.
	std::streamoff pos = static_cast<std::streamoff>(is.tellg());
	CPPUNIT_ASSERT_EQUAL( (std::streamoff)4, pos );
}

void FPObjectSerializationTest::testReadStringBoundaryExactCapAllowed(){
	// A length of exactly kMaxSerializedStringBytes must be accepted (not
	// rejected by the cap check).
	std::string big(kMaxSerializedStringBytes, 'x');
	std::stringstream ss;
	int wrc = m_h->callWriteString(ss, big);
	CPPUNIT_ASSERT_EQUAL( 0, wrc );

	std::string readBack;
	int rrc = m_h->callReadString(ss, readBack);
	CPPUNIT_ASSERT_EQUAL( 0, rrc );
	CPPUNIT_ASSERT_EQUAL( kMaxSerializedStringBytes, readBack.size() );
	CPPUNIT_ASSERT( readBack == big );
}

void FPObjectSerializationTest::testReadStringBoundaryOneOverCapRejected(){
	// One byte over the cap must be rejected. writeString itself refuses to
	// write a string this large, so the oversize length prefix is
	// hand-crafted directly via writeU32 (which has no cap of its own).
	std::ostringstream headerStream;
	int headerRc = m_h->callWriteU32(headerStream, static_cast<uint32_t>(kMaxSerializedStringBytes + 1));
	CPPUNIT_ASSERT_EQUAL( 0, headerRc );

	std::istringstream is(headerStream.str());
	std::string out;
	int rc = m_h->callReadString(is, out);
	CPPUNIT_ASSERT( rc != 0 );
}

void FPObjectSerializationTest::testWriteStringOversizeGuardRejectsWrite(){
	std::string tooBig(kMaxSerializedStringBytes + 1, 'y');
	std::stringstream ss;
	int rc = m_h->callWriteString(ss, tooBig);
	CPPUNIT_ASSERT( rc != 0 );
	// The oversize guard must reject before writing anything, including the
	// length prefix.
	CPPUNIT_ASSERT( ss.str().empty() );
}

void FPObjectSerializationTest::testWriteReadTemplateRoundTrip(){
	std::stringstream ss;
	int value = 424242;
	int wrc = m_h->callWrite(ss, value);
	CPPUNIT_ASSERT_EQUAL( 0, wrc );

	int readBack = 0;
	int rrc = m_h->callRead(ss, readBack);
	CPPUNIT_ASSERT_EQUAL( 0, rrc );
	CPPUNIT_ASSERT_EQUAL( value, readBack );
}

void FPObjectSerializationTest::testFailedStreamReturnsNonzeroForAllHelpers(){
	// write<T> on a stream already in a failed state
	{
		std::ostringstream os;
		os.setstate(std::ios::failbit);
		int x = 5;
		int rc = m_h->callWrite(os, x);
		CPPUNIT_ASSERT( rc != 0 );
	}
	// read<T> on an empty (immediately-failing) input stream
	{
		std::istringstream is("");
		int x = 0;
		int rc = m_h->callRead(is, x);
		CPPUNIT_ASSERT( rc != 0 );
	}
	// writeU32 on a stream already in a failed state
	{
		std::ostringstream os;
		os.setstate(std::ios::failbit);
		int rc = m_h->callWriteU32(os, 42u);
		CPPUNIT_ASSERT( rc != 0 );
	}
	// readU32 on an empty input stream
	{
		std::istringstream is("");
		uint32_t v = 0;
		int rc = m_h->callReadU32(is, v);
		CPPUNIT_ASSERT( rc != 0 );
	}
	// writeString on a stream already in a failed state
	{
		std::ostringstream os;
		os.setstate(std::ios::failbit);
		int rc = m_h->callWriteString(os, std::string("hi"));
		CPPUNIT_ASSERT( rc != 0 );
	}
	// readString on an empty input stream (fails while reading the length prefix)
	{
		std::istringstream is("");
		std::string s;
		int rc = m_h->callReadString(is, s);
		CPPUNIT_ASSERT( rc != 0 );
	}
	// readString on a stream with a valid length prefix but a truncated payload
	{
		std::ostringstream headerStream;
		m_h->callWriteU32(headerStream, 5u);
		std::istringstream is(headerStream.str() + std::string("ab")); // only 2 of 5 payload bytes
		std::string s;
		int rc = m_h->callReadString(is, s);
		CPPUNIT_ASSERT( rc != 0 );
	}
}

void FPObjectSerializationTest::testConstantsHaveExpectedValues(){
	// Supplemental structural checks on the named constants; the behavioral
	// tests above already exercise their runtime effect (cap enforcement,
	// magic byte layout on the wire).
	CPPUNIT_ASSERT_EQUAL( (size_t)65536, kMaxSerializedStringBytes );
	CPPUNIT_ASSERT_EQUAL( (uint32_t)1u, kSaveFormatVersion );
	CPPUNIT_ASSERT_EQUAL( (uint32_t)0x31575353u, kSaveMagic );
}

}
