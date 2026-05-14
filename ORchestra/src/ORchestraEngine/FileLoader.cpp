/*
 * Copyright (C) 2026 Christian Tronhjem
 *
 * This file is part of ORchestra.
 *
 * ORchestra is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORchestra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ORchestra. If not, see <https://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <iostream>

#include "FileLoader.h"
#if defined(_DEBUG)
#include "ScopedTimer.h"
#endif

namespace ORchestra 
{
    FileLoader::FileLoader()
    {
    }

    std::string FileLoader::LoadFile(const std::string& filePath)
    {
#if defined(_DEBUG)
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

    bool FileLoader::SaveToFile(const std::string& filePath, const std::string& data)
    {
        std::ofstream file{ filePath, std::ios::binary };

        if (!file.is_open())
        {
            std::cerr << "Could not save file: " << filePath << std::endl;
            return false;
        }

#if defined(_DEBUG)
        ScopedTimer timer("Write File");
#endif

        file.write(data.c_str(), static_cast<std::streamsize>(data.size()));

        file.close();
        return true;
    }
} // namespace ORchestra
