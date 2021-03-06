#ifndef __H_EXPLOREFILE_H__
#define __H_EXPLOREFILE_H__

#include "ExploreErrors.h"

#include <cstdint>
#include <string>
#include <functional>

#include <Windows.h>

namespace EzExplore
{
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

typedef Errors(*ExploreFileCallback)(_In_ const FileInfo& fileInfo, _In_opt_ void* userContext);

class ExploreFile
{
public:
    ExploreFile();
    ~ExploreFile();

    Errors StartExploreFile(
        _In_ const std::wstring& exploreDirectoryPath,
        _In_ const ExploreFileCallback& exploreFileCallback,
        _In_opt_ void* userContext = nullptr,
        _In_opt_ bool detailFileInfo = false
    );

    Errors GetItemCount(
        _In_ const std::wstring& directoryPath,
        _Out_opt_ uint32_t* fileCount,
        _Out_opt_ uint32_t* directoryCount
    );

private:
    void InitFileInfo_(_Out_ FileInfo& fileInfo);

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