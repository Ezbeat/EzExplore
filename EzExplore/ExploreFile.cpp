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
