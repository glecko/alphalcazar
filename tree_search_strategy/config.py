from game.enums import PieceType

WIN_CONDITION_SCORE = 900
DEPTH_PENALTY = 10

# How much is each piece worth if on the board #
PLACED_PIECE_SCORE = {
    PieceType.one: 20,
    PieceType.two: 30,
    PieceType.three: 30,
    PieceType.four: -10,
    PieceType.five: 30,
}

# Multiplier for pieces on the board based on their coordinates #
TILE_SCORE_MULTIPLIER = {
    1: {1: 1, 2: 1, 3: 1},
    2: {1: 1, 2: 2, 3: 1},
    3: {1: 1, 2: 1, 3: 1},
}
