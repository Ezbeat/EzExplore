#include "ExploreFile.h"

#include <iostream>

EzExplore::Errors exploreFileCallback(
    _In_ const EzExplore::FileInfo& fileInfo, 
    _In_opt_ void* userContext
)
{
    if (fileInfo.isDirectory == true)
    {
        std::wcout << L"[Directory] ";
    }
    else
    {
        std::wcout << L"[File] FileSize: " << fileInfo.fileSize << L", ";
    }

    std::wcout << fileInfo.filePath << std::endl;

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

    // All possible: "D:\\Test", "D:\\Test\\", "D:\\Test\\*"
    retValue = exploreFile.StartExploreFile(L"D:\\Test", exploreFileCallback);
    if(retValue == EzExplore::Errors::kSuccess)
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