/**
 * @file FAssaultScoutTest.cpp
 * @brief Implementation file for the FAssaultScoutTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#include "FAssaultScoutTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FAssaultScoutTest );

void FAssaultScoutTest::setUp(){
	m_v1 = new FAssaultScout();
}

void FAssaultScoutTest::tearDown(){
	delete m_v1;
}

void FAssaultScoutTest::testConstructor(){
	CPPUNIT_ASSERT( m_v1->getADF() == 5);
	CPPUNIT_ASSERT( m_v1->getDCR() == 50);
	CPPUNIT_ASSERT( m_v1->getHeading() == 0);
	CPPUNIT_ASSERT( m_v1->getHP() == 15);
	CPPUNIT_ASSERT( m_v1->getIcon() != NULL);
	unsigned int id = m_v1->getID();
	CPPUNIT_ASSERT( id >= 0);
	CPPUNIT_ASSERT( m_v1->getMaxADF() == 5);
	CPPUNIT_ASSERT( m_v1->getMaxDCR() == 50);
	CPPUNIT_ASSERT( m_v1->getMaxHP() == 15);
	CPPUNIT_ASSERT( m_v1->getMaxMR() == 4);
	CPPUNIT_ASSERT( m_v1->getMR() == 4);
	std::ostringstream os;
	os << "Assault Scout " << id;
	CPPUNIT_ASSERT( m_v1->getName() == os.str());
	CPPUNIT_ASSERT( m_v1->getOwner() == 0);
	CPPUNIT_ASSERT( m_v1->getSpeed() == 0);
	CPPUNIT_ASSERT( m_v1->getType() == "AssaultScout");
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 2);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "AR(x4)  LB  ");
}

void FAssaultScoutTest::testSerialize(){
	m_v1->setHeading(3);
	m_v1->setOwner(2);
	m_v1->setSpeed(13);
	m_v1->setADF(4);
	m_v1->setMR(3);
	m_v1->setDCR(25);
	m_v1->setHP(10);

	std::ofstream os("test2",std::ios::binary);
	m_v1->save(os);
	os.close();
	FAssaultScout v2;
	std::ifstream is("test2",std::ios::binary);
	std::string type;
	readString(is,type);
	v2.load(is);
	is.close();

	CPPUNIT_ASSERT( m_v1->getADF() == 4);
	CPPUNIT_ASSERT( m_v1->getDCR() == 25);
	CPPUNIT_ASSERT( m_v1->getHeading() == 3);
	CPPUNIT_ASSERT( m_v1->getHP() == 10);
	CPPUNIT_ASSERT( m_v1->getMR() == 3);
	CPPUNIT_ASSERT( m_v1->getWeaponCount() == 2);
	CPPUNIT_ASSERT( m_v1->getWeaponString() == "AR(x4)  LB  ");

}

void FAssaultScoutTest::testLoader(){
	FVehicle *v = createShip("AssaultScout");
	CPPUNIT_ASSERT( v->getADF() == 5);
	delete v;
}

void FAssaultScoutTest::testReload(){
	FAssaultScout v2;
	v2.getWeapon(0)->setTarget(m_v1,2,true);
	v2.getWeapon(0)->fire();
	CPPUNIT_ASSERT(v2.getWeapon(0)->getAmmo() == 3);
	v2.reload();
	CPPUNIT_ASSERT(v2.getWeapon(0)->getAmmo() == 4);

}

const int FAssaultScoutTest::save(std::ostream &os) const {return 0;}
int FAssaultScoutTest::load(std::istream &is) {return 0;}


}
