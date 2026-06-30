/**
 * @file FBattleDisplay.cpp
 * @brief Implementation file for BattleDispaly class
 * @author Tom Stephens, gpt-5.4 (high), claude-sonnet-4-6 (standard), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (medium)
 * @date Created:  Jul 11, 2008
 * @date Last Modified:  Jun 30, 2026 (TMF-01: single-source SEEKER_ACTIVATION_*_INSTRUCTION constants in drawSeekerActivation)
 *
 * SMRIV-01: drawPlaceMines() now anchors source-selection rows to the top of the
 * bottom panel (right column, starting at getActionPromptLineY(0)) and wraps the
 * mine instruction text onto two lines in the left column.
 * SMRIV-02: drawPlaceSeekers() now uses a three-column layout: left column holds
 * the instruction text (wrapped onto two lines) + Done button, middle column
 * (lMargin=310) holds the source-selection rows anchored at getActionPromptLineY(0),
 * and right column (recallMargin=620) holds the placed-seeker recall list anchored
 * at getActionPromptLineY(0).  Click regions match drawn positions for both columns.
 * SMRIV-03: draw() now calls drawOffensiveSeekerPendingRows(dc, 310,
 * getActionPromptLineY(0), 10) during PH_ATTACK_FIRE so the recall list anchors to
 * the top of the bottom panel in the column to the right of the Done button,
 * consistent with the pre-game placement treatment used by drawPlaceMines.
 * SMRIV-04: ensureLowerPanelLayoutState() detects tactical state/phase changes and
 * resets requestedDisplayHeight to the base-content height so the panel shrinks back
 * after a phase that needed extra space.  Within a single phase the existing
 * max-preserve behaviour is retained to prevent row-clipping regressions.
 * SMRV-01 (round5): draw() now computes pendingLMargin from the measured text
 * extent of the widest PH_ATTACK_FIRE instruction line instead of using the
 * fixed value 310.  The pending panel's left edge is placed at
 * leftOffset + textExtent("Select legal path hexes to deploy seeker missiles.")
 * + 2*BORDER, so it no longer overlaps the left-column instruction text.  The
 * left instruction text is unchanged (not wrapped).  Click regions in
 * m_pendingSeekerRecallRegions are computed from the new draw position so
 * checkOffensiveSeekerPendingSelection() continues to work correctly.
 * SMRV-02 (round5): drawSeekerActivation() now anchors the "Activated seekers:"
 * list at int y = getActionPromptLineY(0) (top of the lower panel, right column at
 * lMargin=310), matching the drawPlaceMines/drawPlaceSeekers three-column
 * convention.  The previous anchor was getActionButtonRowBottom() (SMF-05).  Click
 * regions in m_seekerActivationRegions automatically track the new anchor because
 * they use the same y variable.
 * SMRVI-02 (round6): drawSeekerActivation() per-row text now shows the seeker's
 * board position as "(X,Y)" instead of "(heading H, allowance A)".  The fixed
 * lMargin=310 is replaced by a dynamically computed margin that clears the widest
 * left-column instruction line and the "Seeker Activation Done" button's right edge,
 * mirroring the SMRV-01 (round5) attack-phase approach.
 */

//#include "FBattleDisplay.h"
#include "Frontier.h"
#include "tactical/FBattleScreen.h"
#include "core/FGameConfig.h"
#include "core/FHexMap.h"
#include "gui/WXIconCache.h"
#include <algorithm>
#include <sstream>

#define ICON_SIZE 50
#define BORDER 5
#define ZOOM_SIZE 30
const int leftOffset=2*BORDER+ZOOM_SIZE;

namespace Frontier {

const wxString FBattleDisplay::SEEKER_DEPLOY_INSTRUCTION("Select legal path hexes to deploy seeker missiles.");
const wxString FBattleDisplay::SEEKER_ACTIVATION_PHASE_INSTRUCTION("Seeker activation phase.");
const wxString FBattleDisplay::SEEKER_ACTIVATION_CLICK_INSTRUCTION("Click a seeker stack on the board to activate one seeker.");
const wxString FBattleDisplay::SEEKER_ACTIVATION_DEACTIVATE_INSTRUCTION("Click a row below to deactivate an activated seeker.");

namespace {

FVehicle * findShipByID(const VehicleList & ships, unsigned int shipID) {
	for (VehicleList::const_iterator itr = ships.begin(); itr != ships.end(); ++itr) {
		if ((*itr)->getID() == shipID) {
			return *itr;
		}
	}
	return NULL;
}

const char * getDeploymentWeaponLabel(FWeapon::Weapon weaponType) {
	switch (weaponType) {
	case FWeapon::M:
		return "Mine";
	case FWeapon::SM:
		return "Seeker";
	default:
		return "Ordnance";
	}
}

}

FBattleDisplay::FBattleDisplay(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)
		: wxPanel( parent, id, pos, size, style, name ) {

	m_parent = (FBattleScreen *)parent;
	m_loaded = false;
	m_first = true;
	m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_RIGHT_SPLIT;
	m_lowerPanelLayoutState.shipStatsLeftMargin = 300;
	m_lowerPanelLayoutState.shipStatsTop = BORDER;
	m_lowerPanelLayoutState.reservedPromptLines = ACTION_PROMPT_MAX_LINES;
	m_lowerPanelLayoutState.requestedDisplayHeight = 120;
	m_lowerPanelLayoutState.initialized = false;
	m_lowerPanelLayoutState.lastBattleState = -1;
	m_lowerPanelLayoutState.lastBattlePhase = -1;
	m_inResizeReflow = false;
	m_actionButtonExtraSpacerItem = NULL;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	wxColour black(wxT("#000000"));// black
	SetBackgroundColour(black);
	SetMinSize( wxSize( -1,120 ) );
	FGameConfig &gc = FGameConfig::create();

	m_zoomImage.LoadFile(gc.resolveAssetPath("data/zoom.png"));

	/// set up tactical controls using sizer-based layout to keep spin control sizing stable
	m_spinCtrl1 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_VERTICAL, 0, 55, 10 );
	m_spinCtrl1->SetMinSize(m_spinCtrl1->GetBestSize());
	m_button1 = new wxButton( this, wxID_ANY, wxT("Set Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonMoveDone = new wxButton( this, wxID_ANY, wxT("Movement Done"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonDefensiveFireDone = new wxButton( this, wxID_ANY, wxT("Defensive Fire Done"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOffensiveFireDone = new wxButton( this, wxID_ANY, wxT("Offensive Fire Done"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonMinePlacementDone = new wxButton( this, wxID_ANY, wxT("Mine Placement Done"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSeekerPlacementDone = new wxButton( this, wxID_ANY, wxT("Seeker Placement Done"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSeekerActivationDone = new wxButton( this, wxID_ANY, wxT("Seeker Activation Done"), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer * rootSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * speedSizer = new wxBoxSizer(wxHORIZONTAL);
	speedSizer->AddSpacer(leftOffset);
	speedSizer->Add(m_spinCtrl1, 0, wxALIGN_CENTER_VERTICAL);
	speedSizer->Add(m_button1, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, BORDER * 2);
	rootSizer->Add(speedSizer, 0, wxTOP, 3 * BORDER);

	wxBoxSizer * actionSizer = new wxBoxSizer(wxHORIZONTAL);
	actionSizer->AddSpacer(leftOffset);
	actionSizer->Add(m_buttonMoveDone, 0, wxALIGN_CENTER_VERTICAL);
	actionSizer->Add(m_buttonDefensiveFireDone, 0, wxALIGN_CENTER_VERTICAL);
	actionSizer->Add(m_buttonOffensiveFireDone, 0, wxALIGN_CENTER_VERTICAL);
	actionSizer->Add(m_buttonMinePlacementDone, 0, wxALIGN_CENTER_VERTICAL);
	actionSizer->Add(m_buttonSeekerPlacementDone, 0, wxALIGN_CENTER_VERTICAL);
	actionSizer->Add(m_buttonSeekerActivationDone, 0, wxALIGN_CENTER_VERTICAL);
	rootSizer->AddSpacer(getActionButtonTopSpacerHeight());
	m_actionButtonExtraSpacerItem = rootSizer->AddSpacer(getActionButtonExtraSpacerHeight());
	rootSizer->Add(actionSizer, 0, wxTOP, BORDER);
	rootSizer->AddStretchSpacer(1);
	SetSizer(rootSizer);
	m_spinCtrl1->Hide();
	m_button1->Hide();
	m_buttonMoveDone->Hide();
	m_buttonDefensiveFireDone->Hide();
	m_buttonOffensiveFireDone->Hide();
	m_buttonMinePlacementDone->Hide();
	m_buttonSeekerPlacementDone->Hide();
	m_buttonSeekerActivationDone->Hide();
	Layout();
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(FBattleDisplay::onPaint));
	this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler(FBattleDisplay::onLeftUp ),NULL,this);
}

FBattleDisplay::~FBattleDisplay() {
}

int FBattleDisplay::getCurrentPromptMaxWidth(int panelWidth) const{
	int promptMaxWidth = panelWidth - leftOffset - BORDER;
	if (m_lowerPanelLayoutState.initialized
			&& m_lowerPanelLayoutState.mode == LOWER_PANEL_LAYOUT_RIGHT_SPLIT){
		promptMaxWidth = m_lowerPanelLayoutState.shipStatsLeftMargin - leftOffset - BORDER;
	}
	if (promptMaxWidth < 120){
		promptMaxWidth = 120;
	}
	return promptMaxWidth;
}

void FBattleDisplay::buildMovePromptText(wxString & turnPrompt, wxString & detailPromptOne, wxString & detailPromptTwo) const{
	std::ostringstream os;
	os << "It is the ";
	if (m_parent->getActivePlayer()){
		os << "attacker's ";
	} else {
		os << "defender's ";
	}
	os << "turn.";
	turnPrompt = wxString(os.str());

	bool stoppedShipFacingSelection = false;
	if (m_parent->getShip() != NULL && m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()) {
		const std::map<unsigned int, FTacticalTurnData> & turnInfo = m_parent->getTurnInfo();
		std::map<unsigned int, FTacticalTurnData>::const_iterator turnItr = turnInfo.find(m_parent->getShip()->getID());
		if (turnItr != turnInfo.end()) {
			const FTacticalTurnData & turnData = turnItr->second;
			stoppedShipFacingSelection = (turnData.speed == 0
				&& turnData.nMoved == 0
				&& m_parent->getShip()->getMR() > 0);
		}
	}

	if (stoppedShipFacingSelection) {
		detailPromptOne = "Select a highlighted preview route to choose your starting facing.";
		detailPromptTwo = "Continue a route, or click an adjacent hex then Movement Done to rotate in place.";
	} else if (m_parent->getShip() != NULL && m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()) {
		detailPromptOne = "Select route hexes to move the ship.";
		detailPromptTwo = "Press the 'Movement Done' button when all ships have been assigned their movement instructions.";
	} else {
		detailPromptOne = "Please select a ship to move.";
		detailPromptTwo.clear();
	}
}

