# Textractor

![How it looks](screenshot.png)

[English](README.md) ● [Español](README_ES.md) ● [简体中文](README_SC.md) ● [Русский](README_RU.md) ● [한국어](README_KR.md) ● [ภาษาไทย](README_TH.md) ● [Français](README_FR.md) ● [Italiano](README_IT.md) ● [日本語](README_JP.md) ● [Bahasa Indonesia](README_ID.md) ● [Português](README_PT.md) ● [Deutsch](README_DE.md)

**Textractor** (a.k.a. NextHooker) is an open-source x86/x64 video game text hooker for Windows 7+ (and Wine) based off of [ITHVNR](https://web.archive.org/web/20160202084144/http://www.hongfire.com/forum/showthread.php/438331-ITHVNR-ITH-with-the-VNR-engine).<br>
Watch the [tutorial video](docs/TUTORIAL.md) for a quick rundown on using it.

## Download

Stable releases of Textractor can be found [here](https://github.com/Xydez/Textractor/releases).

## Features

- Highly extensible and customizable
- Auto hook many game engines (including some not supported by VNR!)
- Hook text using "hook" codes (most AGTH codes supported)
- Automatically search for possible hook codes

### Command line
Texthooker may be launched together with command line parameters:
```
Textractor.exe
  -h, --help
      Show the help page

  -p<process_id|process_name>
      Attach to a process id (-p1234) or a process name (-p"sample.exe")

  -x<extension_path>
      Launch with an extension enabled (extension_path should usually be an absolute path). Note that this will make Textractor overwrite SavedExtensions.txt.

      Example: -x"C:\Program Files (x86)\Textractor\Copy to Clipboard.xdll"
```

## Support

Please report any bugs, games that Textractor has trouble hooking, feature requests, or other suggestions by posting an issue.

## Extensions

See the [Example Extension project](https://github.com/Artikash/ExampleExtension) to see how to build an extension.<br>
See the extensions folder for examples of what extensions can do.

## Contributing

All contributions are appreciated! You should use the standard process of making a pull request (fork, branch, commit changes, make a PR to master).<br>
Contributing a translation is easy: [text.cpp](text.cpp) contains all of the text strings that you need to translate. Translations of this README or the tutorial video transcript are also welcome.

## Compiling
### Installing Qt5
Before compiling Textractor, you need to install Qt5 (tested with version 5.15.2) and Visual Studio with CMake support.

The easiest way to install Qt is using [aqtinstall](https://github.com/miurahr/aqtinstall), which, unlike the official installer, doesn't require an account.

```cmd
> cd %LocalAppData%
> mkdir Qt
> cd Qt
> pip install aqtinstall
> aqt install-qt windows desktop 5.15.2 win32_msvc2019
```

This directory is then linked to by `Qt5_DIR` in `build.bat`; if you would like to install Qt to a different directory, then change this variable.

### Compiling the source code
To get started, clone Textractor's source code.

```cmd
> git clone --recurse-submodules https://github.com/Xydez/Textractor.git
```

Then, run the build script `build.bat` and check that there are no errors.

Finally, open `Textractor.sln` in the `build` directory.

## Project Architecture

The host injects texthook into the target process and connects to it via 2 pipe files.
texthook waits for the pipe to be connected, then injects a few instructions into any text outputting functions (e.g. TextOut, GetGlyphOutline) that cause their input to be sent through the pipe.<br>
Additional information about hooks is exchanged via shared memory.<br>
The text that the host receives through the pipe is then processed a little before being dispatched back to the GUI.<br>
Finally, the GUI dispatches the text to extensions before displaying it.

## [Credits](docs/CREDITS.md)
