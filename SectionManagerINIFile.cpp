#include "SectionManagerINIFile.h"

#include <strsafe.h>

SectionManagerINIFile::SectionManagerINIFile(const wchar_t* iniFilePath, const wchar_t* section):
BasicINIFile(iniFilePath, section),
m_sectionsRead(FALSE)
//m_sectionEnumSize(16384),
//m_sectionEnum(NULL),
//m_sectionEnumStart(NULL),
//m_pCurrentSection(NULL)
{
	ZeroMemory (keys, sizeof(wchar_t) * 32767);
	ZeroMemory (m_filter, sizeof(wchar_t) * MAX_PATH);
}

SectionManagerINIFile::~SectionManagerINIFile()
{
}

int SectionManagerINIFile::getSectionCount (const wchar_t* filter, int filterStartPos)
{
	int sectionCount = 0;
	// Restart procesdure
	const wchar_t* section = getFirstSectionName ();
	while (section)
	{
		// Only increase section counter when filter matches.
		if (filter)
		{
			if (wcslen(section) >= (size_t)filterStartPos)
			{
				section += filterStartPos;
			}
			const wchar_t* found = wcsstr (section, filter);
			if (found)
			{
				sectionCount++;
			}
		}
		else
		{
			sectionCount++;
		}
		section = getNextSectionName ();
	}
	return sectionCount;
}

const wchar_t* SectionManagerINIFile::getFirstSectionName ()
{
	if (!m_sectionsRead)
	{
		updateSections ();
	}
	
	
	m_iter = m_sectionList.begin();	
	return m_iter->c_str();
}

const wchar_t* SectionManagerINIFile::getNextSectionName ()
{
	m_iter++;
	if (m_iter == m_sectionList.end()) 
	{
		return NULL;
	}
	return m_iter->c_str();
	
}

void SectionManagerINIFile::updateSections ()
{
	const int SECTION_BUFFER_MAX = 0x10000;
	wchar_t sectionEnum [SECTION_BUFFER_MAX] = {0};
	DWORD realSectionEnumSize = GetPrivateProfileSectionNamesW (sectionEnum, SECTION_BUFFER_MAX, m_iniFilePath);
	BOOL bufferTooSmall = realSectionEnumSize == (SECTION_BUFFER_MAX -2);
	if (bufferTooSmall)
	{
		return;
	}

	wchar_t* currentSection = sectionEnum;
	m_sectionList.clear ();	

	// Now we start reading the buffer.	
	BOOL moreSections = TRUE;
	while (currentSection && moreSections)
	{
		size_t sectionLength = wcslen(currentSection);
		if (sectionLength)
		{
			// Fill section StringList.
			// m_sectionList.push (currentSection);
			m_sectionList.push_back (currentSection);

			currentSection += sectionLength +1;
		}
		else
		{
			moreSections = FALSE;
		}
	}
	m_sectionsRead = TRUE;
}

void SectionManagerINIFile::invalidateSections ()
{
	m_sectionList.clear ();
	m_sectionsRead = FALSE;
}

BOOL SectionManagerINIFile::keyExists (const wchar_t* key)
{	
	BOOL keyFound = FALSE;
	wchar_t* currentKey = keys;
	while (!keyFound && currentKey[0])
	{
		if (wcsstr(currentKey, key) != NULL)
		{
			keyFound = TRUE;
		}
		currentKey += wcslen (currentKey) +1;
	}
	return keyFound;
}

void SectionManagerINIFile::updateKeys ()
{
	GetPrivateProfileSectionW (m_section, keys, 32767, m_iniFilePath);
	wchar_t* currentKey = keys;
	while (currentKey[0])
	{
		m_currentSectionKeys.push_back (currentKey);		
		currentKey += wcslen (currentKey) +1;
	}
}

std::deque<std::wstring>& SectionManagerINIFile::getCurrentSectionKeys ()
{
	updateKeys ();
	return m_currentSectionKeys;
}