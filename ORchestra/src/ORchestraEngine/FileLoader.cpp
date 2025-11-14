#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "FileLoader.h"
#include "ScopedTimer.h"

namespace ORchestra {


FileLoader::FileLoader()
{
}

std::string FileLoader::LoadFile(const std::string &filePath)
{
#if _DEBUG
    ScopedTimer timer("Read File");
#endif

    std::ifstream file(filePath, std::ios::binary);
    std::string data;

    if (!file)
    {
        std::cerr << "Could not open file " << filePath << std::endl;
        return data;
    }

    file.seekg(0, std::ios::end);
    long fileSize = static_cast<long>(file.tellg());
    file.seekg(0, std::ios::beg);

    data.resize(static_cast<size_t>(fileSize));
    file.read(&data[0], fileSize);

    file.close();
    return data;
}

bool FileLoader::SaveToFile(const std::string &filePath, const std::string &data)
{
    std::ofstream file{filePath, std::ios::binary};

    if (!file.is_open())
    {
        std::cerr << "Could not save file: " << filePath << std::endl;
        return false;
    }

#if _DEBUG
    ScopedTimer timer("Write File");
#endif

    file.write(data.c_str(), static_cast<std::streamsize>(data.size()));

    file.close();
    return true;
}


} // namespace ORchestra