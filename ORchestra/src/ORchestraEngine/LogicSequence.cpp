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

namespace ORchestra {

    //OLD, JUST KEEPING FOR ARCHIVE

    //#include <assert.h>
    //
    //#include "LogicSequence.h"
    //
    //LogicSequence::LogicSequence(const uChar* start, int length)
    //{
    //    SetSequence(start, length);
    //}
    //
    //inline void LogicSequence::SetSequence(const uChar* start, const int length)
    //{
    //#if DEBUG
    //    assert(length <= MAX_LOGIC_SEQUENCE_LENGTH);
    //#endif
    //
    //    mTrigger = 0;
    //    for (int i = 0; i < length; ++i)
    //    {
    //        mTrigger |= start[i] << i;
    //    }
    //
    //    mLength = length;
    //}
    //
    //const uChar LogicSequence::operator [](const int i) const
    //{
    //    LogicContainer index = static_cast<LogicContainer>(i % mLength);
    //    uChar value = ((1 << index) & mTrigger) >> index;
    //    return value;
    //}


} // namespace ORchestra