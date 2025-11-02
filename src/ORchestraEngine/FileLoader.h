#pragma once

#include <vector>

class FileLoader
{
public:
    FileLoader();
    bool LoadFile(const std::string& filePath);
    bool SaveFile(std::string& data);
    char* GetFileStart() { return &mData[0]; };
    std::string& GetData() { return mData; };
    std::string GetSavedFilePath() { return mFilePath; };

private:
    std::string mData;
    std::string mFilePath;
};
