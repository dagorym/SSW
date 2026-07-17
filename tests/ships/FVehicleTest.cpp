/**
 * @file FVehicleTest.cpp
 * @brief Implementation file for the FVehicleTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#include "FVehicleTest.h"
#include "ships/FArmedStation.h"
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <cstdint>

namespace FrontierTests {
using namespace Frontier;

namespace {

class FVehicleDamageHarness : public FVehicle {
public:
	FVehicleDamageHarness() {
		m_name = "Damage Harness";
		m_type = "DamageHarness";
	}

	~FVehicleDamageHarness() {}

	void configureStats(int hp, int adf, unsigned int mr, unsigned int dcr) {
		m_maxHP = hp;
		m_currentHP = hp;
		m_maxADF = adf;
		m_currentADF = adf;
		m_maxMR = mr;
		m_currentMR = mr;
		m_maxDCR = dcr;
		m_currentDCR = dcr;
	}

	void addWeapon(FWeapon::Weapon type) {
		FWeapon *weapon = createWeapon(type);
		weapon->setParent(this);
		m_weapons.push_back(weapon);
	}

	void addDefense(FDefense::Defense type) {
		delete m_defenses[0];
		m_defenses.clear();
		FDefense *defense = createDefense(type);
		m_defenses.push_back(defense);
		m_currentDefense = defense;
	}

	// Appends a second (or later) live defense without clearing the existing
	// list, so P5-2 round-trip coverage can exercise multi-defense save/load
	// without disturbing whichever defense addDefense() already installed.
	void appendDefense(FDefense::Defense type) {
		m_defenses.push_back(createDefense(type));
	}

	int applyWeaponDamage(int *wList, FTacticalDamageResolution *result) {
		return damageWeapon(wList, result);
	}

	int applyDefenseDamage(int *dList, FTacticalDamageResolution *result) {
		return damageDefense(dList, result);
	}

	void setPowerSystemDamaged(bool value) { m_powerSystemDamaged = value; }
	void setCombatControlDamaged(bool value) { m_combatControlDamaged = value; }
	void setOnFire(bool value) { m_onFire = value; }
	void setNavControlError(int value) { m_navError = value; }
};

// Exposes an append-only way to give a vehicle multiple *live* defenses
// (without clearing the existing list the way FVehicleDamageHarness::addDefense
// does) so CRIT-3 regression coverage can force FVehicle::load()'s
// defense-rebuild loop to free more than one pre-existing defense object.
class FVehicleDefenseReloadHarness : public FVehicle {
public:
	void appendDefense(FDefense::Defense type) {
		m_defenses.push_back(createDefense(type));
	}
};

const int SEEDED_DAMAGE_TABLE_ROLL = 7;

void applyAdvancedRoll(FVehicleDamageHarness &vehicle, int targetRoll, int damage, FTacticalDamageResolution &result) {
	srand(123);
	vehicle.takeDamage(damage, targetRoll - SEEDED_DAMAGE_TABLE_ROLL, false, &result);
	CPPUNIT_ASSERT(result.usedAdvancedDamageTable);
	CPPUNIT_ASSERT(result.damageTableRoll == targetRoll);
}

void assertSingleHullDamageEffect(
		const FTacticalDamageResolution &result,
		int roll,
		int previousHP,
		int currentHP,
		int damage) {
	CPPUNIT_ASSERT(result.totalHullDamageApplied == damage);
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_HullDamage);
	CPPUNIT_ASSERT(result.effects[0].rollValue == roll);
	CPPUNIT_ASSERT(result.effects[0].previousValue == previousHP);
	CPPUNIT_ASSERT(result.effects[0].newValue == currentHP);
	CPPUNIT_ASSERT(result.effects[0].amount == damage);
	CPPUNIT_ASSERT(result.effects[0].hullDamageApplied == damage);
}

void assertMeterEffect(
		const FTacticalDamageEffect &effect,
		TacticalDamageEffectType effectType,
		int roll,
		int previousValue,
		int newValue,
		int amount) {
	CPPUNIT_ASSERT(effect.effectType == effectType);
	CPPUNIT_ASSERT(effect.rollValue == roll);
	CPPUNIT_ASSERT(effect.previousValue == previousValue);
	CPPUNIT_ASSERT(effect.newValue == newValue);
	CPPUNIT_ASSERT(effect.amount == amount);
	CPPUNIT_ASSERT(effect.hullDamageApplied == 0);
}

void assertStatusEffect(
		const FTacticalDamageEffect &effect,
		TacticalDamageEffectType effectType,
		int roll,
		int previousValue,
		int newValue) {
	CPPUNIT_ASSERT(effect.effectType == effectType);
	CPPUNIT_ASSERT(effect.rollValue == roll);
	CPPUNIT_ASSERT(effect.previousValue == previousValue);
	CPPUNIT_ASSERT(effect.newValue == newValue);
	CPPUNIT_ASSERT(effect.hullDamageApplied == 0);
}

}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FVehicleTest );

void FVehicleTest::setUp(){
	m_v1 = new FVehicle();
}

void FVehicleTest::tearDown(){
	delete m_v1;
}

void FVehicleTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getADF() == 0);
	CPPUNIT_ASSERT( m_v1->getDCR() == 0);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getHP() == 0);
	CPPUNIT_ASSERT( !m_v1->getIconName().empty());
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMR() == 0);
	std::ostringstream os;
	os << "Vehicle " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "none");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 0);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "");
}

void FVehicleTest::testSetters(){
	m_v1->setName("Test Vessel");
	CPPUNIT_ASSERT(m_v1->getName() == "Test Vessel");
	m_v1->setHeading(3);
	CPPUNIT_ASSERT(m_v1->getHeading() == 3);
	m_v1->setOwner(2);
	CPPUNIT_ASSERT(m_v1->getOwner() == 2);
	m_v1->setSpeed(13);
	CPPUNIT_ASSERT(m_v1->getSpeed() == 13);
	m_v1->setADF(4);
	CPPUNIT_ASSERT(m_v1->getADF() == 0);
	m_v1->setMR(3);
	CPPUNIT_ASSERT(m_v1->getMR() == 0);
	m_v1->setDCR(25);
	CPPUNIT_ASSERT(m_v1->getDCR() == 0);
	m_v1->setHP(10);
	CPPUNIT_ASSERT(m_v1->getHP() == 0);
	m_v1->setADF(-4);
	CPPUNIT_ASSERT(m_v1->getADF() == 0);
	m_v1->setMR(-3);
	CPPUNIT_ASSERT(m_v1->getMR() == 0);
	m_v1->setDCR(-25);
	CPPUNIT_ASSERT(m_v1->getDCR() == 0);
	m_v1->setHP(-10);
	CPPUNIT_ASSERT(m_v1->getHP() == 0);
	m_v1->setADF(0);
	CPPUNIT_ASSERT(m_v1->getADF() == 0);
	m_v1->setMR(0);
	CPPUNIT_ASSERT(m_v1->getMR() == 0);
	m_v1->setDCR(0);
	CPPUNIT_ASSERT(m_v1->getDCR() == 0);
	m_v1->setHP(0);
	CPPUNIT_ASSERT(m_v1->getHP() == 0);
	m_v1->setIcon("icons/UPFDestroyer.png");
	CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFDestroyer.png"), m_v1->getIconName());
}

void FVehicleTest::testSerialize(){
	m_v1->setName("Test Vessel");
	m_v1->setHeading(3);
	m_v1->setOwner(2);
	m_v1->setSpeed(13);
	m_v1->setADF(4);
	m_v1->setMR(3);
	m_v1->setDCR(25);
	m_v1->setHP(10);
	m_v1->setIcon("icons/UPFFrigate.png");

	const char *filename = "FVehicleTest.tmp";
	std::remove(filename);
	std::ofstream os(filename,std::ios::binary);
	m_v1->save(os);
	os.close();
	FVehicle v2;
	std::ifstream is(filename,std::ios::binary);
	std::string type;
	readString(is,type);
	v2.load(is);
	is.close();
	std::remove(filename);

	CPPUNIT_ASSERT( m_v1->getADF() == 0);
	CPPUNIT_ASSERT( m_v1->getDCR() == 0);
	CPPUNIT_ASSERT( m_v1->getHeading() == 3);
	CPPUNIT_ASSERT( m_v1->getHP() == 0);
	CPPUNIT_ASSERT_EQUAL(std::string("icons/UPFFrigate.png"), m_v1->getIconName());
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 0);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 0);
	CPPUNIT_ASSERT( m_v1->getMR() == 0);
	CPPUNIT_ASSERT( m_v1->getName() == "Test Vessel");
	CPPUNIT_ASSERT( m_v1->getOwner() == 2);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 13);
	CPPUNIT_ASSERT( m_v1->getType() == "none");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 0);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "");

}

void FVehicleTest::testBadType(){
	FVehicle *v = createShip("bob");
	CPPUNIT_ASSERT( v == NULL);
}

void FVehicleTest::testTakeDamageBasicPopulatesOptionalResolution() {
	// AC: basic damage flow emits explicit hull-damage details for higher layers.
	FVehicleDamageHarness vehicle;
	vehicle.configureStats(12, 4, 3, 20);

	FTacticalDamageResolution result;
	vehicle.takeDamage(5, 0, true, &result);

	CPPUNIT_ASSERT(vehicle.getHP() == 7);
	CPPUNIT_ASSERT(!result.usedAdvancedDamageTable);
	CPPUNIT_ASSERT(result.requestedDamage == 5);
	CPPUNIT_ASSERT(result.damageTableModifier == 0);
	CPPUNIT_ASSERT(result.damageTableRoll == -1);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 5);
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_HullDamage);
	CPPUNIT_ASSERT(result.effects[0].previousValue == 12);
	CPPUNIT_ASSERT(result.effects[0].newValue == 7);
	CPPUNIT_ASSERT(result.effects[0].amount == 5);
	CPPUNIT_ASSERT(result.effects[0].hullDamageApplied == 5);
}

void FVehicleTest::testTakeDamageAdvancedPreservesMutationWithOptionalResolution() {
	// AC: optional reporting must not alter existing advanced-damage mutation behavior.
	FVehicleDamageHarness legacyVehicle;
	FVehicleDamageHarness reportingVehicle;
	legacyVehicle.configureStats(12, 4, 3, 20);
	reportingVehicle.configureStats(12, 4, 3, 20);

	legacyVehicle.takeDamage(3, -200, false, NULL);

	FTacticalDamageResolution result;
	reportingVehicle.takeDamage(3, -200, false, &result);

	CPPUNIT_ASSERT(legacyVehicle.getHP() == 6);
	CPPUNIT_ASSERT(reportingVehicle.getHP() == legacyVehicle.getHP());
	CPPUNIT_ASSERT(reportingVehicle.getADF() == legacyVehicle.getADF());
	CPPUNIT_ASSERT(reportingVehicle.getMR() == legacyVehicle.getMR());
	CPPUNIT_ASSERT(reportingVehicle.getDCR() == legacyVehicle.getDCR());
	CPPUNIT_ASSERT(reportingVehicle.isPowerSystemDamaged() == legacyVehicle.isPowerSystemDamaged());
	CPPUNIT_ASSERT(reportingVehicle.isCombatControlDamaged() == legacyVehicle.isCombatControlDamaged());
	CPPUNIT_ASSERT(reportingVehicle.isOnFire() == legacyVehicle.isOnFire());
	CPPUNIT_ASSERT(reportingVehicle.getNavControlError() == legacyVehicle.getNavControlError());

	CPPUNIT_ASSERT(result.usedAdvancedDamageTable);
	CPPUNIT_ASSERT(result.requestedDamage == 3);
	CPPUNIT_ASSERT(result.damageTableModifier == -200);
	CPPUNIT_ASSERT(result.damageTableRoll <= 10);
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 6);
	CPPUNIT_ASSERT(result.effects.size() == 1);
	CPPUNIT_ASSERT(result.effects[0].effectType == TDET_HullDamage);
	CPPUNIT_ASSERT(result.effects[0].previousValue == 12);
	CPPUNIT_ASSERT(result.effects[0].newValue == 6);
	CPPUNIT_ASSERT(result.effects[0].hullDamageApplied == 6);
}

void FVehicleTest::testDamageHelpersReportExplicitComponentMetadata() {
	// AC: component-hit effects identify the specific weapon or defense that was disabled.
	FVehicleDamageHarness weaponVehicle;
	weaponVehicle.configureStats(10, 3, 2, 12);
	weaponVehicle.addWeapon(FWeapon::LB);
	FTacticalDamageResolution weaponResult;
	weaponResult.damageTableRoll = 62;
	int weaponTargets[] = {FWeapon::LB, FWeapon::NONE};

	const FWeapon *weapon = weaponVehicle.getWeapon(0);
	CPPUNIT_ASSERT(weapon != NULL);
	CPPUNIT_ASSERT(weaponVehicle.applyWeaponDamage(weaponTargets, &weaponResult) == 1);
	CPPUNIT_ASSERT(weapon->isDamaged());
	CPPUNIT_ASSERT(weaponResult.effects.size() == 1);
	CPPUNIT_ASSERT(weaponResult.effects[0].effectType == TDET_WeaponDamaged);
	CPPUNIT_ASSERT(weaponResult.effects[0].rollValue == 62);
	CPPUNIT_ASSERT(weaponResult.effects[0].weaponType == weapon->getType());
	CPPUNIT_ASSERT(weaponResult.effects[0].weaponID == weapon->getID());
	CPPUNIT_ASSERT(weaponResult.effects[0].weaponName == weapon->getLongName());

	FVehicleDamageHarness defenseVehicle;
	defenseVehicle.configureStats(10, 3, 2, 12);
	defenseVehicle.addDefense(FDefense::PS);
	FTacticalDamageResolution defenseResult;
	defenseResult.damageTableRoll = 77;
	int defenseTargets[] = {FDefense::PS, FDefense::UNDEF};

	const FDefense *defense = defenseVehicle.getDefense(0);
	CPPUNIT_ASSERT(defense != NULL);
	CPPUNIT_ASSERT(defenseVehicle.applyDefenseDamage(defenseTargets, &defenseResult) == 1);
	CPPUNIT_ASSERT(defense->isDamaged());
	CPPUNIT_ASSERT(defenseResult.effects.size() == 1);
	CPPUNIT_ASSERT(defenseResult.effects[0].effectType == TDET_DefenseDamaged);
	CPPUNIT_ASSERT(defenseResult.effects[0].rollValue == 77);
	CPPUNIT_ASSERT(defenseResult.effects[0].defenseType == defense->getType());
	CPPUNIT_ASSERT(defenseResult.effects[0].defenseName == defense->getLongName());
}

void FVehicleTest::testAdvancedDamageKeepsADFAndMRCumulativeUntilZero() {
	// AC: ADF/MR hits keep reducing points while nonzero, then fall back to hull damage at zero.
	FVehicleDamageHarness adfVehicle;
	adfVehicle.configureStats(20, 2, 2, 10);

	FTacticalDamageResolution adfOneResult;
	applyAdvancedRoll(adfVehicle, 49, 4, adfOneResult);
	CPPUNIT_ASSERT(adfVehicle.getADF() == 1);
	CPPUNIT_ASSERT(adfVehicle.getHP() == 20);
	CPPUNIT_ASSERT(adfOneResult.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(adfOneResult.effects.size() == 1);
	CPPUNIT_ASSERT(adfOneResult.effects[0].effectType == TDET_ADFLoss);
	CPPUNIT_ASSERT(adfOneResult.effects[0].previousValue == 2);
	CPPUNIT_ASSERT(adfOneResult.effects[0].newValue == 1);
	CPPUNIT_ASSERT(adfOneResult.effects[0].amount == 1);

	FTacticalDamageResolution adfHalfResult;
	applyAdvancedRoll(adfVehicle, 52, 4, adfHalfResult);
	CPPUNIT_ASSERT(adfVehicle.getADF() == 0);
	CPPUNIT_ASSERT(adfVehicle.getHP() == 20);
	CPPUNIT_ASSERT(adfHalfResult.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(adfHalfResult.effects.size() == 1);
	CPPUNIT_ASSERT(adfHalfResult.effects[0].effectType == TDET_ADFLoss);
	CPPUNIT_ASSERT(adfHalfResult.effects[0].previousValue == 1);
	CPPUNIT_ASSERT(adfHalfResult.effects[0].newValue == 0);
	CPPUNIT_ASSERT(adfHalfResult.effects[0].amount == 1);

	FTacticalDamageResolution adfFallbackResult;
	applyAdvancedRoll(adfVehicle, 53, 4, adfFallbackResult);
	CPPUNIT_ASSERT(adfVehicle.getADF() == 0);
	CPPUNIT_ASSERT(adfVehicle.getHP() == 16);
	assertSingleHullDamageEffect(adfFallbackResult, 53, 20, 16, 4);

	FVehicleDamageHarness mrVehicle;
	mrVehicle.configureStats(20, 2, 2, 10);

	FTacticalDamageResolution mrOneResult;
	applyAdvancedRoll(mrVehicle, 58, 4, mrOneResult);
	CPPUNIT_ASSERT(mrVehicle.getMR() == 1);
	CPPUNIT_ASSERT(mrVehicle.getHP() == 20);
	CPPUNIT_ASSERT(mrOneResult.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(mrOneResult.effects.size() == 1);
	CPPUNIT_ASSERT(mrOneResult.effects[0].effectType == TDET_MRLoss);
	CPPUNIT_ASSERT(mrOneResult.effects[0].previousValue == 2);
	CPPUNIT_ASSERT(mrOneResult.effects[0].newValue == 1);
	CPPUNIT_ASSERT(mrOneResult.effects[0].amount == 1);

	FTacticalDamageResolution mrAllResult;
	applyAdvancedRoll(mrVehicle, 60, 4, mrAllResult);
	CPPUNIT_ASSERT(mrVehicle.getMR() == 0);
	CPPUNIT_ASSERT(mrVehicle.getHP() == 20);
	CPPUNIT_ASSERT(mrAllResult.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(mrAllResult.effects.size() == 1);
	CPPUNIT_ASSERT(mrAllResult.effects[0].effectType == TDET_MRLoss);
	CPPUNIT_ASSERT(mrAllResult.effects[0].previousValue == 1);
	CPPUNIT_ASSERT(mrAllResult.effects[0].newValue == 0);
	CPPUNIT_ASSERT(mrAllResult.effects[0].amount == 1);

	FTacticalDamageResolution mrFallbackResult;
	applyAdvancedRoll(mrVehicle, 58, 4, mrFallbackResult);
	CPPUNIT_ASSERT(mrVehicle.getMR() == 0);
	CPPUNIT_ASSERT(mrVehicle.getHP() == 16);
	assertSingleHullDamageEffect(mrFallbackResult, 58, 20, 16, 4);
}

void FVehicleTest::testAdvancedDamageFallsBackForAlreadyDamagedSubsystems() {
	// AC: repeated standard non-hull subsystem hits become normal hull damage.
	FVehicleDamageHarness powerVehicle;
	powerVehicle.configureStats(20, 4, 3, 10);
	powerVehicle.setPowerSystemDamaged(true);
	FTacticalDamageResolution powerResult;
	applyAdvancedRoll(powerVehicle, 74, 4, powerResult);
	CPPUNIT_ASSERT(powerVehicle.isPowerSystemDamaged());
	CPPUNIT_ASSERT(powerVehicle.getHP() == 16);
	assertSingleHullDamageEffect(powerResult, 74, 20, 16, 4);

	FVehicleDamageHarness combatVehicle;
	combatVehicle.configureStats(20, 4, 3, 10);
	combatVehicle.setCombatControlDamaged(true);
	FTacticalDamageResolution combatResult;
	applyAdvancedRoll(combatVehicle, 91, 4, combatResult);
	CPPUNIT_ASSERT(combatVehicle.isCombatControlDamaged());
	CPPUNIT_ASSERT(combatVehicle.getHP() == 16);
	assertSingleHullDamageEffect(combatResult, 91, 20, 16, 4);

	FVehicleDamageHarness navVehicle;
	navVehicle.configureStats(20, 4, 3, 10);
	navVehicle.setNavControlError(1);
	FTacticalDamageResolution navResult;
	applyAdvancedRoll(navVehicle, 97, 4, navResult);
	CPPUNIT_ASSERT(navVehicle.getNavControlError() == 1);
	CPPUNIT_ASSERT(navVehicle.getHP() == 16);
	assertSingleHullDamageEffect(navResult, 97, 20, 16, 4);

	FVehicleDamageHarness fireVehicle;
	fireVehicle.configureStats(20, 4, 3, 10);
	fireVehicle.setOnFire(true);
	FTacticalDamageResolution fireResult;
	applyAdvancedRoll(fireVehicle, 105, 4, fireResult);
	CPPUNIT_ASSERT(fireVehicle.isOnFire());
	CPPUNIT_ASSERT(fireVehicle.getHP() == 16);
	assertSingleHullDamageEffect(fireResult, 105, 20, 16, 4);

	FVehicleDamageHarness dcrVehicle;
	dcrVehicle.configureStats(20, 4, 3, 0);
	FTacticalDamageResolution dcrResult;
	applyAdvancedRoll(dcrVehicle, 106, 4, dcrResult);
	CPPUNIT_ASSERT(dcrVehicle.getDCR() == 0);
	CPPUNIT_ASSERT(dcrVehicle.getHP() == 16);
	assertSingleHullDamageEffect(dcrResult, 106, 20, 16, 4);

	FVehicleDamageHarness partialDCRVehicle;
	partialDCRVehicle.configureStats(20, 4, 3, 10);
	partialDCRVehicle.setDCR(5);
	FTacticalDamageResolution partialDCRResult;
	applyAdvancedRoll(partialDCRVehicle, 106, 4, partialDCRResult);
	CPPUNIT_ASSERT(partialDCRVehicle.getDCR() == 5);
	CPPUNIT_ASSERT(partialDCRVehicle.getHP() == 16);
	assertSingleHullDamageEffect(partialDCRResult, 106, 20, 16, 4);
}

void FVehicleTest::testAdvancedDamageStillDamagesEligibleWeaponAndDefenseComponents() {
	// AC: component hit fallback is unchanged when an undamaged eligible component exists.
	FVehicleDamageHarness weaponVehicle;
	weaponVehicle.configureStats(20, 4, 3, 10);
	weaponVehicle.addWeapon(FWeapon::LB);
	FTacticalDamageResolution weaponResult;
	applyAdvancedRoll(weaponVehicle, 62, 4, weaponResult);

	CPPUNIT_ASSERT(weaponVehicle.getHP() == 20);
	CPPUNIT_ASSERT(weaponVehicle.getWeapon(0) != NULL);
	CPPUNIT_ASSERT(weaponVehicle.getWeapon(0)->isDamaged());
	CPPUNIT_ASSERT(weaponResult.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(weaponResult.effects.size() == 1);
	CPPUNIT_ASSERT(weaponResult.effects[0].effectType == TDET_WeaponDamaged);
	CPPUNIT_ASSERT(weaponResult.effects[0].weaponType == FWeapon::LB);

	FVehicleDamageHarness defenseVehicle;
	defenseVehicle.configureStats(20, 4, 3, 10);
	defenseVehicle.addDefense(FDefense::PS);
	FTacticalDamageResolution defenseResult;
	applyAdvancedRoll(defenseVehicle, 77, 4, defenseResult);

	CPPUNIT_ASSERT(defenseVehicle.getHP() == 20);
	CPPUNIT_ASSERT(defenseVehicle.getDefense(0) != NULL);
	CPPUNIT_ASSERT(defenseVehicle.getDefense(0)->isDamaged());
	CPPUNIT_ASSERT(defenseResult.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(defenseResult.effects.size() == 1);
	CPPUNIT_ASSERT(defenseResult.effects[0].effectType == TDET_DefenseDamaged);
	CPPUNIT_ASSERT(defenseResult.effects[0].defenseType == FDefense::PS);
}

void FVehicleTest::testDisastrousFireAppliesFullEligibleBundleWithoutHullDamage() {
	// AC: Disastrous Fire applies each eligible bundled effect without hull fallback.
	FVehicleDamageHarness vehicle;
	vehicle.configureStats(20, 4, 3, 10);

	FTacticalDamageResolution result;
	applyAdvancedRoll(vehicle, 120, 4, result);

	CPPUNIT_ASSERT(vehicle.getHP() == 20);
	CPPUNIT_ASSERT(vehicle.getADF() == 0);
	CPPUNIT_ASSERT(vehicle.getMR() == 0);
	CPPUNIT_ASSERT(vehicle.getDCR() == 5);
	CPPUNIT_ASSERT(vehicle.isCombatControlDamaged());
	CPPUNIT_ASSERT(vehicle.isOnFire());
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(result.effects.size() == 5);
	assertMeterEffect(result.effects[0], TDET_ADFLoss, 120, 4, 0, 4);
	assertMeterEffect(result.effects[1], TDET_MRLoss, 120, 3, 0, 3);
	assertMeterEffect(result.effects[2], TDET_DCRLoss, 120, 10, 5, 5);
	assertStatusEffect(result.effects[3], TDET_CombatControlDamaged, 120, 0, 1);
	assertStatusEffect(result.effects[4], TDET_ElectricalFire, 120, 0, 1);
}

void FVehicleTest::testDisastrousFireReportsOnlyNewPartialEffectsWithoutHullDamage() {
	// AC: partially spent Disastrous Fire bundles report only newly changed effects.
	FVehicleDamageHarness vehicle;
	vehicle.configureStats(20, 4, 3, 10);
	vehicle.setADF(0);
	vehicle.setMR(0);
	vehicle.setDCR(5);
	vehicle.setCombatControlDamaged(true);
	vehicle.setOnFire(false);

	FTacticalDamageResolution result;
	applyAdvancedRoll(vehicle, 120, 4, result);

	CPPUNIT_ASSERT(vehicle.getHP() == 20);
	CPPUNIT_ASSERT(vehicle.getADF() == 0);
	CPPUNIT_ASSERT(vehicle.getMR() == 0);
	CPPUNIT_ASSERT(vehicle.getDCR() == 5);
	CPPUNIT_ASSERT(vehicle.isCombatControlDamaged());
	CPPUNIT_ASSERT(vehicle.isOnFire());
	CPPUNIT_ASSERT(result.totalHullDamageApplied == 0);
	CPPUNIT_ASSERT(result.effects.size() == 1);
	assertStatusEffect(result.effects[0], TDET_ElectricalFire, 120, 0, 1);
}

void FVehicleTest::testDisastrousFireFallsBackToHullWhenNoBundledEffectCanApply() {
	// AC: Disastrous Fire converts to normal hull damage only when no bundle effect can newly apply.
	FVehicleDamageHarness vehicle;
	vehicle.configureStats(20, 4, 3, 10);
	vehicle.setADF(0);
	vehicle.setMR(0);
	vehicle.setDCR(5);
	vehicle.setCombatControlDamaged(true);
	vehicle.setOnFire(true);

	FTacticalDamageResolution result;
	applyAdvancedRoll(vehicle, 120, 4, result);

	CPPUNIT_ASSERT(vehicle.getADF() == 0);
	CPPUNIT_ASSERT(vehicle.getMR() == 0);
	CPPUNIT_ASSERT(vehicle.getDCR() == 5);
	CPPUNIT_ASSERT(vehicle.isCombatControlDamaged());
	CPPUNIT_ASSERT(vehicle.isOnFire());
	CPPUNIT_ASSERT(vehicle.getHP() == 16);
	assertSingleHullDamageEffect(result, 120, 20, 16, 4);
}

void FVehicleTest::testLoadReassignsCurrentDefenseToLiveDefenseAfterMultiDefenseReload() {
	// AC (CRIT-3): after FVehicle::load() rebuilds m_defenses, m_currentDefense
	// must point at the live m_defenses[0] entry, not a freed pre-load defense.
	//
	// The target vehicle below is given TWO live defenses before load() runs, so
	// the defense-rebuild loop frees two chunks in order (index 0 then index 1).
	// The very first defense allocated while repopulating the list reliably
	// reuses the *last* freed chunk (index 1's address), not index 0's. With the
	// bug still present, m_currentDefense is left holding index 0's (now-freed,
	// and address-distinct) pointer, so this identity check fails deterministically
	// against the unfixed code; with the fix, m_currentDefense is explicitly
	// reassigned to the new m_defenses[0], so the identity check passes.
	FVehicle source; // keeps its single constructor-default (FNone) defense

	const char *filename = "FVehicleTest_defense_reload.tmp";
	std::remove(filename);
	std::ofstream os(filename, std::ios::binary);
	source.save(os);
	os.close();

	FVehicleDefenseReloadHarness target;
	target.appendDefense(FDefense::PS); // target now has two live defenses pre-load

	std::ifstream is(filename, std::ios::binary);
	std::string type;
	readString(is, type);
	target.load(is);
	is.close();
	std::remove(filename);

	CPPUNIT_ASSERT(target.getDefenseCount() == 1);
	CPPUNIT_ASSERT(target.getDefense(0) != NULL);
	CPPUNIT_ASSERT(target.getCurrentDefense() == target.getDefense(0));
	CPPUNIT_ASSERT(target.getCurrentDefense()->getType() == FDefense::NONE);
}

void FVehicleTest::testLoadedVehicleSurvivesWeaponFireViaCurrentDefense() {
	// AC (CRIT-3): a loaded vehicle must be safely targetable in the combat
	// damage path. FWeapon::fire() unconditionally reads
	// m_target->getCurrentDefense() while computing to-hit probability (see
	// FWeapon.cpp), so firing at a freshly loaded vehicle exercises the exact
	// call site the fix addresses; this must complete without a crash and
	// report a resolved (fired) outcome.
	FVehicleDamageHarness attacker;
	attacker.configureStats(20, 4, 3, 10);
	attacker.addWeapon(FWeapon::LB);
	FWeapon *weapon = attacker.getWeapon(0);
	CPPUNIT_ASSERT(weapon != NULL);

	FVehicle source;

	const char *filename = "FVehicleTest_fire_after_reload.tmp";
	std::remove(filename);
	std::ofstream os(filename, std::ios::binary);
	source.save(os);
	os.close();

	FVehicle target;
	std::ifstream is(filename, std::ios::binary);
	std::string type;
	readString(is, type);
	target.load(is);
	is.close();
	std::remove(filename);

	weapon->setTarget(&target, 3);
	FTacticalAttackResult result = weapon->fire();

	CPPUNIT_ASSERT(result.skipReason == TASR_None);
	CPPUNIT_ASSERT(result.fired());
	CPPUNIT_ASSERT(result.outcome == TAO_Hit || result.outcome == TAO_Missed);
	// Post-fire access confirms getCurrentDefense() still resolves to a live object.
	CPPUNIT_ASSERT(target.getCurrentDefense() != NULL);
	CPPUNIT_ASSERT(target.getCurrentDefense()->getType() == FDefense::NONE);
}

void FVehicleTest::testSerializeRoundTripsWeaponsDefensesAndState() {
	// AC (P5-2/a): a vehicle with weapons and defenses round-trips through
	// save->load with all weapons, defenses, and
	// HP/MR/ADF/DCR/heading/speed/flags preserved.
	FVehicleDamageHarness source;
	source.configureStats(/*hp*/18, /*adf*/5, /*mr*/4, /*dcr*/14);

	source.addWeapon(FWeapon::LB);
	source.addWeapon(FWeapon::RB);
	source.getWeapon(1)->setMaxAmmo(4);
	source.getWeapon(1)->setCurrentAmmo(2);
	source.getWeapon(1)->setDamageStatus(true);

	source.addDefense(FDefense::RH);
	source.appendDefense(FDefense::PS);
	source.getDefense(1)->setMaxAmmo(5);
	source.getDefense(1)->setCurrentAmmo(3);
	source.getDefense(1)->setDamageStatus(true);

	source.setSpeed(7);
	source.setHeading(3);
	source.setOwner(9);
	source.setPowerSystemDamaged(true);
	source.setCombatControlDamaged(true);
	source.setOnFire(true);
	source.setNavControlError(-1);

	std::stringstream stream;
	source.save(stream);

	FVehicle target;
	std::string type;
	readString(stream, type);
	int rc = target.load(stream);

	CPPUNIT_ASSERT(rc == 0);

	CPPUNIT_ASSERT(target.getWeaponCount() == 2);
	CPPUNIT_ASSERT(target.getWeapon(0) != NULL);
	CPPUNIT_ASSERT(target.getWeapon(0)->getType() == FWeapon::LB);
	CPPUNIT_ASSERT(target.getWeapon(1) != NULL);
	CPPUNIT_ASSERT(target.getWeapon(1)->getType() == FWeapon::RB);
	CPPUNIT_ASSERT(target.getWeapon(1)->getMaxAmmo() == 4);
	CPPUNIT_ASSERT(target.getWeapon(1)->getAmmo() == 2);
	CPPUNIT_ASSERT(target.getWeapon(1)->isDamaged());
	CPPUNIT_ASSERT(!target.getWeapon(0)->isDamaged());

	CPPUNIT_ASSERT(target.getDefenseCount() == 2);
	CPPUNIT_ASSERT(target.getDefense(0) != NULL);
	CPPUNIT_ASSERT(target.getDefense(0)->getType() == FDefense::RH);
	CPPUNIT_ASSERT(target.getDefense(1) != NULL);
	CPPUNIT_ASSERT(target.getDefense(1)->getType() == FDefense::PS);
	CPPUNIT_ASSERT(target.getDefense(1)->getMaxAmmo() == 5);
	CPPUNIT_ASSERT(target.getDefense(1)->getAmmo() == 3);
	CPPUNIT_ASSERT(target.getDefense(1)->isDamaged());

	CPPUNIT_ASSERT(target.getHP() == 18);
	CPPUNIT_ASSERT(target.getMR() == 4);
	CPPUNIT_ASSERT(target.getADF() == 5);
	CPPUNIT_ASSERT(target.getDCR() == 14);
	CPPUNIT_ASSERT(target.getHeading() == 3);
	CPPUNIT_ASSERT(target.getSpeed() == 7);
	CPPUNIT_ASSERT(target.getOwner() == 9);
	CPPUNIT_ASSERT(target.isPowerSystemDamaged());
	CPPUNIT_ASSERT(target.isCombatControlDamaged());
	CPPUNIT_ASSERT(target.isOnFire());
	CPPUNIT_ASSERT(target.getNavControlError() == -1);
}

