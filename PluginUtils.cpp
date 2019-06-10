#include "PluginUtils.h"

#include "PathUtils.h"
#include "BasicINIFile.h"

#include <strsafe.h>

PluginUtils::PluginSettingsLocation PluginUtils::getPluginSettingsFilePath (HINSTANCE hInstance, const wchar_t* pluginName, const wchar_t* defaultPluginSettingsFilePath,
								wchar_t* pluginSettingsFilePath, int pluginSettingsFilePathLength)
{
	//----Initialization-----------------------------------------------------
	wchar_t iniFileName [MAX_PATH] = {0};
	StringCchCopyW (iniFileName, MAX_PATH, pluginName);
	StringCchCatW (iniFileName, MAX_PATH, L".ini");

	// iniFilePathCandidate is the complete path for testing.
	wchar_t iniFilePathCandidate[MAX_PATH] = {0};

	//-----------Construct first candidate---------(directory where Wincmd.ini is located)
	// Construct the base directory from the predefined INI file path.
	StringCchCopyW (iniFilePathCandidate, MAX_PATH, defaultPluginSettingsFilePath);
	wchar_t* fileNamePos = wcsrchr (iniFilePathCandidate, '\\');
	StringCchCopyNW (iniFilePathCandidate, MAX_PATH, iniFilePathCandidate, wcslen(iniFilePathCandidate) - wcslen(fileNamePos) +1);
	// Append the iniFileName to the base directory path.
	StringCchCatW (iniFilePathCandidate, pluginSettingsFilePathLength, iniFileName);	

	// Store default path.
	StringCchCopyW (pluginSettingsFilePath, MAX_PATH, iniFilePathCandidate);

	// Test the first candidate.
	if (PathUtils::pathExists (iniFilePathCandidate))
	{
		return PluginUtils::PluginSettingsLocation::pslWincmdDir;
	}

	//-----------Construct second candidate (.\DirSizeCalc relative to directory where Wincmd.ini is located)
	StringCchCopyW (iniFilePathCandidate, MAX_PATH, defaultPluginSettingsFilePath);
	fileNamePos = wcsrchr (iniFilePathCandidate, '\\');
	StringCchCopyNW (iniFilePathCandidate, MAX_PATH, iniFilePathCandidate, wcslen(iniFilePathCandidate) - wcslen(fileNamePos) +1);
	StringCchCatW (iniFilePathCandidate , MAX_PATH, pluginName);
	StringCchCatW (iniFilePathCandidate , MAX_PATH, L"\\");
	StringCchCatW (iniFilePathCandidate , MAX_PATH, pluginName);
	StringCchCatW (iniFilePathCandidate , MAX_PATH, L".ini");	
	if (PathUtils::pathExists (iniFilePathCandidate))
	{
		// Apply path as INI file.
		StringCchCopyW (pluginSettingsFilePath, pluginSettingsFilePathLength, iniFilePathCandidate);
		return PluginUtils::PluginSettingsLocation::pslBelowWincmdDir;
	}

	wchar_t moduleFileName[MAX_PATH] = {0};

	//----------Third candidate------We search in the directory where the plugin is located.
	GetModuleFileNameW (hInstance, moduleFileName, MAX_PATH);
	StringCchCopyW (iniFilePathCandidate, MAX_PATH, moduleFileName);
	fileNamePos = wcsrchr (moduleFileName, '\\');
	StringCchCopyNW (iniFilePathCandidate, MAX_PATH, iniFilePathCandidate, wcslen(iniFilePathCandidate) - wcslen(fileNamePos) +1);
	StringCchCatW (iniFilePathCandidate , MAX_PATH, iniFileName);	
	if (PathUtils::pathExists (iniFilePathCandidate))
	{
		// Apply path as INI file.
		StringCchCopyW (pluginSettingsFilePath, MAX_PATH, iniFilePathCandidate);
		return PluginUtils::PluginSettingsLocation::pslPluginDir;
	}

	//----------Fourth candidate------We search in the Total Commander directory.
	GetModuleFileNameW (NULL, moduleFileName, MAX_PATH);
	StringCchCopyW (iniFilePathCandidate, MAX_PATH, moduleFileName);
	fileNamePos = wcsrchr (moduleFileName, '\\');
	StringCchCopyNW (iniFilePathCandidate, MAX_PATH, iniFilePathCandidate, wcslen(iniFilePathCandidate) - wcslen(fileNamePos) +1);
	StringCchCatW (iniFilePathCandidate , MAX_PATH, iniFileName);	
	if (PathUtils::pathExists (iniFilePathCandidate))
	{
		// Apply path as INI file.
		StringCchCopyW (pluginSettingsFilePath, pluginSettingsFilePathLength, iniFilePathCandidate);
		return PluginUtils::PluginSettingsLocation::pslTCDir;
	} 
	return PluginUtils::PluginSettingsLocation::pslNone;
}