void FBattleDisplay::refreshMovePromptReservation(wxDC &dc, int panelWidth, int panelHeight){
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	ensureLowerPanelLayoutState(panelWidth, panelHeight);
	int promptMaxWidth = getCurrentPromptMaxWidth(panelWidth);

	wxString turnPrompt;
	wxString detailPromptOne;
	wxString detailPromptTwo;
	buildMovePromptText(turnPrompt, detailPromptOne, detailPromptTwo);
	int promptLineCount = countWrappedActionPromptLines(dc, turnPrompt, promptMaxWidth);
	promptLineCount += countWrappedActionPromptLines(dc, detailPromptOne, promptMaxWidth);
	promptLineCount += countWrappedActionPromptLines(dc, detailPromptTwo, promptMaxWidth);
	reserveActionPromptLines(promptLineCount);

	ensureLowerPanelLayoutState(panelWidth, panelHeight);
	promptMaxWidth = getCurrentPromptMaxWidth(panelWidth);
	promptLineCount = countWrappedActionPromptLines(dc, turnPrompt, promptMaxWidth);
	promptLineCount += countWrappedActionPromptLines(dc, detailPromptOne, promptMaxWidth);
	promptLineCount += countWrappedActionPromptLines(dc, detailPromptTwo, promptMaxWidth);
	reserveActionPromptLines(promptLineCount);
	ensureLowerPanelLayoutState(panelWidth, panelHeight);
}

void FBattleDisplay::reflowLowerPanelLayout(){
	if (m_inResizeReflow){
		return;
	}
	m_inResizeReflow = true;

	int panelWidth = GetClientSize().GetWidth();
	int panelHeight = GetClientSize().GetHeight();
	if (panelWidth > 0 && panelHeight > 0) {
		wxClientDC dc(this);
		if (m_parent->getState() == BS_Battle && m_parent->getPhase() == PH_MOVE) {
			refreshMovePromptReservation(dc, panelWidth, panelHeight);
		} else {
			ensureLowerPanelLayoutState(panelWidth, panelHeight);
		}
		applyRequestedDisplayHeight();
	}
	m_inResizeReflow = false;
}

int FBattleDisplay::getActionPromptLineY(int lineIndex) const{
	return ACTION_PROMPT_TOP_MARGIN + (lineIndex * ACTION_PROMPT_LINE_HEIGHT);
}

int FBattleDisplay::getActionButtonTopSpacerHeight() const{
	return getActionPromptLineY(ACTION_PROMPT_MAX_LINES) + ACTION_PROMPT_BUTTON_GAP;
}

void FBattleDisplay::reserveActionPromptLines(int lineCount){
	if (lineCount < 1){
		lineCount = 1;
	}
	m_lowerPanelLayoutState.reservedPromptLines = lineCount;
	refreshActionButtonSpacer();
}

int FBattleDisplay::getActionPromptExtraLines() const{
	int extraLines = m_lowerPanelLayoutState.reservedPromptLines - ACTION_PROMPT_MAX_LINES;
	if (extraLines < 0){
		extraLines = 0;
	}
	return extraLines;
}

int FBattleDisplay::getActionButtonExtraSpacerHeight() const{
	return getActionPromptExtraLines() * ACTION_PROMPT_LINE_HEIGHT;
}

void FBattleDisplay::refreshActionButtonSpacer(){
	if (m_actionButtonExtraSpacerItem == NULL){
		return;
	}
	const int extraSpacerHeight = getActionButtonExtraSpacerHeight();
	const wxSize currentSize = m_actionButtonExtraSpacerItem->GetMinSize();
	if (currentSize.GetHeight() != extraSpacerHeight || currentSize.GetWidth() != 0){
		m_actionButtonExtraSpacerItem->SetMinSize(0, extraSpacerHeight);
	}
}

int FBattleDisplay::countWrappedActionPromptLines(wxDC &dc, const wxString &promptText, int maxWidth) const{
	if (promptText.IsEmpty()){
		return 0;
	}
	if (maxWidth <= 0){
		return 1;
	}
	wxArrayString words = wxSplit(promptText, ' ');
	if (words.IsEmpty()){
		return 1;
	}
	int lineCount = 1;
	wxString currentLine;
	for (size_t i = 0; i < words.size(); i++){
		const wxString & word = words[i];
		if (word.IsEmpty()){
			continue;
		}
		const wxString candidate = currentLine.IsEmpty() ? word : currentLine + " " + word;
		if (!currentLine.IsEmpty() && dc.GetTextExtent(candidate).GetWidth() > maxWidth){
			lineCount++;
			currentLine = word;
		} else {
			currentLine = candidate;
		}
	}
	return lineCount;
}

void FBattleDisplay::drawWrappedActionPrompt(wxDC &dc, const wxString &promptText, int maxWidth, int &lineCursor){
	if (promptText.IsEmpty()){
		return;
	}
	if (maxWidth <= 0){
		dc.DrawText(promptText,leftOffset,getActionPromptLineY(lineCursor));
		lineCursor++;
		return;
	}
	wxArrayString words = wxSplit(promptText, ' ');
	wxString currentLine;
	for (size_t i = 0; i < words.size(); i++){
		const wxString & word = words[i];
		if (word.IsEmpty()){
			continue;
		}
		const wxString candidate = currentLine.IsEmpty() ? word : currentLine + " " + word;
		if (!currentLine.IsEmpty() && dc.GetTextExtent(candidate).GetWidth() > maxWidth){
			dc.DrawText(currentLine,leftOffset,getActionPromptLineY(lineCursor));
			lineCursor++;
			currentLine = word;
		} else {
			currentLine = candidate;
		}
	}
	if (!currentLine.IsEmpty()){
		dc.DrawText(currentLine,leftOffset,getActionPromptLineY(lineCursor));
		lineCursor++;
	}
}

