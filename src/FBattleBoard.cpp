/**
 * @file FBattleBoard.cpp
 * @brief Implementation file for BattleBoard class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#include "FBattleScreen.h"
#include "Frontier.h"
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
	m_drawRoute = 0;

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
	if (m_drawRoute){
		drawRoute(dc);
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
	// add the coordinates
	dc.SetTextForeground(white);
	dc.SetFont(wxFont((int)(m_size/5),wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	for (int j = 0; j< m_nRow; j++){
		cy = yoff + m_a + m_a + 3*j*m_a;
		cx = xoff+m_d;
		if (j%2){
			cx += m_d;
		}
		for (int i = 0; i< m_nCol; i++){
			char label[5];
			sprintf(label,"%02d%02d",i+1,j+1);
			wxString l = wxString(label);
			wxCoord w,h;
			dc.GetTextExtent(l,&w,&h);
			dc.DrawText(l,(int)round(cx-w/2),(int)round(cy-h/2));
			cx += m_d + m_d;
		}
	}
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
		switch (m_parent->getState()){
		case BS_SetupPlanet:
			if (m_parent->getControlState()){
				m_planetPosition.cx=a;
				m_planetPosition.cy=b;
				m_parent->setPlanetPosition(m_planetPosition);
				m_planetChoice = m_parent->getPlanetChoice();
				m_drawPlanet = true;
				m_parent->setState(BS_SetupStation);
				m_parent->toggleControlState();
				Refresh();
			}
			break;
		case BS_SetupStation:
			if (computeHexDistance(a,b,m_planetPosition.cx,m_planetPosition.cy)==1){
				// valid choice
				hexData h;
				h.cx=a;
				h.cy=b;
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
				if(!(m_planetPosition.cx==a && m_planetPosition.cy==b) && !m_setRotation){  // place the ship
					m_hexData[a][b].ships.push_back(m_parent->getShip());
					m_shipPos = m_hexData[a][b];
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
			break;
		case BS_Battle:
			switch(m_parent->getPhase()){
			case PH_MOVE:
				if (!m_drawRoute){
					selectVessel(event);
				} else {
					checkForTurn(event);
				}
				break;
			default:
				break;
			}
			break;
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
			m_hexData[i][j].cx = x;
			m_hexData[i][j].cy = y;
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
			double dx = m_hexData[i][j].cx - rx;
			double dy = m_hexData[i][j].cy - ry;
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

void FBattleBoard::drawCenteredOnHex(wxImage img, hexData pos, int rot){
	wxClientDC dc(this);
	img = img.Scale(m_size,m_size);  // Scale no matter what
	if (rot) {  // rotate if needed
		if (!(img.HasAlpha())){img.InitAlpha();}
		img = img.Rotate(rot*acos(-1.)/3.,wxPoint(m_size/2,m_size/2));
	}
	wxBitmap b (img);
	wxCoord x = m_hexData[pos.cx][pos.cy].cx - img.GetWidth()/2;
	wxCoord y = m_hexData[pos.cx][pos.cy].cy - img.GetHeight()/2;
	wxCoord xx,yy;
	CalcScrolledPosition(x,y,&xx,&yy);
	dc.DrawBitmap(b,xx,yy);
}

int FBattleBoard::computeHexDistance(int sx, int sy, int ex, int ey){
	//@todo implement hex distance finding algorithm
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
				hexData h;
				h.cx = i;
				h.cy = j;
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
	double angle = atan2((double)(ry-m_shipPos.cy),m_shipPos.cx-rx)*180/acos(-1.0);  // angle in degrees
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
//	std::cerr << "The x and y positions of the click are " << x << ", " << y << std::endl;
//	std::cerr << "The selected hex is " << a << ", " << b << std::endl;
	unsigned int shipCount = m_hexData[a][b].ships.size();
	if (shipCount){  // There is at least one ship in the hex
		if (shipCount == 1){
			m_parent->setShip(m_hexData[a][b].ships[0]);
			m_parent->reDraw();
		} else {  // we've got  more than one ship and need to pick the one in question
			///@todo:  Implement selection of ship when more than one are in a hex
		}
		m_shipPos.cx = a;
		m_shipPos.cy = b;
		if (m_parent->getShip()->getOwner() == m_parent->getCurPlayerID()){
			m_drawRoute = true;
			setInitialRoute();
		} else {
			m_drawRoute = false;
		}
	}

}

void FBattleBoard::drawRoute(wxDC &dc){
	// first let's highlight the part of the path already selected
	drawRouteHexes(dc, m_movedHexes);
	// Draw the hexes straight ahead
	drawRouteHexes(dc, m_movementHexes,m_moved+1);
	// Draw the hexes to the left
	drawRouteHexes(dc, m_leftHexes,m_moved+1);
	// Draw the hexes to the right
	drawRouteHexes(dc, m_rightHexes,m_moved+1);

//	// generate list of points defining the hexagon
//	wxPoint pList[6];
//	pList[0].y = (int)(-2*m_a);
//	pList[0].x = 0;
//	pList[1].y = (int)(-m_a);
//	pList[1].x = (int)(m_d);
//	pList[2].y = (int)(m_a);
//	pList[2].x = (int)(m_d);
//	pList[3].y = (int)(2*m_a);
//	pList[3].x = 0;
//	pList[4].y = (int)(m_a);
//	pList[4].x = (int)(-m_d);
//	pList[5].y = (int)(-m_a);
//	pList[5].x = (int)(-m_d);
//
//	wxColor yellow(wxT("#FFFF00"));
//	wxColor orange(wxT("#FFA900"));
//	dc.SetPen(wxPen(yellow));
//	dc.SetBrush(wxBrush(yellow,wxCROSSDIAG_HATCH));
//
//	int count=1;
//	// first let's trace the selected path through the waypoints
//	std::vector<hexData>::iterator itr;
//	std::cerr << "There are " << m_turnInfo[m_parent->getShip()->getID()].waypoints.size() << " waypoints" << std::endl;
//	for (itr = m_turnInfo[m_parent->getShip()->getID()].waypoints.begin();itr<m_turnInfo[m_parent->getShip()->getID()].waypoints.end();itr++){
//		std::cerr << "Working toward next waypoint" << std::endl;
//		int heading = m_parent->getShip()->getHeading();
//		hexData current=m_shipPos;
//		std::cerr << current.cx << " " << current.cy << " " << (*itr).cx << " " << (*itr).cy << std::endl;
//		while(!(current.cx!=(*itr).cx && current.cy!=(*itr).cy)){
//			current=findNextHex(current,heading);
//			if (count == (m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF())){
//				dc.SetPen(wxPen(orange));
//				dc.SetBrush(wxBrush(orange,wxCROSSDIAG_HATCH));
//			}
//			wxCoord x,y;
//			CalcScrolledPosition(m_hexData[current.cx][current.cy].cx,m_hexData[current.cx][current.cy].cy,&x,&y);
//			dc.DrawPolygon(6,pList,x,y);
//			std::cerr << current.cx << " " << current.cy << " " << (*itr).cx << " " << (*itr).cy << std::endl;
//			count++;
//		}
//	}
//
//	if (m_moved < m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF()){
//		dc.SetPen(wxPen(yellow));
//		dc.SetBrush(wxBrush(yellow,wxCROSSDIAG_HATCH));
//	}
//	for (itr = m_movementHexes.begin(); itr< m_movementHexes.end(); itr++){
//		if (count == (m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF())){
//			dc.SetPen(wxPen(orange));
//			dc.SetBrush(wxBrush(orange,wxCROSSDIAG_HATCH));
//		}
//		wxCoord x,y;
//		CalcScrolledPosition(m_hexData[itr->cx][itr->cy].cx,m_hexData[itr->cx][itr->cy].cy,&x,&y);
//		dc.DrawPolygon(6,pList,x,y);
//		count++;
//	}
////	dc.SetPen(wxPen(orange));
////	dc.SetBrush(wxBrush(orange,wxCROSSDIAG_HATCH));
////	for (itr = m_ADFHexes.begin(); itr< m_ADFHexes.end(); itr++){
////		wxCoord x,y;
////		CalcScrolledPosition(m_hexData[itr->cx][itr->cy].cx,m_hexData[itr->cx][itr->cy].cy,&x,&y);
////		dc.DrawPolygon(6,pList,x,y);
////		///@todo Figure out how to properly draw a shaded hexagon on the map
////	}
//
//	if (m_moved < m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF()){
//		dc.SetPen(wxPen(yellow));
//		dc.SetBrush(wxBrush(yellow,wxCROSSDIAG_HATCH));
//	}
//	count=m_moved+1;
//	itr = m_leftHexes.begin();
//	for (itr = m_leftHexes.begin(); itr< m_leftHexes.end(); itr++){
//		if (count == (m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF())){
//			dc.SetPen(wxPen(orange));
//			dc.SetBrush(wxBrush(orange,wxCROSSDIAG_HATCH));
//		}
//		wxCoord x,y;
//		CalcScrolledPosition(m_hexData[itr->cx][itr->cy].cx,m_hexData[itr->cx][itr->cy].cy,&x,&y);
//		dc.DrawPolygon(6,pList,x,y);
//		count++;
//	}
//
//	if (m_moved < m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF()){
//		dc.SetPen(wxPen(yellow));
//		dc.SetBrush(wxBrush(yellow,wxCROSSDIAG_HATCH));
//	}
//	count=m_moved+1;
//	itr = m_rightHexes.begin();
//	for (itr = m_rightHexes.begin(); itr< m_rightHexes.end(); itr++){
//		if (count == (m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF())){
//			dc.SetPen(wxPen(orange));
//			dc.SetBrush(wxBrush(orange,wxCROSSDIAG_HATCH));
//		}
//		wxCoord x,y;
//		CalcScrolledPosition(m_hexData[itr->cx][itr->cy].cx,m_hexData[itr->cx][itr->cy].cy,&x,&y);
//		dc.DrawPolygon(6,pList,x,y);
//		count++;
//	}
}

void FBattleBoard::setInitialRoute(){
	hexData current=m_shipPos;
	FVehicle *ship = m_parent->getShip();
	int speed = ship->getSpeed();
	int ADF = ship->getADF();
	m_movementHexes.clear();
	for (int i=0; i < speed+ADF; i++){
		current=findNextHex(current,ship->getHeading());
		if(current.cx!=-1) {m_movementHexes.push_back(current);}
	}
	m_leftHexes.clear();
	m_rightHexes.clear();
	m_movedHexes.clear();
	m_turnInfo[ship->getID()].curHeading = ship->getHeading();
	m_turnInfo[ship->getID()].waypoints.clear();
	m_turnInfo[ship->getID()].waypoints.push_back(current);
	m_turnInfo[ship->getID()].turns.clear();
	m_turnInfo[ship->getID()].nMoved=1;
}

hexData FBattleBoard::findNextHex(hexData h, int heading){
	for (int i=h.cx-1;i<=h.cx+1;i++){
		for (int j=h.cy-1;j<=h.cy+1;j++){
			hexData tmp;
			tmp.cx=i;
			tmp.cy=j;
			if (i>=0 && j>=0 && m_parent->computeHeading(h,tmp)==heading && computeHexDistance(h.cx,h.cy,i,j)==1){
				return tmp;
			}
		}
	}
	hexData bad;
	bad.cx=-1;
	bad.cy=-1;
	return bad;
}

bool FBattleBoard::findHexInList(std::vector<hexData> list, hexData ref, int &count){
	std::vector<hexData>::iterator itr = list.begin();
	hexData h;
	while (itr != list.end()){
		if ((*itr).cx==ref.cx && (*itr).cy==ref.cy){
			h.cx=ref.cx;
			h.cy=ref.cy;
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
	hexData h;
	h.cx=a;
	h.cy=b;
	bool found=false;
	int turn = 0; // direction turned 1=left, -1=right 0=no turn
	int moved=m_turnInfo[m_parent->getShip()->getID()].nMoved;
	// check straight ahead
	moved=m_turnInfo[m_parent->getShip()->getID()].nMoved;
	found = findHexInList(m_movementHexes,h,moved);
	if (found){
		turn=0;
	} else {
		moved=m_turnInfo[m_parent->getShip()->getID()].nMoved;
		found= findHexInList(m_leftHexes,h,moved);
		if (found){
			turn=1;
		} else {
			moved=m_turnInfo[m_parent->getShip()->getID()].nMoved;
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
		m_turnInfo[m_parent->getShip()->getID()].waypoints.push_back(h);
		if(turn) {
			m_turnInfo[m_parent->getShip()->getID()].turns.push_back(turn);
		}
		std::vector<hexData>::iterator itr;
		// next pick the list we need to work with
		std::vector<hexData> * curList;
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
		m_turnInfo[m_parent->getShip()->getID()].curHeading+=turn;
		if (m_turnInfo[m_parent->getShip()->getID()].curHeading > 6) {m_turnInfo[m_parent->getShip()->getID()].curHeading -= 6;}
		if (m_turnInfo[m_parent->getShip()->getID()].curHeading < 0) {m_turnInfo[m_parent->getShip()->getID()].curHeading += 6;}
		//copy all the new hexes into the moved hexes list
		itr = (*curList).begin();
		for (int i=0; i< moved; i++){
			m_movedHexes.push_back((*itr));
			itr++;
		}
		itr--;
//		itr=m_movementHexes.begin()+m_moved-1;
		m_moved+=moved;
		int left = m_turnInfo[m_parent->getShip()->getID()].curHeading+1;
		if (left >= 6 ) { left -= 6; }
		int right = m_turnInfo[m_parent->getShip()->getID()].curHeading-1;
		if (right < 0 ) { right += 6; }
		hexData leftHex = (*itr);
		hexData rightHex = (*itr);
		hexData forwardHex = (*itr);
		m_rightHexes.clear();
		m_leftHexes.clear();
		m_movementHexes.clear();
		for (int i=m_moved; i< m_parent->getShip()->getSpeed()+m_parent->getShip()->getADF(); i++){
			forwardHex = findNextHex(forwardHex,m_turnInfo[m_parent->getShip()->getID()].curHeading);
			if(forwardHex.cx!=-1) {m_movementHexes.push_back(forwardHex);}
			if(m_turnInfo[m_parent->getShip()->getID()].turns.size()<m_parent->getShip()->getMR()){
				// we only update these if the MR is not expended
				leftHex=findNextHex(leftHex,left);
				if(leftHex.cx!=-1) {m_leftHexes.push_back(leftHex);}
				rightHex=findNextHex(rightHex,right);
				if(rightHex.cx!=-1) {m_rightHexes.push_back(rightHex);}
			}
		}
		Refresh();
	}
}

void FBattleBoard::resetMoveData(){
	m_turnInfo.clear();
	VehicleList ships = m_parent->getShipList();
	for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
		turnData d;
		d.hasMoved=false;
		d.curHeading=(*itr)->getHeading();
		m_turnInfo[(*itr)->getID()]=d;
	}
}

void FBattleBoard::drawRouteHexes(wxDC &dc, std::vector<hexData> list, int count){
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

	wxColor yellow(wxT("#FFFF00"));
	wxColor orange(wxT("#FFA900"));

	if (m_moved < m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF()){
		dc.SetPen(wxPen(yellow));
		dc.SetBrush(wxBrush(yellow,wxCROSSDIAG_HATCH));
	}
	std::vector<hexData>::iterator itr;
	for (itr = list.begin(); itr< list.end(); itr++){
		if (count == (m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF())){
			dc.SetPen(wxPen(orange));
			dc.SetBrush(wxBrush(orange,wxCROSSDIAG_HATCH));
		}
		wxCoord x,y;
		CalcScrolledPosition(m_hexData[itr->cx][itr->cy].cx,m_hexData[itr->cx][itr->cy].cy,&x,&y);
		dc.DrawPolygon(6,pList,x,y);
		count++;
	}
}

}
