#include "ORchestraToken.h"

namespace ORchestra {

      ORchestraToken::ORchestraToken(ORchestraTokenType tokenType,
            const char* start,
            int length,
            int line) : mTokenType(tokenType),
            mStart(start),
            mLength(length),
            mLine(line)
      {
      }

} // namespace ORchestra
