#include "ExploreFile.h"

#include <iostream>
#include <inttypes.h>

EzExplore::Errors exploreFileCallback(
    const EzExplore::FileInfo& fileInfo,
    /*_In_opt_*/ void* userContext
)
{
    if (fileInfo.isDirectory == true)
    {
#ifdef _WIN32
        wprintf(L"[Directory] ");
#elif __linux__
        printf("[Directory] ");
#endif
    }
    else
    {
#ifdef _WIN32
        wprintf(L"[File] FileSize: %" PRIu64 ", ", fileInfo.fileSize);
#elif __linux__
        printf("[File] FileSize: %" PRIu64 ", ", fileInfo.fileSize);
#endif
    }

#ifdef _WIN32
    wprintf(L"%s \n", fileInfo.filePath.c_str());
#elif __linux__
    printf("%s \n", fileInfo.filePath.c_str());
#endif

    if (fileInfo.isDirectory == true)
    {
        return EzExplore::Errors::kEnterDirectory;
    }

    // To stop: EzExplore::Errors::kStopExplore
    /*
        To enter directory: EzExplore::Errors::kEnterDirectory

        if (fileInfo.isDirectory == true)
        {
            return EzExplore::Errors::kEnterDirectory;
        }
    */

    return EzExplore::Errors::kSuccess;
}

int main()
{
    EzExplore::Errors retValue = EzExplore::Errors::kUnsuccess;
    EzExplore::ExploreFile exploreFile;

    uint32_t fileCount = 0;
    uint32_t directoryCount = 0;

#ifdef _WIN32
    // All possible: "D:\\Test", "D:\\Test\\", "D:\\Test\\*"
    exploreFile.GetItemCount(L"test", &fileCount, &directoryCount);
    retValue = exploreFile.StartExploreFile(L"test", exploreFileCallback);
#elif __linux__
    exploreFile.GetItemCount("test", &fileCount, &directoryCount);
    retValue = exploreFile.StartExploreFile("test", exploreFileCallback);
#endif
    if (retValue == EzExplore::Errors::kSuccess)
    {
        std::wcout << L"Success" << std::endl;
    }
    else if (retValue == EzExplore::Errors::kStopExplore)
    {
        std::wcout << L"Stop" << std::endl;
    }
    else if (retValue == EzExplore::Errors::kUnsuccess)
    {
        std::wcout << L"Unsuccess" << std::endl;
    }

    return 0;
}