void FVehicleTest::testLoadRestoresActiveDefenseSelectionAcrossSaveLoad() {
	// AC (P5-2/b, H2): a vehicle saved with a non-default active defense
	// (Masking Screen raised) reloads with the same defense reported by
	// getCurrentDefense(), with consistent masking-screen/turn-count/ammo
	// state (not re-applied or reset by the reload).
	FArmedStation station;
	// Constructor loadout is [RH(0), MS(1), ICM(2)] with RH active by default.
	CPPUNIT_ASSERT(station.getDefense(1) != NULL);
	CPPUNIT_ASSERT(station.getDefense(1)->getType() == FDefense::MS);
	CPPUNIT_ASSERT(station.getCurrentDefense()->getType() != FDefense::MS);

	// Raising Masking Screen via the real public API decrements ammo (2->1)
	// and, because FArmedStation's m_type matches the station check in
	// setCurrentDefense(), sets the masking-screen turn count to 5.
	station.setCurrentDefense(1);
	CPPUNIT_ASSERT(station.getCurrentDefense()->getType() == FDefense::MS);
	int ammoAfterRaise = station.getCurrentDefense()->getAmmo();
	int turnCountAfterRaise = station.getMSTurnCount();
	CPPUNIT_ASSERT(ammoAfterRaise == 1);
	CPPUNIT_ASSERT(turnCountAfterRaise == 5);

	std::stringstream stream;
	station.save(stream);

	FVehicle target;
	std::string type;
	readString(stream, type);
	int rc = target.load(stream);
	CPPUNIT_ASSERT(rc == 0);

	CPPUNIT_ASSERT(target.getCurrentDefense() != NULL);
	CPPUNIT_ASSERT(target.getCurrentDefense()->getType() == FDefense::MS);
	// State must be restored verbatim from the wire, not re-applied via
	// setCurrentDefense()'s side effects (which would double-decrement ammo
	// to 0 or leave the turn count unset).
	CPPUNIT_ASSERT(target.getCurrentDefense()->getAmmo() == ammoAfterRaise);
	CPPUNIT_ASSERT(target.getMSTurnCount() == turnCountAfterRaise);
}

