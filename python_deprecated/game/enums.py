from enum import IntEnum


class PieceType(IntEnum):
    one = 1
    two = 2
    three = 3
    four = 4
    five = 5


class Direction(IntEnum):
    north = 1
    south = 2
    east = 3
    west = 4


class GameResult(IntEnum):
    loss = -1
    win = 1

