#include "Logger.h"

#include "BasicINIFile.h"
#include "PluginUtils.h"
#include "PathUtils.h"
#include <strsafe.h>


Logger::Logger(const wchar_t* moduleName, const wchar_t* className, const wchar_t* fileName):
m_logFileHandle(NULL)
{
	
	wchar_t folderPath [MAX_PATH] = {0};
	/*
	const wchar_t* name = PluginUtils::getPluginFileNameWithPlatform (moduleName);
	PluginUtils::getPluginDirectoryPath (name, folderPath);
	StringCchCat (folderPath, MAX_PATH, L"\\Logger.ini");
	*/
	StringCchCopyW (folderPath, MAX_PATH, L"C:\\Log\\Logger.ini");

	/*
	const wchar_t* extension = wcsrchr (moduleName, '.');
	StringCchCatN (folderPath, MAX_PATH, moduleName, wcslen(moduleName) - wcslen(extension) +1);	
	StringCchCat (folderPath, MAX_PATH, L"íni");
	*/
	
	BasicINIFile iniFile (folderPath, L"Settings");
	LogType logType = (LogType)iniFile.readInteger (L"LogType", (int)ltNoLog);
	init (fileName, className, logType);
	
}

void Logger::init (const wchar_t* fileName, const wchar_t* className, LogType logType)
{
	if (fileName && logType == ltFile)
	{
		m_logFileHandle = CreateFileW (fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);		
	}
	StringCchCopyW (m_className, MAX_PATH, className);
	m_logType = logType;
	ZeroMemory (m_logText, sizeof(wchar_t) * 1000);	
}

Logger::~Logger()
{
	if (m_logFileHandle)
	{
		CloseHandle( m_logFileHandle);
	}
}

void Logger::log (const wchar_t* functionName, const wchar_t* text, ULONGLONG number)
{
	if (m_logType == ltNoLog)
	{
		return;
	}
	HANDLE semaphore = CreateSemaphoreW (NULL, 1, 1, L"sem");
	WaitForSingleObject (semaphore, INFINITE);
	prepareLogText (text, number, true);
	logAllTypes ();
	ReleaseSemaphore (semaphore, 1, NULL);
}

void Logger::log (const wchar_t* functionName, const wchar_t* text)
{
	if (m_logType == ltNoLog)
	{
		return;
	}
	HANDLE semaphore = CreateSemaphoreW (NULL, 1, 1, L"sem");
	WaitForSingleObject (semaphore, INFINITE);
	prepareLogText (text, 0, false);
	logAllTypes ();
	ReleaseSemaphore (semaphore, 1, NULL);
}

void Logger::logAllTypes ()
{
	if (m_logType == ltFile)
	{		
		DWORD written;
		char buffer [MAX_PATH] = {0};
		WideCharToMultiByte (CP_ACP, 0, m_logText, -1, buffer, MAX_PATH, NULL, NULL);		
		SetFilePointer (m_logFileHandle, 0, 0, FILE_END);
		WriteFile (m_logFileHandle, buffer, strlen(buffer), &written, NULL); 		
		
	}
	else if (m_logType == ltDebugString)
	{
		OutputDebugStringW (m_logText);		
	}
}

void Logger::prepareLogText (const wchar_t* text, ULONGLONG number, bool logNumber)
{
	// Method name
	// StringCchCopy (m_logText, LOG_LEN, m_className);
	// StringCchCat (m_logText, LOG_LEN, L";");

	// Class name
	StringCchCopyW (m_logText, LOG_LEN, m_className);
	StringCchCatW (m_logText, LOG_LEN, L";");

	// Time
	SYSTEMTIME systemTime;
	GetSystemTime (&systemTime);
	wchar_t c [20] = {0};
	_itow_s (systemTime.wHour, c, 10);
	StringCchCatW (m_logText, LOG_LEN, c);
	StringCchCatW (m_logText, LOG_LEN, L":");
	_itow_s (systemTime.wMinute, c, 10);
	StringCchCatW (m_logText, LOG_LEN, c);
	StringCchCatW (m_logText, LOG_LEN, L":");
	_itow_s (systemTime.wSecond, c, 10);
	StringCchCatW (m_logText, LOG_LEN, c);
	StringCchCatW (m_logText, LOG_LEN, L";");

	// Thread info	
	_itow_s (GetCurrentThreadId(), c, 10);
	StringCchCatW (m_logText, LOG_LEN, c);
	StringCchCatW (m_logText, LOG_LEN, L";");
	
	// Text	
	if (text)
	{
		StringCchCatW (m_logText, LOG_LEN, text);
	}
	else
	{
		StringCchCatW (m_logText, LOG_LEN, L"");
	}
	if (logNumber)
	{
		// Numeric info
		StringCchCatW (m_logText, LOG_LEN, L";");	
		_i64tow_s (number, c, 20, 10);
		StringCchCatW (m_logText, LOG_LEN, c);
	}

	// Line end
	StringCchCatW (m_logText, LOG_LEN, L"\n");
}