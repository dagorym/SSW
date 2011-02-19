/**
 * @file FBattleBoard.cpp
 * @brief Implementation file for BattleBoard class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#include "Frontier.h"
#include "tactical/FBattleScreen.h"
#include "core/FHexMap.h"
#include <wx/wx.h>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace Frontier {

FBattleBoard::FBattleBoard(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)
		: wxScrolledWindow( parent, id, pos, size, style, name ) {
	m_parent = (FBattleScreen *)parent;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_nCol = 55;
	m_nRow = 39;
	m_drawPlanet = false;
	m_setRotation = false;
	setConstants(1.0);
	computeCenters();
	m_drawRoute = false;
	m_planetPosition.setPoint(-1,-1);
	FGameConfig &gc = FGameConfig::create();
	m_maskingScreenIcon = new wxImage(gc.getBasePath()+"icons/MaskingScreen.png");

	SetScrollRate( (int)(2*m_d), (int)(3*m_a) );
	SetVirtualSize(m_width,m_height);
	wxColour black(wxT("#000000"));// black
	SetBackgroundColour(black);


	this->Connect(wxEVT_PAINT, wxPaintEventHandler(FBattleBoard::onPaint));
	this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler(FBattleBoard::onLeftUp ),NULL,this);
	this->Connect( wxEVT_MOTION,wxMouseEventHandler(FBattleBoard::onMotion), NULL,this);
}

FBattleBoard::~FBattleBoard() {
}

void FBattleBoard::setConstants(double scale){
	m_scale = scale;
	m_size = (int)(50*m_scale);
	m_trim = (int)(50*m_scale);
	m_d = m_size/2.;
	m_a = m_d/sqrt(3.);
	m_width = (int)((2*m_nCol+1)*m_d) + m_trim;
	m_height = (int)((3*m_nRow+1)*m_a) + m_trim + (int)m_a;

}

void FBattleBoard::draw(wxDC &dc){
	drawGrid(dc);
//  this just draws circles at the center of each hex to check center position
//	wxColour red(wxT("#FF0000"));
//	dc.SetPen(wxPen(red));
//	for (int j=0; j<m_nRow; j++){
//		for (int i=0; i<m_nCol; i++) {
//			dc.DrawCircle(m_hexData[i][j].cx,m_hexData[i][j].cy,2);
//		}
//	}
	if(m_drawPlanet){
		drawCenteredOnHex(m_planetImages[m_planetChoice],m_planetPosition);
	}
	drawShips();
//	if (m_drawRoute){
	if (m_parent->getState()==BS_Battle){
		drawRoute(dc);
	}
	if (m_parent->getWeapon()!=NULL){
		drawWeaponRange(dc);
		drawTarget(dc);
	}
}

void FBattleBoard::onPaint(wxPaintEvent & event){
//	std::cerr << "in FBattleBoard::onPaint()" << std::endl;
	wxPaintDC dc(this);
	draw(dc);
}

void FBattleBoard::drawGrid(wxDC &dc){
	///@todo This could be sped up by only drawing the portion within the DC instead of everything.
	wxColour white(wxT("#FFFFFF"));// white
	wxColour black(wxT("#000000"));// black
	dc.SetBackground(wxBrush(black));
	int x,y;
	dc.SetPen(wxPen(white));
	SetVirtualSize(m_width,m_height);
	CalcUnscrolledPosition(0,0,&x,&y);
	double nx,ny,cx,cy;
	double xoff = m_trim - x;
	double yoff = m_trim - y;
	// draw the slants
	for (int j = 0; j<=m_nRow; j++){
		cy = yoff+m_a+3*j*m_a;
		ny=cy-m_a;
		cx = xoff;
		if (j%2){
			cx += m_d;
			dc.DrawLine(wxPoint((int)round(cx-m_d),(int)round(ny)),wxPoint((int)round(cx),(int)round(cy)));
		}
		for (int i = 0; i<m_nCol; i++){
			nx = cx+m_d;
			if(!(i==0 && j==m_nRow && (m_nRow%2)==0))dc.DrawLine(wxPoint((int)round(cx),(int)round(cy)),wxPoint((int)round(nx),(int)round(ny)));
			cx += 2*m_d;
			if (!(i== m_nCol-1 && j==m_nRow && m_nRow%2)) dc.DrawLine(wxPoint((int)round(nx),(int)round(ny)),wxPoint((int)round(cx),(int)round(cy)));
		}
		if ((j%2)==0){
			nx = cx+m_d;
			if(j!=0) dc.DrawLine(wxPoint((int)round(cx),(int)round(cy)),wxPoint((int)round(nx),(int)round(ny)));
		}
	}
	// draw the vertical sides
	for (int j = 0; j<m_nRow; j++){
		cx=xoff;
		cy=yoff+m_a+3*j*m_a;
		ny = cy + m_a + m_a;
		if (j%2){
			cx += m_d;
		}
		for (int i = 0 ; i<=m_nCol; i++){
			dc.DrawLine(wxPoint((int)round(cx),(int)round(cy)),wxPoint((int)round(cx),(int)round(ny)));
			cx+=m_d+m_d;
		}
	}
	// add the coordinates (currently disabled)
//	dc.SetTextForeground(white);
//	dc.SetFont(wxFont((int)(m_size/5),wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
//	for (int j = 0; j< m_nRow; j++){
//		cy = yoff + m_a + m_a + 3*j*m_a;
//		cx = xoff+m_d;
//		if (j%2){
//			cx += m_d;
//		}
//		for (int i = 0; i< m_nCol; i++){
//			char label[5];
//			sprintf(label,"%02d%02d",i+1,j+1);
//			wxString l = wxString(label);
//			wxCoord w,h;
//			dc.GetTextExtent(l,&w,&h);
//			dc.DrawText(l,(int)round(cx-w/2),(int)round(cy-h/2));
//			cx += m_d + m_d;
//		}
//	}
	Update();
}

void FBattleBoard::onLeftUp(wxMouseEvent & event) {
	wxCoord x,y;
	event.GetPosition(&x,&y);
//	std::cerr << "The x and y positions of the click are " << x << ", " << y << std::endl;
	int a, b;
	if (getHex(x,y,a,b)){
//		char label[5];
//		sprintf(label,"%02d%02d",a+1,b+1);
//		std::cerr << "You selected hex " << label << std::endl;
		FPoint h(a,b);
		switch (m_parent->getState()){
		case BS_SetupPlanet:
			if (m_parent->getControlState()){
				m_planetPosition.setPoint(a,b);
				m_parent->setPlanetPosition(m_planetPosition);
				m_planetChoice = m_parent->getPlanetChoice();
				m_drawPlanet = true;
				m_parent->setState(BS_SetupStation);
				m_parent->toggleControlState();
				Refresh();
			}
			break;
		case BS_SetupStation:
			if (FHexMap::computeHexDistance(a,b,m_planetPosition.getX(),m_planetPosition.getY())==1){
				// valid choice
				m_parent->setStationPosition(h);
//				m_parent->setState(BS_SetupDefendFleet);
				m_hexData[a][b].ships.push_back(m_parent->getStation());
				m_parent->setPhase(PH_SET_SPEED);
				Refresh();
			}
			break;
		case BS_SetupDefendFleet:
		case BS_SetupAttackFleet:
			if (m_parent->getControlState()){
				if( h != m_planetPosition){
					if (!m_setRotation){  // place the ship
//						std::cerr << "Placing ship in hex (" << a << ", " << b << ")" << std::endl;
						m_hexData[a][b].ships.push_back(m_parent->getShip());
						m_shipPos = m_hexData[a][b].pos;
						m_setRotation=true;
						Refresh();
					} else {   // set the rotation
						int heading = computeHeading(event);
						m_parent->getShip()->setHeading(heading);
						m_parent->toggleControlState();
						m_parent->setPhase(PH_SET_SPEED);
						m_setRotation=false;
					}
				}
			}
			break;
		case BS_Battle: {
			switch(m_parent->getPhase()){
			case PH_MOVE:
//				selectVessel(event);
				if (m_drawRoute){
					checkForTurn(event);
				}
				if (!m_drawRoute){  // m_drawRoute can change in the checkForTurn() method so we need another if statement
					selectVessel(event);
				}
				checkMoveStatus();
				break;
			case PH_DEFENSE_FIRE: {
				selectVessel(event);
				break;
			}
			case PH_ATTACK_FIRE: {
				selectVessel(event);
				break;
			}
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
	}
	event.Skip();
}

void FBattleBoard::computeCenters(){
	for (int j=0; j<m_nRow; j++){
		int y = (int)(m_trim + 2 * m_a + (3 * m_a * j));
		for (int i=0; i<m_nCol; i++) {
			int x = (int)(m_trim + m_d + (2 * m_d * i) + m_d * (j%2));  // the m_d * j%2 offsets for odd rows
//			if (j%2) x+=(int)(m_d); //offset odd rows
			m_hexData[i][j].pos.setPoint(x,y);
		}
	}
}

bool FBattleBoard::getHex(int x, int y, int &a, int &b){
	//first we get the mouse position relative to the whole map not just the window
	int rx, ry;
	CalcUnscrolledPosition(x,y,&rx,&ry);
	// It turns out that for a given x,y there are only four possible hex positions we
	// need to check.
	int vIndex = (ry-m_trim)/(int)(3*m_a);  // upper bound for vertical index
	int hIndex = (rx-m_trim)/(int)(2*m_d);  // upper bound for horizontal index
	// check boundaries
	if (vIndex > m_nRow) {vIndex = m_nRow;}
	if (hIndex > m_nCol) {hIndex = m_nCol;}
	a=hIndex;
	b=vIndex;
	double min = 99999999.0;
	for (int i=hIndex; i>= hIndex-1 && i>=0; i--){
		for (int j=vIndex; j>=vIndex-1 && j>=0; j--){
			double dx = m_hexData[i][j].pos.getX() - rx;
			double dy = m_hexData[i][j].pos.getY() - ry;
			double d = sqrt(dx*dx+dy*dy);
			if (d<min) {
				a = i;
				b = j;
				min = d;
			}
		}
	}
//	std::cerr << "The minimum distance was " << min << std::endl;
	if (min > sqrt(m_a*m_a+m_d*m_d)){
		a=-1;
		b=-1;
		return false;
	} else {
		return true;
	}
}

void FBattleBoard::drawCenteredOnHex(wxImage img, FPoint p, int rot){
	wxClientDC dc(this);
	img = img.Scale(m_size,m_size);  // Scale no matter what
	if (rot) {  // rotate if needed
		if (!(img.HasAlpha())){img.InitAlpha();}
		img = img.Rotate(rot*acos(-1.)/3.,wxPoint(m_size/2,m_size/2));
	}
	wxBitmap b (img);
	wxCoord x = m_hexData[p.getX()][p.getY()].pos.getX() - img.GetWidth()/2;
	wxCoord y = m_hexData[p.getX()][p.getY()].pos.getY() - img.GetHeight()/2;
	wxCoord xx,yy;
	CalcScrolledPosition(x,y,&xx,&yy);
	dc.DrawBitmap(b,xx,yy);
}

void FBattleBoard::setScale(double factor) {
//	std::cerr << "Entering FBattleBoard::setScale()" << std::endl;
	setConstants(m_scale*factor);
	computeCenters();
	Refresh();
}

void FBattleBoard::drawShips(){
	for (int i=0; i<m_nCol; i++){
		for (int j=0; j<m_nRow; j++){
			if (m_hexData[i][j].ships.size()){
				FPoint h(i,j);
				for (unsigned int k = 0; k<m_hexData[i][j].ships.size(); k++){
					const wxImage *icon;
					if (m_hexData[i][j].ships[k]->getCurrentDefense()->getType()!=FDefense::MS){
						icon = m_hexData[i][j].ships[k]->getIcon();
					} else {
						icon = m_maskingScreenIcon;
					}
					drawCenteredOnHex(*icon,h,m_hexData[i][j].ships[k]->getHeading());
				}
			}
		}
	}
}

int FBattleBoard::computeHeading(wxMouseEvent &event){
	wxCoord x,y;
	event.GetPosition(&x,&y);  // mouse click position in window
	int rx, ry;
	CalcUnscrolledPosition(x,y,&rx,&ry);  // position relative to map (same coordinates as hex centers
	// Now compute the angle between the hex the ship is in and the mouse click
	// Note: the x- coordinate is actually the negative of what you would expect.  This gets us into
	// the coordinate system you need based on our definition of heading (i.e 0=west and counter clockwise
	// from there).
	double angle = atan2((double)(ry-m_shipPos.getY()),m_shipPos.getX()-rx)*180/acos(-1.0);  // angle in degrees
	if (angle<0) { angle += 360.; }
	int ang = (int)floor(angle+30);
	if (ang>=360) { ang -= 360; }
//	std::cerr << "angle = " << angle << "  ang = " << ang << "  heading = " << ang/60 << std::endl;
	return (ang/60);

}

void FBattleBoard::onMotion(wxMouseEvent & event){
	switch (m_parent->getState()){
	case BS_SetupDefendFleet:
	case BS_SetupAttackFleet:
		if (m_parent->getControlState()){
			if(m_setRotation){
				int heading = computeHeading(event);
				if(heading != m_parent->getShip()->getHeading()){
					m_parent->getShip()->setHeading(heading);
					Refresh();
				}
			}
		}
		break;
	default:
		break;
	}
}

void FBattleBoard::selectVessel(wxMouseEvent &event){
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int a, b;
	getHex(x,y,a,b);
	FVehicle *v = NULL;
//	std::cerr << "The x and y positions of the click are " << x << ", " << y << std::endl;
//	std::cerr << "The selected hex is " << a << ", " << b << std::endl;
	unsigned int shipCount = m_hexData[a][b].ships.size();
	if (shipCount){  // There is at least one ship in the hex
//		std::cerr << "There are " << shipCount << " ships in this hex" << std::endl;
		if (shipCount == 1){
			v= m_hexData[a][b].ships[0];
//			std::cerr << "Setting " << v->getName() << " as current ship." << std::endl;
		} else {  // we've got  more than one ship and need to pick the one in question
			///@todo:  Implement selection of ship when more than one are in a hex
			/// what we want to do is draw a box listing the ships in the hex and based on the selection
			/// pick that ship to work with.
			if (m_parent->getWeapon()!=NULL){
				v=m_parent->getWeapon()->getTarget();
			} else {
				v=m_parent->getShip();
			}
			v = pickShip(v,m_hexData[a][b].ships);
		}
	}
	if (v==NULL) {
		return;
	}
	if (m_parent->getWeapon()!=NULL && m_parent->getActivePlayerID()!=v->getOwner()){
		/// assign as target
		setIfValidTarget(v,FPoint(a,b));
	} else {
		m_parent->setShip(v);
		m_parent->setWeapon(NULL);  // clear current weapons since we have selected a new ship
		m_shipPos.setPoint(a,b);
		///@todo Suspected logic error.  Should getMovingPlayerID() be getActivePlayerID() or is more logic needed.
		if (m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()){
			if (m_parent->getPhase() == PH_MOVE){
				m_drawRoute = true;
				setInitialRoute();
			} else {
				m_drawRoute = false;
			}
		} else {
			m_drawRoute = false;
		}
	}
	m_parent->reDraw();

}

void FBattleBoard::drawRoute(wxDC &dc){
	// iterate over ships in current list and draw their paths
	VehicleList ships = m_parent->getShipList(m_parent->getMovingPlayerID());
	for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
		bool current=false;
//		std::cerr << m_parent->getShip()->getName() << " - " << m_parent->getShip()->getID()
//				<< "   " << (*itr)->getName() << " - " << (*itr)->getID() << std::endl;
		if (m_parent->getShip()!=NULL && m_parent->getShip()->getID()==(*itr)->getID()){
			current = true;
		}
		// first let's highlight the part of the path already selected
		if(	m_turnInfo[(*itr)->getID()].path.getPathLength()>1){
			drawMovedHexes(dc, m_turnInfo[(*itr)->getID()].path.getFullPath(),current);
		}
	}
	if (m_drawRoute){
	// Draw the hexes straight ahead
	drawRouteHexes(dc, m_movementHexes,m_moved+1);
	// Draw the hexes to the left
	drawRouteHexes(dc, m_leftHexes,m_moved+1);
	// Draw the hexes to the right
	drawRouteHexes(dc, m_rightHexes,m_moved+1);
	}

}

void FBattleBoard::setInitialRoute(){
	FPoint current;
	FVehicle *ship = m_parent->getShip();
	int speed = ship->getSpeed();
	int ADF = ship->getADF();
	m_movementHexes.clear();
	m_leftHexes.clear();
	m_rightHexes.clear();
	m_gravityTurns.clear();
	m_gravityTurnFlag = false;

//	m_turnInfo[ship->getID()].path.clear();
//	std::cerr << "For " << ship->getName() << " the current path length is " << m_turnInfo[ship->getID()].path.getPathLength()
//			<< " and nMoved = " << m_turnInfo[ship->getID()].nMoved << std::endl;
	if (m_turnInfo[ship->getID()].path.getPathLength() > 1 ) { // the ship has already moved
		m_moved = m_turnInfo[ship->getID()].path.getPathLength()-1;
		current = m_turnInfo[ship->getID()].path.endPoint();
		computeRemainingMoves(current);
	} else {
		int curHeading = ship->getHeading();
		current=m_shipPos;
		m_turnInfo[ship->getID()].path.addPoint(current);
		m_turnInfo[ship->getID()].curHeading = curHeading;
		m_moved=0;
		for (int i=0; i < speed+ADF; i++){
			if (current.getX() != -1){
				current=FHexMap::findNextHex(current,curHeading);
				checkForPlanetCollision(current,curHeading);
				if(current.getX()!=-1) {
					m_movementHexes.push_back(current);
					curHeading = forceTurn(ship,curHeading,current);  // Force a turn if Navigation hit damage exists
				}
			}
		}
	}
}

bool FBattleBoard::findHexInList(PointList list, FPoint ref, int &count){
	PointList::iterator itr = list.begin();
//	FPoint h;
	while (itr != list.end()){
		if ((*itr)==ref){
//			h=ref;
			return true;
		}
		itr++;
		count++;
	}
	return false;
}

void FBattleBoard::checkForTurn(wxMouseEvent &event){
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int a, b;
	getHex(x,y,a,b);
	FPoint h(a,b);

	// First check to see if we are on the already moved hexes (i.e. we want to change our mind)
	bool backtrack = m_turnInfo[m_parent->getShip()->getID()].path.isPointOnPath(h);
	if (backtrack){
		m_moved=m_turnInfo[m_parent->getShip()->getID()].path.removeTrailingPoints(h)-1;
		m_turnInfo[m_parent->getShip()->getID()].nMoved=m_moved;
		if (m_turnInfo[m_parent->getShip()->getID()].path.getPathLength()==1){
			m_turnInfo[m_parent->getShip()->getID()].curHeading = m_turnInfo[m_parent->getShip()->getID()].startHeading;
		} else {
			m_turnInfo[m_parent->getShip()->getID()].curHeading= m_turnInfo[m_parent->getShip()->getID()].path.getLastHeading();
		}
		computeRemainingMoves(m_turnInfo[m_parent->getShip()->getID()].path.endPoint());
		Refresh();
		return;
	}

	// check to see if it is on one of the forward paths
	bool found=false;
	int turn = 0;
	int moved = 1;
	found = findHexInList(m_movementHexes,h,moved);
	if (found){
		turn=0;
	} else {
		moved=1;
		found= findHexInList(m_leftHexes,h,moved);
		if (found){
			turn=1;
		} else {
			moved=1;
			found= findHexInList(m_rightHexes,h,moved);
			if (found){
				turn=-1;
			}
		}
	}
	if (found) { // yep we're moving the ship.
		// next pick the list we need to work with
		PointList * curList;
		switch(turn){
		case -1:
			curList=&m_rightHexes;
			break;
		case 0:
			curList=&m_movementHexes;
			break;
		case 1:
			curList=&m_leftHexes;
			break;
		}
		// if turning add the turn flag to the path
		if (turn){
			m_turnInfo[m_parent->getShip()->getID()].path.addFlag(h,MR_TURN);
		}
		turnShip(m_turnInfo[m_parent->getShip()->getID()].curHeading,turn);
		//copy all the new hexes into the moved path
		PointList::iterator itr = (*curList).begin();
		for (int i=0; i< moved; i++){
			m_turnInfo[m_parent->getShip()->getID()].path.addPoint(*itr);
			std::map<FPoint,int>::iterator tItr = m_gravityTurns.find(*itr);
			if (tItr != m_gravityTurns.end()){
				m_turnInfo[m_parent->getShip()->getID()].path.addFlag(*itr,GRAVITY_TURN);
				turnShip(m_turnInfo[m_parent->getShip()->getID()].curHeading,tItr->second);
			}
			itr++;
		}
		m_moved+=moved;
		m_turnInfo[m_parent->getShip()->getID()].nMoved+=moved;
		m_gravityTurnFlag=false;
		computeRemainingMoves(m_turnInfo[m_parent->getShip()->getID()].path.endPoint());
	} else {
		m_drawRoute=false;
	}
	Refresh();
}

void FBattleBoard::resetMoveData(){
	m_turnInfo.clear();
	m_gravityTurnFlag=false;
	m_gravityTurns.clear();
	m_parent->setMoveComplete(false);
	VehicleList ships = m_parent->getShipList(m_parent->getMovingPlayerID());
	for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
		turnData d;
		d.hasMoved=false;
		d.nMoved=0;
		d.curHeading=(*itr)->getHeading();
		d.startHeading=d.curHeading;
		d.gravityTurns.clear();
//		std::cerr << (*itr)->getID() << " "  <<(*itr)->getName() << " " << d.hasMoved << std::endl;
		if (m_parent->getStation()!=NULL && (*itr)->getID() == m_parent->getStation()->getID()){
			d.path.addPoint(m_parent->getStationPos());
			FPoint nextHex = FHexMap::findNextHex(m_parent->getStationPos(),d.curHeading);
			d.path.addPoint(nextHex);
			d.path.addFlag(nextHex,GRAVITY_TURN);
			d.nMoved=1;
			int turnDir = getPlanetTurnDirection(nextHex,d.curHeading);
			d.finalHeading = turnShip(d.curHeading,turnDir);
		}
		m_turnInfo[(*itr)->getID()]=d;
	}
}

void FBattleBoard::drawRouteHexes(wxDC &dc, PointList list, int count){
	wxColor yellow(wxT("#FFFF00"));
	wxColor orange(wxT("#FFA900"));
	wxColor c;

	if (count < m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF()){
		c=yellow;
	}
	PointList::iterator itr;
	for (itr = list.begin(); itr< list.end(); itr++){
		if (count >= (m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF())){
			c=orange;
		}
		drawShadedHex(dc,c,m_hexData[itr->getX()][itr->getY()].pos);
		count++;
	}
}

void FBattleBoard::drawMovedHexes(wxDC &dc, PointList list, bool current){
	wxColor red(wxT("#FF0000"));
	wxColor gray(wxT("#AAAAAA"));
	if (current){
		dc.SetPen(wxPen(red,3));
	} else {
		dc.SetPen(wxPen(gray,2));
	}
	// get the ships initial position
	wxCoord lx, ly;
//	std::cerr << "Starting Position:  " << list[0].cx << "," << list[0].cy << std::endl;
	CalcScrolledPosition(m_hexData[list[0].getX()][list[0].getY()].pos.getX(),m_hexData[list[0].getX()][list[0].getY()].pos.getY(),&lx,&ly);
	// now just draw lines for the traveled hexes
	PointList::iterator itr;
	for (itr = list.begin(); itr< list.end(); itr++){
//		std::cerr << "Next Position:  " << itr->cx << "," << itr->cy << std::endl;
		wxCoord x,y;
		CalcScrolledPosition(m_hexData[itr->getX()][itr->getY()].pos.getX(),m_hexData[itr->getX()][itr->getY()].pos.getY(),&x,&y);
		dc.DrawLine(lx,ly,x,y);
		lx=x;
		ly=y;
	}
}

void FBattleBoard::computeRemainingMoves(FPoint start){
	int left = m_turnInfo[m_parent->getShip()->getID()].curHeading+1;
	if (left >= 6 ) { left -= 6; }
	int right = m_turnInfo[m_parent->getShip()->getID()].curHeading-1;
	if (right < 0 ) { right += 6; }
	int forward = m_turnInfo[m_parent->getShip()->getID()].curHeading;
	FPoint leftHex = start;
	FPoint rightHex = start;
	FPoint forwardHex = start;
	unsigned int turnLimit = m_parent->getShip()->getMR();//+m_turnInfo[m_parent->getShip()->getID()].gravityTurns.size();
	m_rightHexes.clear();
	m_leftHexes.clear();
	m_movementHexes.clear();
//	std::cerr << m_moved << ", " << m_parent->getShip()->getSpeed()+m_parent->getShip()->getADF() << std::endl;
	computePath(m_movementHexes,forwardHex,forward);
	if(m_turnInfo[m_parent->getShip()->getID()].path.countFlags(MR_TURN) < turnLimit // haven't used up our MR
			&& m_turnInfo[m_parent->getShip()->getID()].path.getPathLength() > 1){   // and we haven't backtracked to our starting hex
		// we only update these if the MR is not expended
		computePath(m_leftHexes,leftHex,left);
		computePath(m_rightHexes,rightHex,right);
	}
//		std::cerr << "m_movementHexes.size() = " << m_movementHexes.size() << std::endl;
}

void FBattleBoard::computePath(PointList &list, FPoint hex, int heading){
	m_gravityTurnFlag = false;
	if (m_turnInfo[m_parent->getShip()->getID()].gravityTurns.size()>0){
		// check to see if we are sitting on a gravity turn point and set gravity turn flag to true if we are
		std::map<FPoint,int>::iterator itr = m_turnInfo[m_parent->getShip()->getID()].gravityTurns.find(hex);
		m_gravityTurnFlag = (itr!=m_turnInfo[m_parent->getShip()->getID()].gravityTurns.end())?true:false;
		// make sure we are not next to the planet and only one hex past the turn point as well.
		if (FHexMap::computeHexDistance(hex,m_planetPosition)==1){
			FPoint turnHex =  m_turnInfo[m_parent->getShip()->getID()].gravityTurns.begin()->first;
			if(FHexMap::computeHexDistance(hex,turnHex) == 1){
				int back = heading + 3;
				back = (back>5)?back-6:back;
				int back2 = heading + 2;
				back2 = (back2>5)?back2-6:back;
				int back3 = heading + 4;
				back3 = (back3>5)?back3-6:back;
				int dir = FHexMap::computeHeading(m_hexData[hex.getX()][hex.getY()].pos
						,m_hexData[turnHex.getX()][turnHex.getY()].pos
						);
				if (dir==back || dir==back2 || dir==back3){
					m_gravityTurnFlag=true;
				}
			}
		}
	}
	// compute the path forward
	for (int i=m_moved; i< m_parent->getShip()->getSpeed()+m_parent->getShip()->getADF(); i++){
		if (hex.getX()!= -1){
			hex=FHexMap::findNextHex(hex,heading);
			checkForPlanetCollision(hex,heading);
			if(hex.getX()!=-1) {
				list.push_back(hex);
				heading=forceTurn(m_parent->getShip(),heading,hex);
			}
		}
	}

}

void FBattleBoard::checkMoveStatus(){
	VehicleList ships = m_parent->getShipList(m_parent->getMovingPlayerID());
	bool finished = true;
	for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
//		if ((*itr)->getHP()<=0) { continue; } // skip dead ships.
		int min = (*itr)->getSpeed()-(*itr)->getADF();
		if (m_turnInfo[(*itr)->getID()].nMoved<min){ // we still have spaces to move
//			std::cerr << (*itr)->getName() << " is not done moving (" << m_turnInfo[(*itr)->getID()].nMoved << " of " << min <<std::endl;
		///@todo Check for movement off the map or into a planet and allow it.
			FPoint pos;
			if (m_turnInfo[(*itr)->getID()].path.getPathLength()>1){
				pos = m_turnInfo[(*itr)->getID()].path.endPoint();
			} else {
				pos = m_shipPos;
			}
			FPoint next = FHexMap::findNextHex(pos,m_turnInfo[(*itr)->getID()].curHeading);
			if (next.getX()>=0 && next.getX()<m_nCol && next.getY()>=0 && next.getY()<m_nRow && next != m_planetPosition){
				// we're not at the edge of the map and not running into a planet
				finished = false;
			}
		} //else {
//			std::cerr << (*itr)->getName() << " is done moving (" << m_turnInfo[(*itr)->getID()].nMoved << " of " << min <<std::endl;
//		}
	}
	std::string val = (finished)?"true ":"false";
//	std::cerr << "The current movement status is " << val << std::endl;
	m_parent->setMoveComplete(finished);
}

void FBattleBoard::finalizeMove(){
	VehicleList ships = m_parent->getShipList(m_parent->getMovingPlayerID());
	for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
		///@todo Check for mines
		unsigned int id = (*itr)->getID();
		FPoint start = m_turnInfo[id].path.startPoint();
		FPoint finish = m_turnInfo[id].path.endPoint();
		bool changedSpeed = false; // flag for whether or not the ship changed speed
		if (m_turnInfo[id].nMoved != (*itr)->getSpeed()){
			changedSpeed = true;
		}
		(*itr)->setSpeed(m_turnInfo[id].nMoved);
		(*itr)->setHeading(m_turnInfo[id].curHeading);

		FPoint next = FHexMap::findNextHex(finish,m_turnInfo[id].curHeading);
		if (next.getX()>=0 && next.getX()<m_nCol && next.getY()>=0 && next.getY()<m_nRow && next != m_planetPosition){
			// we're not at the edge of the map and not running into a planet
			// move the ship to it's new location
			m_hexData[finish.getX()][finish.getY()].ships.push_back(*itr);
		} else {
			// mark it destroyed
			(*itr)->setHP(0);
		}
		// and remove it from it's old location
		for (VehicleList::iterator i2 = m_hexData[start.getX()][start.getY()].ships.begin(); i2 < m_hexData[start.getX()][start.getY()].ships.end(); i2++){
			if ((*i2)->getID() == id){
				m_hexData[start.getX()][start.getY()].ships.erase(i2);
				break;
			}
		}
		bool isStation=false;
		// update station position
		if ((*itr)->getType()=="ArmedStation" || (*itr)->getType()=="FortifiedStation" || (*itr)->getType()=="Fortress"){
			m_parent->setStationPosition(finish);
			isStation=true;
		}
		// clear masking screen if the vehicle is not a station and has turned and has a MS as current defense
		if ((*itr)->getCurrentDefense()->getType()==FDefense::MS && m_turnInfo[id].path.countFlags(MR_TURN)>0 || changedSpeed==true){
			if (isStation){
				std::cerr << "Decrementing MS Turn Count" << std::endl;
				(*itr)->decrementMSTurnCount();
			} else {
				(*itr)->setCurrentDefense(0);
			}
		}
	}
	m_drawRoute = false;
}

int FBattleBoard::turnShip(int & heading, int turn){
	heading+=turn;
	if (heading >= 6) {
		heading -= 6;
	}
	if (heading < 0) {
		heading += 6;
	}
	return heading;
}

FVehicle * FBattleBoard::pickShip(const FVehicle *v, const VehicleList & list){
	FVehicle * selected = list[0];  // by default set it to the first ship in the list
	if (v==NULL) return selected;
	VehicleList::const_iterator itr = list.begin();
	while (itr < list.end()){  // loop over all the ships in the list
		if ((*itr)->getID()==v->getID()){  // If the currently selected ship is in the this
			if (itr+1 != list.end()){  // make sure it's not the last one in the list
				selected = *(itr+1);   // If not set the selected ship to the next one.  If it is we don't do anything and
			}                          //   will get the first ship in the list.
			break;                     // We've found a ship so we're done.
		}
		itr++;
	}
	return selected;
}

void FBattleBoard::drawWeaponRange(wxDC &dc){
	if (m_parent->getPhase()!=PH_ATTACK_FIRE && m_parent->getPhase()!=PH_DEFENSE_FIRE ){
		return;
	}
	if (m_parent->getMovingPlayerID()==m_parent->getActivePlayerID()){
		// we need to draw the entire range over the movement
		computeMovedWeaponRange();
	}
	wxColour red(wxT("#FF0000"));
	std::set<FPoint>::iterator itr = m_targetHexes.begin();
	for (unsigned int i = 0; i < m_targetHexes.size(); i++){
		drawShadedHex(dc,red,m_hexData[(*itr).getX()][(*itr).getY()].pos);
		itr++;
	}
	wxColour blue(wxT("#0000FF"));
	itr=m_headOnHexes.begin();
	for (unsigned int i = 0; i < m_headOnHexes.size(); i++){
		drawShadedHex(dc,blue,m_hexData[(*itr).getX()][(*itr).getY()].pos);
		itr++;
	}
}

void FBattleBoard::computeWeaponRange(){
	m_headOnHexes.clear();
	m_targetHexes.clear();
	if (m_parent->getWeapon()!=NULL){
		if (m_parent->getWeapon()->isFF()){
			computeFFRange(m_shipPos,m_targetHexes,m_headOnHexes);
		} else {
			computeBatteryRange(m_shipPos,m_targetHexes);
		}
	}
}

void FBattleBoard::computeFFRange(FPoint &pos, PointSet &tList, PointSet &hList, int heading){
	wxColour red(wxT("#FF0000"));
	wxColour blue(wxT("#0000FF"));
	FVehicle *s = m_parent->getShip();
	unsigned int range = m_parent->getWeapon()->getRange();
	if (heading == -1){
		heading = s->getHeading();
	}
	// compute the straight ahead hexes
	FPoint curHex = pos;
	for (unsigned int i = 0; i<=range; i++){
		hList.insert(curHex);
		curHex = FHexMap::findNextHex(curHex,heading);
		if (curHex.getX() < 0 || curHex.getY() < 0 || curHex.getX() > m_nCol || curHex.getY() > m_nRow) {
			break;  // bust out if we move off the map
		}
	}
	// compute the right column
	curHex = pos;
//	heading = s->getHeading();
	curHex = FHexMap::findNextHex(curHex,heading);
	heading=turnShip(heading,-1);
	curHex = FHexMap::findNextHex(curHex,heading);
	heading=turnShip(heading,1);
	while (FHexMap::computeHexDistance(pos,curHex) <= (int)range){
		tList.insert(curHex);
		curHex = FHexMap::findNextHex(curHex,heading);
		if (curHex.getX() < 0 || curHex.getY() < 0 || curHex.getX() > m_nCol || curHex.getY() > m_nRow) {
			break;  // bust out if we move off the map
		}
	}

	// compute the left column
	curHex = pos;
//	heading = s->getHeading();
	curHex = FHexMap::findNextHex(curHex,heading);
	heading=turnShip(heading, 1);
	curHex = FHexMap::findNextHex(curHex,heading);
	heading=turnShip(heading, -1);
	while (FHexMap::computeHexDistance(pos,curHex) <= (int)range){
		tList.insert(curHex);
		curHex = FHexMap::findNextHex(curHex,heading);
		if (curHex.getX() < 0 || curHex.getY() < 0 || curHex.getX() > m_nCol || curHex.getY() > m_nRow) {
			break;  // bust out if we move off the map
		}
	}
}

void FBattleBoard::computeBatteryRange(FPoint &pos, PointSet &tList){
	wxColour blue(wxT("#0000FF"));
	unsigned int range = m_parent->getWeapon()->getRange();
	int xMin = pos.getX() - range;
	if (xMin < 0) { xMin = 0; }
	int xMax = pos.getX() + range;
	if (xMax < m_nCol) { xMax = m_nCol; }
	int yMin = pos.getY() - range;
	if (yMin < 0) { yMin = 0; }
	int yMax = pos.getY() + range;
	if (yMax < m_nRow) { yMax = m_nRow; }

	for (int i=xMin;i<=xMax;i++){
		for (int j=yMin;j<=yMax;j++){
			FPoint tmp(i,j);
			if (FHexMap::computeHexDistance(pos.getX(),pos.getY(),i,j)<=(int)range){
				tList.insert(FPoint(i,j));
			}
		}
	}
}

void FBattleBoard::drawShadedHex(wxDC& dc, wxColour c, FPoint p){
	// generate list of points defining the hexagon
	wxPoint pList[6];
	pList[0].y = (int)(-2*m_a);
	pList[0].x = 0;
	pList[1].y = (int)(-m_a);
	pList[1].x = (int)(m_d);
	pList[2].y = (int)(m_a);
	pList[2].x = (int)(m_d);
	pList[3].y = (int)(2*m_a);
	pList[3].x = 0;
	pList[4].y = (int)(m_a);
	pList[4].x = (int)(-m_d);
	pList[5].y = (int)(-m_a);
	pList[5].x = (int)(-m_d);

	dc.SetPen(wxPen(c));
	dc.SetBrush(wxBrush(c,wxCROSSDIAG_HATCH));
	wxCoord x,y;
	CalcScrolledPosition(p.getX(),p.getY(),&x,&y);
	dc.DrawPolygon(6,pList,x,y);
}

void FBattleBoard::drawTarget(wxDC &dc){
	FVehicle *v = m_parent->getWeapon()->getTarget();
	wxColour black(wxT("#000000"));
	wxColour white(wxT("#FFFFFF"));
	dc.SetTextForeground(white);
	dc.SetTextBackground(black);
	dc.SetPen(wxPen(white));
	dc.SetBrush(wxBrush(black));
	dc.DrawRectangle(0,0,250,35);
	wxFont bold(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	wxFont norm(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	dc.SetFont(bold);
	std::string text = "Current Target:  ";
	dc.DrawText(text,10,10);
	int x = dc.GetTextExtent(text).GetWidth()+10;
	dc.SetFont(norm);
	if (v!=NULL){
		dc.DrawText(v->getName(),x,10);
	} else {
		dc.DrawText("None",x,10);
	}
//	std::ostringstream os;
//	os << m_parent->getWeapon()->getTargetRange();
//	dc.DrawText(os.str(),10,20);
}

void FBattleBoard::setIfValidTarget(FVehicle *v, FPoint p){
	bool isTarget = false;		// flag for vessel being a valid target
	bool headOn = false;		// flag for firing vessel getting the head on bonus
	unsigned int min=99;		// closest distance for general firing arc
	unsigned int headOnMin=99;	// closest distance for head on shot
	FWeapon *w = m_parent->getWeapon();

	bool movingPlayer = (m_parent->getActivePlayerID() == m_parent->getMovingPlayerID())?true:false;
	if (movingPlayer){
		// moving player - For the moving player, we need to step through each hex along it's path
		// and check to see if the target vessel falls within the weapon's field of fire.  If it
		// does we store the closest approach
		FPoint curHex = m_turnInfo[m_parent->getShip()->getID()].path.startPoint();
		int heading = m_turnInfo[m_parent->getShip()->getID()].startHeading;
		PointList path = m_turnInfo[m_parent->getShip()->getID()].path.getFullPath();
		PointList::iterator itr = path.begin();
		while (itr < path.end() ){
			PointSet tList,hList;
			if (w->isFF()){
				computeFFRange((*itr),tList,hList,heading);
			} else {
				computeBatteryRange((*itr),tList);
			}
			unsigned int dis = FHexMap::computeHexDistance(p,(*itr));
			if (tList.find(p)!=tList.end()){
				// target is in general hex list
				if (dis < min) {
//					std::cerr << "Target is in general hex list" << std::endl;
					isTarget = true;
					min = dis;
				}
			} else if (hList.find(p)!=hList.end()){
				// target is in head on hex list
				if (dis < headOnMin){
//					std::cerr << "Target is in head-on hex list" << std::endl;
					isTarget = true;
					headOn = true;
					headOnMin = dis;
				}
			}
//			std::cerr << "MP: Checking hex (" << itr->getX() << ", " << itr->getY()
//					<< ").  Range = " << dis << "  min = " << min << "  headOnMin = " << headOnMin << std::endl;
			itr++;  // move to next point
			if (itr != path.end()){
				unsigned int newHeading = m_turnInfo[m_parent->getShip()->getID()].path.getPointHeading(*itr);
				if (newHeading != (unsigned int)heading ) { // we turned
					itr--; //back up to original hex
					heading=newHeading; // but stay turned so that we compute weapon ranges properly.
				}
			}
		}
	} else {
		// defensive player - For the defensive player we just need to run over the path of the
		// moving ship and see where the closest point in the path is for the the target vessel
		PointList path = m_turnInfo[v->getID()].path.getFullPath();
		for (PointList::iterator itr = path.begin(); itr < path.end(); itr++) {
			unsigned int dis = FHexMap::computeHexDistance(m_shipPos,(*itr));
			if (m_targetHexes.find(*itr)!=m_targetHexes.end()){
				// target is in general hex list
//				std::cerr << "Target is in general hex list" << std::endl;
				if (dis < min) {
					isTarget = true;
					min = dis;
				}
			} else if (m_headOnHexes.find(*itr)!=m_headOnHexes.end()){
				// target is in head on hex list
				if (dis < headOnMin){
//					std::cerr << "Target is in head-on hex list" << std::endl;
					isTarget = true;
					headOn = true;
					headOnMin = dis;
				}
			}
//			std::cerr << "MP: Checking hex (" << itr->getX() << ", " << itr->getY()
//					<< ").  Range = " << dis << "  min = " << min << "  headOnMin = " << headOnMin << std::endl;
		}
	}
	// So if we have a target
	if (isTarget){
		// Since the head on bonus give a +10 to hit if the target was in both areas, we take
		// the non-headon shot if it is more than two hexes closer than the head on shot.
		int range = 99;
		if (min+2 < headOnMin){
//			std::cerr << "Setting range to non head on value of " << min << std::endl;
			range = min;
			headOn=false;
		} else {
//			std::cerr << "Setting range to head on value of " << headOnMin << std::endl;
			range = headOnMin;
		}
		m_parent->getWeapon()->setTarget(v,range,headOn);
//		std::cerr << min << ":" << headOnMin << " - Setting target with range " << range << std::endl;
	}
}

void FBattleBoard::removeShipFromGame(unsigned int id){
	for (int i=0; i<m_nCol; i++){
		for (int j=0; j<m_nRow; j++){
			if (m_hexData[i][j].ships.size()){
				for (VehicleList::iterator itr = m_hexData[i][j].ships.begin(); itr < m_hexData[i][j].ships.end(); itr++){
					if ((*itr)->getID() == id){
						m_hexData[i][j].ships.erase(itr);
						return;
					}
				}
			}
		}
	}
}

void FBattleBoard::computeMovedWeaponRange(){
	// For the moving player, we need to step through each hex along it's path
	// and check to see if the target vessel falls within the weapon's field of fire.  If it
	// does we store the closest approach
	m_headOnHexes.clear();
	m_targetHexes.clear();
	FPoint curHex = m_turnInfo[m_parent->getShip()->getID()].path.startPoint();
	int heading = m_turnInfo[m_parent->getShip()->getID()].startHeading;
	PointList path = m_turnInfo[m_parent->getShip()->getID()].path.getFullPath();
	PointList::iterator itr = path.begin();
	while (itr < path.end() ) {
//		std::cerr << "(" << (*itr).getX() << "," << (*itr).getY() << ") - heading = " << heading << std::endl;
		if (m_parent->getWeapon()->isFF()){
			computeFFRange((*itr),m_targetHexes,m_headOnHexes,heading);
		} else {
			computeBatteryRange((*itr),m_targetHexes);
		}
		itr++;  // move to next point
		if (itr != path.end()){
			unsigned int newHeading = m_turnInfo[m_parent->getShip()->getID()].path.getPointHeading(*itr);
			if (newHeading != (unsigned int)heading ) { // we turned
				itr--; //back up to original hex
				heading=newHeading; // but stay turned so that we compute weapon ranges properly.
			}
		}
	}
}

void FBattleBoard::checkForPlanetCollision(FPoint & currentHex, int & currentHeading){
//	m_gravityTurnDirection = 0;
	// first make sure a planet is on the board
	if (m_planetPosition.getX()>=0){ // is there a planet on the board?
		// next make sure we're not going to hit a planet head on (i.e. this hex or
		// the next hex we would move from here is the planet.
		if (m_planetPosition == currentHex /*|| m_planetPosition == FHexMap::findNextHex(currentHex,currentHeading)*/){
			currentHex = FPoint(-1,-1);
			return;
		}
		// finally, check to see if we are going to be influenced by the planet's gravity
		int turnDir = getPlanetTurnDirection(currentHex,currentHeading);
		if (turnDir){
			m_gravityTurns[currentHex] = turnDir;
			currentHeading = turnShip(currentHeading,turnDir);
			m_gravityTurnFlag = true;
			return;
		}
	}
}

