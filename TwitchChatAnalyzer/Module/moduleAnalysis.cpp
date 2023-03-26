#include "stdafx.h"

#include "moduleAnalysis.h"

ModuleAnalysis::ModuleAnalysis()
{
    _isPrintChat = true;
}

ModuleAnalysis::~ModuleAnalysis()
{
}

CHAT_DATA ModuleAnalysis::ParserChatting( tyStRecvData recvData )
{
    //OutputDebugStringW( ( LPCWSTR )sMsg.toWString().c_str() );

    CHAT_DATA data;
    data.dtChatTime = recvData.dtChatTime;

    QString sParse = recvData.sMsg;

    // 수신을 못 한 만큼 한 MSG에 여러줄이 들어오는 것 같음. 줄바꿈으로 나눠 처리함 
    QStringList lstMsg = sParse.split( "\r\n", Qt::SkipEmptyParts );

    for( auto sMessage : lstMsg )
    {
        QStringList lstParser = sMessage.split( "tmi.twitch.tv", Qt::SkipEmptyParts );

        // ":sharedid!sharedid@sharedid."
        // " JOIN #woowakgood"
        // 와 같은 값을 원함
        if( lstParser.size() < 2 )
            continue;

        {
            // 느낌표가 있는 경우는 접속 이후, 없는 경우는 접속 중/이전으로 판단함
            if( lstParser[ 0 ].contains( "!" ) == true )
            {
                QString sUser = lstParser[ 0 ].remove( ":" ).left( lstParser[ 0 ].indexOf( "!" ) );
                data.sUserName = sUser;
            }
            else
            {
                // 파싱 필요, 임시로 넣어둠
                data.sUserName = "TEST";
            }
        }

        QString sCommand = lstParser[ 1 ].trimmed();

        if( sCommand.startsWith( "PRIVMSG" ) == true )
        {
            data.sChannel = sCommand.split( " " )[ 1 ];
            data.sChannelUnique = data.sChannel.replace( "#", "" );
            data.sMsg = sMessage = sCommand.remove( 0, sCommand.indexOf( ":" ) + 1 );
            QString sString = QString( "[%1] <%2> : %3\r\n" ).arg( data.sChannel ).arg( data.sUserName ).arg( data.sMsg );

            if( _isPrintChat == true )
                OutputDebugStringW( ( LPCWSTR )sString.toStdWString().c_str() );
        }
    }

    return data;
}
