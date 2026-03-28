/**
 * @file FVehicleTest.cpp
 * @brief Implementation file for the FVehicleTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#include "FVehicleTest.h"
#include <cstdio>

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

	int applyWeaponDamage(int *wList, FTacticalDamageResolution *result) {
		return damageWeapon(wList, result);
	}

	int applyDefenseDamage(int *dList, FTacticalDamageResolution *result) {
		return damageDefense(dList, result);
	}
};

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

const int FVehicleTest::save(std::ostream &os) const {return 0;}
int FVehicleTest::load(std::istream &is) {return 0;}


}
