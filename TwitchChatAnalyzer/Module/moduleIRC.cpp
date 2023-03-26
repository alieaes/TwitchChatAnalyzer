#include "stdafx.h"

#include "moduleIRC.h"

#include <winsock2.h>

#include "moduleAnalysis.h"
#include "moduleMainDB.h"

#include "Sqlite/SharedSQLite.h"
#include "Utils/SharedFile.h"

#define MAXDATASIZE 4096

ModuleIRC::ModuleIRC()
{
    _socket = 0;
    _isInit = false;
    _isConnected = false;
    _isStop = false;
    _isRecvRunning = false;
}

ModuleIRC::~ModuleIRC()
{
    _isStop = true;
    _isRecvRunning = false;
    closesocket( _socket );


    if( _thRecv.joinable() == true )
        _thRecv.join();

    if( _thPing.joinable() == true )
        _thPing.join();

    if( _thAnalyze.joinable() == true )
        _thAnalyze.join();
}

bool ModuleIRC::SocketInit()
{
    bool isSuccess = false;

    do
    {
        if( _isInit == true )
        {
            isSuccess = true;
            break;
        }

        WSADATA wsaData;

        if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) )
            break;

        _socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

        if( _socket == INVALID_SOCKET )
            break;

        BOOL bOn = TRUE;
        if( setsockopt( _socket, IPPROTO_TCP, TCP_NODELAY, ( const char* )&bOn, sizeof( bOn ) ) != 0 )
            break;

        int on = 1;
        //if( setsockopt( _socket, SOL_SOCKET, SO_REUSEADDR, ( const char* )&on, sizeof( on ) ) == -1 )
        //    break;

        //u_long lMode = 0;
        //ioctlsocket( _socket, FIONBIO, &lMode );

        _isInit = true;
        isSuccess = true;
    }
    while( false );

    if( isSuccess == false )
        WSACleanup();

    return isSuccess;
}

bool ModuleIRC::Connection( XString host, int port )
{
    bool isSuccess = false;
    hostent* hostent;

    do
    {
        if( _isInit == false )
            break;

        hostent = gethostbyname( host.toString().c_str() );
        if( !hostent )
            break;

        sockaddr_in addr;
        addr.sin_addr = *reinterpret_cast< const in_addr* >( hostent->h_addr );
        addr.sin_port = htons( port );
        addr.sin_family = AF_INET;
        memset( &addr.sin_zero, '\0', 8 );

        if( connect( _socket, ( sockaddr* )&addr, sizeof( addr ) ) == SOCKET_ERROR )
        {
            closesocket( _socket );
            break;
        }

        _isConnected = true;
        isSuccess = true;
        _shost = host;
        _nPort = port;
    }
    while( false );

    if( isSuccess == false )
        WSACleanup();

    return isSuccess;
}

bool ModuleIRC::Authentication( XString sNickname, XString sPassword )
{
    bool isSuccess = false;

    do
    {
        if( sNickname.IsEmpty() == true || sPassword.IsEmpty() == true )
            break;

        isSuccess = Send( "PASS " + sPassword + "\r\n" );
        isSuccess = Send( "NICK " + sNickname + "\r\n" );

        Recv();

        _sNickName = sNickname;
        _sPassword = sPassword;
    }
    while( false );

    return isSuccess;
}

bool ModuleIRC::Join( XString sChannel )
{
    bool isSuccess = false;

    do
    {
        if( sChannel.IsEmpty() == true )
            break;

        isSuccess = Send( "JOIN " + sChannel + "\r\n" );

        _vecJoinChannel.push_back( sChannel );
        TyStMainDB::GetInstance()->MakeInsertThread( sChannel );
    } while( false );

    return isSuccess;
}

bool ModuleIRC::StartRecv()
{
    bool isSuccess = false;

    do
    {
        if( _isRecvRunning == true )
        {
            _isRecvRunning = true;
            break;
        }

        _thRecv = std::thread( std::bind( &ModuleIRC::Recvs, this ) );
        _thPing = std::thread( std::bind( &ModuleIRC::PingThread, this ) );
        _thAnalyze = std::thread( std::bind( &ModuleIRC::InsertThread, this ) );
        _isRecvRunning = true;
    }
    while( false );

    return isSuccess;
}

