#pragma once

#include <windows.h>

const int LOG_LEN = 1024;

enum LogType {ltNoLog, ltDebugString, ltFile};

class Logger
{
public:	

	// Create a new logger.
	Logger(const wchar_t* moduleName, const wchar_t* className, const wchar_t* fileName = NULL);
	~Logger(void);	

	// Logs a text
	void log (const wchar_t* functionName, const wchar_t* text);

	// Logs a text and a number.
	void log (const wchar_t* functionName, const wchar_t* text, ULONGLONG number);
private:
	wchar_t m_className[MAX_PATH];
	HANDLE m_logFileHandle;
	LogType m_logType;
	wchar_t m_logText [LOG_LEN];

	void prepareLogText (const wchar_t* text, ULONGLONG number, bool logNumber);	
	void logAllTypes ();
	void init (const wchar_t* fileName, const wchar_t* className, LogType logType);

};
