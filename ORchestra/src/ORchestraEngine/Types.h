#pragma once

typedef unsigned char DataUnit;
static_assert(sizeof(DataUnit) == 1);

enum class MidiType : DataUnit
{
    NoteOn = 0,
    NoteOff = 1,
    CC = 2
};
