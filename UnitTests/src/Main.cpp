#include <JuceHeader.h>
#include "Test_ArithmeticOperators.h"
#include "Test_LogicalOperators.h"
#include "Test_ComparisonOperators.h"
#include "Test_ArrayOperators.h"
#include "Test_EuclidAndRandom.h"
#include "Test_ErrorHandling.h"
#include "Test_DataSequence.h"
#include "Test_NoteNumbers.h"
#include "Test_CustomStack.h"
#include "Test_Token.h"
#include "Test_Scanner_BasicTokens.h"
#include "Test_Scanner_Operators.h"
#include "Test_Scanner_Expressions.h"
#include "Test_FileLoader.h"
#include "Test_Compiler.h"
#include "Utility.h"

using namespace juce;

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    UnitTestRunner runner;

    Test_ArithmeticOperators arithmeticOperators;
    Test_LogicalOperators logicalOperators;
    Test_ComparisonOperators comparisonOperators;
    Test_ArrayOperators arrayOperators;
    Test_EuclidAndRandom euclidAndRandom;
    Test_ErrorHandling errorHandling;
    Test_DataSequence dataSequence;
    Test_NoteNumbers noteNumbers;
    Test_CustomStack customStack;
    Test_Token token;
    Test_Scanner_BasicTokens scannerBasicTokens;
    Test_Scanner_Operators scannerOperators;
    Test_Scanner_Expressions scannerExpressions;
    Test_FileLoader fileLoader;
    Test_Compiler compiler;

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