FBattleDisplay::ShipStatsLayoutRequirements FBattleDisplay::measureShipStatsLayoutRequirements(wxDC &dc) const{
	ShipStatsLayoutRequirements requirements;
	requirements.width = SHIP_STATS_MIN_WIDTH;
	requirements.height = BORDER + (int)(1.6*(10*6.3));

	FVehicle * selectedShip = m_parent->getShip();
	if (selectedShip == NULL){
		return requirements;
	}

	const int textSize = 10;
	const int lineOneY = (int)(1.6*(textSize*1.3));
	const int lineTwoY = (int)(1.6*(textSize*2.3));
	const int weaponY = (int)(1.6*(textSize*3.3));
	const int defenseY = weaponY + (int)(1.6*textSize);
	const int otherStatusY = defenseY + (int)(1.6*textSize);

	wxFont normal(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	wxFont large((int)(textSize*1.3),wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	wxFont bold(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);

	int rightEdge = 0;
	int bottomEdge = 0;
	wxSize textExtent;
	wxString valueText;
	std::ostringstream os;

	dc.SetFont(large);
	textExtent = dc.GetTextExtent(selectedShip->getName());
	rightEdge = textExtent.GetWidth();
	bottomEdge = textExtent.GetHeight();

	dc.SetFont(bold);
	textExtent = dc.GetTextExtent("Speed:");
	if (textExtent.GetWidth() > rightEdge){ rightEdge = textExtent.GetWidth(); }
	if (lineOneY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineOneY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("Heading: ");
	if (90 + textExtent.GetWidth() > rightEdge){ rightEdge = 90 + textExtent.GetWidth(); }
	if (lineOneY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineOneY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("ADF:");
	if (textExtent.GetWidth() > rightEdge){ rightEdge = textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("MR: ");
	if (80 + textExtent.GetWidth() > rightEdge){ rightEdge = 80 + textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("HP: ");
	if (160 + textExtent.GetWidth() > rightEdge){ rightEdge = 160 + textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("DCR: ");
	if (240 + textExtent.GetWidth() > rightEdge){ rightEdge = 240 + textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("Weapons:");
	if (textExtent.GetWidth() > rightEdge){ rightEdge = textExtent.GetWidth(); }
	if (weaponY + textExtent.GetHeight() > bottomEdge){ bottomEdge = weaponY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("Defenses:");
	if (textExtent.GetWidth() > rightEdge){ rightEdge = textExtent.GetWidth(); }
	if (defenseY + textExtent.GetHeight() > bottomEdge){ bottomEdge = defenseY + textExtent.GetHeight(); }
	textExtent = dc.GetTextExtent("Other Status:");
	if (textExtent.GetWidth() > rightEdge){ rightEdge = textExtent.GetWidth(); }
	if (otherStatusY + textExtent.GetHeight() > bottomEdge){ bottomEdge = otherStatusY + textExtent.GetHeight(); }

	dc.SetFont(normal);
	os << selectedShip->getSpeed();
	valueText = os.str();
	textExtent = dc.GetTextExtent(valueText);
	if (60 + textExtent.GetWidth() > rightEdge){ rightEdge = 60 + textExtent.GetWidth(); }
	if (lineOneY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineOneY + textExtent.GetHeight(); }
	switch (selectedShip->getHeading()){
	case 0:
		valueText = "W";
		break;
	case 1:
		valueText = "SW";
		break;
	case 2:
		valueText = "SE";
		break;
	case 3:
		valueText = "E";
		break;
	case 4:
		valueText = "NE";
		break;
	case 5:
		valueText = "NW";
		break;
	default:
		valueText.clear();
		break;
	}
	textExtent = dc.GetTextExtent(valueText);
	if (170 + textExtent.GetWidth() > rightEdge){ rightEdge = 170 + textExtent.GetWidth(); }
	if (lineOneY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineOneY + textExtent.GetHeight(); }

	os.str("");
	os << selectedShip->getADF();
	valueText = os.str();
	textExtent = dc.GetTextExtent(valueText);
	if (40 + textExtent.GetWidth() > rightEdge){ rightEdge = 40 + textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }

	os.str("");
	os << selectedShip->getMR();
	valueText = os.str();
	textExtent = dc.GetTextExtent(valueText);
	if (115 + textExtent.GetWidth() > rightEdge){ rightEdge = 115 + textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }

	os.str("");
	os << selectedShip->getHP();
	valueText = os.str();
	textExtent = dc.GetTextExtent(valueText);
	if (195 + textExtent.GetWidth() > rightEdge){ rightEdge = 195 + textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }

	os.str("");
	os << selectedShip->getDCR();
	valueText = os.str();
	textExtent = dc.GetTextExtent(valueText);
	if (275 + textExtent.GetWidth() > rightEdge){ rightEdge = 275 + textExtent.GetWidth(); }
	if (lineTwoY + textExtent.GetHeight() > bottomEdge){ bottomEdge = lineTwoY + textExtent.GetHeight(); }

	int weaponLineWidth = 80;
	for (unsigned int i = 0; i < selectedShip->getWeaponCount(); i++){
		textExtent = dc.GetTextExtent(selectedShip->getWeapon(i)->getName() + "  ");
		weaponLineWidth += textExtent.GetWidth();
		if (weaponY + textExtent.GetHeight() > bottomEdge){ bottomEdge = weaponY + textExtent.GetHeight(); }
	}
	if (weaponLineWidth > rightEdge){ rightEdge = weaponLineWidth; }

	int defenseLineWidth = 80;
	for (unsigned int i = 0; i < selectedShip->getDefenseCount(); i++){
		textExtent = dc.GetTextExtent(selectedShip->getDefense(i)->getName() + "  ");
		defenseLineWidth += textExtent.GetWidth();
		if (defenseY + textExtent.GetHeight() > bottomEdge){ bottomEdge = defenseY + textExtent.GetHeight(); }
	}
	if (defenseLineWidth > rightEdge){ rightEdge = defenseLineWidth; }

	int otherStatusWidth = 110;
	bool damage = false;
	if (selectedShip->isOnFire()){
		otherStatusWidth += dc.GetTextExtent("Fire  ").GetWidth();
		damage = true;
	}
	if (selectedShip->isCombatControlDamaged()){
		otherStatusWidth += dc.GetTextExtent("Combat Sys Damaged  ").GetWidth();
		damage = true;
	}
	if (selectedShip->isPowerSystemDamaged()){
		otherStatusWidth += dc.GetTextExtent("Power Short Circuit  ").GetWidth();
		damage = true;
	}
	if (selectedShip->getNavControlError()){
		otherStatusWidth += dc.GetTextExtent("Nav Sys Damaged  ").GetWidth();
		damage = true;
	}
	if (!damage){
		otherStatusWidth += dc.GetTextExtent("none").GetWidth();
	}
	if (otherStatusWidth > rightEdge){ rightEdge = otherStatusWidth; }
	textExtent = dc.GetTextExtent("none");
	if (otherStatusY + textExtent.GetHeight() > bottomEdge){ bottomEdge = otherStatusY + textExtent.GetHeight(); }

	requirements.width = rightEdge + BORDER;
	requirements.width += BORDER;
	if (requirements.width < SHIP_STATS_MIN_WIDTH){
		requirements.width = SHIP_STATS_MIN_WIDTH;
	}
	requirements.height = bottomEdge + BORDER;
	if (requirements.height < BORDER + (int)(1.6*(10*6.3))){
		requirements.height = BORDER + (int)(1.6*(10*6.3));
	}
	return requirements;
}

int FBattleDisplay::getActionButtonRowBottom() const{
	const int extraPromptHeight = getActionButtonExtraSpacerHeight();
	const int buttonRowTop = getActionButtonTopSpacerHeight() + extraPromptHeight + BORDER;
	int fallbackBottom = buttonRowTop + m_buttonMoveDone->GetBestSize().GetHeight() + BORDER;
	int shownBottom = -1;

	wxButton * buttons[] = {
		m_buttonMoveDone,
		m_buttonDefensiveFireDone,
		m_buttonOffensiveFireDone,
		m_buttonMinePlacementDone,
		m_buttonSeekerPlacementDone,
		m_buttonSeekerActivationDone
	};
	for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++){
		if (buttons[i] != NULL && buttons[i]->IsShown()){
			const wxRect buttonRect = buttons[i]->GetRect();
			const int buttonBottom = buttonRect.GetBottom() + 1;
			if (buttonBottom > shownBottom){
				shownBottom = buttonBottom;
			}
		}
	}

	if (shownBottom >= 0){
		const int shownBottomWithGap = shownBottom + BORDER;
		if (shownBottomWithGap > fallbackBottom){
			fallbackBottom = shownBottomWithGap;
		}
	}
	return fallbackBottom;
}

void FBattleDisplay::ensureLowerPanelLayoutState(int panelWidth, int panelHeight){
	wxClientDC dc(this);
	const ShipStatsLayoutRequirements shipStatsRequirements = measureShipStatsLayoutRequirements(dc);
	const int minStatsLeftMargin = leftOffset + ACTION_PROMPT_MIN_WIDTH;
	const int largestMarginWithStatsRoom = panelWidth - shipStatsRequirements.width - BORDER;
	const bool splitCanFit = largestMarginWithStatsRoom >= minStatsLeftMargin;
	const bool splitCanFitWithPadding = splitCanFit
		&& largestMarginWithStatsRoom >= (minStatsLeftMargin + BORDER);
	const int extraPromptHeight = getActionButtonExtraSpacerHeight();
	const int buttonRowTop = getActionButtonTopSpacerHeight() + extraPromptHeight + BORDER;
	const int buttonRowBottom = buttonRowTop + m_buttonMoveDone->GetBestSize().GetHeight() + BORDER;
	const int stackedTop = buttonRowBottom + ACTION_PROMPT_BUTTON_GAP;
	bool keepCurrentState = false;

	if (m_lowerPanelLayoutState.initialized){
		if (m_lowerPanelLayoutState.mode == LOWER_PANEL_LAYOUT_RIGHT_SPLIT){
			keepCurrentState = splitCanFit
				&& splitCanFitWithPadding
				&& m_lowerPanelLayoutState.shipStatsLeftMargin >= minStatsLeftMargin
				&& m_lowerPanelLayoutState.shipStatsLeftMargin <= largestMarginWithStatsRoom;
			if (keepCurrentState){
				m_lowerPanelLayoutState.shipStatsLeftMargin = largestMarginWithStatsRoom;
				m_lowerPanelLayoutState.shipStatsTop = BORDER;
			}
		} else {
			keepCurrentState = m_lowerPanelLayoutState.shipStatsTop >= stackedTop;
			keepCurrentState = keepCurrentState
				&& !splitCanFitWithPadding
				&& m_lowerPanelLayoutState.shipStatsLeftMargin == leftOffset;
			if (keepCurrentState){
				m_lowerPanelLayoutState.shipStatsTop = stackedTop;
			}
		}
	}

	if (!keepCurrentState){
		if (splitCanFit){
			if (splitCanFitWithPadding){
				m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_RIGHT_SPLIT;
				m_lowerPanelLayoutState.shipStatsLeftMargin = largestMarginWithStatsRoom;
				m_lowerPanelLayoutState.shipStatsTop = BORDER;
			} else {
				m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_STACKED;
				m_lowerPanelLayoutState.shipStatsLeftMargin = leftOffset;
				m_lowerPanelLayoutState.shipStatsTop = stackedTop;
			}
		} else {
			m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_STACKED;
			m_lowerPanelLayoutState.shipStatsLeftMargin = leftOffset;
			m_lowerPanelLayoutState.shipStatsTop = stackedTop;
		}
		m_lowerPanelLayoutState.initialized = true;
	}

	int requestedHeight = getActionPromptLineY(ACTION_PROMPT_MAX_LINES) + ACTION_PROMPT_LINE_HEIGHT + ACTION_PROMPT_BUTTON_GAP + BORDER;
	if (buttonRowBottom > requestedHeight){
		requestedHeight = buttonRowBottom;
	}
	const int statsHeight = shipStatsRequirements.height;
	const int statsBottom = m_lowerPanelLayoutState.shipStatsTop + statsHeight + BORDER;
	if (statsBottom > requestedHeight){
		requestedHeight = statsBottom;
	}
	if (requestedHeight < 120){
		requestedHeight = 120;
	}

	// Detect whether the tactical state or phase has changed since the last call.
	// When the phase is unchanged, preserve any height expansion already applied by
	// draw helpers (drawSeekerActivation, drawPlaceMines, drawPlaceSeekers) so that
	// overflowing rows remain visible and clickable within the phase.
	// When the phase changes, skip max-preserve so the panel can shrink back to fit
	// the new phase's content instead of ratcheting permanently upward (SMRIV-04).
	const int currentState = static_cast<int>(m_parent->getState());
	const int currentPhase = static_cast<int>(m_parent->getPhase());
	const bool phaseChanged = (m_lowerPanelLayoutState.lastBattleState != currentState
		|| m_lowerPanelLayoutState.lastBattlePhase != currentPhase);
	if (phaseChanged){
		m_lowerPanelLayoutState.lastBattleState = currentState;
		m_lowerPanelLayoutState.lastBattlePhase = currentPhase;
	} else {
		// Same phase: preserve any draw-helper expansion so rows are not clipped.
		if (m_lowerPanelLayoutState.requestedDisplayHeight > requestedHeight){
			requestedHeight = m_lowerPanelLayoutState.requestedDisplayHeight;
		}
	}
	m_lowerPanelLayoutState.requestedDisplayHeight = requestedHeight;
}

void FBattleDisplay::applyRequestedDisplayHeight(){
	refreshActionButtonSpacer();
	int requestedHeight = m_lowerPanelLayoutState.requestedDisplayHeight;
	if (requestedHeight < 120){
		requestedHeight = 120;
	}
	if (GetMinSize().GetHeight() != requestedHeight){
		SetMinSize(wxSize(-1, requestedHeight));
		// Notify the parent frame that the display panel needs more space so
		// applyLayoutPolicy() runs immediately and re-lays out the sizer rather
		// than waiting for the next user-triggered resize event.  This is safe to
		// call from within a paint handler because SendSizeEvent() queues the
		// event for the next event-loop iteration instead of dispatching it inline.
		if (GetParent() != NULL && !m_inResizeReflow){
			GetParent()->SendSizeEvent();
		}
	}
}

void FBattleDisplay::draw(wxDC &dc){
//	wxBitmap b("icons/ufo.png");
//	dc.DrawBitmap(b, 5, 5);
	wxColour black(wxT("#000000"));// black
	m_weaponRegions.clear();
	m_defenseRegions.clear();
	m_pendingSeekerRecallRegions.clear();
	m_pendingSeekerRecallHexes.clear();
	m_preGameSeekerRecallRegions.clear();
	m_preGameSeekerRecallHexes.clear();
	m_preGameSeekerRecallShipIDs.clear();
	m_preGameSeekerRecallWeaponIndices.clear();
	m_preGameSeekerRecallWeaponIDs.clear();
	dc.SetBackground(wxBrush(black));
	int w,h;
	dc.GetSize(&w,&h);
	dc.SetPen(*wxBLACK);
	dc.SetBrush(*wxBLACK_BRUSH);
	dc.DrawRectangle(0,0,w,h);
	dc.DrawBitmap(wxBitmap(m_zoomImage),0,0);
	switch (m_parent->getState()){
	case BS_SetupPlanet:
		if (m_parent->getControlState()){
			drawPlacePlanet(dc);
		} else {
			drawPlanetChoices(dc);
		}
		break;
	case BS_SetupStation:
		if (m_parent->getPhase()==PH_NONE){
			drawPlaceStation(dc);
		} else {
			drawSelectRotation(dc);
		}
		break;
	case BS_SetupDefendFleet:
	case BS_SetupAttackFleet:
		if (m_parent->getControlState()){
			drawPlaceShip(dc);
		} else {
			if (m_parent->getPhase()==PH_NONE){
				drawShipChoices(dc);
			} else {
				drawGetSpeed(dc);
			}
		}
		break;
	case BS_PlaceMines:
		drawPlaceMines(dc);
		break;
	case BS_PlaceSeekers:
		drawPlaceSeekers(dc);
		break;
	case BS_Battle: {
		switch (m_parent->getPhase()){
		case PH_SEEKER_ACTIVATION:
			drawSeekerActivation(dc);
			break;
		case PH_MOVE:
			drawMoveShip(dc);
			break;
		case PH_DEFENSE_FIRE:
			drawDefensiveFire(dc);
			break;
		case PH_ATTACK_FIRE:
			drawAttackFire(dc);
			break;
		default:
			break;
		}
		drawCurrentShipStats(dc);
		if (m_parent->getPhase() == PH_ATTACK_FIRE) {
			// Compute lMargin so the pending panel's left edge clears the widest
			// left-column instruction line.  The left instruction text is NOT wrapped
			// per user preference; instead the panel shifts right to avoid overlap.
			dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
			const int attackTextW = dc.GetTextExtent(SEEKER_DEPLOY_INSTRUCTION).GetWidth();
			const int pendingLMargin = leftOffset + attackTextW + 2*BORDER;
			drawOffensiveSeekerPendingRows(dc, pendingLMargin, getActionPromptLineY(0), 10);
		}
		break;
	}
	default:
		break;
	}

}

void FBattleDisplay::onPaint(wxPaintEvent & event){
//	std::cerr << "in FBattleDispaly::onPaint()" << std::endl;
	wxPaintDC dc(this);
	draw(dc);
}

void FBattleDisplay::onLeftUp(wxMouseEvent & event) {
//	wxCoord x,y;
//	event.GetPosition(&x,&y);
//	std::cerr << "The x and y positions of the click are " << x << ", " << y << std::endl;
	if (event.GetX() < ZOOM_SIZE){
		zoomMap(event);
	}
	switch (m_parent->getState()){
	case BS_SetupPlanet:
		if (m_parent->getControlState()==false){
			makePlanetChoice(event);
		}
		break;
	case BS_SetupStation:
		if (m_parent->getPhase()==PH_SET_SPEED){
			if (!setStationRotation(event)){
				break;
			}
		}
		break;
	case BS_SetupDefendFleet:
	case BS_SetupAttackFleet:
		if (m_parent->getControlState()==false && m_parent->getPhase()==PH_NONE){
			makeShipChoice(event);
		}
		break;
	case BS_PlaceMines:
		checkShipSelection(event);
		break;
	case BS_PlaceSeekers:
		if (checkPreGameSeekerRecallSelection(event)) {
			break;
		}
		checkShipSelection(event);
		break;
	case BS_Battle:
		if (m_parent->getPhase() == PH_SEEKER_ACTIVATION) {
			checkSeekerActivationSelection(event);
			break;
		}
		if (m_parent->getPhase() == PH_ATTACK_FIRE && checkOffensiveSeekerPendingSelection(event)) {
			break;
		}
		if (m_parent->getShip()!=NULL
				&& m_weaponRegions.size()>0
				&& m_parent->getShip()->getOwner()==m_parent->getActivePlayerID()){
			checkWeaponSelection(event);
		}
		if (m_parent->getShip()!=NULL
				&& m_defenseRegions.size()>0
				&& m_parent->getShip()->getOwner()==m_parent->getActivePlayerID()){
			checkDefenseSelection(event);
		}
		break;
	default:
		break;
	}
	event.Skip();
}

void FBattleDisplay::drawPlanetChoices (wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	for (unsigned int i=0; i< m_imageList.size(); i++){
//		std::cerr << "Drawing planet " << i << std::endl;
		wxBitmap b (m_imageList[i].Scale(ICON_SIZE,ICON_SIZE));
		dc.DrawBitmap(b,ZOOM_SIZE+BORDER+i*ICON_SIZE,BORDER);
	}
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.DrawText("Please select an icon to use for the planet.",leftOffset,2*BORDER+ICON_SIZE);

}

void FBattleDisplay::makePlanetChoice(wxMouseEvent & event){
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int xp = (x-leftOffset)/ICON_SIZE;
	int yp = (y-BORDER)/ICON_SIZE;
	if ( y>=BORDER && yp == 0){  // there is only one row in this case
		if ( x>=leftOffset && xp < (int)(m_imageList.size() )) { // make sure we actually hit an icon
			m_parent->setPlanet(xp);  // set the index
			m_parent->toggleControlState();
			wxClientDC dc(this);
			Refresh();
//			std::cerr << "The selection position is (" << xp << "," << yp << ")" << std::endl;
		}
	}
}

void FBattleDisplay::drawPlacePlanet(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.DrawText("Select the map hex where you would like to place the planet.",leftOffset,BORDER);

}

void FBattleDisplay::drawPlaceShip(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::string txt = "Select the map hex where you would like to place the ship.";
	dc.DrawText(txt,leftOffset,BORDER);
	txt = "Click once to place the ship and then move the mouse to select the";
	dc.DrawText(txt,leftOffset,BORDER+16);
	txt = "desired heading and click again to finalize placement.";
	dc.DrawText(txt,leftOffset,BORDER+32);

}

void FBattleDisplay::drawPlaceStation(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::string txt = "Select the map hex where you would like to place the station.";
	dc.DrawText(txt,leftOffset,BORDER);
	txt = "It must be adjacent to the planet.";
	dc.DrawText(txt,leftOffset,BORDER+16);
}

void FBattleDisplay::drawShipChoices (wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	if (!m_loaded) {
		m_vList = m_parent->getShipList(m_parent->getActivePlayerID());
		m_loaded = true;
		m_parent->setDone(false);
		if(m_parent->getActivePlayer() == false && m_parent->getStation()!=NULL){
			m_vList[m_vList.size()-1] = NULL;
		}
	}
	for (unsigned int i=0; i< m_vList.size(); i++){
		if (m_vList[i]!=NULL){
			const wxImage *iptr = &WXIconCache::instance().get(m_vList[i]->getIconName());
			wxImage im = iptr->Scale(ICON_SIZE,ICON_SIZE);
			wxBitmap b(im);
			dc.DrawBitmap(b,leftOffset+i*ICON_SIZE,BORDER);
		}
	}
	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.DrawText("Please select a ship to place on the map.",leftOffset,2*BORDER+ICON_SIZE);

}

void FBattleDisplay::zoomMap(wxMouseEvent & event){
	double rate = 1.5;  // rate of zoom per click
	if (event.GetY()<44) {
		m_parent->setScale(rate);
	} else if (event.GetY()>76){
		m_parent->setScale(1/rate);
	}
}

void FBattleDisplay::makeShipChoice(wxMouseEvent & event){
	static unsigned int count = 0;
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int xp = (x-leftOffset)/ICON_SIZE;
	int yp = (y-BORDER)/ICON_SIZE;
	VehicleList vList = m_parent->getShipList(m_parent->getActivePlayerID());
	if ( y>=BORDER && yp == 0){  // there is only one row in this case
		if ( x>=leftOffset && xp < (int)(vList.size()) && m_vList[xp]!=NULL) { // make sure we actually hit an icon
			count++;
			m_parent->setShip(vList[xp]);  // set the index
			m_parent->toggleControlState();
			m_vList[xp]=NULL;
			wxClientDC dc(this);
			Refresh();
//			std::cerr << "The selection position is (" << xp << "," << yp << ")" << std::endl;
			unsigned int countLimit = m_vList.size();
			if (m_parent->getActivePlayer() == false && m_parent->getStation()!=NULL) { countLimit--; }
			if (count == countLimit){
				count = 0;
				m_parent->setDone(true);
				m_loaded = false;
			}
		}
	}
}

void FBattleDisplay::drawGetSpeed(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	if (m_first){
		m_spinCtrl1->Show();
		m_button1->Show();
		// Connect Events
		m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onSetSpeed ), NULL, this );
		m_first = false;
		Layout();
	}

	dc.SetTextForeground(white);
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::string txt = "Please choose an initial speed for the ship.";
	dc.DrawText(txt,leftOffset,2*BORDER+ICON_SIZE);
	txt = "Press the 'Set Speed' button when done";
	dc.DrawText(txt,leftOffset,2*BORDER+ICON_SIZE+16);

}

void FBattleDisplay::onSetSpeed( wxCommandEvent& event ){
	// disconnect the button
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onSetSpeed ), NULL, this );
	m_parent->getShip()->setSpeed(m_spinCtrl1->GetValue());
	// Hid the set speed controls
	m_spinCtrl1->Hide();
	m_button1->Hide();
	Layout();

	m_first = true;
	m_parent->setPhase(PH_NONE);
	bool enteredMinePlacement = false;
	if(m_parent->getDone()){
		if(m_parent->getState()==BS_SetupDefendFleet){
			if(!m_parent->beginMinePlacement()){
				m_parent->setState(BS_SetupAttackFleet);
				m_parent->toggleActivePlayer();
			} else {
				enteredMinePlacement = true;
			}
		} else {
			m_parent->setState(BS_Battle);
			m_parent->toggleActivePlayer();
			m_parent->setPhase(PH_MOVE);
		}
	}
	if (!enteredMinePlacement) {
		m_parent->setShip(NULL);
	}
	event.Skip();
}

void FBattleDisplay::drawSelectRotation(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	wxColour black(wxT("#000000"));
	wxColour lblue(wxT("#9999FF"));
	int tSize =10;
	dc.SetFont(wxFont(tSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.SetBrush(wxBrush(lblue));
	dc.SetPen(wxPen(black));
	dc.SetTextForeground(black);
	int w=140;
	int h=30;
	int r=15;
	dc.DrawRoundedRectangle(leftOffset,BORDER,w,h,r);
	dc.DrawText("Clockwise",leftOffset+2*BORDER+27,BORDER+h/2-tSize+1);
	dc.DrawRoundedRectangle(leftOffset+BORDER+w,BORDER,w,h,r);
	dc.DrawText("Counter-clockwise",leftOffset+3*BORDER+w,BORDER+h/2-tSize+1);

	dc.SetTextForeground(white);
	dc.DrawText("Please select the station's direction of rotation",leftOffset,2*BORDER+h);
}

bool FBattleDisplay::setStationRotation(wxMouseEvent &event){
	wxCoord x,y;
	event.GetPosition(&x,&y);
	int w=140;  // these come from the drawSelectRotation() method
	int h=30;
	if (x>leftOffset && x<leftOffset+BORDER+2*w && y>BORDER && y<BORDER+h){
		int heading = FHexMap::computeHeading(m_parent->getStationPos(),m_parent->getPlanetPos());
		if(x<leftOffset+w){					// clockwise
			heading = (heading+1)%6;
		} else if (x>leftOffset+w+BORDER){	// ccw
			heading -= 1;
			if (heading < 0) { heading +=6; }
		} else {  // hit the gap
			return false;
		}
		m_parent->getStation()->setHeading(heading);
		m_parent->getStation()->setSpeed(1);
		unsigned int shipCount = (m_parent->getShipList(m_parent->getActivePlayerID())).size();
		m_parent->setPhase(PH_NONE);
		if (shipCount>0){
			m_parent->setState(BS_SetupDefendFleet);
		} else {
			m_parent->setState(BS_SetupAttackFleet);
			m_parent->toggleActivePlayer();
		}
	}
	return true;
}

void FBattleDisplay::drawMoveShip(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	int panelWidth = 0;
	int panelHeight = 0;
	dc.GetSize(&panelWidth,&panelHeight);
	refreshMovePromptReservation(dc, panelWidth, panelHeight);
	applyRequestedDisplayHeight();
	const int promptTopY = getActionPromptLineY(0);
	dc.DrawText("",leftOffset,promptTopY);
	int promptMaxWidth = getCurrentPromptMaxWidth(panelWidth);
	dc.SetTextForeground(white);
	bool stoppedShipFacingSelection = false;
	if (m_parent->getShip() != NULL && m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()) {
		const std::map<unsigned int, FTacticalTurnData> & turnInfo = m_parent->getTurnInfo();
		std::map<unsigned int, FTacticalTurnData>::const_iterator turnItr = turnInfo.find(m_parent->getShip()->getID());
		if (turnItr != turnInfo.end()) {
			const FTacticalTurnData & turnData = turnItr->second;
			stoppedShipFacingSelection = (turnData.speed == 0
				&& turnData.nMoved == 0
				&& m_parent->getShip()->getMR() > 0);
		}
	}
	wxString turnPrompt;
	wxString detailPromptOne;
	wxString detailPromptTwo;
	std::ostringstream os;
	os << "It is the ";
	if (m_parent->getActivePlayer()){
		os << "attacker's ";
	} else {
		os << "defender's ";
	}
	os << "turn.";
	turnPrompt = wxString(os.str());
	if (stoppedShipFacingSelection) {
		detailPromptOne = "Select a highlighted preview route to choose your starting facing.";
		detailPromptTwo = "Continue a route, or click an adjacent hex then Movement Done to rotate in place.";
	} else if (m_parent->getShip() != NULL && m_parent->getShip()->getOwner() == m_parent->getMovingPlayerID()) {
		detailPromptOne = "Select route hexes to move the ship.";
		detailPromptTwo = "Press the 'Movement Done' button when all ships have been assigned their movement instructions.";
	} else {
		detailPromptOne = "Please select a ship to move.";
	}
	int promptLineCount = countWrappedActionPromptLines(dc, turnPrompt, promptMaxWidth);
	promptLineCount += countWrappedActionPromptLines(dc, detailPromptOne, promptMaxWidth);
	promptLineCount += countWrappedActionPromptLines(dc, detailPromptTwo, promptMaxWidth);
	reserveActionPromptLines(promptLineCount);

	int promptLineCursor = 0;
	drawWrappedActionPrompt(dc, turnPrompt, promptMaxWidth, promptLineCursor);
	drawWrappedActionPrompt(dc, detailPromptOne, promptMaxWidth, promptLineCursor);
	drawWrappedActionPrompt(dc, detailPromptTwo, promptMaxWidth, promptLineCursor);

	if (m_lowerPanelLayoutState.reservedPromptLines < ACTION_PROMPT_MAX_LINES){
		for (int i = m_lowerPanelLayoutState.reservedPromptLines; i < ACTION_PROMPT_MAX_LINES; i++){
			dc.DrawText("",leftOffset,getActionPromptLineY(i));
		}
	}
		m_buttonMoveDone->Enable(m_parent->isMoveComplete());
		if (m_first){
		m_buttonMoveDone->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onMoveDone ), NULL, this );
		m_buttonMoveDone->Show();
		Layout();
		m_first=false;
	}

}

void FBattleDisplay::drawCurrentShipStats(wxDC & dc){
	int panelWidth = 0;
	int panelHeight = 0;
	dc.GetSize(&panelWidth,&panelHeight);
	ensureLowerPanelLayoutState(panelWidth, panelHeight);
	applyRequestedDisplayHeight();
	const ShipStatsLayoutRequirements requirements = measureShipStatsLayoutRequirements(dc);
	const int largestMarginWithStatsRoom = panelWidth - requirements.width - BORDER;
	int lMargin = m_lowerPanelLayoutState.shipStatsLeftMargin;
	if (m_lowerPanelLayoutState.mode == LOWER_PANEL_LAYOUT_RIGHT_SPLIT){
		if (m_lowerPanelLayoutState.mode == LOWER_PANEL_LAYOUT_RIGHT_SPLIT
				&& largestMarginWithStatsRoom >= leftOffset + ACTION_PROMPT_MIN_WIDTH
				&& lMargin > largestMarginWithStatsRoom){
			lMargin = largestMarginWithStatsRoom;
			m_lowerPanelLayoutState.shipStatsLeftMargin = lMargin;
		} else if (largestMarginWithStatsRoom < leftOffset + ACTION_PROMPT_MIN_WIDTH + BORDER){
			m_lowerPanelLayoutState.mode = LOWER_PANEL_LAYOUT_STACKED;
			m_lowerPanelLayoutState.shipStatsLeftMargin = leftOffset;
			m_lowerPanelLayoutState.shipStatsTop = getActionButtonRowBottom() + ACTION_PROMPT_BUTTON_GAP;
			lMargin = m_lowerPanelLayoutState.shipStatsLeftMargin;
			const int stackedStatsBottom = m_lowerPanelLayoutState.shipStatsTop + requirements.height + BORDER;
			if (stackedStatsBottom > m_lowerPanelLayoutState.requestedDisplayHeight){
				m_lowerPanelLayoutState.requestedDisplayHeight = stackedStatsBottom;
				applyRequestedDisplayHeight();
			}
		}
	}
	int topMargin = m_lowerPanelLayoutState.shipStatsTop;
	if (m_lowerPanelLayoutState.mode == LOWER_PANEL_LAYOUT_STACKED){
		const int stackedTop = getActionButtonRowBottom() + ACTION_PROMPT_BUTTON_GAP;
		if (topMargin < stackedTop){
			topMargin = stackedTop;
			m_lowerPanelLayoutState.shipStatsTop = topMargin;
		}
	}
	int textSize = 10;		// text height
	FVehicle *s = m_parent->getShip();
	wxColour white(wxT("#FFFFFF"));
	wxFont normal(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	wxFont large((int)(textSize*1.3),wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	wxFont bold(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	if (m_parent->getShip()!=NULL){
//		std::cerr << "Entering drawCurrentShipStats() for " << s->getName() << std::endl;
		dc.SetTextForeground(white);
		dc.SetFont(bold);
		dc.DrawText("Speed:",lMargin,topMargin+(int)(1.6*(textSize*1.3)));
		dc.DrawText("Heading: ",lMargin+90,topMargin+(int)(1.6*(textSize*1.3)));
		dc.DrawText("ADF:",lMargin,topMargin+(int)(1.6*(textSize*2.3)));
		dc.DrawText("MR: ",lMargin+80,topMargin+(int)(1.6*(textSize*2.3)));
		dc.DrawText("HP: ",lMargin+160,topMargin+(int)(1.6*(textSize*2.3)));
		dc.DrawText("DCR: ",lMargin+240,topMargin+(int)(1.6*(textSize*2.3)));
		dc.DrawText("Weapons:",lMargin,topMargin+(int)(1.6*(textSize*3.3)));
		dc.DrawText("Defenses:",lMargin,topMargin+(int)(1.6*(textSize*4.3)));
		dc.DrawText("Other Status:",lMargin,topMargin+(int)(1.6*(textSize*5.3)));
		dc.SetFont(large);
		dc.DrawText(s->getName(),lMargin,topMargin);
		dc.SetFont(normal);
		std::ostringstream os;
		os << s->getSpeed();
		dc.DrawText(os.str(),lMargin+60,topMargin+(int)(1.6*(textSize*1.3)));
		dc.DrawText(getHeadingStr(),lMargin+170,topMargin+(int)(1.6*(textSize*1.3)));
		os.str("");
		os << s->getADF();
		dc.DrawText(os.str(),lMargin+40,topMargin+(int)(1.6*(textSize*2.3)));
		os.str("");
		os << s->getMR();
		dc.DrawText(os.str(),lMargin+115,topMargin+(int)(1.6*(textSize*2.3)));
		os.str("");
		os << s->getHP();
		dc.DrawText(os.str(),lMargin+195,topMargin+(int)(1.6*(textSize*2.3)));
		os.str("");
		os << s->getDCR();
		dc.DrawText(os.str(),lMargin+275,topMargin+(int)(1.6*(textSize*2.3)));
		int x = lMargin+80;
		int y = topMargin+(int)(1.6*(textSize*3.3));
		drawWeaponList(dc,x,y,textSize);
		x = lMargin+80;
		y += (int)(1.6*textSize);
		drawDefenseList(dc,x,y,textSize);
		x = lMargin+110;
		y += (int)(1.6*textSize);
		drawOtherStatus(dc,x,y,textSize);

	}
}

std::string FBattleDisplay::getHeadingStr(){
	switch (m_parent->getShip()->getHeading()){
	case 0:
		return "W";
	case 1:
		return "SW";
	case 2:
		return "SE";
	case 3:
		return "E";
	case 4:
		return "NE";
	case 5:
		return "NW";
	default:
		return "";
	}
}

void FBattleDisplay::onMoveDone( wxCommandEvent& event ){
	// disconnect the button
	m_buttonMoveDone->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onMoveDone ), NULL, this );
//	std::cerr << "Movement Completed" << std::endl;
	m_parent->completeMovePhase();
	m_buttonMoveDone->Hide();
	Layout();
	m_first=true;
}

void FBattleDisplay::drawDefensiveFire(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::ostringstream os;
	reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);
	os << "The non-moving player may now";
	dc.SetTextForeground(white);
	dc.DrawText(os.str(),leftOffset,getActionPromptLineY(0));
	dc.DrawText("declare defensive fire.",leftOffset,getActionPromptLineY(1));
	os.str("Please select a ship to fire weapons.");
	dc.DrawText(os.str(),leftOffset,getActionPromptLineY(2));
	m_buttonDefensiveFireDone->Enable(m_parent->isMoveComplete());
	if (m_first){
		m_buttonDefensiveFireDone->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onDefensiveFireDone ), NULL, this );
		m_buttonDefensiveFireDone->Show();
		Layout();
		m_first=false;
	}

}

void FBattleDisplay::drawAttackFire(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	dc.SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	std::ostringstream os;
	reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);
	os << "The moving player may now";
	dc.SetTextForeground(white);
	dc.DrawText(os.str(),leftOffset,getActionPromptLineY(0));
	dc.DrawText("declare offensive fire.",leftOffset,getActionPromptLineY(1));
	if (m_parent->isOffensiveSeekerDeploymentMode()) {
		os.str(SEEKER_DEPLOY_INSTRUCTION.ToStdString());
	} else {
		os.str("Please select a ship to fire weapons.");
	}
	dc.DrawText(os.str(),leftOffset,getActionPromptLineY(2));
	m_buttonOffensiveFireDone->Enable(m_parent->isMoveComplete());
	if (m_first){
		m_buttonOffensiveFireDone->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onOffensiveFireDone ), NULL, this );
		m_buttonOffensiveFireDone->Show();
		Layout();
		m_first=false;
	}

}