BOOL PluginUtils::getTCINIFilePath (const wchar_t* defaultPluginINIFilePath, wchar_t* TCINIFilePath)
{	
	// TC 6.55 introduces a new environment strings which provides the path to its INI file.
	ExpandEnvironmentStringsW (L"%COMMANDER_INI%", TCINIFilePath, MAX_PATH);	
	if (PathUtils::pathExists (TCINIFilePath))
	{
		return TRUE;
	}

	BOOL pathFound = FALSE;

	// Compatibility code for old TC versions <7.
	wchar_t iniFileDir [MAX_PATH] = {0};

	if (!defaultPluginINIFilePath || !wcslen(defaultPluginINIFilePath))
	{
		return FALSE;
	}

	// Prepare INI file directory for common use.
	const wchar_t* fileNamePos = wcsrchr (defaultPluginINIFilePath, '\\');
	if (fileNamePos)
	{
		fileNamePos++;
		StringCchCopyNW (iniFileDir, MAX_PATH, defaultPluginINIFilePath, wcslen(defaultPluginINIFilePath) - wcslen(fileNamePos));
	}

	// Check command line for ini file path.
	if (getTCINIFilePathFromCommandLine (TCINIFilePath))
	{
		// We just need the filename (this simplification let's avoid dealing with environment strings.
		wchar_t* commandLineIniFileName = wcsrchr (TCINIFilePath, '\\');			
		if (commandLineIniFileName)
		{	
			commandLineIniFileName++;
		}
		// Save the pointer before its lost.
		wchar_t commandLineIniFileNameBuffer [MAX_PATH] = {0};
		StringCchCopyW (commandLineIniFileNameBuffer, MAX_PATH, commandLineIniFileName);

		StringCchCopyW (TCINIFilePath, MAX_PATH, iniFileDir);
		StringCchCatW (TCINIFilePath, MAX_PATH, commandLineIniFileNameBuffer);

		pathFound = PathUtils::pathExists (TCINIFilePath);
	}

	// Check UseIniInProgramDir setting.
	int UseIniInProgramDir = 0;
	if (!pathFound)
	{
		wchar_t wincmdINIPath [MAX_PATH] = {0};	
		ExpandEnvironmentStringsW (L"%COMMANDER_PATH%", wincmdINIPath, MAX_PATH);
		WIN32_FIND_DATAW findData;
		ZeroMemory (&findData, sizeof (WIN32_FIND_DATAW));
		StringCchCatW (wincmdINIPath, MAX_PATH, L"\\Wincmd.ini");
		if (PathUtils::pathExists (wincmdINIPath))
		{
			BasicINIFile iniFile (wincmdINIPath, L"Configuration");
			UseIniInProgramDir = iniFile.readInteger (L"UseIniInProgramDir", 0);
			// Check if the Wincmd.ini is read from Total Commander directory and ignores registry settings
			if (UseIniInProgramDir & 5)
			{
				StringCchCopyW (TCINIFilePath, MAX_PATH, wincmdINIPath);
				pathFound = PathUtils::pathExists (TCINIFilePath);
			}
		}
	}

	// Check registry settings.
	// Read setting from current user first.
	if (!pathFound)
	{		
		pathFound = getTCINIFilePathFromRegistry (TCINIFilePath, HKEY_CURRENT_USER, iniFileDir);
	}

	if (!pathFound)
	{		
		pathFound = getTCINIFilePathFromRegistry (TCINIFilePath, HKEY_LOCAL_MACHINE, iniFileDir);
	}

	// Get directory path from contentpluginINIFilePath.	
	if (!pathFound)
	{
		// Try "Wincmd.ini".
		StringCchCopyW (TCINIFilePath, MAX_PATH, iniFileDir);
		StringCchCatW (TCINIFilePath, MAX_PATH, L"Wincmd.ini");
		pathFound = PathUtils::pathExists (TCINIFilePath);
	}	

	if (!pathFound)
	{
		StringCchCopyW (TCINIFilePath, MAX_PATH, L"");
	}
	return pathFound;
}

