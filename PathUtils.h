#include <windows.h>
#include "StringList.h"

#ifndef PathUtilsH
#define PathUtilsH

const int MAX_PATH_UNICODE_BYTES = MAX_PATH * 2;

typedef WINBASEAPI BOOL (WINAPI* GetVolumePathNameFn)
	(
    LPCWSTR lpszFileName,
    LPWSTR lpszVolumePathName,
    DWORD cchBufferLength
    );

struct UNIQUE_FILE_IDENTIFIER
{
	DWORD volumeSerialNumber;
	ULARGE_INTEGER fileIndex;
};


class PathUtils
{

public: 
	static BOOL pathExists (const wchar_t* path);
	static BOOL isRelativePath (wchar_t* path);
	static BOOL isDirectoryEmpty (const wchar_t* path);
	static BOOL deleteDirectory (const wchar_t* path, BOOL deleteFiles);
	static int deleteDirectoryFiles (const wchar_t* path);
	static BOOL getTempDirectoryPath (const wchar_t* baseDirectoryPath, const wchar_t* prefix, DWORD number, wchar_t* tempDirectoryName);
	static BOOL pathAreOnSameVolume (const wchar_t* path1, const wchar_t* path2);
	static wchar_t* getDirectoryPathFromFilePath (const wchar_t* filePath, wchar_t* directoryPath);
	static const wchar_t* getExtensionFromFilePath (const wchar_t* filePath);
	static UNIQUE_FILE_IDENTIFIER getUniqueFileIdentifier (const wchar_t* path);
	static BOOL isFileValidIdentifier (const UNIQUE_FILE_IDENTIFIER& id);
	static BOOL isPathDirectory (const wchar_t* path);
	static void separatePath (const wchar_t* path, BOOL absolutePath, StringList& paths);
	static void shortenPath (wchar_t* path);
	static void collapeEnvironmentString (const wchar_t* expandedPath, wchar_t* collapsedPath, UINT len);
	static void preprendLongFileNamePrefix (const wchar_t* fileName, wchar_t* longFileName, const DWORD longFileNameLen);
	static size_t getNormalizedPath (const wchar_t* path);
	static const wchar_t* getParentPath(const wchar_t* path); 
	

	static BOOL areFilesIdentical (const UNIQUE_FILE_IDENTIFIER& id1, const UNIQUE_FILE_IDENTIFIER& id2);

	enum PathType {ptInvalid, ptFullPath, ptUNCPath, ptRelativePath, ptFileName};
	static PathType getPathType (const wchar_t* path);

	enum DirectoryType {dtNormal, dtJunction, dtMountPoint, dtSymbolicLink};
	static DirectoryType getDirectoryType (const wchar_t* directoryPath, wchar_t* targetDirectoryPath = NULL);


// PRE: Path may not exist already.
//static BOOL canBeCreatedHere (wchar_t* path, BOOL directory = FALSE);

};

#endif
