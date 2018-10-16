# Sailcut CAD - sail designer

<img width="912" alt="screen shot 2018-10-16 at 10 18 40" src="https://user-images.githubusercontent.com/1189716/47034688-e9700900-d12c-11e8-851e-d0037211f470.png">

## Authors

Copyright (C) 1993-2018 Robert & Jeremy Laine

See [AUTHORS](/AUTHORS) file for a full list of contributors.

## Documentation

Available in 3 languages at [sailcut.com](http://www.sailcut.com/handbook/en/).

## Building

This is a Qt app and the build system relies on `qmake`, which comes with Qt distributions. You can install Qt from:

- macOS: `brew install qt`
- Windows: download an [official release](http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe) (LGPLv3 license)

From the root of the repo, run:

```
qmake
```

This will prepare a `Makefile`, which can be used to make the binaries for the app. This is done by simply running:

```
make
```

Which will make the following runnable apps:

- macOS: `bin/Sailcut CAD.app`
- Windows: `bin/sailcut.exe`

And that's it!

## License - GPLv2

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA