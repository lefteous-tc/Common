#include "PathUtils.h"
#include <VersionHelpers.h>
#include <strsafe.h>
#include "reparse.h"

BOOL PathUtils::pathExists (const wchar_t* path)
{	
	if (!path)
	{
		return NULL;
	}
	WIN32_FIND_DATAW findData;
	ZeroMemory (&findData, sizeof(WIN32_FIND_DATAW));
	HANDLE hFind = FindFirstFileW (path, &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose (hFind);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL PathUtils::isRelativePath (wchar_t* path)
{
	return wcschr (path, ':') == NULL;
}

BOOL PathUtils::isDirectoryEmpty (const wchar_t* path)
{
	WIN32_FIND_DATAW findData;
	ZeroMemory (&findData, sizeof(WIN32_FIND_DATAW));
	wchar_t subPath [MAX_PATH] = {0};
	StringCchCopyW (subPath, MAX_PATH, path);
	StringCchCatW (subPath, MAX_PATH, L"\\*.*");
	HANDLE findHandle = FindFirstFileW (subPath, &findData);
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	BOOL directoryEmpty = TRUE;
	BOOL found = TRUE;
	while (found && directoryEmpty)
	{
		// Ignore current and upper directory.
		if (!(wcscmp (findData.cFileName, L".") == 0 || wcscmp (findData.cFileName, L"..") == 0))
		{
			directoryEmpty = FALSE;
		}
		found = FindNextFileW (findHandle, &findData);
	}
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		FindClose (findHandle);	
	}	
	return directoryEmpty;
}

BOOL PathUtils::deleteDirectory (const wchar_t* path, BOOL deleteFiles)
{
	BOOL empty = TRUE;
	if (isDirectoryEmpty(path))
	{
		return RemoveDirectoryW (path);
	}
	else
	{
		wchar_t searchPath [MAX_PATH] = {0};
		StringCchCopyW (searchPath, MAX_PATH, path);
		StringCchCatW (searchPath, MAX_PATH, L"\\*.*");
		WIN32_FIND_DATAW findData;
		ZeroMemory (&findData, sizeof (WIN32_FIND_DATAW));
		HANDLE findHandle = FindFirstFileW (searchPath, &findData);
		BOOL found = TRUE;
		while (found)
		{
			wchar_t foundPath [MAX_PATH] = {0};
			StringCchCopyW (foundPath, MAX_PATH, path);
			StringCchCatW (foundPath, MAX_PATH, L"\\");
			StringCchCatW (foundPath, MAX_PATH, findData.cFileName);
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
				{
					deleteDirectory (foundPath, deleteFiles);
				}
			}
			else if (deleteFiles)
			{			
				DeleteFileW (foundPath);
			}
			found = FindNextFileW (findHandle, &findData);
		}
		if (findHandle != INVALID_HANDLE_VALUE)
		{
			FindClose (findHandle);
		}

	}
	if (isDirectoryEmpty(path))
	{
		RemoveDirectoryW (path);
	}
	return empty;
}


int PathUtils::deleteDirectoryFiles (const wchar_t* path)
{
	BOOL empty = TRUE;
	wchar_t searchPath [MAX_PATH] = {0};
	StringCchCopyW (searchPath, MAX_PATH, path);
	StringCchCatW (searchPath, MAX_PATH, L"\\*.*");
	WIN32_FIND_DATAW findData;
	ZeroMemory (&findData, sizeof (WIN32_FIND_DATAW));
	HANDLE findHandle = FindFirstFileW (searchPath, &findData);
	BOOL found = TRUE;
	while (found)
	{
		wchar_t foundPath [MAX_PATH] = {0};
		StringCchCopyW (foundPath, MAX_PATH, path);
		StringCchCatW (foundPath, MAX_PATH, L"\\");
		StringCchCatW (foundPath, MAX_PATH, findData.cFileName);
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			DeleteFileW (foundPath);
		}
		found = FindNextFileW (findHandle, &findData);
	}
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		FindClose (findHandle);
	}
	return empty;
}

