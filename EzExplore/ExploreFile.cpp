#include "ExploreFile.h"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <sys/stat.h>
#include <dirent.h>
#endif

EzExplore::ExploreFile::ExploreFile()
{
}


EzExplore::ExploreFile::~ExploreFile()
{
}

#ifdef _WIN32
EzExplore::Errors EzExplore::ExploreFile::StartExploreFile(
    const std::wstring& exploreDirectoryPath,
    const ExploreFileCallback& exploreFileCallback,
    /*_In_opt_*/ void* userContext /*= nullptr*/,
    /*_In_opt_*/ bool detailFileInfo /*= false */
)
{
    Errors retValue = Errors::kUnsuccess;

    std::wstring exploreDirectoryPath_ = exploreDirectoryPath;

    HANDLE findHandle = NULL;
    WIN32_FIND_DATAW findData = { 0, };
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

        fileInfo.fileSize = (static_cast<uint64_t>(findData.nFileSizeHigh) << (sizeof(uint32_t) * 8)) + findData.nFileSizeLow;
        fileInfo.fileName = findData.cFileName;
        fileInfo.filePath = (exploreDirectoryPath_ + fileInfo.fileName);
        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
            fileInfo.isDirectory = true;
        }

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
#elif __linux__

EzExplore::Errors EzExplore::ExploreFile::StartExploreFile(
    const std::string& exploreDirectoryPath,
    const ExploreFileCallback& exploreFileCallback,
    /*_In_opt_*/ void* userContext /*= nullptr*/,
    /*_In_opt_*/ bool detailFileInfo /*= false */
)
{
    Errors retValue = Errors::kUnsuccess;

    std::string exploreDirectoryPath_ = exploreDirectoryPath;

    DIR* dir = nullptr;
    dirent* file = nullptr;
    struct stat fileStatus = { 0, };
    bool getFileStatus = false;
    FileInfo fileInfo;

    auto raii = RAIIRegister([&]
        {
            if (dir != nullptr)
            {
                if (closedir(dir) != 0)
                {
                    retValue = Errors::kUnsuccess;
                }
                else
                {
                    dir = nullptr;
                }
            }
        });

    if (exploreDirectoryPath_.back() != '/')
    {
        exploreDirectoryPath_.append("/");
    }

    dir = opendir(exploreDirectoryPath_.c_str());
    if (dir == nullptr)
    {
        return retValue;
    }

    while ((file = readdir(dir)) != nullptr)
    {
        if ((file->d_name[0] == '.' && file->d_name[1] == '\x00') ||
            (file->d_name[0] == '.' && file->d_name[1] == '.' && file->d_name[2] == '\x00'))
        {
            continue;
        }

        // Set FileInfo
        InitFileInfo_(fileInfo);

        fileInfo.fileName = file->d_name;
        fileInfo.filePath = exploreDirectoryPath_ + fileInfo.fileName;
        if (stat(fileInfo.filePath.c_str(), &fileStatus) == 0)
        {
            getFileStatus = true;

            fileInfo.fileSize = fileStatus.st_size;
            if (S_ISDIR(fileStatus.st_mode) == true)
            {
                fileInfo.isDirectory = true;
            }
        }

        if (detailFileInfo == true && getFileStatus == true)
        {
            fileInfo.deviceId = fileStatus.st_dev;
            fileInfo.inodeNumber = fileStatus.st_ino;
            fileInfo.mode = fileStatus.st_mode;
            fileInfo.uid = fileStatus.st_uid;
            fileInfo.gid = fileStatus.st_gid;
            fileInfo.lastAccessTime = fileStatus.st_atime;
            fileInfo.lastModificationTime = fileStatus.st_mtime;
            fileInfo.lastStatusChangeTime = fileStatus.st_ctime;
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
    }

    if (errno != 0)
    {
        return retValue;
    }

    retValue = Errors::kSuccess;
    return retValue;
}
#endif

EzExplore::Errors EzExplore::ExploreFile::GetItemCount(
#ifdef _WIN32
    const std::wstring& directoryPath,
#elif __linux__
    const std::string& directoryPath,
#endif

    /*_Out_opt_*/ uint32_t* fileCount,
    /*_Out_opt_*/ uint32_t* directoryCount
)
{
    Errors retValue = Errors::kUnsuccess;

    uint32_t countArray[2] = { 0, }; // Index 0: File Count, Index 1: Directory Count

    auto exploreCallbackLambda = [](const FileInfo& fileInfo, /*_In_opt_*/ void* userContext)->Errors
    {
        if (fileInfo.isDirectory == true)
        {
            (static_cast<uint32_t*>(userContext))[1]++;
            return Errors::kEnterDirectory;
        }

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
    /*_Out_*/ FileInfo& fileInfo
)
{
    // Detail
#ifdef _WIN32
    fileInfo.fileAttributes = 0;
    fileInfo.creationTime = 0;
    fileInfo.lastAccessTime = 0;
    fileInfo.lastWriteTime = 0;
#elif __linux__
    fileInfo.deviceId = 0;
    fileInfo.inodeNumber = 0;
    fileInfo.mode = 0;
    fileInfo.uid = 0;
    fileInfo.gid = 0;
    fileInfo.lastAccessTime = 0;
    fileInfo.lastModificationTime = 0;
    fileInfo.lastStatusChangeTime = 0;
#endif

    // Normal
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