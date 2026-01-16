#Conway's Game of Life

A terminal-based implementation of Conway's Game of Life written in C with ncurses for ASCII graphics visualization.

## About

The Game of Life is a cellular automaton devised by mathematician John Horton Conway in 1970. It's a zero-player game where evolution is determined by the initial state, requiring no further input. The universe consists of an infinite two-dimensional grid of cells, each either alive or dead.

### Rules

- Any live cell with 2-3 live neighbors survives
- Any dead cell with exactly 3 live neighbors becomes alive
- All other live cells die in the next generation
- All other dead cells stay dead

## Features

- 80x25 cell grid with wraparound edges (toroidal topology)
- Interactive speed control
- ASCII graphics using ncurses library
- 5+ predefined initial patterns (stable figures, oscillators, gliders, guns, etc.)
- Configuration via stdin

## Requirements

- GCC compiler
- ncurses library
- Linux/Unix environment
