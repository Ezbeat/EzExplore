#include "ExploreFile.h"

EzExplore::ExploreFile::ExploreFile()
{
}


EzExplore::ExploreFile::~ExploreFile()
{
}

EzExplore::Errors EzExplore::ExploreFile::StartExploreFile(
    const std::wstring& exploreFolderPath, 
    const ExploreFileCallback& exploreFileCallback
)
{
    Errors retValue = Errors::kUnsuccess;

    std::wstring exploreFolderPath_ = exploreFolderPath;

    HANDLE findHandle = NULL;
    WIN32_FIND_DATA findData = { 0, };
    FileInfo fileInfo;

    auto raii = RAIIRegister([&]
    {
        if (findHandle != NULL)
        {
            if (FindClose(findHandle) == FALSE)
            {
                retValue = Errors::kUnsuccess;
            }
            else
            {
                findHandle = NULL;
            }            
        }
    });

    if (exploreFolderPath_.back() != L'*')
    {
        if (exploreFolderPath_.back() == L'\\')
        {
            exploreFolderPath_.append(L"*");
        }
        else
        {
            exploreFolderPath_.append(L"\\*");
        }
    }

    findHandle = FindFirstFileW(exploreFolderPath_.c_str(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE)
    {
        return retValue;
    }

    exploreFolderPath_.pop_back();

    do 
    {
        fileInfo.fileAttributes = findData.dwFileAttributes;
        fileInfo.creationTime = (static_cast<uint64_t>(findData.ftCreationTime.dwHighDateTime) << (sizeof(uint32_t) * 8)) + findData.ftCreationTime.dwLowDateTime;
        fileInfo.lastAccessTime = (static_cast<uint64_t>(findData.ftLastAccessTime.dwHighDateTime) << (sizeof(uint32_t) * 8)) + findData.ftLastAccessTime.dwLowDateTime;
        fileInfo.lastWriteTime = (static_cast<uint64_t>(findData.ftLastWriteTime.dwHighDateTime) << (sizeof(uint32_t) * 8)) + findData.ftLastWriteTime.dwLowDateTime;
        fileInfo.fileSize = (static_cast<uint64_t>(findData.nFileSizeHigh) << (sizeof(uint32_t) * 8)) + findData.nFileSizeLow;
        fileInfo.fileName = findData.cFileName;
        fileInfo.filePath = (exploreFolderPath_ + fileInfo.fileName);

        retValue = exploreFileCallback(fileInfo);
        if (retValue == Errors::kStopExplore)
        {
            return retValue;
        }
        else
        {
            retValue = Errors::kUnsuccess;
        }

    } while (FindNextFileW(findHandle, &findData) != FALSE);

    if (GetLastError() != ERROR_NO_MORE_FILES)
    {
        return retValue;
    }

    retValue = Errors::kSuccess;
    return retValue;
}
