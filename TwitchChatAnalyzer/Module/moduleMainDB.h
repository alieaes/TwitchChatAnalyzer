#ifndef __HDR_SHARED_MAIN_DB__
#define __HDR_SHARED_MAIN_DB__

#pragma once

#include <string>

#include "curl.h"
#include "moduleAnalysis.h"
#include "SharedBase.h"

#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB

class moduleMainDB
{
public:
    moduleMainDB();
    ~moduleMainDB();

    void                                                       MakeInsertThread( XString sChannel );
    void                                                       InsertChatData( CHAT_DATA data );

    void                                                       AppendChatData( XString sChannel, CHAT_DATA data );

    void                                                       InsertThread( XString sChannel );


private:
    bool                                                       _isStop = false;

    std::map< XString, std::thread >                           _mapNameToThread;
    std::map< XString, std::mutex >                            _mapMutex;
    std::map< XString, std::vector< CHAT_DATA > >              _mapNameToChatData;
};


typedef Shared::Singletons::Singleton<moduleMainDB> TyStMainDB;

#endif