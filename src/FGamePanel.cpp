#include "FGamePanel.h"



FGamePanel::FGamePanel(wxFrame * parent) : wxPanel(parent)
{
	m_parent = parent;
	m_game = NULL;
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(FGamePanel::onPaint));

}


FGamePanel::~FGamePanel()
{
}

void FGamePanel::onPaint(wxPaintEvent & event) {
	SetClientSize(m_parent->GetClientSize());
	wxPaintDC dc(this);
	if (m_game != NULL) {
		m_gameDisplay.draw(dc, *m_game);
	}
}
