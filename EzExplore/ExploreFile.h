#ifndef __H_EXPLOREFILE_H__
#define __H_EXPLOREFILE_H__

#include "ExploreErrors.h"

#include <cstdint>
#include <string>
#include <functional>

namespace EzExplore
{
#ifdef _WIN32
struct FileInfoW
{
    FileInfoW()
    {
        fileAttributes = 0;
        creationTime = 0;
        lastAccessTime = 0;
        lastWriteTime = 0;

        fileSize = 0;
        isDirectory = false;
    }

    // Detail
    uint32_t fileAttributes;        // https://docs.microsoft.com/en-us/windows/desktop/FileIO/file-attribute-constants
    uint64_t creationTime;
    uint64_t lastAccessTime;
    uint64_t lastWriteTime;

    // Normal
    uint64_t fileSize;
    std::wstring fileName;
    std::wstring filePath;
    bool isDirectory;
};

struct FileInfoA
{
    FileInfoA()
    {
        fileAttributes = 0;
        creationTime = 0;
        lastAccessTime = 0;
        lastWriteTime = 0;

        fileSize = 0;
        isDirectory = false;
    }

    // Detail
    uint32_t fileAttributes;        // https://docs.microsoft.com/en-us/windows/desktop/FileIO/file-attribute-constants
    uint64_t creationTime;
    uint64_t lastAccessTime;
    uint64_t lastWriteTime;

    // Normal
    uint64_t fileSize;
    std::string fileName;
    std::string filePath;
    bool isDirectory;
};
#elif __linux__
struct FileInfoA
{
    FileInfoA()
    {
        deviceId = 0;
        inodeNumber = 0;
        mode = 0;
        uid = 0;
        gid = 0;
        lastAccessTime = 0;
        lastModificationTime = 0;
        lastStatusChangeTime = 0;

        fileSize = 0;
        isDirectory = false;
    }

    // Detail
    dev_t deviceId;
    ino_t inodeNumber;
    mode_t mode;
    uid_t uid;
    gid_t gid;
    time_t lastAccessTime;
    time_t lastModificationTime;
    time_t lastStatusChangeTime;

    // Normal
    uint64_t fileSize;
    std::string fileName;
    std::string filePath;
    bool isDirectory;
};
#endif

#ifdef _WIN32
typedef Errors(*ExploreFileCallbackW)(const FileInfoW& fileInfo, /*_In_opt_*/ void* userContext);
#endif
typedef Errors(*ExploreFileCallbackA)(const FileInfoA& fileInfo, /*_In_opt_*/ void* userContext);

class ExploreFile
{
public:
    ExploreFile();
    ~ExploreFile();

#ifdef _WIN32
    Errors StartExploreFile(
        const std::wstring& exploreDirectoryPath,
        const ExploreFileCallbackW& exploreFileCallback,
        /*_In_opt_*/ void* userContext = nullptr,
        /*_In_opt_*/ bool detailFileInfo = false
    );
#endif
    Errors StartExploreFile(
        const std::string& exploreDirectoryPath,
        const ExploreFileCallbackA& exploreFileCallback,
        /*_In_opt_*/ void* userContext = nullptr,
        /*_In_opt_*/ bool detailFileInfo = false
    );

#ifdef _WIN32
    Errors GetItemCount(
        const std::wstring& directoryPath,
        /*_Out_opt_*/ uint32_t* fileCount,
        /*_Out_opt_*/ uint32_t* directoryCount
    );
#endif
    Errors GetItemCount(
        const std::string& directoryPath,
        /*_Out_opt_*/ uint32_t* fileCount,
        /*_Out_opt_*/ uint32_t* directoryCount
    );

private:
#ifdef _WIN32
    void InitFileInfo_(/*_Out_*/ FileInfoW& fileInfo);
#endif
    void InitFileInfo_(/*_Out_*/ FileInfoA& fileInfo);

    class RAIIRegister
    {
    private:
        std::function<void()> raiiFunction_;

    public:
        RAIIRegister(std::function<void()> raiiFunc) : raiiFunction_(raiiFunc)
        {

        }
        ~RAIIRegister()
        {
            raiiFunction_();
        }
    };
};
} // namespace EzExplore

#endif // #ifndef __H_EXPLOREFILE_H__