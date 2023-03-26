#include "stdafx.h"
#include "cTwitchChatAnalyzer.h"
#include "moduleTwitchAPI.h"

#include "curl/curl.h"
#include "moduleIRC.h"

#pragma comment(lib, "wldap32.lib")

cTwitchChatAnalyzer::cTwitchChatAnalyzer(QWidget *parent)
    : QMainWindow(parent)
{
    auto stAPI = TyStAPI::GetInstance();
    //stAPI->GetEmoteList();
    //stAPI->GetAccessToken();
    //stAPI->GetClientSecretId();
    //stAPI->GetClientId();

    auto stIRC = TyStIRC::GetInstance();

    bool isSuccess = stIRC->SocketInit();
    isSuccess = stIRC->Connection( "irc.chat.twitch.tv", 6667 );

    stIRC->Authentication( "sharedid", "oauth:e0gct046kh7alt5x1kev75idnmfirr" );
    stIRC->Join( "#woowakgood" );
    stIRC->Join( "#vo_ine" );
    stIRC->StartRecv();

    ui.setupUi( this );
}

cTwitchChatAnalyzer::~cTwitchChatAnalyzer()
{}