void FVehicleTest::testLoadAdvancesNextIDPastLoadedID() {
	// AC (P5-2/c, H3): after loading a vehicle with a given m_ID, a newly
	// constructed vehicle receives an ID strictly greater than every loaded
	// ID. Use an ID far larger than any ID this test binary could plausibly
	// have assigned so far so the assertion is not sensitive to test order.
	const uint32_t loadedID = 5000000u;

	std::stringstream stream;
	writeString(stream, "TestVehicle");
	writeU32(stream, loadedID);
	writeString(stream, "Loaded Vessel");
	writeString(stream, "icons/ufo.png");
	write(stream, (unsigned int)0);   // currentMR
	write(stream, (int)0);            // currentADF
	write(stream, (int)0);            // currentHP
	write(stream, (int)0);            // speed
	write(stream, (int)0);            // heading
	writeU32(stream, (uint32_t)0);    // owner
	write(stream, (unsigned int)0);   // currentDCR
	write(stream, (int)0);            // maskingScreenTurnCount
	write(stream, false);             // combatControlDamaged
	write(stream, false);             // onFire
	write(stream, false);             // powerSystemDamaged
	write(stream, (int)0);            // navError
	writeU32(stream, (uint32_t)0);    // weapon count
	writeU32(stream, (uint32_t)0);    // defense count
	writeU32(stream, (uint32_t)0);    // active-defense index (defenses empty; falls back to fabricated FNone)

	FVehicle target;
	std::string type;
	readString(stream, type); // strip the type string load() does not consume itself
	int rc = target.load(stream);
	CPPUNIT_ASSERT(rc == 0);
	CPPUNIT_ASSERT(target.getID() == loadedID);

	FVehicle freshlyConstructed;
	CPPUNIT_ASSERT(freshlyConstructed.getID() > loadedID);
}