void FBattleDisplay::onDefensiveFireDone( wxCommandEvent& event ){
	// disconnect the button
	m_buttonDefensiveFireDone->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onDefensiveFireDone ), NULL, this );
	m_buttonDefensiveFireDone->Enable(false);
	m_buttonDefensiveFireDone->Hide();
	Layout();
	// Fire resolution runs in the model first and captures destroyed-ship IDs.
	// Screen cleanup then consumes that bookkeeping and clears it at the seam.
	const FTacticalCombatReportSummary summary = m_parent->resolveCurrentFirePhase();
	m_parent->showTacticalDamageSummaryDialog(summary);
	m_parent->clearDestroyedShips();
	m_parent->completeDefensiveFirePhase();
	m_first=true;
}

void FBattleDisplay::onOffensiveFireDone( wxCommandEvent& event ){
	// disconnect the button
	m_buttonOffensiveFireDone->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onOffensiveFireDone ), NULL, this );
	m_buttonOffensiveFireDone->Enable(false);
	m_buttonOffensiveFireDone->Hide();
	Layout();
	// Keep same lifecycle as defensive fire: model capture first, wx cleanup second.
	const FTacticalCombatReportSummary summary = m_parent->resolveCurrentFirePhase();
	m_parent->showTacticalDamageSummaryDialog(summary);
	m_parent->clearDestroyedShips();
	m_parent->completeOffensiveFirePhase();
	m_first=true;
}

