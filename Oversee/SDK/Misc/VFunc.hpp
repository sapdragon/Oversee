#pragma once
#include <cstdint>

#ifndef OVERSEE_DEV

static __declspec( noinline ) void* __fastcall GetXoredD( void* pBaseAddress, int nVirtual )
{
	unsigned long dwVirtual = ( unsigned long ) ( ( void* ) ( ( *( uintptr_t** ) ( pBaseAddress ) )[ _rotr( nVirtual, 28 ) ^ 0xDEADBEEF ] ) );
	__asm
	{
		add dword ptr[ esp ], 1
	}

	return ( void* ) ( _rotl( ( ( dwVirtual ) ^ ( unsigned long ) ( pBaseAddress ) ), 32 ) );
}

typedef void* BIGVOID;
inline auto GetVFunctionPointer( void* pBaseAddr, int nVirtual )
{
	volatile unsigned long dwFunctionAddress = ( unsigned long ) ( &GetXoredD );
	if ( !dwFunctionAddress )
	{
		__asm
		{
			mov edx, dwFunctionAddress
			mov esp, edx
			xor ecx, 0xDEADC0DE
			pop edx
			ror esp, 31
			bswap esp
			imul esp, 30
			bswap esp
		}
	}

	volatile BIGVOID pFunction = ( void* ) ( ( unsigned long ) ( pBaseAddr ) ^ nVirtual );
	if ( !pFunction )
	{
		__asm
		{
			lea esp, dword ptr[ ebp - dwFunctionAddress ]
			xor dword ptr[ esp ], 0xDEADC0DE
			pop ebp
			lea esp, dword ptr[ ebp + 0xDEADBEEF ]
			ror esp, 14
			__asm _emit 0xE8
			__asm _emit 0x01
			__asm _emit 0x00
			__asm _emit 0xC1
			__asm _emit 0xC8
			cmp ebp, esp
			test ebp, esp
			cmp esp, [ ebp ]
		}
	}

	__asm
	{
		ror pFunction, 23
		sub pFunction, 0xD1C3AC3
		rol pFunction, 15
		add pFunction, 0x54F12F43
		ror pFunction, 51
		xor pFunction, 0xBadf00d
		mov ecx, pBaseAddr
		mov edx, nVirtual
		call dwFunctionAddress
		__asm _emit 0xC0
		ror eax, 32
		xor eax, pBaseAddr
		mov pFunction, eax
	}

	return pFunction;
}
#pragma optimize( "", on )

#define VFunc( strFunctionName, nIndex, Type, ...) \
auto strFunctionName { \
return ( reinterpret_cast < Type >( GetVFunctionPointer( this, _rotl( nIndex ^ 0xDEADBEEF, 28 ) ) ) ) ( this, __VA_ARGS__ ); \
}; 

#endif
static auto __declspec( noinline ) GetVFunction_NoObf( void* pBaseAddress, int nFunctionIndex )
{
	return ( *( int** ) ( pBaseAddress ) )[ nFunctionIndex ];
}
#ifdef OVERSEE_DEV
static auto __declspec( noinline ) GetVFunction( void* pBaseAddress, int nFunctionIndex )
{
	return ( *( int** ) ( pBaseAddress ) )[ nFunctionIndex ];
}

#define VFunc( strFunctionName, nIndex, Type, ...) \
auto strFunctionName { \
return ( reinterpret_cast < Type >( GetVFunction( this, nIndex ) ) ) ( this, __VA_ARGS__ ); }; \ 

#endif

