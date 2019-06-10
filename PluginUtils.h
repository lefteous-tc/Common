#ifndef wxxutilsH
#define wxxutilsH

#include <windows.h>

class PluginUtils
{
public:
enum PluginSettingsLocation {pslNone, pslWincmdDir, pslBelowWincmdDir, pslPluginDir, pslTCDir};

static PluginSettingsLocation getPluginSettingsFilePath (HINSTANCE hInstance, 
														 const wchar_t* pluginName, const wchar_t* defaultPluginSettingsFilePath,
														 wchar_t* pluginSettingsFilePath, int pluginSettingsFilePathLength);

static void getPathFromPluginSettingsLocation (PluginSettingsLocation settingsLocation, HINSTANCE hInstance, const wchar_t* pluginName, 
											   const wchar_t* defaultPluginSettingsFilePath,
											   wchar_t* path, int pathLength, const wchar_t* belowDirName);

static BOOL getTCINIFilePath (const wchar_t* defaultPluginINIFilePath, wchar_t* TCINIFilePath);

static BOOL getRedirectedTCINIFilePath (const wchar_t* defaultPluginINIFilePath, const wchar_t* section, wchar_t* redirectedTCINIFilePath);

static void getPluginFilePath (const wchar_t* pluginFileName, wchar_t* pluginFilePath);

// Returns plugin's directory path (without trailing backslash).
static void getPluginDirectoryPath (const wchar_t* pluginFileName, wchar_t* pluginDirectoryPath);

static void copyDefaultINIFile(HMODULE hModule, int internalResourceIdentifier, const wchar_t* copyDestination);

static wchar_t* getPluginFileNameWithPlatform (wchar_t* name);

protected:

static BOOL getTCINIFilePathFromCommandLine (wchar_t* TCINIFilePath);

static BOOL getTCINIFilePathFromRegistry (wchar_t* TCINIFilePath, HKEY hkey, const wchar_t* iniFileDir);

static BOOL getRedirectedTCINIFilePathFromFileName (const wchar_t* fileName, wchar_t* TCINIFilePath);



};

#endif