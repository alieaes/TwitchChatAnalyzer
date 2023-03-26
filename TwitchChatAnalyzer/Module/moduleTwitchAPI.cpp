#include "stdafx.h"

#include "moduleTwitchAPI.h"

// Python과 Qt를 같이 사용하는 경우 slots define에서 충돌이 발생함. 다음과 같이 사용하도록 해야함.
// 또한, 디버그로 사용하는 경우에는 pyhton()_d.lib를 찾게되는데, 소스코드 빌드해서 사용해야함.
// 귀찮다면 pyconfig.h, 에서 python()_d.lib를 python().lib로 변경하고
// object.h에서 #  define Py_REF_DEBUG 부분을 주석처리 해주자.
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

#include "Utils/SharedFile.h"

#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#undef _DEBUG
#endif

#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG 1
#endif

#include "Json/nlohmann/json.hpp"

moduleTwitchAPI::moduleTwitchAPI()
{
    Py_Initialize();
    XString sPath = Shared::Format::Format( "sys.path.append(r'{}')", Shared::File::GetCurrentPath() );
    PyRun_SimpleString( "import sys" );
    PyRun_SimpleString( sPath.toString().c_str() );
}

moduleTwitchAPI::~moduleTwitchAPI()
{
    Py_Finalize();
}

bool moduleTwitchAPI::GetAccessToken()
{
    bool isSuccess = false;

    do
    {
        PyObject* pName, * pModule, * pFunc, * pValue;

        pName = PyUnicode_FromString( "twitchAPI" ); // testFile.py를 PyObject로 생성한다.
        pModule = PyImport_Import( pName ); // 생성한 PyObject pName을 import한다.
        pFunc = PyObject_GetAttrString( pModule, "GetAuthAccessToken" ); // 실행할 함수인 test_func을 PyObject에 전달한다. 
        pValue = PyObject_CallObject( pFunc, NULL ); // pFunc에 매개변수를 전달해서 실행한다. 현재 매개변수가 NULL인 경우이다.

        XString sValue = PyUnicode_AsUTF8( pValue );
        
        if( sValue.contains( "access_token", true ) == true )
        {
            isSuccess = true;

            nlohmann::json json = nlohmann::json::parse( sValue.toString() );

            XString sAccessToken = json[ "access_token" ].get<std::string>();

            if( sAccessToken.IsEmpty() == false )
            {
                _sAccessToken = sAccessToken;
                isSuccess = true;
            }
        }
    }
    while( false );

    return isSuccess;
}

bool moduleTwitchAPI::GetClientId()
{
    bool isSuccess = false;

    do
    {
        PyObject* pName, *pModule, *pFunc, *pValue;

        pName = PyUnicode_FromString( "twitchAPI" ); 
        pModule = PyImport_Import( pName );
        pFunc = PyObject_GetAttrString( pModule, "GetClientId" ); 
        pValue = PyObject_CallObject( pFunc, NULL );

        XString sValue = PyUnicode_AsUTF8( pValue );

        if( sValue.IsEmpty() == false )
        {
            _sClientId = sValue;
            isSuccess = true;
        }

    } while( false );

    return isSuccess;
}

bool moduleTwitchAPI::GetClientSecretId()
{
    bool isSuccess = false;

    do
    {
        PyObject* pName, * pModule, * pFunc, * pValue;

        pName = PyUnicode_FromString( "twitchAPI" );
        pModule = PyImport_Import( pName );
        pFunc = PyObject_GetAttrString( pModule, "GetClientSecretId" );
        pValue = PyObject_CallObject( pFunc, NULL );

        XString sValue = PyUnicode_AsUTF8( pValue );

        if( sValue.IsEmpty() == false )
        {
            _sClientSecretId = sValue;
            isSuccess = true;
        }

    } while( false );

    return isSuccess;
}

std::vector< XString > moduleTwitchAPI::GetEmoteList( BROADCASTER_ID bId )
{
    bool isSuccess = false;
    std::vector< XString > vecList;

    do
    {
        PyObject* pArgs;
        PyObject* pName, * pModule, * pFunc, * pValue;

        XString nId = bId;
        pArgs = PyTuple_New( 1 );
        PyTuple_SetItem( pArgs, 0, PyUnicode_FromString( nId.toString().c_str() ) );

        pName = PyUnicode_FromString( "twitchAPI" );
        pModule = PyImport_Import( pName );
        pFunc = PyObject_GetAttrString( pModule, "GetEmoteList" );
        pValue = PyObject_CallObject( pFunc, pArgs );

        int nSize = PyList_Size( pValue );
        if( nSize > 0 )
        {
            for( int idx = 0; idx < nSize; idx++ )
                vecList.push_back( PyUnicode_AsUTF8( PyList_GetItem( pValue, idx ) ) );
        }

    } while( false );

    return vecList;
}

bool moduleTwitchAPI::IsOnline( USER_ID bId )
{
    bool isOnline = false;

    do
    {
        PyObject* pArgs;
        PyObject* pName, * pModule, * pFunc, * pValue;

        XString nId = bId;
        pArgs = PyTuple_New( 1 );
        PyTuple_SetItem( pArgs, 0, PyUnicode_FromString( nId.toString().c_str() ) );

        pName = PyUnicode_FromString( "twitchAPI" );
        pModule = PyImport_Import( pName );
        pFunc = PyObject_GetAttrString( pModule, "IsOnline" );
        pValue = PyObject_CallObject( pFunc, pArgs );

        isOnline = PyObject_IsTrue( pValue );

    } while( false );

    return isOnline;
}
