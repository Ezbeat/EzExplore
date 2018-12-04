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
    }

    uint32_t fileAttributes;        // https://docs.microsoft.com/en-us/windows/desktop/FileIO/file-attribute-constants
    uint64_t creationTime;
    uint64_t lastAccessTime;
    uint64_t lastWriteTime;
    uint64_t fileSize;
    std::wstring fileName;
    std::wstring filePath;
};

typedef Errors(*ExploreFileCallback)(const FileInfo& fileInfo);

class ExploreFile
{
public:
    ExploreFile();
    ~ExploreFile();

    Errors StartExploreFile(const std::wstring& exploreFolderPath, const ExploreFileCallback& exploreFileCallback);

private:
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
}


#endif // #ifndef __H_EXPLOREFILE_H__