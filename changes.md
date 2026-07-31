### v0.2.4

- Fixing general building on Linux (ubuntu) and adding checks to run on PRs for ubuntu as well.
- Improve Cmake to have separate config for VST and AU to be able to export proper midi effect for AU and normal audio plugin for VST.

Bugs:
- VST was not working, as audio input was removed, adding it back for vst.
- Setting bpm from DAW was not working correctly, now properly gets the BPM, if it's not set by the script.


### v0.2.3

- Reorganized all testing code under `Tests/` (UnitTests, Fuzzing, StressTest).
- Added a 3-thread stress harness and ThreadSanitizer build for catching race conditions.
- Replaced debug-only AssertMutex tripwires with real mutexes in `ErrorReporting`, the instruction string copy path, and ring buffer slots.

Bugs:
- Fix data race on `mLogEntries` that could SEGV when compiling frequently while the console refreshed.
- Fix data race on `mInstructionData` / `mPendingInstructionData` that could crash state save/restore during a recompile.
- Fix UI thread reading ring buffer slots while the worker thread rewrote them after a compile.
- Fix negative array index wrap in `DataSequence` after a DAW loop-back.
- Make audio-thread `print` logging non-blocking (drops on contention) so the audio thread never waits on the log mutex.
- Publish BPM and beat division to the UI via atomics instead of reading `TransportData` cross-thread.
- Fix first step after a DAW seek/loop-back being skipped when its step number equaled the last processed step.


### v0.2.2

Select audio output in the menu, to select the audio driver. This allows the user to get more steady triggering in standalone mode.
Adding logging for debug builds to be able to find bugs easier when running in the DAW.
Improving the safety of Worker thread and audio thread handling to minimize race condition bugs.
Adding comment toggling in the code editor for easier bringing in and out chunks of code.

Bugs:
- Fix issue where consequtive midi notes triggering on every beat wouldn't work as note off would cause it to fire only very short note. 
- Fix crash when skipping around in the DAW timeline. We would reset the VM while recompiling. Worker thread now is the only one to reset the compiler.
- Right Click menu in code editor was not using same color scheme as the rest. 


### v0.2.1

- Changing built in function from `bpmDiv()` to `beat()` 
- Removing ptn entirely and rely on functions only.
- Arrays are now either value arrays or function arrays, these can't be mixed. This allows for the same use with ptn but with functions instead.
- General cleanup and refactor on how functions work internally.

Bugs:
- Fixed wrong sizing and placement of UI when re opening on VST and AU
- Fixed issue where looping in a daw would leave hanging midi notes.
- Fixing midi notes being cut if an overlapping note is fired, now we choke the earlier note and then fire the new note.
- Fixed notes block not appearing when compiling and only when plyaing. 
- Ensure that functions can only be used in expressions if they return a value.
- Note duration is properly displayed now when notes are longer or shorter than the beat division. Trigger rectangle reflects the note length too.


### v0.1.1 

Official Alpha release.
A bunch of bug fixing and polishing of features, and finalizing the core functionality.
ORchestra is at this point in a state where it has all the functionality I envisioned and 
this seems like a good point in time to set a version for an alpha version.
See documentation for full feature set. 

Note worthy features added recently in patching.
- First pass on the layout done the way it should be. 
- User functions and patterns where introduced.
- Bpm, beat and note length is now scriptable. 
- Transpose added, and introducing negation to be able to have negative global transpose. 
- UI is more or less complete the way I want it. 
- Full scriptiung capability with variables.
