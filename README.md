Digital Clock Next
==================

[![GitHub License](https://img.shields.io/github/license/Kolcha/DigitalClockNext)](https://www.gnu.org/licenses/gpl-3.0.html)
[![SourceForge Downloads](https://img.shields.io/sourceforge/dm/digitalclock4)](https://sourceforge.net/projects/digitalclock4/files/next)

This is "next" version of my [Digital Clock 4](https://github.com/Kolcha/DigitalClock4). It has completely new written from scratch skin engine (but old skins are supported) which adds many new features and possibilities giving even more flexibility in customization.

Some key features:

* full compatibility with old skin format
* good font rendering (minimal difference with text editors)
* good Unicode support (including emoji)
* font-like rendering for image-based skins due to [skin format extension](https://github.com/Kolcha/DigitalClockNext/wiki/Extended-legacy-skin-format)
* gradient filling for numbers and background
* horizontal/vertical layout out-of-the-box
* [multiple time zones](https://github.com/Kolcha/DigitalClockNext/wiki/Multiple-timezones) (or just multiple windows) support
* tray icon displays local time (implemented just-for-fun)
* completely [new skin format](https://github.com/Kolcha/DigitalClockNext/wiki/Modern-skin) with a lot of customization options

A lot of useful information about app usage and possible customization options can be found in the [Wiki](https://github.com/Kolcha/DigitalClockNext/wiki). Also some useful links and contact information can be found in the app's About dialog.

Downloads are published on SourceForge.

[![Download Digital Clock Next](https://a.fsdn.com/con/app/sf-download-button)](https://sourceforge.net/projects/digitalclock4/files/next)

How to build
------------

For now the only external dependency is Qt. At least Qt 6.4 is required, Qt 6.5 is recommended, anything newer is not tested yet.

Some recently implemented C++20 features are used in the source code, so pretty new compiler is required:

* Windows: Visual Studio 2019 (at least version 16.10 with compiler version 19.29)
* macOS: Xcode 14.3.1 (with Apple clang 14.0.3), Xcode 15 is recommended
* Linux: GCC 10 (10.5.0 is known to work), GCC 11 or above is recommended

License
-------

The source code is licensed under the [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.html) license with exception for a few files which are [MIT](https://opensource.org/license/mit)-licensed, see files headers for license details.

The most of artwork that can be found in this repository is public domain ([CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/)) unless otherwise explicitly specified, see the corresponding README files in the same folders as artwork for details.

The macOS and Windows app icons are unique artwork that licensed under [CC-BY-4.0](https://creativecommons.org/licenses/by/4.0/).
