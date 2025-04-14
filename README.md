# Sailcut CAD - sail designer

## About

Sailcut CAD is a software for designing boat sails and developing them into flat panels.

<img width="912" alt="Sailcut CAD screenshot" src="https://user-images.githubusercontent.com/1189716/47034688-e9700900-d12c-11e8-851e-d0037211f470.png">

It is distributed under the [GNU General Public License v2.0](LICENSE).

See the [AUTHORS](AUTHORS) file for a full list of contributors.

## Documentation

The Sailcut CAD Handbook is available in 3 languages at [www.sailcut.org](https://www.sailcut.org/handbook/en/).

## Building

Sailcut CAD is built using the [Qt 6](https://www.qt.io) development framework.

To build Sailcut CAD yourself, get the source code, [install Qt](https://www.qt.io/download-qt-installer-oss) then run:

```shell
qmake
make
```

This will produce the following binaries:

- Linux: `bin/sailcut`
- macOS: `bin/Sailcut CAD.app`
- Windows: `bin/sailcut.exe`
