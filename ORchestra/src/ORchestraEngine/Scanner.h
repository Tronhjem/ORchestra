#pragma once

#include <vector>
#include <string>

#include "ORchestraToken.h"

namespace ORchestra
{

    class ErrorReporting;

    class Scanner
    {
    public:
        Scanner(ErrorReporting& logger);
        ~Scanner();

        bool ScanFile(const std::string& data);
        std::vector<ORchestraToken>& GetTokens() { return mTokens; }
        void Reset();

    private:
        Scanner() = delete;

        ORchestraToken ScanToken();
        ORchestraToken MakeToken(ORchestraTokenType token);
        ORchestraToken MakeErrorToken(const std::string_view& message, char symbol);
        ORchestraToken BuildDigit();
        ORchestraToken BuildIdentifier();
        ORchestraTokenType IdentifierToken();
        void SkipWhiteSpace();

        // Helpers
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
        std::string mData; // Store the data to keep pointers valid

        const char* mStart = nullptr;
        const char* mCurrent = nullptr;

        int mCurrentLine = 1;
    };

} // namespace ORchestra
