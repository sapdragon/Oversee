#pragma once

#include "Menu.hpp"
#include "../../SDK/SDK.hpp"

namespace SteamManager
{
	void CopyConvert( const uint8_t* rgba, uint8_t* out, const size_t size )
	{
		auto in = reinterpret_cast< const uint32_t* >( rgba );
		auto buf = reinterpret_cast< uint32_t* >( out );
		for ( auto i = 0u; i < ( size / 4 ); ++i )
		{
			const auto pixel = *in++;
			*buf++ = ( pixel & 0xFF00FF00 ) | ( ( pixel & 0xFF0000 ) >> 16 ) | ( ( pixel & 0xFF ) << 16 );
		}
	}

	LPDIRECT3DTEXTURE9 SteamImageBySteamID( CSteamID steam )
	{
		LPDIRECT3DTEXTURE9 asdgsdgadsg;

		int iImage = SDK::Steam::SteamFriends->GetLargeFriendAvatar( steam );
		if ( iImage == -1 )
			return nullptr;

		uint32 uAvatarWidth, uAvatarHeight;
		if ( !SteamUtils->GetImageSize( iImage, &uAvatarWidth, &uAvatarHeight ) )
			return nullptr;

		const int uImageSizeInBytes = uAvatarWidth * uAvatarHeight * 4;
		uint8* pAvatarRGBA = new uint8[ uImageSizeInBytes ];

		if ( !SteamUtils->GetImageRGBA( iImage, pAvatarRGBA, uImageSizeInBytes ) )
		{
			delete[ ] pAvatarRGBA;
			return nullptr;
		}

		auto res = SDK::Interfaces::DirectDevice->CreateTexture( uAvatarWidth, uAvatarHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &asdgsdgadsg, nullptr );

		std::vector<uint8_t> texData;
		texData.resize( uAvatarWidth * uAvatarHeight * 4u );
		CopyConvert( pAvatarRGBA, texData.data( ), uAvatarWidth * uAvatarHeight * 4u );

		D3DLOCKED_RECT rect;
		res = asdgsdgadsg->LockRect( 0, &rect, nullptr, D3DLOCK_DISCARD );
		auto src = texData.data( );
		auto dst = reinterpret_cast< uint8_t* >( rect.pBits );
		for ( auto y = 0u; y < uAvatarHeight; ++y )
		{
			std::copy( src, src + ( uAvatarWidth * 4 ), dst );
			src += uAvatarWidth * 4;
			dst += rect.Pitch;
		}

		res = asdgsdgadsg->UnlockRect( 0 );
		delete[ ] pAvatarRGBA;

		return asdgsdgadsg;
	}

	LPDIRECT3DTEXTURE9 LocalSteamImage( )
	{
		LPDIRECT3DTEXTURE9 asdgsdgadsg;

		int iImage = SDK::Steam::SteamFriends->GetLargeFriendAvatar( SDK::Steam::SteamUser->GetSteamID( ) );
		if ( iImage == -1 )
			return nullptr;

		uint32 uAvatarWidth, uAvatarHeight;
		if ( !SteamUtils->GetImageSize( iImage, &uAvatarWidth, &uAvatarHeight ) )
			return nullptr;

		const int uImageSizeInBytes = uAvatarWidth * uAvatarHeight * 4;
		uint8* pAvatarRGBA = new uint8[ uImageSizeInBytes ];

		if ( !SteamUtils->GetImageRGBA( iImage, pAvatarRGBA, uImageSizeInBytes ) )
		{
			delete[ ] pAvatarRGBA;
			return nullptr;
		}

		auto res = SDK::Interfaces::DirectDevice->CreateTexture( uAvatarWidth, uAvatarHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &asdgsdgadsg, nullptr );

		std::vector<uint8_t> texData;
		texData.resize( uAvatarWidth * uAvatarHeight * 4u );
		CopyConvert( pAvatarRGBA, texData.data( ), uAvatarWidth * uAvatarHeight * 4u );

		D3DLOCKED_RECT rect;
		res = asdgsdgadsg->LockRect( 0, &rect, nullptr, D3DLOCK_DISCARD );

		auto src = texData.data( );
		auto dst = reinterpret_cast< uint8_t* >( rect.pBits );

		for ( auto y = 0u; y < uAvatarHeight; ++y )
		{
			std::copy( src, src + ( uAvatarWidth * 4 ), dst );

			src += uAvatarWidth * 4;
			dst += rect.Pitch;
		}
		res = asdgsdgadsg->UnlockRect( 0 );
		delete[ ] pAvatarRGBA;
		return asdgsdgadsg;
	}
}