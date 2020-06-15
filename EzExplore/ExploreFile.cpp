#include "ExploreFile.h"

EzExplore::ExploreFile::ExploreFile()
{
}


EzExplore::ExploreFile::~ExploreFile()
{
}

EzExplore::Errors EzExplore::ExploreFile::StartExploreFile(
    _In_ const std::wstring& exploreDirectoryPath,
    _In_ const ExploreFileCallback& exploreFileCallback,
    _In_opt_ void* userContext /*= nullptr*/,
    _In_opt_ bool detailFileInfo /*= false */
)
{
    Errors retValue = Errors::kUnsuccess;

    std::wstring exploreDirectoryPath_ = exploreDirectoryPath;

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

    if (exploreDirectoryPath_.back() != L'*')
    {
        if (exploreDirectoryPath_.back() == L'\\')
        {
            exploreDirectoryPath_.append(L"*");
        }
        else
        {
            exploreDirectoryPath_.append(L"\\*");
        }
    }

    findHandle = FindFirstFileW(exploreDirectoryPath_.c_str(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE)
    {
        return retValue;
    }

    exploreDirectoryPath_.pop_back();

    do
    {
        if ((findData.cFileName[0] == L'.' && findData.cFileName[1] == NULL) ||
            (findData.cFileName[0] == L'.' && findData.cFileName[1] == L'.' && findData.cFileName[2] == NULL))
        {
            continue;
        }

        // Set FileInfo
        InitFileInfo_(fileInfo);
        if (detailFileInfo == true)
        {
            fileInfo.fileAttributes = findData.dwFileAttributes;
            fileInfo.creationTime =
                (static_cast<uint64_t>(findData.ftCreationTime.dwHighDateTime) << (sizeof(uint32_t) * 8)) +
                findData.ftCreationTime.dwLowDateTime;
            fileInfo.lastAccessTime =
                (static_cast<uint64_t>(findData.ftLastAccessTime.dwHighDateTime) << (sizeof(uint32_t) * 8)) +
                findData.ftLastAccessTime.dwLowDateTime;
            fileInfo.lastWriteTime =
                (static_cast<uint64_t>(findData.ftLastWriteTime.dwHighDateTime) << (sizeof(uint32_t) * 8)) +
                findData.ftLastWriteTime.dwLowDateTime;
        }

        fileInfo.fileSize = (static_cast<uint64_t>(findData.nFileSizeHigh) << (sizeof(uint32_t) * 8)) + findData.nFileSizeLow;
        fileInfo.fileName = findData.cFileName;
        fileInfo.filePath = (exploreDirectoryPath_ + fileInfo.fileName);
        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
            fileInfo.isDirectory = true;
        }

        retValue = exploreFileCallback(fileInfo, userContext);
        if (retValue == Errors::kEnterDirectory && fileInfo.isDirectory == true)
        {
            retValue = this->StartExploreFile(fileInfo.filePath, exploreFileCallback, userContext, detailFileInfo);
            if (retValue == Errors::kStopExplore)
            {
                return retValue;
            }
        }
        else if (retValue == Errors::kStopExplore)
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

EzExplore::Errors EzExplore::ExploreFile::GetItemCount(
    _In_ const std::wstring& directoryPath,
    _Out_opt_ uint32_t* fileCount,
    _Out_opt_ uint32_t* directoryCount
)
{
    Errors retValue = Errors::kUnsuccess;

    uint32_t countArray[2] = { 0, }; // Index 0: File Count, Index 1: Directory Count

    auto exploreCallbackLambda = [](_In_ const FileInfo& fileInfo, _In_opt_ void* userContext)->Errors
    {
        if (fileInfo.isDirectory == true)
        {
            wprintf(L"[DIR] %s \n", fileInfo.filePath.c_str());
            (static_cast<uint32_t*>(userContext))[1]++;
            return Errors::kEnterDirectory;
        }

        wprintf(L"[FILE] %s \n", fileInfo.filePath.c_str());
        (static_cast<uint32_t*>(userContext))[0]++;
        return Errors::kSuccess;
    };

    if (this->StartExploreFile(directoryPath, exploreCallbackLambda, countArray) != Errors::kSuccess)
    {
        return retValue;
    }

    if (fileCount != nullptr)
    {
        *fileCount = countArray[0];
    }

    if (directoryCount != nullptr)
    {
        *directoryCount = countArray[1];
    }

    retValue = Errors::kSuccess;
    return retValue;
}

void EzExplore::ExploreFile::InitFileInfo_(
    _Out_ FileInfo& fileInfo
)
{
    fileInfo.fileAttributes = 0;
    fileInfo.creationTime = 0;
    fileInfo.lastAccessTime = 0;
    fileInfo.lastWriteTime = 0;
    fileInfo.fileSize = 0;
    if (fileInfo.fileName.length() != 0)
    {
        fileInfo.fileName.clear();
    }
    if (fileInfo.filePath.length() != 0)
    {
        fileInfo.filePath.clear();
    }
    fileInfo.isDirectory = false;
}
