#pragma once

//#include <stdint.h>

typedef unsigned char DataUnit;
//static_assert(sizeof(DataUnit) == 1);
//typedef int DataUnit;


enum class MidiType : DataUnit
{
    NoteOn = 0,
    NoteOff = 2,
    CC = 3
};
