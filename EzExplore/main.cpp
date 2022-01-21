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
        printf("[Directory] ");
    }
    else
    {
        printf("[File] FileSize: %" PRIu64 ", ", fileInfo.fileSize);
    }

    printf("%s \n", fileInfo.filePath.c_str());

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

    // [Windows] All possible: "D:\\Test", "D:\\Test\\", "D:\\Test\\*"
#ifdef _WIN32
    // Unicode Test
    exploreFile.GetItemCount(L"test", &fileCount, &directoryCount);
    retValue = exploreFile.StartExploreFile("test", exploreFileCallback);
    if (retValue == EzExplore::Errors::kSuccess)
    {
        printf("Success \n");
    }
    else if (retValue == EzExplore::Errors::kStopExplore)
    {
        printf("Stop \n");
    }
    else if (retValue == EzExplore::Errors::kUnsuccess)
    {
        printf("Unsuccess \n");
    }
#endif
    // Ansi Test
    exploreFile.GetItemCount("test", &fileCount, &directoryCount);
    retValue = exploreFile.StartExploreFile("test", exploreFileCallback);
    if (retValue == EzExplore::Errors::kSuccess)
    {
        printf("Success \n");
    }
    else if (retValue == EzExplore::Errors::kStopExplore)
    {
        printf("Stop \n");
    }
    else if (retValue == EzExplore::Errors::kUnsuccess)
    {
        printf("Unsuccess \n");
    }

    return 0;
}