bool ModuleIRC::Send( XString sMsg )
{
    int nResult = send( _socket, sMsg.toString().c_str(), sMsg.size(), NULL );

    OutputDebugStringW( ( LPCWSTR )sMsg.c_str() );

    if( nResult == SOCKET_ERROR )
        return false;
    else
        return true;
}

XString ModuleIRC::Recv()
{
    XString sRet;
    {
        char buf[ MAXDATASIZE ];
        memset( buf, 0, MAXDATASIZE );

        int nRet = recv( _socket, buf, sizeof( buf ), 0 );
        if( nRet > 0 )
        {
            sRet = buf;
            OutputDebugStringA( ( LPCSTR )buf );
        }
    }

    return sRet;
}

void ModuleIRC::Recvs()
{
    // TODO : 필요없다면 제거
    _wsetlocale( LC_ALL, TEXT( "Korean" ) );
    SetConsoleOutputCP( 65001 );
    OutputDebugStringW( L"한글" );
    auto stAnalysis = TyStAnalysis::GetInstance();

    while( _isStop == false )
    {
        if( _isPending == true )
            continue;

        char buf[ MAXDATASIZE ];
        memset( buf, 0, MAXDATASIZE );

        int nRet = recv( _socket, ( char* )buf, sizeof( buf ), 0 );
        if( nRet > 0 )
        {
            tyStRecvData data;

            data.dtChatTime = QDateTime::currentDateTime();
            data.sMsg = XString( buf );
            _queueMsg.push( data );
        }
        else
        {
            // 이쪽으로 빠지면 연결 자체가 정상적이지 않은 걸로 보고 모듈 종료 후 재연결 하도록 함.
            Restart();
        }
    }
}

void ModuleIRC::PingThread()
{
    while( _isStop == false )
    {
        if( _isPending == true )
            continue;

        Send( "PING \r\n" );

        for( int idx = 0; idx < 60 * 5; idx++ )
        {
            if( _isStop == true )
                break;

            Sleep( 1000 );
        }
    }
}

void ModuleIRC::InsertThread()
{
    auto stAnalysis = TyStAnalysis::GetInstance();
    auto stSqliteMgr = TyStSqliteMgr::GetInstance();

    while( _isStop == false )
    {
        if( _queueMsg.size() <= 0 )
            continue;

        auto recvData = _queueMsg.front();
        _queueMsg.pop();

        if( recvData.sMsg.IsEmpty() == true )
            continue;

        auto data = stAnalysis->ParserChatting( recvData );

        if( data.sChannel.IsEmpty() == true || data.sUserName.IsEmpty() == true || data.sMsg.IsEmpty() == true )
            continue;

        TyStMainDB::GetInstance()->AppendChatData( data.sChannel, data );
    }
}

void ModuleIRC::AnalyzeThread()
{
    while( _isStop == false )
    {
        /*
         * 분간, 시간, 일간, 월간, 주간데이터를 마감함
         *
         * 평가할 데이터 : 마감 주기에 따른 전체 채팅 개수, 
         */
    }
}

void ModuleIRC::Restart()
{
    _isPending = true;
    _isInit = false;
    int nRet = closesocket( _socket );

    if( nRet != 0 )
        CONSOLEP( "CLOSESOCKET ERROR = {}", ::GetLastError() );

    bool isSuccess = SocketInit();

    if( isSuccess == false )
        CONSOLEP( "SocketInit ERROR = {}", ::GetLastError() );

    isSuccess = Connection( _shost, _nPort );

    if( isSuccess == false )
        CONSOLEP( "Connection ERROR = {}", ::GetLastError() );

    isSuccess = Authentication( _sNickName, _sPassword );

    if( isSuccess == false )
        CONSOLEP( "Connection ERROR = {}", ::GetLastError() );

    auto vec = _vecJoinChannel;
    _vecJoinChannel.clear();

    for( auto chan : vec )
    {
        isSuccess = Join( chan );

        if( isSuccess == false )
            CONSOLEP( "Join ERROR = {}", ::GetLastError() );
    }

    _isPending = false;
}
