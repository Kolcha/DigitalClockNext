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

Downloads are published on SourceForge.

[![Download Digital Clock Next](https://a.fsdn.com/con/app/sf-download-button)](https://sourceforge.net/projects/digitalclock4/files/next)

How to build
------------

For now the only external dependency is Qt. At least Qt 6.4 is required, Qt 6.5 is recommended, anything newer is not tested yet.

Some recently implemented C++20 features are used in the source code, so pretty new compiler is required:

* Windows: Visual Studio 2019 (at least version 16.10 with compiler version 19.29)
* macOS: Xcode 14.3.1 (with Apple clang 14.0.3), Xcode 15 is recommended
* Linux: GCC 10 (10.5.0 is known to work), GCC 11 or above is recommended