void FVehicleTest::testLoadReturnsNonzeroOnUnknownWeaponType() {
	// AC (P5-2/d): FVehicle::load() returns nonzero (no NULL deref) on an
	// unknown weapon type. Hand-craft a stream whose single weapon entry
	// carries an out-of-range FWeapon::Weapon tag that createWeapon(...)
	// cannot resolve (returns NULL).
	std::stringstream stream;
	writeString(stream, "TestVehicle");
	writeU32(stream, (uint32_t)1);
	writeString(stream, "Corrupt Weapon Vessel");
	writeString(stream, "icons/ufo.png");
	write(stream, (unsigned int)0);
	write(stream, (int)0);
	write(stream, (int)0);
	write(stream, (int)0);
	write(stream, (int)0);
	writeU32(stream, (uint32_t)0);
	write(stream, (unsigned int)0);
	write(stream, (int)0);
	write(stream, false);
	write(stream, false);
	write(stream, false);
	write(stream, (int)0);
	writeU32(stream, (uint32_t)1); // weapon count = 1
	FWeapon::Weapon badWeaponType = (FWeapon::Weapon)9999;
	write(stream, badWeaponType);  // corrupt weapon type tag; no further weapon payload needed

	FVehicle target;
	std::string type;
	readString(stream, type); // strip the type string load() does not consume itself
	int rc = target.load(stream);
	CPPUNIT_ASSERT(rc != 0);
}