void PluginUtils::getPathFromPluginSettingsLocation (PluginSettingsLocation settingsLocation, HINSTANCE hInstance, 
													 const wchar_t* pluginName, const wchar_t* defaultPluginSettingsFilePath,
													 wchar_t* path, int pathLength, const wchar_t* belowDirName)
{
	switch (settingsLocation)
	{
	case pslNone:
		break;
	case pslWincmdDir:
		{
			StringCchCopyW (path, pathLength, defaultPluginSettingsFilePath);
			wchar_t* fileNamePos = wcsrchr (path, '\\');
			StringCchCopyNW (path, pathLength, path, wcslen(path) - wcslen(fileNamePos) +1);
			StringCchCatW (path, pathLength, pluginName);
			StringCchCatW (path, pathLength, L".ini");
		}
		break;
	case pslBelowWincmdDir:
		{
			StringCchCopyW (path, pathLength, defaultPluginSettingsFilePath);
			wchar_t* fileNamePos = wcsrchr (path, '\\');
			StringCchCopyNW (path, pathLength, path, wcslen(path) - wcslen(fileNamePos) +1);
			StringCchCatW (path, pathLength, belowDirName);
			StringCchCatW (path, pathLength, L"\\");
			StringCchCatW (path, pathLength, pluginName);
			StringCchCatW (path, pathLength, L".ini");	
		}
		break;
	case pslPluginDir:
		{
			wchar_t moduleFileName[MAX_PATH] = {0};
			GetModuleFileNameW (hInstance, moduleFileName, MAX_PATH);
			StringCchCopyW (path, pathLength, moduleFileName);
			wchar_t* fileNamePos = wcsrchr (moduleFileName, '\\');
			StringCchCopyNW (path, pathLength, path, wcslen(moduleFileName) - wcslen(fileNamePos) +1);
			StringCchCatW (path, pathLength, pluginName);
			StringCchCatW (path, pathLength, L".ini");
		}
		break;
	case pslTCDir:
		{
			wchar_t moduleFileName[MAX_PATH] = {0};
			GetModuleFileNameW (NULL, moduleFileName, MAX_PATH);
			StringCchCopyW (path, pathLength, moduleFileName);
			wchar_t* fileNamePos = wcsrchr (moduleFileName, '\\');
			StringCchCopyNW (path, pathLength, path, wcslen(moduleFileName) - wcslen(fileNamePos) +1);
			StringCchCatW (path, pathLength, pluginName);
			StringCchCatW (path, pathLength, L".ini");
		}
		break;
	}
}

void PluginUtils::getPluginFilePath (const wchar_t* pluginFileName, wchar_t* pluginFilePath)
{
	HMODULE pluginHandle = GetModuleHandleW (pluginFileName);	
	if (!pluginHandle)
	{
		return;
	}
	GetModuleFileNameW (pluginHandle, pluginFilePath, MAX_PATH);
}

void PluginUtils::getPluginDirectoryPath (const wchar_t* pluginFileName, wchar_t* pluginDirectoryPath)
{	
	HMODULE pluginHandle = GetModuleHandleW (pluginFileName);	
	
	if (!pluginHandle)
	{
		return;
	}
	GetModuleFileNameW (pluginHandle, pluginDirectoryPath, MAX_PATH);
	size_t pluginDirectoryPathLen = wcslen (pluginDirectoryPath);
	size_t pluginFileNameLen = wcslen (pluginFileName);
	// Check bounds
	if (pluginDirectoryPathLen || pluginFileNameLen)
	{
		// Set a 0 where the last backslash has been before.
		pluginDirectoryPath [pluginDirectoryPathLen - pluginFileNameLen -1] = 0;
	}
}

