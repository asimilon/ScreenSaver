# Simple Windows ScreenSaver

I wanted a simple screen saver that will show a slideshow of pictures with a "Ken Burns" style effect, but couldn't find anything suitable and without a price tag attached.  In those situations you just need to make it yourself!

## Building

1. Clone the repository
2. Run `cmake -B build`
3. Run `cmake --build build`
4. Find `ScreenSaver.scr` in the `build` folder, put it somewhere safe and then right click and `Install`

## Usage

1. Download [ScreenSaver.scr](https://github.com/asimilon/ScreenSaver/blob/main/Binary/ScreenSaver.scr) from this repository, put it somewhere safe and then right click and `Install`.
2. Click `Settings...` from the screen saver settings dialogue.
3. Choose a folder of pictures to be used as the slideshow, and pick how long you want each image to show for.

I would suggest downloading some [excellent images](https://conspiracy.hu/files/screenshots/empires/8k_cranked/) from Conspiracy's website if you need some inspiration.

## Contributing

Any improvements will be happily merged if I also find them useful, or just fork and have fun.

If anyone knows how to make JUCE play nicely with the "real" screensaver preview in Windows, please let me know!

### Acknowledgements

This repository includes CPM.cmake
```
  Copyright (c) 2019-2023 Lars Melchior and contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
```