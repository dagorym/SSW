/**
 * @file FVehicleTest.cpp
 * @brief Implementation file for the FVehicleTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#include "FVehicleTest.h"

namespace FrontierTests {
using namespace Frontier;

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
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
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
	CPPUNIT_ASSERT(m_v1->getIcon() != NULL);
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

	std::ofstream os("test2",std::ios::binary);
	m_v1->save(os);
	os.close();
	FVehicle v2;
	std::ifstream is("test2",std::ios::binary);
	std::string type;
	readString(is,type);
	v2.load(is);
	is.close();

	CPPUNIT_ASSERT( m_v1->getADF() == 0);
	CPPUNIT_ASSERT( m_v1->getDCR() == 0);
	CPPUNIT_ASSERT( m_v1->getHeading() == 3);
	CPPUNIT_ASSERT( m_v1->getHP() == 0);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
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

const int FVehicleTest::save(std::ostream &os) const {return 0;}
int FVehicleTest::load(std::istream &is) {return 0;}


}
