/**
 * @file FDefense.h
 * @brief Implementation file for FDefense class
 * @date Created: Oct 22, 2009
 * @author Tom Stephens
 */

#include "defenses/FDefense.h"

namespace Frontier {

FDefense::FDefense() {
	m_name="NAD";
	m_fullName = "Not a Defense";
	m_type=UNDEF;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FDefense::~FDefense() {
	// TODO Auto-generated destructor stub
}

const int FDefense::save(std::ostream &os) const{
	write(os, m_type);
	write(os, m_maxAmmo);
	write(os, m_currentAmmo);
	write(os, m_isDamaged);
	return 0;
}

int FDefense::load(std::istream &is){
	read(is, m_maxAmmo);
	read(is, m_currentAmmo);
	read(is, m_isDamaged);
	return 0;
}

void FDefense::reload(){
	if (m_currentAmmo < m_maxAmmo){
		m_currentAmmo = m_maxAmmo;
	}
}


FDefense * createDefense(Defense type){
	FDefense *d;
	if(type == NONE){
//		d = new FLaserBattery;
	} else if (type == RH) {
//		d = new FLaserCannon;
	} else if (type == MS) {
//		d = new FElectronBattery;
	} else if (type == ICM) {
//		d = new FProtonBattery;
	} else if (type == SS) {
//		d = new FRocketBattery;
	} else if (type == PS) {
//		d = new FDisruptorCannon;
	} else if (type == ES) {
//		d = new FTorpedo;
	} else {
		// there was an error
		d = NULL;
	}
	return d;
}

}