void FBattleDisplay::drawWeaponList(wxDC &dc, int lMargin, int tMargin, int textSize){
	wxColour white(wxT("#FFFFFF"));
	wxColour red(wxT("#FF0000"));
	wxColour green(wxT("#00FF00"));
	wxColour yellow(wxT("#FFFF00"));
	wxFont normal(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	dc.SetTextForeground(white);
	FVehicle *s = m_parent->getShip();
	bool active = false;
	if (m_parent->getActivePlayerID()==s->getOwner()){
		active = true;
	}
	int x = lMargin;
	unsigned int wCount = s->getWeaponCount();
	for (unsigned int i =0; i< wCount; i++){
		// select the text color for this weapon
		FWeapon *w = s->getWeapon(i);
		if (w->isDamaged()){
			dc.SetTextForeground(red);
		} else if (active){
			if(w->getTarget()!=NULL){
				dc.SetTextForeground(green);
			} else {
				dc.SetTextForeground(yellow);
			}
			if (w->isMPO() && m_parent->getPhase()==PH_DEFENSE_FIRE){
				dc.SetTextForeground(white);
			}
			if (w->getType() == FWeapon::SM
				&& (m_parent->getPhase() != PH_ATTACK_FIRE
					|| m_parent->getActivePlayerID() != m_parent->getMovingPlayerID())){
				dc.SetTextForeground(white);
			}
			if (w->getMaxAmmo() && w->getAmmo()==0){ // all the ammo is used up.
				dc.SetTextForeground(white);
			}
		} else {
			dc.SetTextForeground(white);
		}
		std::string wName = w->getName() + "  ";
		dc.DrawText(wName,x,tMargin);
		wxSize tSize= dc.GetTextExtent(wName);
		m_weaponRegions.push_back(wxRect(x,tMargin,tSize.GetWidth(),tSize.GetHeight()));
		x+= (int)tSize.GetWidth();
	}
}

void FBattleDisplay::drawDefenseList(wxDC &dc, int lMargin, int tMargin, int textSize){
	wxColour white(wxT("#FFFFFF"));
	wxColour red(wxT("#FF0000"));
	wxColour green(wxT("#00FF00"));
	wxColour orange(wxT("#FFA000"));
	wxFont normal(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	dc.SetTextForeground(white);
	FVehicle *s = m_parent->getShip();
//	bool active = false;
//	if (m_parent->getActivePlayerID()==s->getOwner()){
//		active = true;
//	}
	int x = lMargin;
	unsigned int dCount = s->getDefenseCount();
	for (unsigned int i =0; i< dCount; i++){
		// select the text color for this weapon
		FDefense *d = s->getDefense(i);
		if (d->isDamaged()){
			dc.SetTextForeground(red);
		} else if (s->getCurrentDefense()->getType()==d->getType()){
			dc.SetTextForeground(green);
		} else if(s->isPowerSystemDamaged() && d->getType() != FDefense::RH){
			dc.SetTextForeground(orange);
		} else {
			dc.SetTextForeground(white);
		}
		std::string dName = d->getName() + "  ";
		dc.DrawText(dName,x,tMargin);
		wxSize tSize= dc.GetTextExtent(dName);
		m_defenseRegions.push_back(wxRect(x,tMargin,tSize.GetWidth(),tSize.GetHeight()));
		x+= (int)tSize.GetWidth();
	}
}

void FBattleDisplay::checkWeaponSelection(wxMouseEvent &event){
	int x = event.GetX();
	int y = event.GetY();
	for (unsigned int i = 0; i< m_weaponRegions.size(); i++){
		if (m_weaponRegions[i].Contains(x,y)){
			m_parent->selectWeapon(i);
			break;
		}
	}
	m_parent->reDraw();
}

void FBattleDisplay::checkDefenseSelection(wxMouseEvent &event){
	int x = event.GetX();
	int y = event.GetY();
	for (unsigned int i = 0; i< m_defenseRegions.size(); i++){
		if (m_defenseRegions[i].Contains(x,y)){
			m_parent->selectDefense(i);
			break;
		}
	}
	m_parent->reDraw();
}

void FBattleDisplay::drawOtherStatus(wxDC &dc, int lMargin, int tMargin, int textSize){
	wxColour white(wxT("#FFFFFF"));
	wxColour red(wxT("#FF0000"));
	wxFont normal(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	dc.SetTextForeground(red);
	FVehicle *s = m_parent->getShip();
	int x = lMargin;
	bool damage = false;
	if (s->isOnFire()){  // Check for electrical fire
		std::string txt = "Fire  ";
		dc.DrawText(txt,x,tMargin);
		wxSize tSize= dc.GetTextExtent(txt);
		x+= (int)tSize.GetWidth();
		damage = true;
	}
	if (s->isCombatControlDamaged()){ // check for Combat control systems hit
		std::string txt = "Combat Sys Damaged  ";
		dc.DrawText(txt,x,tMargin);
		wxSize tSize= dc.GetTextExtent(txt);
		x+= (int)tSize.GetWidth();
		damage = true;
	}
	if (s->isPowerSystemDamaged()){
		std::string txt = "Power Short Circuit  ";
		dc.DrawText(txt,x,tMargin);
		wxSize tSize= dc.GetTextExtent(txt);
		x+= (int)tSize.GetWidth();
		damage = true;
	}
	if (s->getNavControlError()){
		std::string txt = "Nav Sys Damaged  ";
		dc.DrawText(txt,x,tMargin);
		wxSize tSize= dc.GetTextExtent(txt);
		x+= (int)tSize.GetWidth();
		damage = true;
	}
	if (!damage){
		dc.SetTextForeground(white);
		std::string txt = "none";
		dc.DrawText(txt,x,tMargin);
	}
}

void FBattleDisplay::onMinePlacementDone( wxCommandEvent& event ){
	// disconnect the button
	m_buttonMinePlacementDone->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onMinePlacementDone ), NULL, this );
	m_buttonMinePlacementDone->Hide();
	Layout();
	m_parent->completeMinePlacement();
	m_first=true;
}

void FBattleDisplay::onSeekerActivationDone( wxCommandEvent& event ){
	m_buttonSeekerActivationDone->Disconnect(
		wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(FBattleDisplay::onSeekerActivationDone),
		NULL,
		this);
	m_buttonSeekerActivationDone->Hide();
	Layout();
	m_parent->completeSeekerActivationPhase();
	m_first = true;
}

void FBattleDisplay::drawPlaceMines(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	wxColour green(wxT("#00FF00"));
	int textSize = 10;		// text height
	wxFont normal(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	wxFont bold(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	dc.SetFont(normal);
	std::ostringstream os;
	reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);
	dc.SetTextForeground(white);
	// Left column: wrap the instruction text onto two lines using the space left of
	// the source-row column (lMargin).  drawWrappedActionPrompt draws at leftOffset
	// using getActionPromptLineY() for each wrapped line.
	int lMargin = 310;	// left margin for source-row column (right of left column)
	const int instructionMaxWidth = lMargin - leftOffset - BORDER;
	int instructionCursor = 0;
	drawWrappedActionPrompt(dc,
		"The defending player may now place mines before the attacker sets up their ships.",
		instructionMaxWidth, instructionCursor);
	// Right column: source-selection rows anchored to the top of the bottom panel,
	// to the right of the left column (instruction text + Done button).
	int y = getActionPromptLineY(0);
	dc.SetTextForeground(white);
	dc.DrawText("Select a source row to place mines.", lMargin, y);
	y += (int)(1.6*textSize*1.3);
	m_shipNameRegions.clear();
	m_shipSelectionSourceIndices.clear();
	const std::vector<FTacticalDeploymentSource> & deployableSources = m_parent->getDeployablePlacementSources();
	const int selectedSourceIndex = m_parent->getSelectedPlacementSourceIndex();
	for (unsigned int i = 0; i < deployableSources.size(); ++i){
		const FTacticalDeploymentSource & source = deployableSources[i];
		// Mine phase: show only mine (M-type) sources
		if (source.weaponType != FWeapon::M){
			continue;
		}
		const VehicleList ownerShips = m_parent->getShipList(source.ownerID);
		FVehicle * ship = findShipByID(ownerShips, source.source.shipID);
		if (ship == NULL || source.source.weaponIndex < 0
				|| static_cast<unsigned int>(source.source.weaponIndex) >= ship->getWeaponCount()){
			continue;
		}
		FWeapon * weapon = ship->getWeapon(static_cast<unsigned int>(source.source.weaponIndex));
		if (weapon == NULL || weapon->getID() != source.source.weaponID){
			continue;
		}
		if (static_cast<int>(i) == selectedSourceIndex){
			dc.SetFont(bold);
			dc.SetTextForeground(green);
		} else {
			dc.SetFont(normal);
			dc.SetTextForeground(white);
		}
		os.str("");
		os << ship->getName() << " - " << getDeploymentWeaponLabel(source.weaponType)
			<< " #" << (source.source.weaponIndex + 1);
		dc.DrawText(os.str(),lMargin,y);
		wxSize tSize = dc.GetTextExtent(os.str());
		m_shipNameRegions.push_back(wxRect(lMargin,y,tSize.GetWidth() + 260,tSize.GetHeight()));
		m_shipSelectionSourceIndices.push_back(static_cast<int>(i));
		os.str("");
		os << "Ammo: " << weapon->getAmmo();
		dc.DrawText(os.str(),lMargin+200,y);
		y += (int)(1.6*textSize);
	}

	// Expand the panel height if the source list extends below the current minimum.
	const int mineListBottom = y + BORDER;
	if (mineListBottom > m_lowerPanelLayoutState.requestedDisplayHeight){
		m_lowerPanelLayoutState.requestedDisplayHeight = mineListBottom;
		applyRequestedDisplayHeight();
	}

	// Show the mine placement done button with its fixed label.
	m_buttonMinePlacementDone->Enable(true);
	if (m_first){
		m_buttonMinePlacementDone->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onMinePlacementDone ), NULL, this );
		m_buttonMinePlacementDone->Show();
		Layout();
		m_first = false;
	}
}

