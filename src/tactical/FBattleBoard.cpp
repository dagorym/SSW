/**
 * @file FBattleBoard.cpp
 * @brief Implementation file for BattleBoard class
 * @author Tom Stephens
 * @date Created:  Jul 11, 2008
 *
 */

#include "Frontier.h"
#include "tactical/FBattleScreen.h"
#include "gui/WXIconCache.h"
#include "core/FGameConfig.h"
#include <wx/wx.h>
#include <cmath>

namespace Frontier {

FBattleBoard::FBattleBoard(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)
: wxScrolledWindow( parent, id, pos, size, style, name ) {
m_parent = (FBattleScreen *)parent;

this->SetSizeHints( wxDefaultSize, wxDefaultSize );
m_nCol = 55;
m_nRow = 39;
setConstants(1.0);
computeCenters();
FGameConfig &gc = FGameConfig::create();
m_maskingScreenIcon = new wxImage(gc.getBasePath()+"icons/MaskingScreen.png");

SetScrollRate( (int)(2*m_d), (int)(3*m_a) );
SetVirtualSize(m_width,m_height);
wxColour black(wxT("#000000"));
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

const FPoint & planetPos = m_parent->getPlanetPos();
const int planetChoice = m_parent->getPlanetChoice();
if (planetPos.getX() >= 0 && planetPos.getY() >= 0
&& planetChoice >= 0
&& (unsigned int)planetChoice < m_planetImages.size()) {
drawCenteredOnHex(m_planetImages[planetChoice], planetPos);
}

drawShips();
if (m_parent->getState() == BS_Battle) {
drawRoute(dc);
}
if (m_parent->getState() == BS_PlaceMines) {
drawMinedHexes(dc);
}
if (m_parent->getWeapon() != NULL) {
drawWeaponRange(dc);
drawTarget(dc);
if (m_parent->getWeapon()->getType() == FWeapon::M) {
drawMinedHexes(dc);
}
}
}

void FBattleBoard::onPaint(wxPaintEvent & event){
wxPaintDC dc(this);
draw(dc);
}

void FBattleBoard::drawGrid(wxDC &dc){
wxColour white(wxT("#FFFFFF"));
wxColour black(wxT("#000000"));
dc.SetBackground(wxBrush(black));
dc.Clear();
int w,h;
dc.GetSize(&w,&h);
dc.SetPen(*wxBLACK);
dc.SetBrush(*wxBLACK_BRUSH);
dc.DrawRectangle(0,0,w,h);
int x,y;
dc.SetPen(wxPen(white));
SetVirtualSize(m_width,m_height);
CalcUnscrolledPosition(0,0,&x,&y);
double nx,ny,cx,cy;
double xoff = m_trim - x;
double yoff = m_trim - y;
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
Update();
}

void FBattleBoard::onLeftUp(wxMouseEvent & event) {
wxCoord x,y;
event.GetPosition(&x,&y);
int a, b;
if (getHex(x,y,a,b)){
m_parent->handleHexClick(FPoint(a,b));
}
event.Skip();
}

void FBattleBoard::computeCenters(){
for (int j=0; j<m_nRow; j++){
int y = (int)(m_trim + 2 * m_a + (3 * m_a * j));
for (int i=0; i<m_nCol; i++) {
int x = (int)(m_trim + m_d + (2 * m_d * i) + m_d * (j%2));
m_hexCenters[i][j].setPoint(x,y);
}
}
}

bool FBattleBoard::getHex(int x, int y, int &a, int &b){
int rx, ry;
CalcUnscrolledPosition(x,y,&rx,&ry);
int vIndex = (ry-m_trim)/(int)(3*m_a);
int hIndex = (rx-m_trim)/(int)(2*m_d);
if (vIndex > m_nRow) {vIndex = m_nRow;}
if (hIndex > m_nCol) {hIndex = m_nCol;}
a=hIndex;
b=vIndex;
double min = 99999999.0;
for (int i=hIndex; i>= hIndex-1 && i>=0; i--){
for (int j=vIndex; j>=vIndex-1 && j>=0; j--){
double dx = m_hexCenters[i][j].getX() - rx;
double dy = m_hexCenters[i][j].getY() - ry;
double d = sqrt(dx*dx+dy*dy);
if (d<min) {
a = i;
b = j;
min = d;
}
}
}
if (min > sqrt(m_a*m_a+m_d*m_d)){
a=-1;
b=-1;
return false;
}
return true;
}

void FBattleBoard::drawCenteredOnHex(wxImage img, FPoint p, int rot){
wxClientDC dc(this);
img = img.Scale(m_size,m_size);
if (rot) {
if (!(img.HasAlpha())){img.InitAlpha();}
img = img.Rotate(rot*acos(-1.)/3.,wxPoint(m_size/2,m_size/2));
}
wxBitmap b (img);
wxCoord x = m_hexCenters[p.getX()][p.getY()].getX() - img.GetWidth()/2;
wxCoord y = m_hexCenters[p.getX()][p.getY()].getY() - img.GetHeight()/2;
wxCoord xx,yy;
CalcScrolledPosition(x,y,&xx,&yy);
dc.DrawBitmap(b,xx,yy);
}

void FBattleBoard::setScale(double factor) {
setConstants(m_scale*factor);
computeCenters();
Refresh();
}

void FBattleBoard::drawShips(){
for (int i=0; i<m_nCol; i++){
for (int j=0; j<m_nRow; j++){
const FPoint hex(i,j);
const VehicleList & ships = m_parent->getHexOccupants(hex);
for (VehicleList::const_iterator itr = ships.begin(); itr != ships.end(); ++itr){
const wxImage *icon;
if ((*itr)->getCurrentDefense()->getType()!=FDefense::MS){
icon = &WXIconCache::instance().get((*itr)->getIconName());
} else {
icon = m_maskingScreenIcon;
}
drawCenteredOnHex(*icon,hex,(*itr)->getHeading());
}
}
}
}

void FBattleBoard::onMotion(wxMouseEvent & event){
switch (m_parent->getState()){
case BS_SetupDefendFleet:
case BS_SetupAttackFleet:
if (m_parent->getControlState() && m_parent->hasShipPlacementPendingRotation()) {
wxCoord x,y;
event.GetPosition(&x,&y);
int a, b;
if (getHex(x,y,a,b) && m_parent->setShipPlacementHeadingByHex(FPoint(a,b))) {
Refresh();
}
}
break;
default:
break;
}
}

void FBattleBoard::drawRoute(wxDC &dc){
VehicleList ships = m_parent->getShipList(m_parent->getMovingPlayerID());
const std::map<unsigned int, FTacticalTurnData> & turnInfo = m_parent->getTurnInfo();
for (VehicleList::iterator itr=ships.begin(); itr<ships.end();itr++){
std::map<unsigned int, FTacticalTurnData>::const_iterator tItr = turnInfo.find((*itr)->getID());
if (tItr == turnInfo.end()) {
continue;
}
bool current=false;
if (m_parent->getShip()!=NULL && m_parent->getShip()->getID()==(*itr)->getID()){
current = true;
}
PointList movedPath = tItr->second.path.getFullPath();
if(movedPath.size()>1){
drawMovedHexes(dc, movedPath, current);
}
}

if (m_parent->getPhase() == PH_MOVE && m_parent->getShip() != NULL
&& m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()) {
const int movedCount = m_parent->getTurnInfo().find(m_parent->getShip()->getID()) != m_parent->getTurnInfo().end()
? m_parent->getTurnInfo().find(m_parent->getShip()->getID())->second.nMoved
: 0;
drawRouteHexes(dc, m_parent->getMovementHexes(), movedCount+1);
drawRouteHexes(dc, m_parent->getLeftTurnHexes(), movedCount+1);
drawRouteHexes(dc, m_parent->getRightTurnHexes(), movedCount+1);
}
}

void FBattleBoard::drawRouteHexes(wxDC &dc, const std::vector<FPoint> & list, int count){
wxColor yellow(wxT("#FFFF00"));
wxColor orange(wxT("#FFA900"));
wxColor c = yellow;

if (m_parent->getShip() == NULL) {
return;
}

if (count < m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF()){
c=yellow;
}
for (std::vector<FPoint>::const_iterator itr = list.begin(); itr != list.end(); ++itr){
if (count >= (m_parent->getShip()->getSpeed()-m_parent->getShip()->getADF())){
c=orange;
}
drawShadedHex(dc,c,m_hexCenters[itr->getX()][itr->getY()]);
count++;
}
}

void FBattleBoard::drawMovedHexes(wxDC &dc, PointList list, bool current){
if (list.size() == 0) {
return;
}
wxColor red(wxT("#FF0000"));
wxColor gray(wxT("#AAAAAA"));
if (current){
dc.SetPen(wxPen(red,3));
} else {
dc.SetPen(wxPen(gray,2));
}
wxCoord lx, ly;
CalcScrolledPosition(m_hexCenters[list[0].getX()][list[0].getY()].getX(),m_hexCenters[list[0].getX()][list[0].getY()].getY(),&lx,&ly);
for (PointList::iterator itr = list.begin(); itr< list.end(); itr++){
wxCoord x,y;
CalcScrolledPosition(m_hexCenters[itr->getX()][itr->getY()].getX(),m_hexCenters[itr->getX()][itr->getY()].getY(),&x,&y);
dc.DrawLine(lx,ly,x,y);
lx=x;
ly=y;
}
}

void FBattleBoard::drawWeaponRange(wxDC &dc){
if (m_parent->getPhase()!=PH_ATTACK_FIRE && m_parent->getPhase()!=PH_DEFENSE_FIRE ){
return;
}
wxColour red(wxT("#FF0000"));
for (PointSet::const_iterator itr = m_parent->getTargetHexes().begin(); itr != m_parent->getTargetHexes().end(); ++itr){
drawShadedHex(dc,red,m_hexCenters[(*itr).getX()][(*itr).getY()]);
}
wxColour blue(wxT("#0000FF"));
for (PointSet::const_iterator itr = m_parent->getHeadOnHexes().begin(); itr != m_parent->getHeadOnHexes().end(); ++itr){
drawShadedHex(dc,blue,m_hexCenters[(*itr).getX()][(*itr).getY()]);
}
}

void FBattleBoard::drawShadedHex(wxDC& dc, wxColour c, FPoint p){
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
dc.SetBrush(wxBrush(c, wxBRUSHSTYLE_CROSSDIAG_HATCH));
wxCoord x,y;
CalcScrolledPosition(p.getX(),p.getY(),&x,&y);
dc.DrawPolygon(6,pList,x,y);
}

void FBattleBoard::drawTarget(wxDC &dc){
if (m_parent->getWeapon() == NULL) {
return;
}
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

void FBattleBoard::drawMinedHexes(wxDC &dc){
wxColour green(wxT("#00FF00"));
for (PointSet::const_iterator itr = m_parent->getMinedHexes().begin(); itr != m_parent->getMinedHexes().end(); ++itr){
drawShadedHex(dc,green,m_hexCenters[itr->getX()][itr->getY()]);
}
}

}
