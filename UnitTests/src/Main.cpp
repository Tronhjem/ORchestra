#include <JuceHeader.h>
#include "Test_Operators.h"
#include "Test_EuclidAndRandom.h"
#include "Test_ErrorHandling.h"
#include "Test_DataSequence.h"
#include "Test_NoteNumbers.h"
#include "Utility.h"

using namespace juce;

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    UnitTestRunner runner;

    Test_Operators opcodeResults;
    Test_EuclidAndRandom euclidAndRandom;
    Test_ErrorHandling errorHandling;
    Test_DataSequence dataSequence;
    Test_NoteNumbers noteNumbers;

    runner.runAllTests();
    
    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        if (runner.getResult (i)->failures > 0)
        {
            std::cout << "Errors for: " << runner.getResult(i)->unitTestName << std::endl;
            std::cout << runner.getResult(i)->subcategoryName << std::endl;
            for (auto mes : runner.getResult(i)->messages)
                std::cout << mes << std::endl;

            return 1;
        }
    }

    return 0;
}