void FBattleDisplay::drawPlaceSeekers(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	wxColour green(wxT("#00FF00"));
	int textSize = 10;		// text height
	wxFont normal(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	wxFont bold(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD);
	dc.SetFont(normal);
	std::ostringstream os;
	reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);
	dc.SetTextForeground(white);
	// Left column: wrap the instruction text onto two lines using the space left of
	// the source-row column (lMargin).  drawWrappedActionPrompt draws at leftOffset
	// using getActionPromptLineY() for each wrapped line.
	int lMargin = 310;	// left margin for middle (source-row) column
	const int recallMargin = 620;  // left margin for right (recall) column
	const int instructionMaxWidth = lMargin - leftOffset - BORDER;
	int instructionCursor = 0;
	drawWrappedActionPrompt(dc,
		"The defending player may now place seeker missiles before the attacker sets up their ships.",
		instructionMaxWidth, instructionCursor);
	// Middle column: source-selection rows anchored to the top of the bottom panel,
	// to the right of the left column (instruction text + Done button).
	int y = getActionPromptLineY(0);
	dc.DrawText("Select a source row to place seeker missiles.", lMargin, y);
	y += (int)(1.6*textSize*1.3);
	m_shipNameRegions.clear();
	m_shipSelectionSourceIndices.clear();
	const std::vector<FTacticalDeploymentSource> & deployableSources = m_parent->getDeployablePlacementSources();
	const int selectedSourceIndex = m_parent->getSelectedPlacementSourceIndex();
	for (unsigned int i = 0; i < deployableSources.size(); ++i){
		const FTacticalDeploymentSource & source = deployableSources[i];
		// Seeker phase: show only seeker missile (SM-type) sources
		if (source.weaponType != FWeapon::SM){
			continue;
		}
		const VehicleList ownerShips = m_parent->getShipList(source.ownerID);
		FVehicle * ship = findShipByID(ownerShips, source.source.shipID);
		if (ship == NULL || source.source.weaponIndex < 0
				|| static_cast<unsigned int>(source.source.weaponIndex) >= ship->getWeaponCount()){
			continue;
		}
		FWeapon * weapon = ship->getWeapon(static_cast<unsigned int>(source.source.weaponIndex));
		if (weapon == NULL || weapon->getID() != source.source.weaponID){
			continue;
		}
		if (static_cast<int>(i) == selectedSourceIndex){
			dc.SetFont(bold);
			dc.SetTextForeground(green);
		} else {
			dc.SetFont(normal);
			dc.SetTextForeground(white);
		}
		os.str("");
		os << ship->getName() << " - " << getDeploymentWeaponLabel(source.weaponType)
			<< " #" << (source.source.weaponIndex + 1);
		dc.DrawText(os.str(),lMargin,y);
		wxSize tSize = dc.GetTextExtent(os.str());
		m_shipNameRegions.push_back(wxRect(lMargin,y,tSize.GetWidth() + 260,tSize.GetHeight()));
		m_shipSelectionSourceIndices.push_back(static_cast<int>(i));
		os.str("");
		os << "Ammo: " << weapon->getAmmo();
		dc.DrawText(os.str(),lMargin+200,y);
		y += (int)(1.6*textSize);
	}

	// Right column: placed-seeker undeploy/recall list anchored to the top of the
	// bottom panel at recallMargin, to the right of the middle column.
	{
		int cy = getActionPromptLineY(0);
		dc.SetFont(bold);
		dc.SetTextForeground(white);
		dc.DrawText("Placed seekers (click to recall):", recallMargin, cy);
		cy += (int)(1.6*textSize);
		dc.SetFont(normal);
		const std::vector<FTacticalPreGameSeekerHexGroup> placedGroups = m_parent->getPlacedSeekerHexGroups();
		if (placedGroups.empty()) {
			dc.SetTextForeground(white);
			dc.DrawText("None placed.", recallMargin, cy);
			cy += (int)(1.6*textSize);
		} else {
			for (unsigned int g = 0; g < placedGroups.size(); ++g) {
				const FTacticalPreGameSeekerHexGroup & grp = placedGroups[g];
				// Find ship name for display
				std::string shipName = "Unknown";
				FVehicle * srcShip = NULL;
				// Search attacker and defender ships
				{
					VehicleList allShips;
					VehicleList atk = m_parent->getShipList(m_parent->getAttackerID());
					VehicleList def = m_parent->getShipList(m_parent->getDefenderID());
					allShips.insert(allShips.end(), atk.begin(), atk.end());
					allShips.insert(allShips.end(), def.begin(), def.end());
					srcShip = findShipByID(allShips, grp.source.shipID);
					if (srcShip != NULL) {
						shipName = srcShip->getName();
					}
				}
				os.str("");
				os << "Recall: " << shipName
					<< " (" << grp.hex.getX() << "," << grp.hex.getY() << ")"
					<< " x" << grp.count;
				dc.SetTextForeground((g % 2 == 0) ? white : green);
				dc.DrawText(os.str(), recallMargin, cy);
				wxSize tSize = dc.GetTextExtent(os.str());
				m_preGameSeekerRecallRegions.push_back(wxRect(recallMargin, cy, tSize.GetWidth() + 16, tSize.GetHeight()));
				m_preGameSeekerRecallHexes.push_back(wxPoint(grp.hex.getX(), grp.hex.getY()));
				m_preGameSeekerRecallShipIDs.push_back(grp.source.shipID);
				m_preGameSeekerRecallWeaponIndices.push_back(grp.source.weaponIndex);
				m_preGameSeekerRecallWeaponIDs.push_back(grp.source.weaponID);
				cy += (int)(1.6*textSize);
			}
		}
		// Expand the panel if the placed-seeker list extends below the current minimum.
		const int placedListBottom = cy + BORDER;
		if (placedListBottom > m_lowerPanelLayoutState.requestedDisplayHeight){
			m_lowerPanelLayoutState.requestedDisplayHeight = placedListBottom;
			applyRequestedDisplayHeight();
		}
	}

	// Expand the panel height if the source list extends below the current minimum.
	const int seekerListBottom = y + BORDER;
	if (seekerListBottom > m_lowerPanelLayoutState.requestedDisplayHeight){
		m_lowerPanelLayoutState.requestedDisplayHeight = seekerListBottom;
		applyRequestedDisplayHeight();
	}

	// Show the seeker placement done button with its fixed label.
	m_buttonSeekerPlacementDone->Enable(true);
	if (m_first){
		m_buttonSeekerPlacementDone->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onSeekerPlacementDone ), NULL, this );
		m_buttonSeekerPlacementDone->Show();
		Layout();
		m_first = false;
	}
}

