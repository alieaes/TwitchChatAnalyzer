#ifndef __HDR_SHARED_TWITCH_API__
#define __HDR_SHARED_TWITCH_API__

#pragma once

#include <string>

#include "SharedBase.h"

#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB

typedef unsigned int                           BROADCASTER_ID;
typedef unsigned int                           USER_ID;

class moduleTwitchAPI
{
public:
    moduleTwitchAPI();
    ~moduleTwitchAPI();

    bool                                       GetAccessToken();
    bool                                       GetClientId();
    bool                                       GetClientSecretId();

    std::vector< XString >                     GetEmoteList( BROADCASTER_ID bId );
    bool                                       IsOnline( USER_ID bId );

private:
    XString                                    _sAccessToken;
    XString                                    _sClientId;
    XString                                    _sClientSecretId;
};


typedef Shared::Singletons::Singleton<moduleTwitchAPI> TyStAPI;

#endif