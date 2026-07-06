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