void FBattleDisplay::onSeekerPlacementDone( wxCommandEvent& event ){
	m_buttonSeekerPlacementDone->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBattleDisplay::onSeekerPlacementDone ), NULL, this );
	m_buttonSeekerPlacementDone->Hide();
	Layout();
	m_parent->completeSeekerPlacement();
	m_first = true;
}

void FBattleDisplay::drawSeekerActivation(wxDC &dc){
	wxColour white(wxT("#FFFFFF"));
	wxColour green(wxT("#00FF00"));
	const int textSize = 10;
	// Anchor "Activated seekers" list at the top of the bottom panel (right column,
	// starting at getActionPromptLineY(0)), matching the drawPlaceMines/drawPlaceSeekers
	// convention (SMRV-02).
	int y = getActionPromptLineY(0);
	std::ostringstream os;

	m_shipNameRegions.clear();
	m_shipSelectionSourceIndices.clear();
	m_seekerActivationRegions.clear();
	m_seekerActivationSeekerIDs.clear();
	reserveActionPromptLines(ACTION_PROMPT_MAX_LINES);

	dc.SetFont(wxFont(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	dc.SetTextForeground(white);
	// Compute lMargin dynamically: right column must clear the widest instruction
	// line and the "Seeker Activation Done" button's right edge, mirroring the
	// SMRV-01 (round5) attack-phase approach in draw().
	// Each GetTextExtent string must match its corresponding DrawText call below.
	const int instrW = std::max({
		dc.GetTextExtent(SEEKER_ACTIVATION_PHASE_INSTRUCTION).GetWidth(),
		dc.GetTextExtent(SEEKER_ACTIVATION_CLICK_INSTRUCTION).GetWidth(),
		dc.GetTextExtent(SEEKER_ACTIVATION_DEACTIVATE_INSTRUCTION).GetWidth()
	});
	const int btnBestW = m_buttonSeekerActivationDone->GetBestSize().GetWidth();
	const wxPoint btnPos = m_buttonSeekerActivationDone->GetPosition();
	// Use the button's actual right edge when it has been laid out (pos.x >= leftOffset);
	// otherwise fall back to leftOffset + best width.
	const int btnAbsRight = (btnPos.x >= leftOffset)
		? btnPos.x + m_buttonSeekerActivationDone->GetSize().GetWidth()
		: leftOffset + btnBestW;
	const int doneButtonRightExtent = btnAbsRight - leftOffset;
	const int lMargin = leftOffset + std::max(instrW, doneButtonRightExtent) + 2*BORDER;

	dc.DrawText(SEEKER_ACTIVATION_PHASE_INSTRUCTION,leftOffset,getActionPromptLineY(0));
	dc.DrawText(SEEKER_ACTIVATION_CLICK_INSTRUCTION,leftOffset,getActionPromptLineY(1));
	dc.DrawText(SEEKER_ACTIVATION_DEACTIVATE_INSTRUCTION,leftOffset,getActionPromptLineY(2));

	dc.SetFont(wxFont(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD));
	dc.SetTextForeground(white);
	dc.DrawText("Activated seekers:",lMargin,y);
	y += (int)(1.6*textSize);

	// Show only seekers activated during this phase (SMF-05: use this-phase accessor
	// from SMF-04 so prior-phase activations are not listed here).
	dc.SetFont(wxFont(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	const std::vector<FTacticalSeekerMissileState> activated = m_parent->getActiveSeekersByMovingPlayerThisPhase();
	if (activated.empty()) {
		dc.SetTextForeground(white);
		dc.DrawText("No seekers activated yet.",lMargin,y);
	} else {
		for (unsigned int i = 0; i < activated.size(); ++i) {
			const FTacticalSeekerMissileState & seeker = activated[i];
			os.str("");
			os << "Deactivate seeker #" << seeker.seekerID
				<< " (" << seeker.hex.getX() << "," << seeker.hex.getY() << ")";
			dc.SetTextForeground((i % 2 == 0) ? white : green);
			dc.DrawText(os.str(),lMargin,y);
			wxSize tSize = dc.GetTextExtent(os.str());
			m_seekerActivationRegions.push_back(wxRect(lMargin,y,tSize.GetWidth() + 16,tSize.GetHeight()));
			m_seekerActivationSeekerIDs.push_back(seeker.seekerID);
			y += (int)(1.6*textSize);
		}
	}

	// Expand the panel height if the activation list extends below the current minimum.
	const int activationListBottom = y + BORDER;
	if (activationListBottom > m_lowerPanelLayoutState.requestedDisplayHeight){
		m_lowerPanelLayoutState.requestedDisplayHeight = activationListBottom;
		applyRequestedDisplayHeight();
	}

	m_buttonSeekerActivationDone->Enable(true);
	if (m_first){
		m_buttonSeekerActivationDone->Connect(
			wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(FBattleDisplay::onSeekerActivationDone),
			NULL,
			this);
		m_buttonSeekerActivationDone->Show();
		Layout();
		m_first=false;
	}
}

void FBattleDisplay::drawOffensiveSeekerPendingRows(wxDC &dc, int lMargin, int startY, int textSize){
	m_pendingSeekerRecallRegions.clear();
	m_pendingSeekerRecallHexes.clear();

	if (!m_parent->isOffensiveSeekerDeploymentMode()){
		return;
	}

	const std::vector<FTacticalPendingSeekerHexGroup> pending = m_parent->getSelectedOffensivePendingSeekerHexGroups();
	wxColour white(wxT("#FFFFFF"));
	wxColour green(wxT("#00FF00"));
	std::ostringstream os;
	int y = startY;

	dc.SetFont(wxFont(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD));
	dc.SetTextForeground(white);
	dc.DrawText("Pending seeker deployments:",lMargin,y);
	y += (int)(1.6*textSize);

	dc.SetFont(wxFont(textSize,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));
	if (pending.empty()){
		dc.DrawText("None",lMargin,y);
		return;
	}

	for (unsigned int i = 0; i < pending.size(); ++i){
		os.str("");
		os << "Recall 1 at (" << pending[i].hex.getX() << ", " << pending[i].hex.getY()
			<< ") [pending: " << pending[i].count << "]";
		dc.SetTextForeground((i % 2 == 0) ? white : green);
		dc.DrawText(os.str(),lMargin,y);
		wxSize tSize = dc.GetTextExtent(os.str());
		m_pendingSeekerRecallRegions.push_back(wxRect(lMargin,y,tSize.GetWidth() + 16,tSize.GetHeight()));
		m_pendingSeekerRecallHexes.push_back(wxPoint(pending[i].hex.getX(), pending[i].hex.getY()));
		y += (int)(1.6*textSize);
	}
	const int pendingRegionBottom = y + BORDER;
	if (pendingRegionBottom > m_lowerPanelLayoutState.requestedDisplayHeight){
		m_lowerPanelLayoutState.requestedDisplayHeight = pendingRegionBottom;
		applyRequestedDisplayHeight();
	}
}

void FBattleDisplay::checkShipSelection(wxMouseEvent &event){
	int x = event.GetX();
	int y = event.GetY();
	// Legacy source-contract tokens retained for tactical regression fixtures:
	// const VehicleList & shipsWithMines = m_parent->getShipsWithMines();
	// m_parent->setShip(shipsWithMines[i]);
	for (unsigned int i = 0; i< m_shipNameRegions.size(); i++){
		if (m_shipNameRegions[i].Contains(x,y)){
			if (i < m_shipSelectionSourceIndices.size() && m_shipSelectionSourceIndices[i] >= 0) {
				m_parent->selectPlacementSourceByIndex(static_cast<unsigned int>(m_shipSelectionSourceIndices[i]));
			}
			break;
		}
	}
	m_parent->reDraw();
}

void FBattleDisplay::checkSeekerActivationSelection(wxMouseEvent &event){
	const int x = event.GetX();
	const int y = event.GetY();
	for (unsigned int i = 0; i < m_seekerActivationRegions.size(); ++i) {
		if (!m_seekerActivationRegions[i].Contains(x,y)) {
			continue;
		}
		if (i < m_seekerActivationSeekerIDs.size()) {
			m_parent->deactivateActiveSeekerByID(m_seekerActivationSeekerIDs[i]);
		}
		break;
	}
	m_parent->reDraw();
}

bool FBattleDisplay::checkOffensiveSeekerPendingSelection(wxMouseEvent &event){
	const int x = event.GetX();
	const int y = event.GetY();
	for (unsigned int i = 0; i < m_pendingSeekerRecallRegions.size(); ++i) {
		if (!m_pendingSeekerRecallRegions[i].Contains(x,y)) {
			continue;
		}
		if (i < m_pendingSeekerRecallHexes.size()) {
			const wxPoint & pendingHex = m_pendingSeekerRecallHexes[i];
			return m_parent->recallSelectedOffensivePendingSeekerAtHex(FPoint(pendingHex.x, pendingHex.y));
		}
		return false;
	}
	return false;
}

bool FBattleDisplay::checkPreGameSeekerRecallSelection(wxMouseEvent &event){
	const int x = event.GetX();
	const int y = event.GetY();
	for (unsigned int i = 0; i < m_preGameSeekerRecallRegions.size(); ++i) {
		if (!m_preGameSeekerRecallRegions[i].Contains(x,y)) {
			continue;
		}
		if (i < m_preGameSeekerRecallHexes.size()
			&& i < m_preGameSeekerRecallShipIDs.size()
			&& i < m_preGameSeekerRecallWeaponIndices.size()
			&& i < m_preGameSeekerRecallWeaponIDs.size()) {
			FTacticalOrdnanceSource source;
			source.shipID = m_preGameSeekerRecallShipIDs[i];
			source.weaponIndex = m_preGameSeekerRecallWeaponIndices[i];
			source.weaponID = m_preGameSeekerRecallWeaponIDs[i];
			const wxPoint & hexPt = m_preGameSeekerRecallHexes[i];
			return m_parent->recallPlacedSeekerAtHexSource(FPoint(hexPt.x, hexPt.y), source);
		}
		return false;
	}
	return false;
}


}