BOOL PathUtils::getTempDirectoryPath (const wchar_t* baseDirectoryPath, const wchar_t* prefix, DWORD number, wchar_t* tempDirectoryName)
{
	StringCchCopyW (tempDirectoryName, MAX_PATH, baseDirectoryPath);
	StringCchCatW (tempDirectoryName, MAX_PATH, prefix);
	wchar_t numberString [20] = {0};
	_itow_s (number, numberString, 10);
	StringCchCatW (tempDirectoryName, MAX_PATH, numberString);
	while (pathExists(tempDirectoryName))
	{
		number++;
		StringCchCopyW (tempDirectoryName, MAX_PATH, baseDirectoryPath);
		StringCchCatW (tempDirectoryName, MAX_PATH, prefix);		
		_itow_s (number, numberString, 10);
		StringCchCatW (tempDirectoryName, MAX_PATH, numberString);
	}
	return tempDirectoryName[0];
}

BOOL PathUtils::pathAreOnSameVolume (const wchar_t* path1, const wchar_t* path2)
{	
	BOOL sameVolume = FALSE;
	if (!IsWindowsXPOrGreater())
	{		
		// Check drive root on Win 9x systems
		wchar_t root1 [MAX_PATH] = {0};
		wchar_t root2 [MAX_PATH] = {0};
		StringCchCopyNW (root1, MAX_PATH, path1, 2);
		StringCchCopyNW (root2, MAX_PATH, path2, 2);
		sameVolume = _wcsicmp (root1, root2) == 0;
	}
	else
	{
		// Use system function on Windows NT systems
		HMODULE kernelHandle = LoadLibrary (TEXT("Kernel32.dll"));
		GetVolumePathNameFn pGetVolumePathName = (GetVolumePathNameFn)GetProcAddress(kernelHandle, "GetVolumePathNameW");
		
		if (pGetVolumePathName)
		{
			wchar_t volumePathName1 [MAX_PATH] = {0};
			(*pGetVolumePathName) (path1, volumePathName1, MAX_PATH);			
			wchar_t volumePathName2 [MAX_PATH] = {0};
			(*pGetVolumePathName) (path2, volumePathName2, MAX_PATH);
			sameVolume = _wcsicmp (volumePathName1, volumePathName2) == 0;
		}
		if (kernelHandle)
		{
			FreeLibrary (kernelHandle);
		}
	}
	return sameVolume;
}

wchar_t* PathUtils::getDirectoryPathFromFilePath (const wchar_t* filePath, wchar_t* directoryPath)
{
	const wchar_t* fileName = wcsrchr (filePath, '\\');
	if (fileName && directoryPath)
	{
		fileName++;
		StringCchCopyNW (directoryPath, MAX_PATH, filePath, wcslen(filePath) - wcslen(fileName));
	}	
	return directoryPath;
}


/*
BOOL PathUtils::canBeCreatedHere (wchar_t* path, BOOL directory = FALSE)
{
	BOOL canBeCreatedhere = TRUE;
	if (directory)
	{
		canBeCreatedhere = CreateDirectory (path, NULL) != 0;
		RemoveDirectory (path);
	}
	else
	{
		HANDLE hFile = CreateFile (path, 
	}
}
*/

