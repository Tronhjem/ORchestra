#pragma once

#include "../catch.hpp"
#include "FileLoader.h"
#include <fstream>

TEST_CASE("FileLoader: SaveToFile() and LoadFile() roundtrip preserves simple text 'Hello, ORchestra!'", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_simple.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData = "Hello, ORchestra!";
            
            // Save file
            bool saved = loader.SaveToFile(testPath, testData);
            REQUIRE(saved);
            
            // Load file
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData);
            
            // Cleanup
            tempFile.deleteFile();
        }

TEST_CASE("FileLoader: SaveToFile() and LoadFile() roundtrip preserves multi-line text with newlines", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_multiline.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData = "line1\nline2\nline3";
            
            bool saved = loader.SaveToFile(testPath, testData);
            REQUIRE(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData);
            
            tempFile.deleteFile();
        }

TEST_CASE("FileLoader: SaveToFile() and LoadFile() roundtrip preserves empty string", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_empty.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData = "";
            
            bool saved = loader.SaveToFile(testPath, testData);
            REQUIRE(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData);
            
            tempFile.deleteFile();
        }

TEST_CASE("FileLoader: SaveToFile() and LoadFile() roundtrip preserves ORchestra script syntax", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_script.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData = "a = [1, 2, 3]\nb = a + 10\ntest b";
            
            bool saved = loader.SaveToFile(testPath, testData);
            REQUIRE(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData);
            
            tempFile.deleteFile();
        }

TEST_CASE("FileLoader: LoadFile() returns empty string for non-existent file", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("this_file_does_not_exist_12345.txt");
            std::string nonExistentPath = tempFile.getFullPathName().toStdString();
            
            std::string loadedData = loader.LoadFile(nonExistentPath);
            REQUIRE(loadedData.empty());
        }

TEST_CASE("FileLoader: SaveToFile() and LoadFile() roundtrip preserves special characters", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_special.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData = "!@#$%^&*()_+-=[]{}|;:',.<>?/`~";
            
            bool saved = loader.SaveToFile(testPath, testData);
            REQUIRE(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData);
            
            tempFile.deleteFile();
        }

TEST_CASE("FileLoader: SaveToFile() and LoadFile() roundtrip preserves numeric string '0123456789'", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_numbers.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData = "0123456789";
            
            bool saved = loader.SaveToFile(testPath, testData);
            REQUIRE(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData);
            
            tempFile.deleteFile();
        }

TEST_CASE("FileLoader: SaveToFile() overwrites existing file with new content", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_overwrite.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData1 = "First content";
            std::string testData2 = "Second content";
            
            // Save first content
            loader.SaveToFile(testPath, testData1);
            
            // Overwrite with second content
            bool saved = loader.SaveToFile(testPath, testData2);
            REQUIRE(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData2);
            REQUIRE(loadedData != testData1);
            
            tempFile.deleteFile();
        }

TEST_CASE("FileLoader: SaveToFile() and LoadFile() roundtrip preserves 100-line content", "[FileLoader]")
{
            
            FileLoader loader;
            File tempFile = File::getSpecialLocation(File::tempDirectory).getChildFile("orchestra_test_long.txt");
            std::string testPath = tempFile.getFullPathName().toStdString();
            std::string testData;
            
            // Create a longer string
            for (int i = 0; i < 100; ++i)
            {
                testData += "This is line " + std::to_string(i) + "\n";
            }
            
            bool saved = loader.SaveToFile(testPath, testData);
            REQUIRE(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            REQUIRE(loadedData == testData);
            
            tempFile.deleteFile();

