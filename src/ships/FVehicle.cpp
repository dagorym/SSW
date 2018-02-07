/**
 * @file FVehicle.cpp
 * @brief Implementation file for FVehicle class
 * @author Tom Stephens
 * @date Created:  Jan 12, 2005
 *
 */
#include "ships/FVehicle.h"
#include "weapons/FWeapon.h"
#include "defenses/FDefense.h"
#include "defenses/FNone.h"
#include "ships/ships.h"
#include "Frontier.h"
#include <sstream>

namespace Frontier
{
unsigned int FVehicle::m_nextID = 0;
unsigned int FVehicle::m_classCount = 0;

FVehicle::FVehicle(){
	FGameConfig &gc = FGameConfig::create();
	m_ID = m_nextID++;
	m_maxMR = 0;
	m_maxADF = 0;
	m_maxHP = 0;
	m_currentMR = 0;
	m_currentADF = 0;
	m_currentHP = 0;
	std::ostringstream os;
	os << "Vehicle " << m_ID;
	m_name = os.str();
	m_type = "none";
	m_iconName = "icons/ufo.png";
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	m_classCount++;
	m_speed = 0;
	m_heading = 0;
	m_owner = 0;
	m_maxDCR = 0;
	m_currentDCR = 0;
	FDefense *d = new FNone();
	m_defenses.push_back(d);
	m_currentDefense = d;
	m_combatControlDamaged = false;
	m_onFire=false;
	m_powerSystemDamaged = false;
	m_navError = 0;
	m_maskingScreenTurnCount = 0;
}

FVehicle::~FVehicle(){
//	std::cerr << "Deleting " << m_name << std::endl;
	delete m_icon;
	for (unsigned int i=0; i< m_weapons.size(); i++){  // delete weapons
		delete m_weapons[i];
	}
	for (unsigned int i=0; i< m_defenses.size(); i++){  // delete defenses
		delete m_defenses[i];
	}
	m_classCount--;
	if (m_classCount==0){  // if all vehicles have been deleted
		m_nextID=0;  // reset the id counter
	}
}

const int FVehicle::save(std::ostream &os) const {
	writeString(os,m_type);
	write(os,m_ID);
	writeString(os,m_name);
	writeString(os,m_iconName);
	write(os,m_currentMR);
	write(os,m_currentADF);
	write(os,m_currentHP);
	write(os,m_speed);
	write(os,m_heading);
	write(os,m_owner);
	write(os,m_currentDCR);
	write(os,m_maskingScreenTurnCount);
	write(os,m_combatControlDamaged);
	write(os,m_onFire);
	write(os,m_powerSystemDamaged);
	write(os,m_navError);
	write(os,m_weapons.size());
	for (WeaponList::const_iterator itr = m_weapons.begin(); itr != m_weapons.end(); itr++){
		(*itr)->save(os);
	}
	write(os,m_defenses.size());
	for (DefenseList::const_iterator itr = m_defenses.begin(); itr != m_defenses.end(); itr++){
		(*itr)->save(os);
	}
	return 0;
}

void FVehicle::setIcon(std::string icon) {
	FGameConfig &gc = FGameConfig::create();
	m_iconName = icon;
	m_icon->LoadFile(gc.getBasePath()+icon);
}

int FVehicle::load(std::istream &is) {
	FGameConfig &gc = FGameConfig::create();
//	std::cerr << "Entering FVehicle::load" << std::endl;
	read(is,m_ID);
	readString(is,m_name);
	readString(is,m_iconName);
	delete m_icon;
	m_icon = new wxImage(gc.getBasePath()+m_iconName);
	read(is,m_currentMR);
	read(is,m_currentADF);
	read(is,m_currentHP);
	read(is,m_speed);
	read(is,m_heading);
	read(is,m_owner);
	read(is,m_currentDCR);
	read(is,m_maskingScreenTurnCount);
	read(is,m_combatControlDamaged);
	read(is,m_onFire);
	read(is,m_powerSystemDamaged);
	read(is,m_navError);
	size_t count = 0;
	read(is,count);
	for (unsigned int i=0; i< m_weapons.size(); i++){	// they were populated with default values
		delete m_weapons[i];				// at creation so we need to clear them this is a bit
	}										// wasteful in resources and could be done better.
	m_weapons.clear();
	for (unsigned int i=0; i<count; i++){
		FWeapon::Weapon type;
		read(is,type);
		FWeapon *w = createWeapon(type);
		w->setParent(this);
		w->load(is);
		m_weapons.push_back(w);
	}
	read(is,count);
	for (unsigned int i=0; i< m_defenses.size(); i++){	// they were populated with default values
		delete m_defenses[i];				// at creation so we need to clear them this is a bit
	}										// wasteful in resources and could be done better.
	m_defenses.clear();
	for (unsigned int i=0; i<count; i++){
		FDefense::Defense defType;
		read(is,defType);
		FDefense *d = createDefense(defType);
		d->load(is);
		m_defenses.push_back(d);
	}
	return 0;
}

void FVehicle::setMR(unsigned int mr){
	if (mr < 0){
		m_currentMR = 0;
	} else if (mr > m_maxMR){
		m_currentMR = m_maxMR;
	} else {
		m_currentMR = mr;
	}
}

void FVehicle::setADF(int adf){
	if (adf < 0){
		m_currentADF = 0;
	} else if (adf > m_maxADF){
		m_currentADF = m_maxADF;
	} else {
		m_currentADF = adf;
	}
}

void FVehicle::setHP(int hp){
	if (hp < 0){
		m_currentHP = 0;
	} else if (hp > m_maxHP){
		m_currentHP = m_maxHP;
	} else {
		m_currentHP = hp;
	}
}
void FVehicle::setDCR(unsigned int dcr){
	if (dcr < 0){
		m_currentDCR = 0;
	} else if (dcr > m_maxDCR){
		m_currentDCR = m_maxDCR;
	} else {
		m_currentDCR = dcr;
	}
}

std::string FVehicle::getWeaponString(){
	std::ostringstream os;
	for (unsigned int i = 0; i < m_weapons.size(); i++){
		FWeapon *w = m_weapons[i];
		os << w->getName() << "  ";
	}
	return os.str();

}

std::string FVehicle::getDefenseString(){
	std::ostringstream os;
	for (unsigned int i = 0; i < m_defenses.size(); i++){
		FDefense *d = m_defenses[i];
		os << d->getName() << "  ";
	}
	return os.str();

}

FVehicle * createShip(std::string type){
	FVehicle *v;
	if(type == "AssaultScout"){
		v = new FAssaultScout;
	} else if (type == "Fighter") {
		v = new FFighter;
	} else if (type == "AssaultCarrier") {
		v = new FAssaultCarrier;
	} else if (type == "Battleship") {
		v = new FBattleship;
	} else if (type == "Destroyer") {
		v = new FDestroyer;
	} else if (type == "Frigate") {
		v = new FFrigate;
	} else if (type == "HvCruiser") {
		v = new FHvCruiser;
	} else if (type == "LtCruiser") {
		v = new FLtCruiser;
	} else if (type == "Minelayer") {
		v = new FMinelayer;
	} else if (type == "Fortress") {
		v = new FFortress;
	} else if (type == "FortifiedStation") {
		v = new FFortifiedStation;
	} else if (type == "ArmedStation") {
		v = new FArmedStation;
	} else {
		// there was an error
		v = NULL;
	}
	return v;
}

FVehicle * createShip(std::string type, std::string name){
	FVehicle *v= createShip(type);
	v->setName(name);

	return v;
}

void FVehicle::takeDamage (int damage, int damageMod, bool basic){
	if (basic){  // if we're just using the basic damage rules all there is is hull damage
		takeHullDamage(damage);
	} else {
		advancedDamage(damage,damageMod);
	}
}

void FVehicle::reload(){
	for (WeaponList::iterator itr = m_weapons.begin(); itr < m_weapons.end(); itr++){
		(*itr)->reload();
	}
}

void FVehicle::setCurrentDefense(unsigned int i) {
	if (i<m_defenses.size()) {
		// turning on Masking Screen
		if (m_defenses[i]->getType() == FDefense::MS && m_defenses[i]!=m_currentDefense){
			if (m_defenses[i]->getAmmo()>0){
				m_defenses[i]->setCurrentAmmo(m_defenses[i]->getAmmo()-1);
				if (m_type=="ArmedStation" || m_type=="FortifiedStation" || m_type == "Fortress"){
					m_maskingScreenTurnCount = 5;
				}
			} else {
				//Only allow it to be set if there was a masking screen to use.
				//If not, return without changing the current defense
				return;
			}
		}
		m_currentDefense=m_defenses[i];
	} else {
		m_currentDefense=m_defenses[0];
	}
}

void FVehicle::decrementMSTurnCount() {
	if (m_maskingScreenTurnCount>0) {
		m_maskingScreenTurnCount--;
	} else {
		m_currentDefense=m_defenses[0];
	}

}

unsigned int FVehicle::hasDefense(FDefense::Defense d){
	for (unsigned int i = 0; i< m_defenses.size(); i++){
//		std::cerr << "checking defense " << i << " = " << m_defenses[i]->getName() << std::endl;
		if (m_defenses[i]->getType()==d) {
			return i;
		}
	}
	return 0;
}

unsigned int FVehicle::hasWeapon(FWeapon::Weapon w){
	for (unsigned int i = 0; i< m_weapons.size(); i++){
		if (m_weapons[i]->getType()==w) {
			return i;
		}
	}
	return 0;
}

void FVehicle::takeHullDamage(int damage){
	m_currentHP -= damage;
	if (m_currentHP < 0) {
		m_currentHP = 0;
	}
}

void FVehicle::advancedDamage(int damage, int damageMod){
	int roll = irand(100) + damageMod; // roll on the damage table
	if ( roll <= 10 ) {            // double hull damage
		takeHullDamage(damage*2);
	} else if ( roll <= 45 ) {     // normal hull damage
		takeHullDamage(damage);
	} else if ( roll <= 49 ) {     //  Lose 1 ADF
		setADF(getADF()-1);
	} else if ( roll <= 52 ) {     //  Lose 1/2 ADF
		int adfLost = getMaxADF()/2 + getMaxADF()%2;  // half of original ADF rounded up.
		setADF(getADF()-adfLost);
	} else if ( roll == 53 ) {     //  Lose all ADF
		setADF(0);
	} else if ( roll <= 58 ) {     //  Lose 1 MR
		setMR(getMR()-1);
	} else if ( roll <= 60 ) {     //  Lose all MR
		setMR(0);
	} else if ( roll <= 62 ) {     //  Weapon Hit
		int wList[] = {FWeapon::LC,FWeapon::LB,FWeapon::PB,FWeapon::EB,FWeapon::AR,FWeapon::RB,FWeapon::NONE};
		if (damageWeapon(wList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 64 ) {     //  Weapon Hit
		int wList[] = {FWeapon::PB,FWeapon::EB,FWeapon::LB,FWeapon::RB,FWeapon::T,FWeapon::AR,FWeapon::NONE};
		if (damageWeapon(wList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 66 ) {     //  Weapon Hit
		int wList[] = {FWeapon::DC,FWeapon::LC,FWeapon::AR,FWeapon::T,FWeapon::LB,FWeapon::NONE};
		if (damageWeapon(wList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 68 ) {     //  Weapon Hit
		int wList[] = {FWeapon::T,FWeapon::AR,FWeapon::EB,FWeapon::PB,FWeapon::LB,FWeapon::RB,FWeapon::NONE};
		if (damageWeapon(wList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 70 ) {     //  Weapon Hit
		int wList[] = {FWeapon::LB,FWeapon::RB,FWeapon::T,FWeapon::AR,FWeapon::PB,FWeapon::EB,FWeapon::LC,FWeapon::NONE};
		if (damageWeapon(wList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 74 ) {     //  Loose all screens and ICMs
		m_powerSystemDamaged = true;
	} else if ( roll <= 77 ) {     //  Defense Hit
		int dList[] = {FDefense::PS,FDefense::ES,FDefense::SS,FDefense::MS,FDefense::ICM,FDefense::UNDEF};
		if (damageDefense(dList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 80 ) {     //  Defense Hit
		int dList[] = {FDefense::MS,FDefense::ICM,FDefense::SS,FDefense::PS,FDefense::ES,FDefense::UNDEF};
		if (damageDefense(dList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 84 ) {     //  Defense Hit
		int dList[] = {FDefense::ICM,FDefense::SS,FDefense::PS,FDefense::ES,FDefense::MS,FDefense::UNDEF};
		if (damageDefense(dList)==0) { // didn't hit a weapon
			takeHullDamage(damage);
		}
	} else if ( roll <= 91 ) {     //  Combat Control System Hit (-10%)
		m_combatControlDamaged = true;
	} else if ( roll <= 97 ) {     //  Navigation Hit
		if (irand(2)==1){
			m_navError = -1;
		} else {
			m_navError = 1;
		}
	} else if ( roll <= 105 ) {    //  Electrical Fire
		m_onFire=true;
	} else if ( roll <= 116 ) {    //  Lose 1/2 DCR
		int dcrLost = getMaxDCR()/2 + getMaxDCR()%2;  // half of original DCR rounded up.
		setDCR(getDCR()-dcrLost);
	} else if ( roll <= 120 ) {    //  Disastrous Fire
		setADF(0);
		setMR(0);
		int dcrLost = getMaxDCR()/2 + getMaxDCR()%2;  // half of original DCR rounded up.
		setDCR(getDCR()-dcrLost);
		m_combatControlDamaged = true;
		m_onFire=true;
	} else {                       // we should never get here
		takeHullDamage(damage);
	}
}

int FVehicle::damageWeapon(int * wList){
	bool wHit = false;
	// loop over weapon types
	while ((*wList)!=FWeapon::NONE && wHit == false){  // only check until a weapon has been hit
		WeaponList::iterator wItr;
		for (wItr= m_weapons.begin(); wItr < m_weapons.end(); wItr++){
			if ((*wItr)->getType() == *wList && ((*wItr)->isDamaged()==false)){  // if this weapon exists and is functional
				(*wItr)->setDamageStatus(true);  // knock it out
				wHit=true;
				return 1;  // stop after first weapon has been hit
			}
		}
		wList++;
	}
	return 0;
}

int FVehicle::damageDefense(int * dList){
	bool dHit = false;
	// loop over defense types
	while ((*dList)!=FDefense::UNDEF && dHit == false){  // only check until a defense has been hit
		DefenseList::iterator dItr;
		for (dItr= m_defenses.begin(); dItr < m_defenses.end(); dItr++){
			if ((*dItr)->getType() == *dList && ((*dItr)->isDamaged()==false)){  // if this defense exists and is functional
				(*dItr)->setDamageStatus(true);  // knock it out
				dHit=true;
				return 1;  // stop after first defense has been hit
			}
		}
		dList++;
	}
	return 0;
}

};