void FVehicleTest::testLoadReturnsNonzeroOnUnknownDefenseType() {
	// AC (P5-2/d): FVehicle::load() returns nonzero (no NULL deref) on an
	// unknown defense type. Hand-craft a stream with zero weapons and a
	// single defense entry carrying an out-of-range FDefense::Defense tag
	// that createDefense(...) cannot resolve (returns NULL).
	std::stringstream stream;
	writeString(stream, "TestVehicle");
	writeU32(stream, (uint32_t)1);
	writeString(stream, "Corrupt Defense Vessel");
	writeString(stream, "icons/ufo.png");
	write(stream, (unsigned int)0);
	write(stream, (int)0);
	write(stream, (int)0);
	write(stream, (int)0);
	write(stream, (int)0);
	writeU32(stream, (uint32_t)0);
	write(stream, (unsigned int)0);
	write(stream, (int)0);
	write(stream, false);
	write(stream, false);
	write(stream, false);
	write(stream, (int)0);
	writeU32(stream, (uint32_t)0);  // weapon count = 0
	writeU32(stream, (uint32_t)1);  // defense count = 1
	FDefense::Defense badDefenseType = (FDefense::Defense)9999;
	write(stream, badDefenseType);  // corrupt defense type tag; no further defense payload needed

	FVehicle target;
	std::string type;
	readString(stream, type); // strip the type string load() does not consume itself
	int rc = target.load(stream);
	CPPUNIT_ASSERT(rc != 0);
}

const int FVehicleTest::save(std::ostream &os) const {return 0;}
int FVehicleTest::load(std::istream &is) {return 0;}


}
