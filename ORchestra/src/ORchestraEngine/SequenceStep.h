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

#pragma once

#include "Defines.h"
#include "StepData.h"

namespace ORchestra 
{
    class SequenceStep
    {
    public:
        SequenceStep( SequenceStepType midiType,
                      StepData shouldTrigger,
                      const StepData firstData,
                      const StepData secondData,
                      const StepData channel,
                      const int duration) : 

                      mType(midiType),
                      mShouldTrigger(shouldTrigger),
                      mFirst(firstData),
                      mSecond(secondData),
                      mChannel(channel),
                      mDuration(duration)
        {
        }

        SequenceStepType mType;
        StepData mShouldTrigger;
        StepData mFirst;
        StepData mSecond;
        StepData mChannel;
        int mDuration;

    private:
        SequenceStep() = delete;
    };
} // namespace ORchestra

