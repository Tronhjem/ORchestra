#pragma once

#include <vector>

class FileLoader
{
public:
    FileLoader();
    std::string LoadFile(const std::string& filePath);
    bool SaveToFile(const std::string& filePath, const std::string& data);
};
