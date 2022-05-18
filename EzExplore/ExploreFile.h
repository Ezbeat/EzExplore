#ifndef __H_EXPLOREFILE_H__
#define __H_EXPLOREFILE_H__

#include "ExploreErrors.h"

#include <cstdint>
#include <string>
#include <functional>

#ifdef _WIN32
#elif __linux__
#include <sys/stat.h>
#include <string.h>
#endif

namespace EzExplore
{
#ifdef _WIN32
struct FileInfo
{
    FileInfo()
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
#elif __linux__
struct FileInfo
{
    FileInfo()
    {
        attributes = 0;
        numberOfHardLinks = 0;
        uid = 0;
        gid = 0;
        fileMode = 0;
        inodeNumber = 0;
        memset(&lastAccessTime, 0, sizeof(statx_timestamp));
        memset(&creationTime, 0, sizeof(statx_timestamp));
        memset(&lastAttributeChangeTime, 0, sizeof(statx_timestamp));
        memset(&lastModificationTime, 0, sizeof(statx_timestamp));

        fileSize = 0;
        isDirectory = false;
    }

    // Detail
    uint64_t attributes;
    uint32_t numberOfHardLinks;
    uint32_t uid;
    uint32_t gid;
    uint16_t fileMode;
    uint64_t inodeNumber;
    struct statx_timestamp lastAccessTime;
    struct statx_timestamp creationTime;
    struct statx_timestamp lastAttributeChangeTime;
    struct statx_timestamp lastModificationTime;

    // Normal
    uint64_t fileSize;
    std::string fileName;
    std::string filePath;
    bool isDirectory;
};
#endif

typedef Errors(*ExploreFileCallback)(const FileInfo& fileInfo, /*_In_opt_*/ void* userContext);

class ExploreFile
{
public:
    ExploreFile();
    ~ExploreFile();

    Errors StartExploreFile(
#ifdef _WIN32
        const std::wstring& exploreDirectoryPath,
#elif __linux__
        const std::string& exploreDirectoryPath,
#endif
        const ExploreFileCallback& exploreFileCallback,
        /*_In_opt_*/ void* userContext = nullptr,
        /*_In_opt_*/ bool detailFileInfo = false
    );

    Errors GetItemCount(
#ifdef _WIN32
        const std::wstring& directoryPath,
#elif __linux__
        const std::string& directoryPath,
#endif
        /*_Out_opt_*/ uint32_t* fileCount,
        /*_Out_opt_*/ uint32_t* directoryCount
    );

private:
    void InitFileInfo_(/*_Out_*/ FileInfo& fileInfo);

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
