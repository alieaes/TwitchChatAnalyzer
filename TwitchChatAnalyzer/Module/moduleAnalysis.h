#ifndef __HDR_SHARED_ANALYSIS__
#define __HDR_SHARED_ANALYSIS__

#pragma once

#include <string>

#include "SharedBase.h"

#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB

typedef struct _tyRecvData
{
    XString sMsg;
    QDateTime dtChatTime;
} tyStRecvData;

typedef struct _chatData
{
    XString                   sUserName;
    XString                   sChannel;
    XString                   sChannelUnique;

    XString                   sMsg;

    QDateTime                 dtChatTime;
} CHAT_DATA;

class ModuleAnalysis
{
public:
    ModuleAnalysis();
    ~ModuleAnalysis();

    CHAT_DATA                               ParserChatting( tyStRecvData recvData );
private:

    bool                                    _isPrintChat;
};


typedef Shared::Singletons::Singleton<ModuleAnalysis> TyStAnalysis;

#endif