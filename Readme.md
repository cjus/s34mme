# s34mme

## Purpose

The Section34 Multimedia Engine is a multimedia library featuring a spite animation engine which I wrote between 1998-1999 for use with 
Windows 95 using C++. The code is posted here for educational reasons.

## Some points of interests

[PixelFormatConvertor](http://github.com/cjus/s34mme/blob/master/PixelFormatConvertor.cpp): Converts Pixel data for use on 15, 15 and 24 bit graphic cards.
[S34Bitmap](http://github.com/cjus/s34mme/blob/master/S34Bitmap.cpp): Demonstrates handling of MSWindows bitmap images.
[S34Palette](http://github.com/cjus/s34mme/blob/master/S34Palette.cpp): Various color handling routines.
[S34Image](http://github.com/cjus/s34mme/blob/master/S34Image.cpp): S34 custom image format.  Demonstrates RLE image encoding as a means of addressing both image compression and transparency.
[S34Scene](http://github.com/cjus/s34mme/blob/master/S34Scene.cpp): Demonstrates animation scene management of sprite animation objects.  Shows handling of 2D Z-ordering and dirty rectangle management.

## Code I did not write!

S34Dither: 24bit to 8bit color quantizing.  Written by a very talented friend of mine.
JPEGLib: JPEG image handling library.

