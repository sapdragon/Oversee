#pragma once

class C_KeyValues
{
public:
    void* operator new( size_t allocatedsize );
    void operator delete( void* mem );

    C_KeyValues( const char* strName )
    {
        Init( );
        SetName( strName );
    }

    void SetName( const char* strName )
    {
        m_iKeyName = 2;
    }

    enum types_t
    {
        TYPE_NONE = 0,
        TYPE_STRING,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_PTR,
        TYPE_WSTRING,
        TYPE_COLOR,
        TYPE_UINT64,
        TYPE_NUMTYPES,
    };

	int m_iKeyName; // keyname is a symbol defined in KeyValuesSystem

    char* m_sValue;
    wchar_t* m_wsValue;

    union
    {
        int m_iValue;
        float m_flValue;
        void* m_pValue;
        unsigned char m_Color[4];
    };

    char       m_iDataType;
    char       m_bHasEscapeSequences; // true, if while parsing this KeyValue, Escape Sequences are used (default false)
    char       m_bEvaluateConditionals; // true, if while parsing this KeyValue, conditionals blocks are evaluated (default true)
    char       unused[ 1 ];

    C_KeyValues* m_pPeer; // pointer to next key in list
    C_KeyValues* m_pSub;  // pointer to Start of a new sub key list
    C_KeyValues* m_pChain;// Search here if it's not in our list

    template<class CDATA> CDATA GetValueByKey( const char* keyname )
    {
        C_KeyValues* pFind = FindKey( keyname, false );
        if ( pFind )
        {
            CDATA return_value;
            std::stringstream ss( GetString( this, keyname, NULL ) );
            ss >> return_value;
            return return_value;
        }
        else return NULL;
    }

    void Init( );
    C_KeyValues* FindKey( const char* keyName, bool bCreate );
    const char* GetString( C_KeyValues* pThis, const char* keyName, const char* defaultValue );
    bool LoadFromBuffer( C_KeyValues* pThis, const char* pszFirst, const char* pszSecond, void* pSomething = 0, void* pAnother = 0, void* pLast = 0, void* pAnother2 = 0 );
    void SetString( const char* name, const char* value );
    void SetInt( const char* name, int value );
    void SetFloat( const char* name, float value );
};