UNIQUE_FILE_IDENTIFIER PathUtils::getUniqueFileIdentifier (const wchar_t* path)
{
	UNIQUE_FILE_IDENTIFIER id;
	ZeroMemory (&id, sizeof (UNIQUE_FILE_IDENTIFIER));
	
	wchar_t volumePathName [MAX_PATH] = {0};
	if (!IsWindowsXPOrGreater())
	{
		StringCchCopyNW (volumePathName, MAX_PATH, path, 3);
	}
	else
	{		
		HMODULE kernelHandle = LoadLibrary (TEXT("Kernel32.dll"));
		GetVolumePathNameFn pGetVolumePathName = (GetVolumePathNameFn)GetProcAddress(kernelHandle, "GetVolumePathNameW");		
		if (pGetVolumePathName)
		{		
			(*pGetVolumePathName) (path, volumePathName, MAX_PATH);
		}
		if (kernelHandle)
		{
			FreeLibrary (kernelHandle);
		}
	}

	// Get volume serial number.
	BOOL succeeded = GetVolumeInformationW (volumePathName, NULL, 0, &id.volumeSerialNumber, NULL, NULL, NULL, 0);

	// Get file id.
	HANDLE fileHandle = CreateFileW (path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{		
		BY_HANDLE_FILE_INFORMATION fi;
		ZeroMemory (&fi, sizeof (BY_HANDLE_FILE_INFORMATION));
		succeeded = GetFileInformationByHandle (fileHandle, &fi);
		id.fileIndex.LowPart = fi.nFileIndexLow;
		id.fileIndex.HighPart = fi.nFileIndexHigh;
	}
	CloseHandle (fileHandle);
	return id;
}

BOOL PathUtils::isFileValidIdentifier (const UNIQUE_FILE_IDENTIFIER& id)
{
	return id.fileIndex.QuadPart != 0;
}

BOOL PathUtils::areFilesIdentical (const UNIQUE_FILE_IDENTIFIER& id1, const UNIQUE_FILE_IDENTIFIER& id2)
{	
	return PathUtils::isFileValidIdentifier(id1) && PathUtils::isFileValidIdentifier(id2) && 
		id1.fileIndex.QuadPart == id2.fileIndex.QuadPart && id1.volumeSerialNumber == id2.volumeSerialNumber;
}

PathUtils::PathType PathUtils::getPathType (const wchar_t* path)
{
	PathType pathType = ptInvalid;
	if (path)
	{
		const wchar_t* fileName = wcsrchr (path, '\\');
		if (!fileName)
		{
			pathType = ptFileName;
		}
		else
		{
			switch (path[0])
			{
			case '\\':
				pathType = ptUNCPath;
				break;
			case '.':
				pathType = ptRelativePath;
				break;
			case '%':
				pathType = ptFullPath;
				break;
			default:
				if (path[1] == ':')
				{
					pathType = ptFullPath;
				}
			}
		}
	}
	return pathType;
}
//-----------------------------------------------------------------------------
PathUtils::DirectoryType PathUtils::getDirectoryType (const wchar_t* directoryPath, wchar_t* targetDirectoryPath)
{
	DirectoryType directoryType = dtNormal;

	const int MAX_NAME_LENGTH = 1024;
	BYTE reparseBuffer[MAX_REPARSE_SIZE];
	PBYTE reparseData;
	wchar_t name[MAX_NAME_LENGTH];
	ZeroMemory (name, MAX_NAME_LENGTH * sizeof (wchar_t));
	wchar_t name1[MAX_NAME_LENGTH];
	ZeroMemory (name1, MAX_NAME_LENGTH * sizeof (wchar_t));
	PREPARSE_GUID_DATA_BUFFER reparseInfo = (PREPARSE_GUID_DATA_BUFFER) reparseBuffer;
	PREPARSE_DATA_BUFFER msReparseInfo = (PREPARSE_DATA_BUFFER) reparseBuffer;
	DWORD	returnedLength = 0;
	HANDLE fileHandle = NULL;

	fileHandle = CreateFileW( directoryPath, 0,
					FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
					OPEN_EXISTING, 
					FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OPEN_REPARSE_POINT , 0 );
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		CloseHandle (fileHandle);
		return directoryType;
	}

	if (DeviceIoControl( fileHandle, FSCTL_GET_REPARSE_POINT, NULL, 0, reparseInfo, sizeof( reparseBuffer ), &returnedLength, NULL ))
	{
		if (IsReparseTagMicrosoft( reparseInfo->ReparseTag))
		{			
			switch( reparseInfo->ReparseTag ) 
			{
			case IO_REPARSE_TAG_MOUNT_POINT:
				reparseData = (PBYTE) &msReparseInfo->MountPointReparseBuffer.PathBuffer;

				StringCchCopyW (name, msReparseInfo->MountPointReparseBuffer.PrintNameLength,
					(PWCHAR) (reparseData + msReparseInfo->MountPointReparseBuffer.PrintNameOffset));
				
				name[msReparseInfo->MountPointReparseBuffer.PrintNameLength] = 0;
				
				StringCchCopyW (name1, msReparseInfo->MountPointReparseBuffer.SubstituteNameLength,
					(PWCHAR) (reparseData + msReparseInfo->MountPointReparseBuffer.SubstituteNameOffset));

				name1[msReparseInfo->MountPointReparseBuffer.SubstituteNameLength] = 0;
				if (wcsstr( name1, L":" )) 
				{
					directoryType = dtJunction;
				}
				else
				{
					directoryType = dtMountPoint;
				}
				if (targetDirectoryPath)
				{
					StringCchCopyW (targetDirectoryPath, MAX_PATH, name1);
				}	
				break;
			case IO_REPARSE_TAG_SYMLINK:
				directoryType = dtSymbolicLink;
				break;
			}
		}		
	}	
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle (fileHandle);
	}

	return directoryType;
}
//-----------------------------------------------------------------------------

