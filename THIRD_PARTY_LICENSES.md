# Third-Party Licenses & Notices

The source code in this repository is licensed under the **MIT License** (see `LICENSE`).

Prebuilt Windows single-file Release binaries (`c-review-trainer.exe`) bundle the following unmodified third-party runtime components:

## 1. Tiny C Compiler (TCC 0.9.27, win32)
- **Website**: https://bellard.org/tcc/
- **License**: GNU Lesser General Public License v2.1 (LGPL-2.1)
- **Role**: Embedded in-memory C99 compiler (`tcc -run`) used to compile and grade exercises offline.
- **Note**: TCC is bundled unmodified as an external compiler toolchain invoked via subprocess. Full TCC source code is available from the official repository: https://repo.or.cz/tinycc.git

## 2. Scintilla & Lexilla (4.4.6, win32)
- **Website**: https://www.scintilla.org/
- **License**: Historical Permission Notice and Disclaimer (MIT-compatible permissive license)
- **Copyright**: Copyright 1998-2020 by Neil Hodgson `<neilh@scintilla.org>`
- **Role**: Code editing widget (`Scintilla.dll`) and C/C++ syntax lexer (`Lexilla.dll`).
