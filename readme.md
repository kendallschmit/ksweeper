# ksweeper

Simple Minesweeper program written in C using GLFW and GLAD. Built on Windows using MinGW. Might build on other platforms too if I didn't mess anything up.

The project needs a lot of cleanup.

## Features
- Three difficulty modes
	- Easy (8x8, 10)
	- Medium (16x16, 40)
	- Expert (30x16, 99)
	- Press `E`, `M`, or `X` to start a new game
- Start with an island every time
	- First tile and directly adjacent tiles are always safe
- No ads
- Loads quickly

## About `resgen`
`resgen` is a small program that generates .h files to include binary data. You need to build `resgen` before you can build ksweeper. The process should look something like:
```
cd resgen
make
```