const wchar_t* PathUtils::getExtensionFromFilePath (const wchar_t* filePath)
{
	const wchar_t* extension = wcsrchr (filePath, '.');
	if (extension)
	{
		extension++;
	}
	return extension;
}
//-----------------------------------------------------------------------------
BOOL PathUtils::isPathDirectory (const wchar_t* path)
{
	WIN32_FIND_DATAW findData;
	ZeroMemory (&findData, sizeof(WIN32_FIND_DATAW));
	HANDLE hFind = FindFirstFileW (path, &findData);
	if (hFind != INVALID_HANDLE_VALUE && findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose (hFind);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//-----------------------------------------------------------------------------
void PathUtils::separatePath (const wchar_t* path, BOOL, StringList& paths)
{
	size_t pathLen = wcslen(path);
	const wchar_t* foundPart = wcschr (path, '\\');
	if (foundPart)
	{
		foundPart++;
	}
	size_t copiedPartLen = 0;
	wchar_t part [MAX_PATH] = {0};
	while (foundPart)
	{		
		//StringCchCopy (part, MAX_PATH, foundPart);
		StringCchCopyNW (part, MAX_PATH, path + copiedPartLen, pathLen - wcslen(foundPart) - copiedPartLen);
		copiedPartLen += wcslen(part);
		paths.push (part);
		foundPart++;
		if (foundPart)
		{
			foundPart = wcschr (foundPart, '\\');
			if (foundPart)
			{
				foundPart++;
			}
		}
	}
	// Add remaining part.
	if (copiedPartLen < pathLen)
	{
		StringCchCopyW (part, MAX_PATH, path + copiedPartLen);
		paths.push (part);
	}
}
//-----------------------------------------------------------------------------
void PathUtils::shortenPath (wchar_t* path)
{
	/*
	wchar_t shortenedPath [MAX_PATH] = {0};
	StringCchCopy (shortenedPath, MAX_PATH, path);
	HMODULE shlwapiHandle = LoadLibrary (TEXT("shlwapi.dll"));
	if (shlwapiHandle)
	{
		PathCompactPathAFn pPathCompactPath = (PathCompactPathAFn)GetProcAddress(shlwapiHandle, "PathCompactPathA");
		if (pPathCompactPath)
		{
			SIZE size;
			GetWindowExtEx (GetDC(control), &size);
			WINDOWINFO windowInfo;
			windowInfo.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo (control, &windowInfo);
			int width = windowInfo.rcWindow.right - windowInfo.rcWindow.left;
			width = (int)((double)width * 1.3);
			BOOL ok = (*pPathCompactPath) (GetDC(control), shortenedPath, width);
		}


		FreeLibrary (shlwapiHandle);
	}
	else
	{

	}
	SetWindowText (control, shortenedPath);	
	*/
}
//-----------------------------------------------------------------------------
void PathUtils::collapeEnvironmentString (const wchar_t* expandedPath, wchar_t* collapsedPath, UINT len)
{
	// Get a list of all environment strings for the current process.
	wchar_t candidate [MAX_PATH] = {0};
	StringCchCopyW (candidate, MAX_PATH, expandedPath);

	LPTSTR lpszVariable; 
	LPVOID lpvEnv; 
	lpvEnv = GetEnvironmentStrings();

	// There are no environment strings --> return expandedPath as collapsedPath.
	if (lpvEnv == NULL)
	{
		StringCchCopyW (collapsedPath, MAX_PATH, expandedPath);
		return;
	}		
	 
	for (lpszVariable = (LPTSTR) lpvEnv; *lpszVariable; lpszVariable++) 
	{ 
		// Maximum env. block can be 32768 so the only guarentee is to use such large buffers.
		wchar_t s [32768] = {0};		
		wchar_t v [32768] = {0};		
		WORD i = 0;
		WORD j = 0;
		BOOL value = FALSE;
		while (*lpszVariable) 
		{
			if (*lpszVariable == '=' && !value)
			{
				value = TRUE;
				lpszVariable++;
			}
			if (value)
			{
				v[j] = *lpszVariable;			
				j++;
			}
			else
			{
				s[i] = *lpszVariable;			
				i++;
			}
			lpszVariable++;
		}
		
		// Check if path exists and search for value in expanded path.
		wchar_t expandedPathLow [32768] = {0};
		wchar_t vLow [32768] = {0};
		StringCchCopyW (expandedPathLow, 32768, expandedPath);
		StringCchCopyW (vLow, 32768, v);
		_wcslwr_s (expandedPathLow);
		_wcslwr_s (vLow);
		wchar_t* partString = wcsstr (expandedPathLow, vLow);
		if (PathUtils::pathExists (v) && partString)
		{			
			// Ensure that path actually starts with env. string.
			if (_wcsicmp (partString, expandedPath) == 0)
			{
				// Assemble collapsed path.
				wchar_t newCandidate [MAX_PATH] = {0};
				StringCchCopyW (newCandidate, MAX_PATH, L"%");
				StringCchCatW (newCandidate, MAX_PATH, s);
				StringCchCatW (newCandidate, MAX_PATH, L"%");
				StringCchCatNW (newCandidate, MAX_PATH, expandedPath + wcslen(v), wcslen(expandedPath) - wcslen(v));
				// Then compare length of shortest path with current.
				if (wcslen(newCandidate) < wcslen(candidate))
				{
					StringCchCopyW (candidate, MAX_PATH, newCandidate);
				}
			}						
		}
	}
	// Free resources.
	FreeEnvironmentStrings(LPTSTR(lpvEnv));
	// Apply optimized path.
	StringCchCopyW (collapsedPath, MAX_PATH, candidate);	
}
//-----------------------------------------------------------------------------
void PathUtils::preprendLongFileNamePrefix (const wchar_t* fileName, wchar_t* longFileName, const DWORD longFileNameLen)
{
	if (*fileName == '\\')
	{
		StringCchCopyW (longFileName, longFileNameLen, L"\\\\?\\UNC\\");
		fileName +=2;
	}
	else
	{
		StringCchCopyW (longFileName, longFileNameLen, L"\\\\?\\");
	}	
	StringCchCatW (longFileName, longFileNameLen, fileName);
}
//-----------------------------------------------------------------------------
size_t getNormalizedPath (const wchar_t* path)
{
	size_t result =  wcsspn (path, L"\\..\\");
	if (!result)
	{
		return 5762;
	}
	return result;
}
//-----------------------------------------------------------------------------
const wchar_t* PathUtils::getParentPath(const wchar_t* path)
{
	return NULL;
}
//-----------------------------------------------------------------------------