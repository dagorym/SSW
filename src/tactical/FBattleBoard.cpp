/**
 * @file FBattleBoard.cpp
 * @brief Implementation file for BattleBoard class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#include "Frontier.h"
#include "tactical/FBattleScreen.h"
#include <wx/wx.h>
#include <cmath>
#include <algorithm>

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
			if (computeHexDistance(a,b,m_planetPosition.getX(),m_planetPosition.getY())==1){
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
						std::cerr << "Placing ship in hex (" << a << ", " << b << ")" << std::endl;
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

int FBattleBoard::computeHexDistance(int sx, int sy, int ex, int ey){
	int dis=1;
    // calculate hexspace coordinates of A and B
    int x1 = sx - Floor2(sy);
    int y1 = sx + Ceil2(sy);
    int x2 = ex - Floor2(ey);
    int y2 = ex + Ceil2(ey);
    // calculate distance using hexcoords as per previous algorithm
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (sign(dx) == sign(dy)) {
        dis = std::max(abs(dx),abs(dy));
    } else {
        dis = abs(dx) + abs(dy);
    }
	return dis;
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
					drawCenteredOnHex(*(m_hexData[i][j].ships[k]->getIcon()),h,m_hexData[i][j].ships[k]->getHeading());
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
		m_parent->getWeapon()->setTarget(v);
	} else {
		m_parent->setShip(v);
		m_parent->setWeapon(NULL);  // clear current weapons since we have selected a new ship
		m_shipPos.setPoint(a,b);
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
		if(	m_turnInfo[(*itr)->getID()].waypoints.size()>1){
			drawMovedHexes(dc, m_turnInfo[(*itr)->getID()].waypoints,current);
		}
	}
	if (m_drawRoute){
	// Draw moved hexes for present ship
//	drawRouteHexes(dc, m_movedHexes);
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
	m_movedHexes.clear();
	if (m_turnInfo[ship->getID()].hasMoved){
		unsigned int wpCount=0;
		int curHeading = ship->getHeading();
		current=m_turnInfo[ship->getID()].waypoints[wpCount];
		m_moved=m_turnInfo[ship->getID()].nMoved;
		computeRemainingMoves(*(m_turnInfo[ship->getID()].waypoints.end()-1));
		for (int i = 0; i<m_turnInfo[ship->getID()].nMoved;i++){
			current=findNextHex(current,curHeading);
			m_movedHexes.push_back(current);
//			std::cerr << current.cx << "," << current.cy << " -> "
//				<< m_turnInfo[ship->getID()].waypoints[wpCount+1].cx << ","
//				<< m_turnInfo[ship->getID()].waypoints[wpCount+1].cy << std::endl;
			if (current==m_turnInfo[ship->getID()].waypoints[wpCount+1]){
				curHeading = turnShip(curHeading,m_turnInfo[ship->getID()].turns[wpCount]);
				wpCount++;
			}
		}
	} else {
		current=m_shipPos;
		m_moved=0;
		m_turnInfo[ship->getID()].waypoints.clear();
		m_turnInfo[ship->getID()].waypoints.push_back(current);
		for (int i=0; i < speed+ADF; i++){
			current=findNextHex(current,ship->getHeading());
			if(current.getX()!=-1) {m_movementHexes.push_back(current);}
		}
		m_turnInfo[ship->getID()].curHeading = ship->getHeading();
		m_turnInfo[ship->getID()].turns.clear();
		m_turnInfo[ship->getID()].nMoved=0;
//		m_turnInfo[ship->getID()].hasMoved=true;
	}
}

FPoint FBattleBoard::findNextHex(FPoint h, int heading){
	for (int i=h.getX()-1;i<=h.getX()+1;i++){
		for (int j=h.getY()-1;j<=h.getY()+1;j++){
			FPoint tmp(i,j);
			if (i>=0 && j>=0 && m_parent->computeHeading(h,tmp)==heading && computeHexDistance(h.getX(),h.getY(),i,j)==1){
				return tmp;
			}
		}
	}
	FPoint bad(-1,-1);
	return bad;
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
	bool found=false;
	int turn = 0; // direction turned 1=left, -1=right 0=no turn
	int moved = 1;
	// First check to see if we are on the already moved hexes (i.e. we want to change our mind)
	bool backtrack = findHexInList(m_movedHexes,h,moved);
	if (backtrack){
//		std::cerr << "Backtracking" << std::endl;
		std::vector<int>::iterator tItr = m_turnInfo[m_parent->getShip()->getID()].turns.begin();
		PointList::iterator wItr = m_turnInfo[m_parent->getShip()->getID()].waypoints.begin()+1;
		PointList::iterator hItr;
		int moved=0;
		m_turnInfo[m_parent->getShip()->getID()].curHeading = m_parent->getShip()->getHeading();
		for (hItr = m_movedHexes.begin();hItr<m_movedHexes.end();hItr++){
			if ((*hItr)==h){ // we've reached the selected hex
				// first delete every thing beyond this point in the waypoint and turn list
				m_turnInfo[m_parent->getShip()->getID()].turns.erase(tItr,m_turnInfo[m_parent->getShip()->getID()].turns.end());
				m_turnInfo[m_parent->getShip()->getID()].waypoints.erase(wItr,m_turnInfo[m_parent->getShip()->getID()].waypoints.end());
				// add current position as new last waypoint
				m_turnInfo[m_parent->getShip()->getID()].waypoints.push_back(*hItr);
				m_movedHexes.erase(hItr+1,m_movedHexes.end());
				m_turnInfo[m_parent->getShip()->getID()].nMoved=m_movedHexes.size();
				m_moved=m_movedHexes.size();
//				std::cerr << m_parent->getShip()->getName() << " has made "
//						<< m_turnInfo[m_parent->getShip()->getID()].turns.size() << " turns and has moved "
//						<< moved << "(" << m_turnInfo[m_parent->getShip()->getID()].waypoints.size() << ") spaces." << std::endl;
				computeRemainingMoves(*hItr);
				Refresh();
				return;
			}
			if ((*hItr)==(*wItr)){  // we've hit a waypoint
				turnShip(m_turnInfo[m_parent->getShip()->getID()].curHeading,(*tItr));
				wItr++;  // step to next waypoint
				tItr++;  // step to next turn
//				turnShip(m_turnInfo[m_parent->getShip()->getID()].curHeading,(*tItr));
			}
			moved++;
		}

	}
	// check straight ahead
	moved=1;
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
	if(found){
//		std::cerr << "You selected a turn at hex " << a+1 << "," << b+1 << std::endl;
//		std::cerr << "After moving " << m_moved << " hexes." << std::endl;
		//store the waypoint;
//		m_moved=moved;
		m_turnInfo[m_parent->getShip()->getID()].hasMoved = true;
		m_turnInfo[m_parent->getShip()->getID()].waypoints.push_back(h);
		if(turn) {
			m_turnInfo[m_parent->getShip()->getID()].turns.push_back(turn);
		}
//		std::cerr << m_parent->getShip()->getName() << " has "
//				<< m_turnInfo[m_parent->getShip()->getID()].turns.size() << " turns and has "
//				<< m_turnInfo[m_parent->getShip()->getID()].waypoints.size() << " waypoints." << std::endl;
		PointList::iterator itr;
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
		turnShip(m_turnInfo[m_parent->getShip()->getID()].curHeading,turn);
		//copy all the new hexes into the moved hexes list
		itr = (*curList).begin();
		for (int i=0; i< moved; i++){
			m_movedHexes.push_back((*itr));
			itr++;
		}
		itr--;
		m_moved+=moved;
		m_turnInfo[m_parent->getShip()->getID()].nMoved+=moved;
		computeRemainingMoves(*itr);
	} else {
		m_drawRoute=false;
	}
	Refresh();
}

void FBattleBoard::resetMoveData(){
	m_turnInfo.clear();
	m_parent->setMoveComplete(false);
	VehicleList ships = m_parent->getShipList(m_parent->getMovingPlayerID());
	for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
		turnData d;
		d.hasMoved=false;
		d.nMoved=0;
		d.curHeading=(*itr)->getHeading();
//		std::cerr << (*itr)->getID() << " "  <<(*itr)->getName() << " " << d.hasMoved << std::endl;
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
	FPoint leftHex = start;
	FPoint rightHex = start;
	FPoint forwardHex = start;
	m_rightHexes.clear();
	m_leftHexes.clear();
	m_movementHexes.clear();
//	std::cerr << m_moved << ", " << m_parent->getShip()->getSpeed()+m_parent->getShip()->getADF() << std::endl;
	for (int i=m_moved; i< m_parent->getShip()->getSpeed()+m_parent->getShip()->getADF(); i++){
		forwardHex = findNextHex(forwardHex,m_turnInfo[m_parent->getShip()->getID()].curHeading);
		if(forwardHex.getX()!=-1) {m_movementHexes.push_back(forwardHex);}
		if(m_turnInfo[m_parent->getShip()->getID()].turns.size()<m_parent->getShip()->getMR()){
			// we only update these if the MR is not expended
			leftHex=findNextHex(leftHex,left);
			if(leftHex.getX()!=-1) {m_leftHexes.push_back(leftHex);}
			rightHex=findNextHex(rightHex,right);
			if(rightHex.getX()!=-1) {m_rightHexes.push_back(rightHex);}
		}
	}
//		std::cerr << "m_movementHexes.size() = " << m_movementHexes.size() << std::endl;
}

void FBattleBoard::checkMoveStatus(){
	VehicleList ships = m_parent->getShipList(m_parent->getMovingPlayerID());
	bool finished = true;
	for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
		int min = (*itr)->getSpeed()-(*itr)->getADF();
		if (m_turnInfo[(*itr)->getID()].nMoved<min){
//			std::cerr << (*itr)->getName() << " is not done moving (" << m_turnInfo[(*itr)->getID()].nMoved << " of " << min <<std::endl;
			finished = false;
		} else {
//			std::cerr << (*itr)->getName() << " is done moving (" << m_turnInfo[(*itr)->getID()].nMoved << " of " << min <<std::endl;
		}
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
		FPoint &start = m_turnInfo[id].waypoints[0];
		FPoint &finish = *(m_turnInfo[id].waypoints.end()-1);
		(*itr)->setSpeed(m_turnInfo[id].nMoved);
		(*itr)->setHeading(m_turnInfo[id].curHeading);
		// move the ship to it's new location
		m_hexData[finish.getX()][finish.getY()].ships.push_back(*itr);
		// and remove it from it's old location
		for (VehicleList::iterator i2 = m_hexData[start.getX()][start.getY()].ships.begin(); i2 < m_hexData[start.getX()][start.getY()].ships.end(); i2++){
			if ((*i2)->getID() == id){
				m_hexData[start.getX()][start.getY()].ships.erase(i2);
				break;
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
			computeFFRange();
		} else {
			computeBatteryRange();
		}
	}
}

void FBattleBoard::computeFFRange(){
	wxColour red(wxT("#FF0000"));
	wxColour blue(wxT("#0000FF"));
	FVehicle *s = m_parent->getShip();
	unsigned int range = m_parent->getWeapon()->getRange();
	int heading = s->getHeading();
	// compute the straight ahead hexes
	FPoint curHex = m_shipPos;
	for (unsigned int i = 0; i<=range; i++){
		m_headOnHexes.insert(curHex);
		curHex = findNextHex(curHex,heading);
	}
	// compute the right column
	curHex = m_shipPos;
	heading = s->getHeading();
	curHex = findNextHex(curHex,heading);
	heading=turnShip(heading,-1);
	curHex = findNextHex(curHex,heading);
	heading=turnShip(heading,1);
	while (computeHexDistance(m_shipPos.getX(),m_shipPos.getY(),curHex.getX(),curHex.getY()) <= (int)range){
		m_targetHexes.insert(curHex);
		curHex = findNextHex(curHex,heading);
	}

	// compute the left column
	curHex = m_shipPos;
	heading = s->getHeading();
	curHex = findNextHex(curHex,heading);
	heading=turnShip(heading, 1);
	curHex = findNextHex(curHex,heading);
	heading=turnShip(heading, -1);
	while (computeHexDistance(m_shipPos.getX(),m_shipPos.getY(),curHex.getX(),curHex.getY()) <= (int)range){
		m_targetHexes.insert(curHex);
		curHex = findNextHex(curHex,heading);
	}
}

void FBattleBoard::computeBatteryRange(){
	wxColour blue(wxT("#0000FF"));
	unsigned int range = m_parent->getWeapon()->getRange();
	int xMin = m_shipPos.getX() - range;
	if (xMin < 0) { xMin = 0; }
	int xMax = m_shipPos.getX() + range;
	if (xMax < m_nCol) { xMax = m_nCol; }
	int yMin = m_shipPos.getY() - range;
	if (yMin < 0) { yMin = 0; }
	int yMax = m_shipPos.getY() + range;
	if (yMax < m_nRow) { yMax = m_nRow; }

	for (int i=xMin;i<=xMax;i++){
		for (int j=yMin;j<=yMax;j++){
			FPoint tmp(i,j);
			if (computeHexDistance(m_shipPos.getX(),m_shipPos.getY(),i,j)<=(int)range){
				m_targetHexes.insert(FPoint(i,j));
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
}

}
