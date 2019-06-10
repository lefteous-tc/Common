#ifndef BasicINIFileH
#define BasicINIFileH

#include <windows.h>

class BasicINIFile
{
public:
	BasicINIFile(const wchar_t* iniFilePath, const wchar_t* section);
	~BasicINIFile();

	void setINIFilePath (const wchar_t* iniFilePath);

	void setSection (const wchar_t* section);
	wchar_t* getsection ();

	void writeString (const wchar_t* keyName, const wchar_t* value) const;
	wchar_t* readString (const wchar_t* keyName, const wchar_t* defaultString);
	wchar_t* readStringIntoBuffer (const wchar_t* keyName, const wchar_t* defaultString, wchar_t* buffer, DWORD bufferSize);

	void writeInteger (const wchar_t* keyName, int value) const;
	int readInteger (const wchar_t* keyName, int defaultInteger) const;
	
	void writeInteger64 (const wchar_t* keyName, DWORDLONG value) const;
	LONGLONG readInteger64 (const wchar_t* keyName, LONGLONG nDefault);

	ULONGLONG readUnsignedNumber64 (const wchar_t* keyName, ULONGLONG nDefault);

	BOOL canwrite() const;

	// Deletes the complete current section.
	// POST: Section representad by m_section is deleted.
	void deleteSection (const wchar_t* newSection);
	
	/*	
	void deleteKey (const wchar_t* keyName);
	*/

	void renameSection (const wchar_t* newSection);



protected:
	wchar_t m_iniFilePath[MAX_PATH];
	wchar_t m_section[MAX_PATH];
	wchar_t m_value[MAX_PATH];

};

#endif