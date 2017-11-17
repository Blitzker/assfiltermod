# AssFilterMod

### Description
AssFilterMod is a fork of [AssFilter](https://github.com/alexmarsev/assfilter) by Alex Marsev.

Open-source DirectShow subtitle renderer based on libass and the SubRenderIntf interfaces. Like the well
known XySubFilter, AssFilter don't render the subtitles directly onto the video. It instead communicate
directly with the video renderer to send the subtitles. This process allow the video renderer to do what
it wants with the subtitles.

AssFilterMod is a version of AssFilter with new features and bug fixes:
* Added the Settings and Status tabs
* Added support for SRT subtitles
* Works with built-in video renderers in MPC-HC
* Fix crash when the subtitle track is changed
* Use latest versions of libraries
* Auto-loading of external subtitles

General Settings:

![img](https://i.imgur.com/VjPng7m.png)

Multiple folders need to be seperated by using the delimiter `;` in the External Subtitles Folders.
**Note: Only subfolders of the currently playing media file folder can be used.**

SRT Settings:

![img](https://i.imgur.com/uL7vLCE.png)

In the Custom Tags edit control, you can enter [Ass Tags](http://docs.aegisub.org/3.1/ASS_Tags/) and customize your SRT subtitles the way you want.

Ex: If you want your subtitles to be **BOLD**, enter `{\b1}`.

**Important notes:**
* All tags that you use in the Customs Tags will overwrite the default settings.
* Settings are saved in the registry: `HKEY_CURRENT_USER\Software\AssFilter`
* Only ASS and SRT subtitles are supported.

### Requirements
* [Visual Studio 2015 Update 3 Redistributables](https://www.microsoft.com/en-us/download/details.aspx?id=53840)
* [Visual Studio 2017 Redistributables x32](https://go.microsoft.com/fwlink/?LinkId=746571)
* [Visual Studio 2017 Redistributables x64](https://go.microsoft.com/fwlink/?LinkId=746572)
* [DirectX 9 Redistributables](https://www.microsoft.com/en-ca/download/details.aspx?id=8109)

Windows Vista, 7, 8, 8.1 and 10 (32 and 64 bits)

### Releases
Can be found here https://github.com/Blitzker/assfiltermod/releases

### Compilation
Compiling AssFilterMod is pretty straightforward:

* Have `Visual Studio 2015 Update 3 or Visual Studio 2017` installed.
* Check out `master` branch of assfiltermod.
* Ensure that all submodules are up-to-date by running `git submodule update --init --recursive` from inside the tree.
* Open `assfiltermod.sln` solution file and build.

### 3rd-party libraries

* [DirectShow Base Classes](https://msdn.microsoft.com/en-us/library/windows/desktop/dd375456%28v=vs.85%29.aspx)
* [FreeType](https://www.freetype.org/)
* [FriBidi](https://www.fribidi.org/)
* [HarfBuzz](https://www.freedesktop.org/wiki/Software/HarfBuzz/)
* [libass](https://github.com/libass/libass)

### Credits

* [alexmarsev](https://github.com/alexmarsev) For the great AssFilter working prototype
* [LAV Filters](https://github.com/nevcairiel/lavfilters) Nevcairiel's awesome DirectShow Media Splitter and Decoders
* [MPC-HC](https://github.com/mpc-hc/mpc-hc) Light-weight media player for Windows
* [stackoverflow](https://stackoverflow.com/) Not a project, but a lot of useful ideas and code
* [VideoLAN Org](https://www.videolan.org/vlc/index.html) VLC Media player (SRT subtitles parser code)
