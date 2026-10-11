# Usage

## Installation

ORchestra is distributed as a plugin package that contains the Audio Unit (AU) and VST3 formats, plus a standalone application.

Download the latest release zip from the [ORchestra GitHub releases page](https://github.com/Tronhjem/ORchestra/releases), then extract it and copy the files into the folders below for your operating system. After installing, restart your DAW and rescan plugins.

### macOS

After extracting the release zip, you will see `.component` (AU), `.vst3`, and optionally the Standalone app.

| Format | File | Copy to |
|---|---|---|
| Audio Unit (AU) | `ORchestra.component` | `/Library/Audio/Plug-Ins/Components/` (all users) or `~/Library/Audio/Plug-Ins/Components/` (current user only) |
| VST3 | `ORchestra.vst3` | `/Library/Audio/Plug-Ins/VST3/` (all users) or `~/Library/Audio/Plug-Ins/VST3/` (current user only) |
| Standalone | `ORchestra.app` | Move to `/Applications/` or any folder you prefer |

**Notes:**
- Copy the **entire** `.component` and `.vst3` bundles, not just files inside them.
- You may need to authenticate with an administrator password when copying to `/Library/`.
- On Apple Silicon Macs, the Standalone app must be signed or you can allow it via **System Settings → Privacy & Security** the first time you open it.
- After copying, restart your DAW and rescan/revalidate plugins. Logic Pro X validates AU plugins automatically on launch.

### Windows

After extracting the release zip, you will see a `.vst3` file and/or a Standalone folder.

| Format | File | Copy to |
|---|---|---|
| VST3 | `ORchestra.vst3` | `C:\Program Files\Common Files\VST3\` (64-bit, recommended) |
| Standalone | `ORchestra.exe` | Run from any folder, or place in `C:\Program Files\ORchestra\` for convenience |

**Notes:**
- Audio Units (AU) are a macOS-only format and are not included in the Windows build.
- Copy the entire `ORchestra.vst3` folder/file as it appears in the zip.
- If your DAW is 32-bit, use the 32-bit VST3 path instead: `C:\Program Files (x86)\Common Files\VST3\`. Most modern DAWs are 64-bit.
- After copying, restart your DAW and rescan plugins.

## Setup as plugin in Major DAWs

Once ORchestra is built or installed as a plugin, adding it to a project and routing MIDI differs slightly from DAW to DAW. The notes below cover the most common hosts to get you sequencing quickly.

### Ableton Live
### Logic Pro X

Logic Pro and Audio Units (AU) supports midi plugins. Simply put ORchestra as a midi plugin at the top of a instrument track.
Make sure to enable the midi recording after ORchestra, by left-clicking under ORchestra in the midi effect plugin list and set "Record MIDI to Track Here"  
<img src="https://raw.githubusercontent.com/Tronhjem/ORchestra/main/img/AU-Midi-Effect-Set-Record.png" width="50%" height="50%"/>

### Reaper 
### Cubase

## Getting Started

