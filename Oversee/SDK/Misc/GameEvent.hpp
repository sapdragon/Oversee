#pragma once

class bf_write;
class bf_read;

class IGameEvent
{
public:
    virtual                 ~IGameEvent() = 0;
    virtual const char*     GetName() const = 0;

    virtual bool            IsReliable() const = 0;
    virtual bool            IsLocal() const = 0;
    virtual bool            IsEmpty(const char* keyName = nullptr) = 0;

    virtual bool            GetBool(const char* keyName = nullptr, bool defaultValue = false) = 0;
    virtual int             GetInt(const char* keyName = nullptr, int defaultValue = 0) = 0;
    virtual __int64         GetUint64(const char* keyName = nullptr, unsigned long defaultValue = 0) = 0;
    virtual float           GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) = 0;
    virtual const char*     GetString(const char* keyName = nullptr, const char* defaultValue = "") = 0;
    virtual const wchar_t*  GetWString(const char* keyName, const wchar_t* defaultValue = L"") = 0;

    virtual void            SetBool(const char* keyName, bool value) = 0;
    virtual void            SetInt(const char* keyName, int value) = 0;
    virtual void            SetUint64(const char* keyName, unsigned long value) = 0;
    virtual void            SetFloat(const char* keyName, float value) = 0;
    virtual void            SetString(const char* keyName, const char* value) = 0;
    virtual void            SetWString(const char* keyName, const char* value) = 0;
};