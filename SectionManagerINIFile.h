#ifndef SectionManagerINIFileH
#define SectionManagerINIFileH


#include "BasicINIFile.h"
#include <string>
#include <deque>

class SectionManagerINIFile : public BasicINIFile
{
public:
	SectionManagerINIFile(void);
	SectionManagerINIFile(const wchar_t* iniFilePath, const wchar_t* section);
	~SectionManagerINIFile(void);

	std::deque<std::wstring>& getCurrentSectionKeys ();

	// Returns the number of sections in the INI file. 
	// PARAMS: 
	// wchar_t* pattern:
	// Only if the pattern matches the section name the section will be counted.
	// Set pattern to NULL to count all sections.
	// int patternStartPos:
	// The start position for the 
	int getSectionCount (const wchar_t* filter, int filterStartPos);

	const wchar_t* getFirstSectionName ();
	const wchar_t* getNextSectionName ();
	// void sectionNamesClose ();

	BOOL keyExists (const wchar_t* key);
	void updateKeys ();
	

	void updateSections ();	
	void invalidateSections ();

protected:
	wchar_t m_filter [MAX_PATH];

	wchar_t keys [32767];

	// std::map<std::wstring, std::wstring> m_sectionList;
	std::deque<std::wstring> m_sectionList;
	std::deque<std::wstring> m_currentSectionKeys;
	BOOL m_sectionsRead;

	std::deque<std::wstring>::iterator m_iter;


};

#endif 