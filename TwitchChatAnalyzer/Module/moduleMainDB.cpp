#include "stdafx.h"

#include "moduleMainDB.h"
#include "moduleAnalysis.h"

#include "Sqlite/SharedSQLite.h"

#include "Utils/SharedFile.h"

#include "curl.h"

moduleMainDB::moduleMainDB()
{
}

moduleMainDB::~moduleMainDB()
{
}

void moduleMainDB::MakeInsertThread( XString sChannel )
{
    if( _mapNameToThread.count( sChannel ) > 0 )
        return;

    _mapNameToThread[ sChannel ] = std::thread( std::bind( &moduleMainDB::InsertThread, this, sChannel ) );
}

void moduleMainDB::InsertChatData( CHAT_DATA data )
{
    auto stSqliteMgr = TyStSqliteMgr::GetInstance();

    auto dbChannel = stSqliteMgr->GetDB( data.sChannelUnique );

    XString sQuery = Shared::Format::Format( "INSERT OR REPLACE INTO TBL_CHAT(ChatDateTime, userName, chatMsg) VALUES( datetime(:datetime), :userName, :chaMsg );" );
    Shared::Sqlite::cStmt stmt;

    dbChannel->PrepareSQL( stmt, sQuery );
    dbChannel->BindValue( stmt, ":datetime", data.dtChatTime.toString( DEFAULT_TIME_STRING ) );
    dbChannel->BindValue( stmt, ":userName", data.sUserName );
    dbChannel->BindValue( stmt, ":chaMsg", data.sMsg );
    dbChannel->ExecuteSQL( stmt );
}

void moduleMainDB::AppendChatData( XString sChannel, CHAT_DATA data )
{
    std::lock_guard<std::mutex> lck( _mapMutex[ sChannel ] );
    auto& vec = _mapNameToChatData[ sChannel ];
    vec.push_back( data );
}

void moduleMainDB::InsertThread( XString sChannel )
{
    auto stSqliteMgr = TyStSqliteMgr::GetInstance();

    while( _isStop == false )
    {
        if( _mapNameToChatData[ sChannel ].size() < 100 )
        {
            Sleep( 1000 );
            continue;
        }

        std::vector< CHAT_DATA > vecData;
        {
            std::lock_guard<std::mutex> lck( _mapMutex[ sChannel ] );

            vecData = _mapNameToChatData[ sChannel ];
            _mapNameToChatData[ sChannel ].clear();
        }

        {
            auto dbChannel = stSqliteMgr->GetDB( sChannel.replace( "#", "" ) );

            if( dbChannel == NULLPTR )
            {
                XString sDBJson = Shared::File::GetCurrentPath( true ) + "chatData.json";

                XString sDBPath = Shared::Format::Format( "{}/{}.db", Shared::File::GetCurrentPath(), sChannel.replace( "#", "" ) );

                Shared::Sqlite::DB_INFO dbInfo;
                dbInfo.sDBName = sChannel.replace( "#", "" );
                dbInfo.sFilePath = sDBPath;
                dbInfo.sJsonPath = sDBJson;

                stSqliteMgr->InitDB( dbInfo );

                dbChannel = stSqliteMgr->GetDB( sChannel.replace( "#", "" ) );
            }

            Shared::Sqlite::cDBTransaction t( dbChannel->Data() );

            for( auto data : vecData )
                InsertChatData( data );
        }
    }
}
