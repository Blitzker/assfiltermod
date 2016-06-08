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

The SRT subtitles can be configured via the Settings tab.

![img](http://i.imgur.com/9J121tH.png)

In the Custom Tags edit control, you can enter [Ass Tags](http://docs.aegisub.org/3.1/ASS_Tags/) and customize your SRT subtitles the way you want.

Ex: If you want your subtitles to be **BOLD**, enter `{\b1}`.

**Important notes:**
* All tags that you use in the Customs Tags will overwrite the default settings.
* Settings are saved in the registry: `HKEY_CURRENT_USER\Software\AssFilter`
* Only subtitles (ASS and SRT) muxed in MKV files are supported.

### Requirements
* [Visual Studio 2015 Update 2 Redistribuables](https://www.microsoft.com/en-us/download/details.aspx?id=51682)
* [DirectX 9 Redistribuables](https://www.microsoft.com/en-ca/download/details.aspx?id=8109)

Supports Windows Vista, 7, 8 and 10 (32 and 64 bits)

### Releases
Can be found here https://github.com/Blitzker/assfiltermod/releases

### Compilation
Compiling AssFilterMod is pretty straightforward:

* Have `Visual Studio Community 2015 Update 2` installed.
* Check out `master` branch of assfiltermod.
* Ensure that all submodules are up-to-date by running `git submodule update --init --recursive` from inside the tree.
* Open `assfiltermod.sln` solution file and build.

### 3rd-party libraries

* [DirectShow Base Classes](https://msdn.microsoft.com/en-us/library/windows/desktop/dd375456%28v=vs.85%29.aspx)
* [FreeType](https://www.freetype.org/)
* [GNU FriBidi](https://www.fribidi.org/)
* [libass](https://github.com/libass/libass)
* [libiconv](https://www.gnu.org/software/libiconv/)

### Credits

* [alexmarsev](https://github.com/alexmarsev) For the great AssFilter working prototype
* [LAV Filters](https://github.com/nevcairiel/lavfilters) Nevcairiel's awesome DirectShow Media Splitter and Decoders
* [MPC-HC](https://github.com/mpc-hc/mpc-hc) Light-weight media player for Windows
* [stackoverflow](https://stackoverflow.com/) Not a project, but a lot of useful ideas and code
