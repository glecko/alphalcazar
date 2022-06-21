# Alphalcazar

Implementation of the advanced tic tac toe board game, invented by a friend of mine (Adrià Balcazar), together with several implementations of an AI for said game.

## Rules

The rules of the game can be found in the [rules document](docs/RULES.md).

## Implementations

The game was originally implemented in Python 3.7. The high computational requirements of the minmax algorithm implementation for this game resulted in the deprecation of that implementation. As of May 2022, the project has been ported to C++17, which is the actively mantained version. It can be found in the [cpp](cpp) folder.

The Python implementation has been kept on the main branch (in the [python_deprecated](python_deprecated) folder) for benchmarking purposes and because some of its features have not been fully ported.
