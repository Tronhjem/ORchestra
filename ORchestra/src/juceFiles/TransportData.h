#pragma once

namespace ORchestra
{
    struct TransportData
    {
        double bpm = 0.0;
        double sampleRate = 0;
        float bpmDivision = 2.f;
        int noteLengthInSamples = 0;
        int64_t timeInSamples = 0;
        bool isPlaying = false;

        //    double time = 0.0;
        //    double ppq = 0.0;
        //    int bar = 0;
        //    int beat = 0;
    };
} // namespace ORchestra
