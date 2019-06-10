#include "BasicINIfile.h"

#include <strsafe.h>
#include <Vsstyle.h>

BasicINIFile::BasicINIFile(const wchar_t* iniFilePath, const wchar_t* section)
{
	setINIFilePath (iniFilePath);
	setSection (section);

	//wchar_t* redirectedINIFilePath = readString ("RedirectSection", "");
	//StringCchCopy (m_iniFilePath, MAX_PATH, redirectedINIFilePath);

	ZeroMemory (m_value, sizeof (m_value));	
}

BasicINIFile::~BasicINIFile()
{
}

void BasicINIFile::setINIFilePath (const wchar_t* iniFilePath)
{
	ZeroMemory (m_iniFilePath, sizeof (wchar_t) * MAX_PATH);
	StringCchCopyW (m_iniFilePath, MAX_PATH, iniFilePath);
}

void BasicINIFile::setSection (const wchar_t* section)
{
	StringCchCopyW (m_section, MAX_PATH, section);	
}

wchar_t* BasicINIFile::getsection ()
{
	return m_section;
}

void BasicINIFile::writeString (const wchar_t* keyName, const wchar_t* value) const
{
	WritePrivateProfileStringW (m_section, keyName, value, m_iniFilePath);
}

wchar_t* BasicINIFile::readString (const wchar_t* keyName, const wchar_t* defaultString)
{		
	GetPrivateProfileStringW (m_section, keyName, defaultString, m_value, MAX_PATH, m_iniFilePath);
	return m_value;
}

wchar_t* BasicINIFile::readStringIntoBuffer (const wchar_t* keyName, const wchar_t* defaultString, wchar_t* buffer, DWORD bufferSize)
{	
	GetPrivateProfileStringW (m_section, keyName, defaultString, buffer, bufferSize, m_iniFilePath);
	return buffer;
}

void BasicINIFile::writeInteger (const wchar_t* keyName, int value) const
{
	wchar_t c [32] = {0};
	_itow_s (value, c, 10);	
	writeString (keyName, c);
}

int BasicINIFile::readInteger (const wchar_t* keyName, int defaultInteger) const
{
	return GetPrivateProfileIntW (m_section, keyName, defaultInteger, m_iniFilePath);
}

void BasicINIFile::writeInteger64 (const wchar_t* keyName, DWORDLONG value) const
{
	wchar_t intValueBuffer [30] = {0};
	_ui64tow_s (value, intValueBuffer, 30, 10);
	writeString (keyName, intValueBuffer);	
}

LONGLONG BasicINIFile::readInteger64 (const wchar_t* keyName, LONGLONG nDefault)
{
	const int BUFFER_SIZE = 30;
	wchar_t lpDefault [BUFFER_SIZE] = {0};
	_i64tow_s (nDefault, lpDefault, 30, 10);	
	readString (keyName, lpDefault);	
	// Convert back to DWORDLONG.
	return _wtoi64 (m_value);
}

ULONGLONG BasicINIFile::readUnsignedNumber64 (const wchar_t* keyName, ULONGLONG nDefault)
{
	const int BUFFER_SIZE = 30;
	wchar_t lpDefault [BUFFER_SIZE] = {0};
	_ui64tow_s (nDefault, lpDefault, 30, 10);	
	readString (keyName, lpDefault);	
	// Convert back to DWORDLONG.
	return (ULONGLONG)_wtoi64 (m_value);
}

	
BOOL BasicINIFile::canwrite() const
{
	const wchar_t* testKey = L"Test";
	writeInteger (testKey, 1);
	return readInteger (testKey, 0);
}
