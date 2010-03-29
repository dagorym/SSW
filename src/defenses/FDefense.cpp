/**
 * @file FDefense.h
 * @brief Implementation file for FDefense class
 * @date Created: Oct 22, 2009
 * @author Tom Stephens
 */

#include "defenses/FDefense.h"
#include "defenses/defenses.h"
#include <sstream>

namespace Frontier {

FDefense::FDefense() {
	m_name="NAD";
	m_fullName = "Not a Defense";
	m_type=FDefense::UNDEF;
	m_maxAmmo=0;
	m_currentAmmo=m_maxAmmo;
	m_isDamaged = false;

}

FDefense::~FDefense() {
	// TODO Auto-generated destructor stub
}

//int getAttackModifier (FWeapon::Weapon wType){
//	return 0;
//}

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

const std::string FDefense::getName() const {
	std::ostringstream os;
	os << m_name;
	if (m_maxAmmo){
		os << "(x" << m_currentAmmo << ")";
	}
	return os.str();
}


FDefense * createDefense (FDefense::Defense type){
	FDefense *d;
	if(type == FDefense::NONE){
		d = new FNone;
	} else if (type == FDefense::RH) {
		d = new FReflectiveHull;
	} else if (type == FDefense::MS) {
		d = new FMaskingScreen;
	} else if (type == FDefense::ICM) {
		d = new FICM;
	} else if (type == FDefense::SS) {
		d = new FStasisScreen;
	} else if (type == FDefense::PS) {
		d = new FProtonScreen;
	} else if (type == FDefense::ES) {
		d = new FElectronScreen;
	} else {
		// there was an error
		d = NULL;
	}
	return d;
}

};
