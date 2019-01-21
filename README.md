# sampleZone

[![Build Status](https://travis-ci.org/sgreene570/sampleZone.svg?branch=master)](https://travis-ci.org/sgreene570/sampleZone)

Play samples from a sequencer like command line interface.

Work in progress.

# System requirements:

Debian:

`libasound2-dev`

`ncurses-dev`

Fedora: 

`libasound2-dev`

`ncurses-devel`

Ubuntu:

`libasound2-dev`

`ncurses*`

`libncurses*`

`clang` (Use clang for building on Ubuntu)

# Building

Debian and Fedora:

Simply run `make`

Ubuntu:

`make CC=clang`

# Usage

`./sampleZone <file1.wav> <file2.wav> ...`
