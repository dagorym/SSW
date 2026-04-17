/**
 * @file FGameConfigTest.cpp
 * @brief Implementation file for the FGameConfigTest class
 * @author Tom Stephens
 * @date Created: Aug 2, 2009
 */

#include "FGameConfigTest.h"
#include <cstdio>
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

}
