/**
 * @file FWeapon.cpp
 * @brief Implementation file for the FWeapon class
 * @date Created: Feb 27, 2009
 * @author: Tom Stephens
 */

#include "weapons/FWeapon.h"
#include "weapons/weapons.h"

namespace Frontier {

FWeapon::FWeapon() {
	m_name="NAW";
	m_fullName = "Not a Weapon";
	m_type=NONE;
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
}

FWeapon::~FWeapon() {

}

//void FWeapon::fire(FVehicle * v){
//
//}

const int FWeapon::save(std::ostream & os) const{
	write(os, m_type);
	write(os, m_maxAmmo);
	write(os, m_currentAmmo);
	return 0;
}

int FWeapon::load(std::istream &is){
	read(is, m_maxAmmo);
	read(is, m_currentAmmo);
	return 0;
}

FWeapon * createWeapon(int type){
	FWeapon *w;
	if(type == LB){
		w = new FLaserBattery;
	} else if (type == LC) {
		w = new FLaserCannon;
	} else if (type == EB) {
		w = new FElectronBattery;
	} else if (type == PB) {
		w = new FProtonBattery;
	} else if (type == RB) {
		w = new FRocketBattery;
	} else if (type == DC) {
		w = new FDisruptorCannon;
	} else if (type == T) {
		w = new FTorpedo;
	} else if (type == AR) {
		w = new FAssaultRocket;
	} else if (type == SM) {
		w = new FSeekerMissileLauncher;
	} else if (type == M) {
		w = new FMineLauncher;
	} else {
		// there was an error
		w = NULL;
	}
	return w;
}

};
