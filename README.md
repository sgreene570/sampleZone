# sampleZone

[![Build Status](https://travis-ci.org/sgreene570/sampleZone.svg?branch=master)](https://travis-ci.org/sgreene570/sampleZone)

Play samples from a sequencer like command line interface.

Work in progress! Accepting pull requests.

# System requirements:

Debian:

`libasound2-dev`

`ncurses-dev`

Fedora: 

`libasound2-dev`

`ncurses-devel`

Ubuntu:

`libasound2-dev`

Additional ncurses packages may or may not be needed for Ubuntu.

# Building

Simply run `make`

# Usage

`./sampleZone [-w] [-h] <file1.wav> <file2.wav> ...`

Where `w` and `h` are optional width/height grid parameters.
