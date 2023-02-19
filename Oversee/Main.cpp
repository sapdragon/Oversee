#include "SDK/SDK.hpp"
#include "Hooks/Hooks.hpp"

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved  )
{
    if ( dwReasonForCall == DLL_PROCESS_ATTACH )
        SDK::Initialize( hModule );

    return TRUE;
}