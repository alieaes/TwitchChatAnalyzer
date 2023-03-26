#ifndef __HDR_SHARED_IRC__
#define __HDR_SHARED_IRC__

#pragma once

#include <queue>
#include <string>

#include "SharedBase.h"
#include "moduleAnalysis.h"

#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB

class ModuleIRC
{
public:
    ModuleIRC();
    ~ModuleIRC();

    bool                                 SocketInit();

    bool                                 Connection( XString host, int port );
    bool                                 Authentication( XString sNickname, XString sPassword );
    bool                                 Join( XString sChannel );

    bool                                 StartRecv();

    bool                                 Send( XString sMsg );
    XString                              Recv();

    void                                 Recvs();
    void                                 PingThread();
    void                                 InsertThread();
    void                                 AnalyzeThread();

    void                                 Restart();
private:
    int                                  _socket;

    bool                                 _isStop = false;
    bool                                 _isInit = false;
    bool                                 _isConnected = false;
    bool                                 _isRecvRunning = false;
    bool                                 _isPending = false;

    XString                              _sNickName;
    XString                              _sPassword;
    std::vector< XString >               _vecJoinChannel;
    XString                              _shost;
    int                                  _nPort;

    std::thread                          _thRecv;
    std::thread                          _thPing;
    std::thread                          _thAnalyze;

    std::queue< tyStRecvData >           _queueMsg;
};


typedef Shared::Singletons::Singleton<ModuleIRC> TyStIRC;

#endif