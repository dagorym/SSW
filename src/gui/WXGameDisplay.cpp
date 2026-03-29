/*
 * @file WXGameDisplay.cpp
 * @brief Implementation file for the WXGameDisplay class
 * @author Tom Stephens
 * @date Created:  Aug 3, 2009
 */

#include "gui/WXGameDisplay.h"
#include "gui/WXMapDisplay.h"
#include "gui/WXPlayerDisplay.h"
#include "gui/WXIconCache.h"
#include "strategic/FGame.h"
#include <sstream>

namespace Frontier
{

WXGameDisplay::WXGameDisplay() {
	// TODO Auto-generated constructor stub

}

WXGameDisplay::~WXGameDisplay() {
	// TODO Auto-generated destructor stub
}

void WXGameDisplay::draw(wxDC &dc, FGame &game) {
	dc.Clear();
	WXMapDisplay mapDisplay;
	mapDisplay.draw(dc);

	WXPlayerDisplay playerDisplay;
	const PlayerList &players = game.getPlayers();
	for (PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr) {
		if (*itr != NULL) {
			playerDisplay.drawFleets(dc, *itr);
		}
	}

	drawTurnCounter(dc, game);
}

void WXGameDisplay::drawTurnCounter(wxDC &dc, const FGame &game) {
	wxCoord w, h, s;
	dc.GetSize(&w, &h);
	s = ((w > h) ? h : w) / 20;

	wxColour white, blue, red, lgray, black, dblue, dred, lred, lblue;
	white.Set(wxT("#FFFFFF"));
	blue.Set(wxT("#0000FF"));
	red.Set(wxT("#FF0000"));
	lgray.Set(wxT("#999999"));
	black.Set(wxT("#000000"));
	dblue.Set(wxT("#000099"));
	dred.Set(wxT("#770000"));
	lblue.Set(wxT("#9999FF"));
	lred.Set(wxT("#FF9999"));

	dc.SetBrush(wxBrush(white));
	dc.SetPen(wxPen(black));
	dc.SetTextForeground(black);
	dc.SetFont(wxFont((int)s / 2, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			dc.DrawRectangle(i * s, j * s, s, s);
			std::ostringstream os;
			os << i + 5 * j;
			dc.DrawText(os.str(), (wxCoord)(i * s + 0.3 * s), (wxCoord)(j * s + 0.1 * s));
		}
	}

	const unsigned int round = game.getRound();
	const int day = round % 10;
	const int tenday = round / 10;
	int row = tenday / 5;
	int col = tenday % 5;
	const wxImage &tendayImage = WXIconCache::instance().get("icons/tenday.png");
	dc.DrawBitmap(wxBitmap(tendayImage.Scale(4 * s / 5, 4 * s / 5)), (wxCoord)(col * s + 0.1 * s), (wxCoord)(row * s + 0.1 * s));
	row = day / 5;
	col = day % 5;
	const wxImage &dayImage = WXIconCache::instance().get("icons/day.png");
	dc.DrawBitmap(wxBitmap(dayImage.Scale(4 * s / 5, 4 * s / 5)), (wxCoord)(col * s + 0.2 * s), (wxCoord)(row * s + 0.2 * s));

	dc.SetFont(wxFont((int)(s / 3.), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	const PlayerList &players = game.getPlayers();
	if (players.size() >= 2 && players[0] != NULL && game.getCurrentPlayerID() == players[0]->getID()) {
		dc.SetBrush(wxBrush(lblue));
		dc.DrawRoundedRectangle(0, 2 * s, 4 * s, s, s * 0.25);
		dc.DrawText("End UPF Turn", (wxCoord)(0.5 * s), (wxCoord)(2.25 * s));
	} else {
		dc.SetBrush(wxBrush(lred));
		dc.DrawRoundedRectangle(0, 2 * s, 4 * s, s, s * 0.25);
		dc.DrawText("End Sathar Turn", (wxCoord)(0.2 * s), (wxCoord)(2.25 * s));
	}
}

}
