Extended legacy skin format
===========================

As this new format is based on legacy skin format, so the latter is completely valid according to new specification and supported with no behavior change. Compared to the legacy format, new one brings many relaxed requirements and some new features. New features are designed to be backward compatible (i.e. new skin can be loaded in older application, unless "relaxed requirements are used).

Legacy skin format
------------------

Valid skin is a folder with 11-14 image files of the same type and special configuration file. Folder name as image files names can be any, configuration file must be named '_skin.ini_'.

Skin should have next image files:

* 10 images for digits (0-9) **_[required]_**
* 1 or 2 images for separator ("dot") **_[1 required + 1 optional]_**
* 2 images for AM/PM symbols **_[optional]_**

2 images for separator are used to animate separator - they are swapped every 0.5 sec.

2 images for AM/PM symbols (1 for AM and 1 for PM) are optional.

Only 2 image formats are supported: PNG for raster graphics and SVG for vector graphics. All images must be in same format for whole skin.

Configuration file for the skin is standard _.ini_ file with only 2 sections. Both of them are required.
First section contains info about the skin and its author.

```ini
[info]                          ; section name, must be [info]
name=Skin example               ; skin name (this will be displayed in list)    [required]
version=1.0                     ; skin version (you can set anything here)      [optional]
author=Nick Korotysh            ; skin author, your name/nickname               [optional]
e-mail=nick.korotysh@gmail.com  ; your e-mail if you want to show it            [optional]
comment=dummy skin              ; short skin description                        [optional]
```

Second section contains info about skin images. It describes images purpose.

```ini
[files]                         ; section name, must be [files]
1=img_1.svg                     ; image file for digit '1'                      [required]
2=img_2.svg                     ; image file for digit '2'                      [required]
3=img_3.svg                     ; image file for digit '3'                      [required]
4=img_4.svg                     ; image file for digit '4'                      [required]
5=img_5.svg                     ; image file for digit '5'                      [required]
6=img_6.svg                     ; image file for digit '6'                      [required]
7=img_7.svg                     ; image file for digit '7'                      [required]
8=img_8.svg                     ; image file for digit '8'                      [required]
9=img_9.svg                     ; image file for digit '9'                      [required]
0=img_0.svg                     ; image file for digit '0'                      [required]
s1=sep_dot.svg                  ; 1st image for separator                       [required]
s2=sep_empty.svg                ; 2nd image for separator                       [required]
am=am.svg                       ; image for 'AM' symbol                         [optional]
pm=pm.svg                       ; image for 'PM' symbol                         [optional]
```

Values for `s1` and `s2` keys can be the same (if there is only one image for separator). No separator animation is supported in that case.

Keys (and sections) order in the file is not important. Configuration file name must be _'skin.ini'_.

Relaxed requirements
--------------------

New legacy format relaxes next requirements (hence making new skins **incompatible** with older application):

* `s2` key under `[files]` section is optional and can be omitted at all, but separator animation is still supported - `s1` is hidden for 0.5 sec every 0.5 sec
* everything in configuration file and configuration file itself is optional, but this is "all or nothing": some missing required key under `[files]` section leads to invalid skin (but no `[files]` section is fine)
  * image files must have predefined names, the same as keys in `[files]` section
  * if no `[info]` section is provided, skin name defaults to folder name
  * empty (or without `[files]` section) configuration file is equivalent to no configuration file at all
* image files can be of different types, i.e. some of them are PNG, some are SVG

The extension
-------------

Extension to legacy skin format introduces HighDPI skins support (for raster images, vector images are HighDPI-compatible by default). To enable this feature, files with `@2x` suffix should be added alongside with main (without the suffix) files, e.g. _'s1.png'_ and _'s1@2x.png'_. Only `@2x` suffix is supported for now. Only files without the suffix should be referenced in _'skin.ini'_.

Another extension to legacy format is much more important - it brings font-like rendering, i.e. the possibility to describe how to align glyphs relative to some baseline, what spacing between characters to use (not necessary image width!), and where to put the next character. More details are below.

Visual representation (example with 2 adjacent characters):

```txt
   ---------------> x
|        (x,y)              w
|  -- --o---------+   |<- - - - ->|
|  ^    |         |   |           |
|  |    |         |   o-----------+
| h|    |         |   |           |
|  |    |  (0,0)  |   |    (0,0)  |
|  v   -|-o- - - -|- -|- -o- - - -|- - - - - baseline
|  -- --+-+-------+   |   |       |
|         |           |   |       |
|         |           +---+-------+
|         |<- - - - - - ->|
V                ax
y
```

All this additional information is stored in separate configuration file called _'geometry.ini'_. File can contain one or more section named as keys under `[files]` section in _'skin.ini'_. All sections are optional and all keys in section are optional.

Possible keys in section and their default values:

* x - top-left x, can be negative (as for fonts), defaults to 0, relative to character's (0,0)
* y - top-left y, can be negative (as for fonts), defaults to 0, relative to character's (0,0)
* w - glyph width, can be not equal image width, defaults to image width
* h - glyph height, can be not equal image height, defaults to image height
* ax - horizontal distance to the next glyph, from (0,0) of current to (0,0) of next, defaults to w, can be both more or less than w
* ay - vertical distance to the next glyph, from (0,0) of current to (0,0) of next, defaults to h, can be both more or less than h

Example section:

```ini
[s1]                            ; section name, first separator image in this example
x=1.083                         ; can be interpreted as left margin
y=28.961                        ; can be interpreted as top margin
w=11.641                        ; desired image width, if not equal to actual, image is scaled
h=38.052                        ; desired image height, if not equal to actual, image is scaled
ax=13.807                       ; horizontal distance from this symbol to the next one, between (0,0)
ay=95.971                       ; vertical distance from this symbol to the next one, between (0,0)
```

The first 4 values describe glyph's geometry as it (alone) would be placed at (0,0). `ax` and `ay` are for layout, to help it to place glyphs correctly one after another.

No "safe values" (from designer point of view) are provided/guessed for mixed (values and defaults) cases, it is an author's responsibility to provide correct/required/desired values.

This works both for vector and raster images. In case of raster images all coordinates must be specified based on "original" (not-scaled, without suffix) images. For vector images, any coordinates can be specified as fractional numbers.
