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
				selectVessel(event);
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
	dc.SetPen(wxPen(yellow));
	dc.SetBrush(wxBrush(yellow,wxCROSSDIAG_HATCH));

	std::vector<hexData>::iterator itr = m_movementHexes.begin();
	for (itr = m_movementHexes.begin(); itr< m_movementHexes.end(); itr++){
		wxCoord x,y;
		CalcScrolledPosition(m_hexData[itr->cx][itr->cy].cx,m_hexData[itr->cx][itr->cy].cy,&x,&y);
		dc.DrawPolygon(6,pList,x,y);
	}
	dc.SetPen(wxPen(orange));
	dc.SetBrush(wxBrush(orange,wxCROSSDIAG_HATCH));
	for (itr = m_ADFHexes.begin(); itr< m_ADFHexes.end(); itr++){
		wxCoord x,y;
		CalcScrolledPosition(m_hexData[itr->cx][itr->cy].cx,m_hexData[itr->cx][itr->cy].cy,&x,&y);
		dc.DrawPolygon(6,pList,x,y);
	}
}

void FBattleBoard::setInitialRoute(){
	hexData current=m_shipPos;
	int speed = m_parent->getShip()->getSpeed();
	int ADF = m_parent->getShip()->getADF();
	for (int i=0; i < speed-ADF-1; i++){
		current=findNextHex(current,m_parent->getShip()->getHeading());
		m_movementHexes.push_back(current);
	}
	for (int i=0; i<2*ADF+1; i++){
		current=findNextHex(current,m_parent->getShip()->getHeading());
		m_ADFHexes.push_back(current);

	}
}

hexData FBattleBoard::findNextHex(hexData h, int heading){
	for (int i=h.cx-1;i<=h.cx+1;i++){
		for (int j=h.cy-1;j<=h.cy+1;j++){
			hexData tmp;
			tmp.cx=i;
			tmp.cy=j;
			if (m_parent->computeHeading(h,tmp)==heading && computeHexDistance(h.cx,h.cy,i,j)==1){
				return tmp;
			}
		}
	}
	hexData bad;
	bad.cx=0;
	bad.cy=0;
	return bad;
}

}