int FBattleBoard::getPlanetTurnDirection(FPoint currentHex, int currentHeading){
	// It only affects us if the planet is in our starboard or port back hexes
	int turnDir=0;
	// check port side
	int portBackDir = currentHeading - 2;
	portBackDir = (portBackDir<0)?portBackDir+6:portBackDir;
	FPoint testHex = FHexMap::findNextHex(currentHex,portBackDir);
	if (testHex == m_planetPosition && m_gravityTurnFlag == false){
		turnDir = -1;
	}
	// check starboard side
	int starboardBackDir = currentHeading + 2;
	starboardBackDir = (starboardBackDir>5)?starboardBackDir-6:starboardBackDir;
	testHex = FHexMap::findNextHex(currentHex,starboardBackDir);
	if (testHex == m_planetPosition && m_gravityTurnFlag == false){
		turnDir = 1;
	}
	return turnDir;
}

VehicleList * FBattleBoard::getShipList(FVehicle * v){
	for (int i=0; i< m_nRow; i++){
		for (int j=0; j<m_nCol; j++){
			if (m_hexData[i][j].ships.size()>0){
				for (VehicleList::iterator itr = m_hexData[i][j].ships.begin(); itr < m_hexData[i][j].ships.end(); itr++){
					if ((*itr)->getID() == v->getID()){
						return & m_hexData[i][j].ships;
					}
				}
			}
		}
	}
	return NULL;
}

int FBattleBoard::forceTurn(FVehicle * ship, int curHeading, FPoint current){
	int turnDirection = ship->getNavControlError();
	if (turnDirection != 0 && m_turnInfo[ship->getID()].path.countFlags(MR_TURN) < ship->getMR() ){
		curHeading = turnShip(curHeading,turnDirection);
		m_turnInfo[ship->getID()].path.addPoint(current);
		m_turnInfo[ship->getID()].path.addFlag(current,MR_TURN);
		m_turnInfo[ship->getID()].curHeading = curHeading;
		m_turnInfo[ship->getID()].nMoved++;
		m_moved++;
	}
	return curHeading;
}

}
