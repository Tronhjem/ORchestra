#pragma once

using namespace juce;
#include "FileLoader.h"
#include <fstream>

class Test_FileLoader : public UnitTest
{
public:
    Test_FileLoader() : UnitTest("Test_FileLoader") {}
    
    void runTest() override
    {
        {
            beginTest("FileLoader: SaveToFile() and LoadFile() roundtrip preserves simple text 'Hello, ORchestra!'");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_simple.txt";
            std::string testData = "Hello, ORchestra!";
            
            // Save file
            bool saved = loader.SaveToFile(testPath, testData);
            expect(saved);
            
            // Load file
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData);
            
            // Cleanup
            std::remove(testPath.c_str());
        }
        {
            beginTest("FileLoader: SaveToFile() and LoadFile() roundtrip preserves multi-line text with newlines");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_multiline.txt";
            std::string testData = "line1\nline2\nline3";
            
            bool saved = loader.SaveToFile(testPath, testData);
            expect(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData);
            
            std::remove(testPath.c_str());
        }
        {
            beginTest("FileLoader: SaveToFile() and LoadFile() roundtrip preserves empty string");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_empty.txt";
            std::string testData = "";
            
            bool saved = loader.SaveToFile(testPath, testData);
            expect(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData);
            
            std::remove(testPath.c_str());
        }
        {
            beginTest("FileLoader: SaveToFile() and LoadFile() roundtrip preserves ORchestra script syntax");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_script.txt";
            std::string testData = "a = [1, 2, 3]\nb = a + 10\ntest b";
            
            bool saved = loader.SaveToFile(testPath, testData);
            expect(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData);
            
            std::remove(testPath.c_str());
        }
        {
            beginTest("FileLoader: LoadFile() returns empty string for non-existent file");
            
            FileLoader loader;
            std::string nonExistentPath = "/tmp/this_file_does_not_exist_12345.txt";
            
            std::string loadedData = loader.LoadFile(nonExistentPath);
            expect(loadedData.empty());
        }
        {
            beginTest("FileLoader: SaveToFile() and LoadFile() roundtrip preserves special characters");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_special.txt";
            std::string testData = "!@#$%^&*()_+-=[]{}|;:',.<>?/`~";
            
            bool saved = loader.SaveToFile(testPath, testData);
            expect(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData);
            
            std::remove(testPath.c_str());
        }
        {
            beginTest("FileLoader: SaveToFile() and LoadFile() roundtrip preserves numeric string '0123456789'");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_numbers.txt";
            std::string testData = "0123456789";
            
            bool saved = loader.SaveToFile(testPath, testData);
            expect(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData);
            
            std::remove(testPath.c_str());
        }
        {
            beginTest("FileLoader: SaveToFile() overwrites existing file with new content");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_overwrite.txt";
            std::string testData1 = "First content";
            std::string testData2 = "Second content";
            
            // Save first content
            loader.SaveToFile(testPath, testData1);
            
            // Overwrite with second content
            bool saved = loader.SaveToFile(testPath, testData2);
            expect(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData2);
            expect(loadedData != testData1);
            
            std::remove(testPath.c_str());
        }
        {
            beginTest("FileLoader: SaveToFile() and LoadFile() roundtrip preserves 100-line content");
            
            FileLoader loader;
            std::string testPath = "/tmp/orchestra_test_long.txt";
            std::string testData;
            
            // Create a longer string
            for (int i = 0; i < 100; ++i)
            {
                testData += "This is line " + std::to_string(i) + "\n";
            }
            
            bool saved = loader.SaveToFile(testPath, testData);
            expect(saved);
            
            std::string loadedData = loader.LoadFile(testPath);
            expect(loadedData == testData);
            
            std::remove(testPath.c_str());
        }
    }
};
