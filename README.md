# sampleZone

[![Build Status](https://travis-ci.org/sgreene570/sampleZone.svg?branch=master)](https://travis-ci.org/sgreene570/sampleZone)

Play samples from a sequencer like command line interface.

Work in progress! Accepting pull requests.

Uses vim controls (h,j,k,l) for cursor movement.

# System requirements:

Debian:

`libasound2-dev`

`ncurses-dev`

Fedora: 

`alsa-lib-devel`

`ncurses-devel`

Ubuntu:

`libasound2-dev`

Additional ncurses packages may or may not be needed for Ubuntu.

# Building

Simply run `make`.

# Usage

`./sampleZone [-w] [-h] <file1.wav> <file2.wav> ...`

Where `w` and `h` are optional width/height grid parameters.

# Example wav File

See the (attempted) `Dockerfile` for a link to an example wav file.

# Screenshots

![samplezone screenshot](https://i.imgur.com/8GwcYri.png)