BOOL PluginUtils::getTCINIFilePathFromCommandLine (wchar_t* TCINIFilePath)
{	
	wchar_t commandLine[MAX_PATH_UNICODE_BYTES] = { 0 };
	StringCchCopy(commandLine, MAX_PATH_UNICODE_BYTES, GetCommandLineW());
	errno_t convertToLowerCaseResult = _wcslwr_s(commandLine, MAX_PATH_UNICODE_BYTES);
	wchar_t* param = wcsstr (commandLine, L"/i");	
	if (!param)
	{	
		return FALSE;
	}
	size_t commandLineLength = wcslen (commandLine);
	size_t paramLength = wcslen(param);
	if (commandLineLength - paramLength > 0)
	{			
		param--;			
		// Do we start with a " character
		if (*param == '\"')
		{					
			// advance over "/i= (4 characters).
			param +=4;				
			paramLength = wcslen(param);
			// Find next " character.
			wchar_t* paramend = wcschr (param, '\"');
			if (paramend)
			{
				StringCchCopyNW (TCINIFilePath, MAX_PATH, param, paramLength - wcslen(paramend));				
			}				
		}
		// Just the path is sorrounded with " characters
		else
		{				
			// advance over SPACE/i= (4 characters).
			param +=4;				
			if (*param == '\"')
			{					
				param++;					
				paramLength = wcslen(param);
				wchar_t* paramend = wcschr (param, '\"');
				if (paramend)
				{
					StringCchCopyNW (TCINIFilePath, MAX_PATH, param, paramLength - wcslen(paramend));
				}					
			}
			else
			{					
				// advance over SPACE/i= (4 characters).				
				paramLength = wcslen(param);
				wchar_t* paramend = wcschr (param, ' ');
				if (paramend)
				{
					StringCchCopyNW (TCINIFilePath, MAX_PATH, param, paramLength - wcslen(paramend));
				}
				// param is the last parameter.
				else
				{
					StringCchCopyW (TCINIFilePath, MAX_PATH, param);
				}					
			}
		}
	}	
	return TCINIFilePath[0] != 0;
}

BOOL PluginUtils::getTCINIFilePathFromRegistry (wchar_t* TCINIFilePath, HKEY hkey, const wchar_t* iniFileDir)
{
	BOOL pathFound = FALSE;
	HKEY iniFileRegKey = NULL;
	RegOpenKeyExW (hkey, L"Software\\Ghisler\\Total Commander", 0, KEY_QUERY_VALUE, &iniFileRegKey);
	wchar_t regINIPath [MAX_PATH] = {0};
	DWORD bufferSize = MAX_PATH;
	DWORD type = 0;
	if (RegQueryValueExW (iniFileRegKey, L"IniFileName", NULL, &type, (LPBYTE)regINIPath, &bufferSize) == ERROR_SUCCESS)
	{			
		// We just need the filename (this simplification let's avoid dealing with environment strings.
		wchar_t* regIniFileName = wcsrchr (regINIPath, '\\');
		StringCchCopyW (TCINIFilePath, MAX_PATH, iniFileDir);
		if (regIniFileName && wcslen(regIniFileName))
		{	
			regIniFileName++;			
			StringCchCatW (TCINIFilePath, MAX_PATH, regIniFileName);			
		}
		// Is it just a simple filename without relative or absolute paths.
		else if (wcslen(regINIPath))
		{	
			StringCchCatW (TCINIFilePath, MAX_PATH, regINIPath);			
		}
		pathFound = PathUtils::pathExists (TCINIFilePath);
	}
	RegCloseKey (iniFileRegKey);
	return pathFound;
}

