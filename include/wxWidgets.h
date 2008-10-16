#ifndef _WXWIDGETS_H_
#define _WXWIDGETS_H_

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

enum
{
    ID_Quit = 1,
    ID_About,
    ID_New,
    ID_Open,
    ID_Save,
    ID_Close,
    ID_ShowPlayers,
    ID_EndUPFTurn,
    ID_EndSatharTurn,
    ID_PlaceNova,
    ID_AddSatharShips,
    ID_ShowRetreatCond,
    ID_ShowBattleScreen
};


#endif //_WXWIDGETS_H_
