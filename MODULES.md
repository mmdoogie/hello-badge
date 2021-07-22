# Modules

## EPD Library
EPD library source code originally from Waveshare.
[GitHub - waveshare/e-Paper](https://github.com/waveshare/e-Paper/)

BSD-style license is provided in the header of those sourced files.

Additional modifications made by Bodmer to format into classes for ESP32, which are the files used as a base here.
[GitHub - Bodmer/EPD_Libraries](https://github.com/Bodmer/EPD_Libraries/tree/master/epd2in7b)

Further modifications by myself to add additional drawing capabilities, more flexible font format, and to interface with the new V2 display chip.

## Typefaces
Several rendered typefaces are included to produce the "hello my name is" tag procedurally.

These were preprocessed from the original font files to a format more conducive for use with small bitmapped displays using a tool I created [GitHub - mmdoogie/auto-lcd-font](https://github.com/mmdoogie/auto-lcd-font)

All typefaces here are originally available via Google Fonts under the SIL Open Font License.
* [Dancing Script](https://fonts.google.com/specimen/Dancing+Script) by Impallari Type
* [Nova Flat](https://fonts.google.com/specimen/Nova+Flat) by Wojciech Kalinowski
* [Nunito](https://fonts.google.com/specimen/Nunito) by Vernon Adams and Cyreal
