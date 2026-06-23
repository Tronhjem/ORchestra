### v0.2.0

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
