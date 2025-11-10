cmake_minimum_required(VERSION 3.22)

project(ORCHESTRA VERSION 0.0.1)
add_subdirectory(JUCE)                  # If you've put JUCE in a subdirectory called JUCE
add_subdirectory(UnitTests)
add_subdirectory(ORchestra)