void PluginUtils::copyDefaultINIFile(HMODULE hModule, int internalResourceIdentifier, const wchar_t* copyDestination)
{
	// Extract the language resource from the executable.
	HRSRC hRes = FindResourceW (hModule, MAKEINTRESOURCEW(internalResourceIdentifier), L"INI");
	DWORD nSize = SizeofResource (hModule, hRes);
	HGLOBAL loadedRes = LoadResource (hModule, hRes);
	wchar_t* lockedRes = (wchar_t*)LockResource (loadedRes);
	// Save resource to ini file. 
	HANDLE hFile = CreateFileW (copyDestination, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD numberOfBytesWritten = 0;
		WriteFile (hFile, lockedRes, nSize, &numberOfBytesWritten, NULL);
	}
	CloseHandle (hFile);
}

BOOL PluginUtils::getRedirectedTCINIFilePath (const wchar_t* defaultPluginINIFilePath, const wchar_t* section, wchar_t* redirectedTCINIFilePath)
{
	if (!PluginUtils::getTCINIFilePath (defaultPluginINIFilePath, redirectedTCINIFilePath))
	{
		return FALSE;
	}
	// Currently only the "Configuration" section is supported here.
	// TODO: Support all sections.
	if (_wcsicmp (section, L"Configuration") == 0)
	{
		BasicINIFile iniFile (redirectedTCINIFilePath, section);
		// Read the "RedirectSection" value.
		wchar_t* redirectSetting = iniFile.readString (L"RedirectSection", L"0");
		// Check if it is redirected using AlternateUserIni or using a file name.
		if (_wcsicmp (redirectSetting, L"1") == 0)
		{
			// Redirected using AlternateUserIni
			wchar_t* alternateUserIni = iniFile.readString (L"AlternateUserIni", L"");

			// It seems to be redirected using a file
			PathUtils::PathType pathType = PathUtils::getPathType (alternateUserIni);
			switch (pathType)
			{
			case PathUtils::ptFileName:
			case PathUtils::ptRelativePath:
				getRedirectedTCINIFilePathFromFileName (alternateUserIni, redirectedTCINIFilePath);
				break;
			case PathUtils::ptFullPath:
			case PathUtils::ptUNCPath:
				ExpandEnvironmentStringsW (alternateUserIni, redirectedTCINIFilePath, MAX_PATH);
				break;
			}
		}
		else if (_wcsicmp (redirectSetting, L"0") == 0)
		{
			// Not redirected.
			return PathUtils::pathExists (redirectedTCINIFilePath);
		}
		else 
		{
			// It seems to be redirected using a file
			PathUtils::PathType pathType = PathUtils::getPathType (redirectSetting);
			switch (pathType)
			{
			case PathUtils::ptFileName:
			case PathUtils::ptRelativePath:
				getRedirectedTCINIFilePathFromFileName (redirectSetting, redirectedTCINIFilePath);
				break;
			case PathUtils::ptFullPath:
			case PathUtils::ptUNCPath:
				ExpandEnvironmentStringsW (redirectSetting, redirectedTCINIFilePath, MAX_PATH);
				break;
			}
		}
	}
	return PathUtils::pathExists (redirectedTCINIFilePath);
}

BOOL PluginUtils::getRedirectedTCINIFilePathFromFileName (const wchar_t* fileName, wchar_t* TCINIFilePath)
{
	if (!TCINIFilePath)
	{
		return FALSE;
	}
	wchar_t* mainIniFileName = wcsrchr (TCINIFilePath, '\\');
	if (mainIniFileName)
	{
		mainIniFileName++;
		StringCchCopyNW (TCINIFilePath, MAX_PATH, TCINIFilePath, wcslen(TCINIFilePath) - wcslen(mainIniFileName));
		// Append redirected file name.
		StringCchCatW (TCINIFilePath, MAX_PATH, fileName);
	}
	return mainIniFileName != NULL;
}

wchar_t* PluginUtils::getPluginFileNameWithPlatform (wchar_t* name)
{	
	if (sizeof(int*) == 8)
	{
		StringCchCatW (name, MAX_PATH, L"64");		
	}	
	return name;
}