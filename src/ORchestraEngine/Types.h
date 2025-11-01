#pragma once

#include <stdint.h>
#include "DataSequenceStep.h"

typedef unsigned char uChar;
typedef DataSequenceStep StepData;

static_assert(sizeof(uChar) == 1);

enum class MidiType : uChar
{
    NoteOn = 0,
    NoteOff = 2,
    CC = 3
};
