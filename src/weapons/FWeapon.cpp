/**
 * @file FWeapon.cpp
 * @brief Implementation file for the FWeapon class
 * @date Created: Feb 27, 2009
 * @author: Tom Stephens
 */

#include "weapons/FWeapon.h"
#include "weapons/weapons.h"
#include "ships/FVehicle.h"
#include "Frontier.h"
#include "sstream"

namespace Frontier {

unsigned int FWeapon::m_nextID = 0;

FWeapon::FWeapon() {
	m_ID = m_nextID++;
	m_name="NAW";
	m_fullName = "Not a Weapon";
	m_type=FWeapon::NONE;
	m_range=0;
	m_nDice=0;
	m_dMod=0;
	m_MPO=false;
	m_FF=false;
	m_RD=false;
	m_maxAmmo=0;
	m_ICMMod=0;
	m_currentAmmo=m_maxAmmo;
	m_damageTableMod = 0;
	m_target=NULL;
	m_targetRange = -1;
	m_isDamaged = false;
	m_baseToHitProb = 0;
	m_isHeadOn = false;
	m_assignedICMCount = 0;
}

FWeapon::~FWeapon() {

}

const std::string FWeapon::getName() const {
	std::ostringstream os;
	os << m_name;
	if (m_maxAmmo){
		os << "(x" << m_currentAmmo << ")";
	}
	return os.str();
}

void FWeapon::fire(){
	// don't do anything if a LTD weapon and out of ammo
	if (m_maxAmmo > 0 && m_currentAmmo == 0) {
		return;
	}
	if (!m_isDamaged && m_target!=NULL && m_targetRange>=0){
		int roll = irand(100);
		int toHitProb = m_baseToHitProb + ((m_isHeadOn)?10:0);
		// modify based on target's current defensive system or the attacker's if the attacker has a MS up
		if (m_parent->getCurrentDefense()->getType()==FDefense::MS){
			toHitProb += m_parent->getCurrentDefense()->getAttackModifier(m_type);
		} else {
			toHitProb += m_target->getCurrentDefense()->getAttackModifier(m_type);
		}
//		if (toHitProb<0)std::cerr << m_name << ":  " << "toHitProb = " << toHitProb << std::endl;
		if (m_RD){
			toHitProb -= 5*m_targetRange;
//			if (toHitProb<0) std::cerr << m_name << ":  " << "range adjusted toHitProb = " << toHitProb << std::endl;
		}
//		if (toHitProb<0)std::cerr << m_name << ":  " << "m_baseToHitProb = " << m_baseToHitProb << "  range = " << m_targetRange << std::endl;
//		if (toHitProb<0)std::cerr << m_name << ":  " << "The chance to hit is " << toHitProb << " and we rolled a " << roll << std::endl;
		// you always have a 5% chance to hit
		if (toHitProb < 5){
			toHitProb = 5;
		}
		// reduce to hit probablity for ICMs
		toHitProb += m_assignedICMCount * m_ICMMod;
		m_assignedICMCount=0;  // clear out the used ICMs

		// reduce to hit probabilty if ship's combat control system is damaged
		if (m_parent->isCombatControlDamaged()){
			toHitProb -= 10;
		}

//		std::cerr << m_name << ": toHitProb = "<< toHitProb << "  roll = " << roll << std::endl;

		if (roll <= toHitProb){
			int damage = 0;
			for (unsigned int i = 0; i < m_nDice; i++){
				damage += irand(10);
			}
			damage += m_dMod;
			// reduce damage if shooting a laser weapon at (or out of) a masking screen
			if((m_type==FWeapon::LC || m_type==FWeapon::LB) &&
					(m_target->getCurrentDefense()->getType()==FDefense::MS||m_parent->getCurrentDefense()->getType()==FDefense::MS)){
				damage = damage/2 + damage%2;  // half damage rounded up
			}
			m_target->takeDamage(damage);
//			std::cerr << "The target, " << m_target->getName() << " was hit for "
//					<< damage << " points of damage." << std::endl;
		}
		// weapon has fired, clear target and reduce ammo if appropriate
		m_target=NULL;
		m_targetRange = -1;
		if (m_currentAmmo){
			m_currentAmmo--;
		}
	}
}

void FWeapon::setTarget(FVehicle *v, int r, bool headOn){
	m_target = v;
	m_targetRange = r;
	m_isHeadOn = headOn;

}

const int FWeapon::save(std::ostream & os) const{
	write(os, m_type);
	write(os, m_maxAmmo);
	write(os, m_currentAmmo);
	write(os, m_isDamaged);
	return 0;
}

int FWeapon::load(std::istream &is){
	read(is, m_maxAmmo);
	read(is, m_currentAmmo);
	read(is, m_isDamaged);
	return 0;
}

void FWeapon::reload(){
	if (m_currentAmmo < m_maxAmmo){
		m_currentAmmo = m_maxAmmo;
	}
}

FWeapon * createWeapon(FWeapon::Weapon type){
	FWeapon *w;
	if(type == FWeapon::LB){
		w = new FLaserBattery;
	} else if (type == FWeapon::LC) {
		w = new FLaserCannon;
	} else if (type == FWeapon::EB) {
		w = new FElectronBattery;
	} else if (type == FWeapon::PB) {
		w = new FProtonBattery;
	} else if (type == FWeapon::RB) {
		w = new FRocketBattery;
	} else if (type == FWeapon::DC) {
		w = new FDisruptorCannon;
	} else if (type == FWeapon::T) {
		w = new FTorpedo;
	} else if (type == FWeapon::AR) {
		w = new FAssaultRocket;
	} else if (type == FWeapon::SM) {
		w = new FSeekerMissileLauncher;
	} else if (type == FWeapon::M) {
		w = new FMineLauncher;
	} else {
		// there was an error
		w = NULL;
	}
	return w;
}

};
