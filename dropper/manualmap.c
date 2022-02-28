#include <Windows.h>
#include "misc.h"
//Zer0Mem0ry's MM

DWORD __stdcall
LibraryLoader(
	LPVOID Memory
)
{

	loaderdata* LoaderParams = ( loaderdata* )Memory;

	PIMAGE_BASE_RELOCATION pIBR = LoaderParams->BaseReloc;

	DWORD delta = ( DWORD )( ( LPBYTE )LoaderParams->ImageBase - LoaderParams->NtHeaders->OptionalHeader.ImageBase ); // Calculate the delta

	while ( pIBR->VirtualAddress )
	{
		if ( pIBR->SizeOfBlock >= sizeof( IMAGE_BASE_RELOCATION ) )
		{
			int count = ( pIBR->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION ) ) / sizeof( WORD );
			PWORD list = ( PWORD )( pIBR + 1 );

			for ( int i = 0; i < count; i++ )
			{
				if ( list[ i ] )
				{
					PDWORD ptr = ( PDWORD )( ( LPBYTE )LoaderParams->ImageBase + ( pIBR->VirtualAddress + ( list[ i ] & 0xFFF ) ) );
					*ptr += delta;
				}
			}
		}

		pIBR = ( PIMAGE_BASE_RELOCATION )( ( LPBYTE )pIBR + pIBR->SizeOfBlock );
	}

	PIMAGE_IMPORT_DESCRIPTOR pIID = LoaderParams->ImportDirectory;

	// Resolve DLL imports
	while ( pIID->Characteristics )
	{
		PIMAGE_THUNK_DATA OrigFirstThunk = ( PIMAGE_THUNK_DATA )( ( LPBYTE )LoaderParams->ImageBase + pIID->OriginalFirstThunk );
		PIMAGE_THUNK_DATA FirstThunk = ( PIMAGE_THUNK_DATA )( ( LPBYTE )LoaderParams->ImageBase + pIID->FirstThunk );

		HMODULE hModule = LoaderParams->fnLoadLibraryA( ( LPCSTR )LoaderParams->ImageBase + pIID->Name );

		if ( !hModule )
			return FALSE;

		while ( OrigFirstThunk->u1.AddressOfData )
		{
			if ( OrigFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
			{
				// Import by ordinal
				DWORD Function = ( DWORD )LoaderParams->fnGetProcAddress( hModule,
					( LPCSTR )( OrigFirstThunk->u1.Ordinal & 0xFFFF ) );

				if ( !Function )
					return FALSE;

				FirstThunk->u1.Function = Function;
			}
			else
			{
				// Import by name
				PIMAGE_IMPORT_BY_NAME pIBN = ( PIMAGE_IMPORT_BY_NAME )( ( LPBYTE )LoaderParams->ImageBase + OrigFirstThunk->u1.AddressOfData );
				DWORD Function = ( DWORD )LoaderParams->fnGetProcAddress( hModule, ( LPCSTR )pIBN->Name );
				if ( !Function )
					return FALSE;

				FirstThunk->u1.Function = Function;
			}
			OrigFirstThunk++;
			FirstThunk++;
		}
		pIID++;
	}

	if ( LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint )
	{
		dllmain EntryPoint = ( dllmain )( ( LPBYTE )LoaderParams->ImageBase + LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint );

		return EntryPoint( ( HMODULE )LoaderParams->ImageBase, DLL_PROCESS_ATTACH, NULL ); // Call the entry point
	}
	return TRUE;
}


DWORD __stdcall
stub( 

)
{
	return 0;
}