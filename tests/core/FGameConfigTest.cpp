/**
 * @file FGameConfigTest.cpp
 * @brief Implementation file for the FGameConfigTest class
 * @author Tom Stephens
 * @date Created: Aug 2, 2009
 */

#include "FGameConfigTest.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>

namespace FrontierTests {
using namespace Frontier;

namespace
{
bool pathExists(const std::string &path)
{
	std::ifstream stream(path.c_str(), std::ios::binary);
	return stream.good();
}

std::string readFile(const std::string &path)
{
	std::ifstream stream(path.c_str(), std::ios::binary);
	CPPUNIT_ASSERT_MESSAGE(path, stream.good());
	return std::string((std::istreambuf_iterator<char>(stream)),
			std::istreambuf_iterator<char>());
}

void assertContains(const std::string &haystack, const std::string &needle)
{
	CPPUNIT_ASSERT_MESSAGE(
			std::string("Expected to find '") + needle + "' in inspected source",
			haystack.find(needle) != std::string::npos);
}
}

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

void FGameConfigTest::testResolveAssetPathReturnsUnchangedForEmptyAndAbsolutePaths(){
	// AC: resolver should pass empty and absolute paths through unchanged.
	const std::string empty;
	CPPUNIT_ASSERT(m_c1->resolveAssetPath(empty).empty());

	const std::string absolutePath = "/abs/path/to/asset.png";
	CPPUNIT_ASSERT(m_c1->resolveAssetPath(absolutePath) == absolutePath);
}

void FGameConfigTest::testResolveAssetPathFindsRepoAssetWhenRunningFromTests(){
	// AC: repo-relative assets resolve to a usable existing path.
	const std::string resolved = m_c1->resolveAssetPath("data/zoom.png");
	CPPUNIT_ASSERT(!resolved.empty());
	CPPUNIT_ASSERT(pathExists(resolved));
}

void FGameConfigTest::testResolveAssetPathNormalizesLeadingDotSlashAndSeparators(){
	// AC: resolver should normalize ./ prefixes and path separators.
	const std::string resolved = m_c1->resolveAssetPath("./data\\zoom.png");
	CPPUNIT_ASSERT(!resolved.empty());
	CPPUNIT_ASSERT(pathExists(resolved));
}

void FGameConfigTest::testResolveAssetPathIncludesExecutableParentFallbackContract(){
	// AC: resolver keeps executable-parent fallback so binaries in child dirs still find repo assets.
	const std::string sourcePath =
			std::string(CORE_TEST_REPO_ROOT) + "/src/core/FGameConfig.cpp";
	const std::string source = readFile(sourcePath);

	assertContains(source,
			"const std::string executableParentCandidate =");
	assertContains(source,
			"joinPath(m_executablePath, std::string(\"../\") + normalizedAssetPath);");
	assertContains(source, "if (pathExists(executableParentCandidate)) {");
	assertContains(source, "return executableParentCandidate;");
}

void FGameConfigTest::testComputeSafeTerminatorIndexWithinBoundsReturnsRawResult(){
	// CRIT-6 AC1 (normal path arithmetic): a readlink() result that is
	// strictly within buffer capacity is returned unchanged so the
	// constructor's normal-path substr/ensureTrailingSeparator logic is
	// unaffected by the new clamping helper.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5),
			FGameConfig::computeSafeTerminatorIndex(5, 1000));
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(998),
			FGameConfig::computeSafeTerminatorIndex(998, 1000));
}

void FGameConfigTest::testComputeSafeTerminatorIndexClampsWhenResultEqualsCapacity(){
	// CRIT-6 AC3: a readlink() result exactly at buffer capacity would
	// previously write buf[capacity] (one past the last valid index); the
	// helper must clamp it to capacity - 1.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(999),
			FGameConfig::computeSafeTerminatorIndex(1000, 1000));
}

void FGameConfigTest::testComputeSafeTerminatorIndexClampsWhenResultExceedsCapacity(){
	// CRIT-6 AC3: a readlink() result above buffer capacity must also clamp
	// to capacity - 1, never producing an out-of-bounds index.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(999),
			FGameConfig::computeSafeTerminatorIndex(5000, 1000));
}

void FGameConfigTest::testComputeSafeTerminatorIndexReturnsZeroOnNegativeReadlinkResult(){
	// CRIT-6 AC2: readlink() failure is signaled by -1. Before the fix, a
	// size_t capture of -1 became a huge unsigned value and produced an
	// out-of-bounds buf[size] = 0 write. The helper must return 0.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0),
			FGameConfig::computeSafeTerminatorIndex(-1, 1000));
}

void FGameConfigTest::testComputeSafeTerminatorIndexReturnsZeroOnZeroReadlinkResult(){
	// CRIT-6 AC2: a degenerate zero-length readlink() result carries no
	// usable path data and must not be treated as a valid index.
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0),
			FGameConfig::computeSafeTerminatorIndex(0, 1000));
}

void FGameConfigTest::testComputeSafeTerminatorIndexReturnsZeroWhenBufferCapacityZero(){
	// Degenerate buffer capacity of zero must never produce a valid-looking
	// nonzero index (there is no valid index into a zero-capacity buffer,
	// so 0 is the documented safe sentinel).
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0),
			FGameConfig::computeSafeTerminatorIndex(500, 0));
}

void FGameConfigTest::testComputeSafeTerminatorIndexNeverWritesPastBufferAcrossBoundaryValues(){
	// CRIT-6 AC2/AC3 end-to-end: reproduce the constructor's exact write
	// pattern (buf[computeSafeTerminatorIndex(...)] = 0) against a small
	// guarded buffer and prove no byte outside the buffer is ever touched,
	// for both the failure case and the at/above-capacity boundary case
	// that previously caused an out-of-bounds write.
	struct GuardedBuffer {
		unsigned char before[4];
		char buf[8];
		unsigned char after[4];
	};

	const long boundaryReadlinkResults[] = { -1, 0, 8, 9, 100 };
	const size_t bufferCapacity = sizeof(static_cast<GuardedBuffer *>(0)->buf);

	for (size_t i = 0; i < sizeof(boundaryReadlinkResults) / sizeof(boundaryReadlinkResults[0]); ++i) {
		GuardedBuffer guarded;
		std::memset(guarded.before, 0xAA, sizeof(guarded.before));
		std::memset(guarded.buf, 0x55, sizeof(guarded.buf));
		std::memset(guarded.after, 0xAA, sizeof(guarded.after));

		const size_t terminatorIndex = FGameConfig::computeSafeTerminatorIndex(
				boundaryReadlinkResults[i], bufferCapacity);

		CPPUNIT_ASSERT(terminatorIndex < bufferCapacity);
		guarded.buf[terminatorIndex] = 0;

		for (size_t g = 0; g < sizeof(guarded.before); ++g) {
			CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(0xAA), guarded.before[g]);
		}
		for (size_t g = 0; g < sizeof(guarded.after); ++g) {
			CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(0xAA), guarded.after[g]);
		}
	}
}

}
