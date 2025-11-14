#pragma once

#include <vector>
#include <string>

#include "ORchestraToken.h"

namespace ORchestra {


class ErrorReporting;

class Scanner
{
public:
    Scanner(ErrorReporting& logger);
    ~Scanner();
    
    bool ScanFile(const char *data);
    std::vector<ORchestraToken>& GetTokens() { return mTokens; }

private:
    ORchestraToken ScanToken();
    ORchestraToken MakeToken(ORchestraTokenType token);
    ORchestraToken MakeErrorToken(char* message, char symbol);
    ORchestraToken BuildString();
    ORchestraToken BuildDigit();
    ORchestraToken BuildIdentifier();
    ORchestraTokenType IdentifierToken();
    void SkipWhiteSpace();
    
    //Helpers
    inline bool IsAtEnd();
    inline bool Match(char expected);
    inline char PeekCurrent();
    inline char PeekNext();
    inline char AdvanceCurrent();
    inline bool IsAlpha(char c);
    inline bool IsDigit(char c);

private:
    ErrorReporting& mErrorReporting;
    std::vector<ORchestraToken> mTokens;

    const char* mStart;
    const char* mCurrent;
    int mCurrentLine = 1;
    char ERROR_UNEXPECTED_CHAR[50] = "ERROR: Unexpected character ";
    char ERROR_NO_END_QUOTE[50] = "ERROR: Expected \" but didn't find one ";
};


} // namespace ORchestra