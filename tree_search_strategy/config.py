from game.enums import PieceType, Direction

WIN_CONDITION_SCORE = 900
DEPTH_PENALTY = 10

# How much is each piece worth if on the board #
PLACED_PIECE_SCORE = {
    PieceType.one: 15,
    PieceType.two: 30,
    PieceType.three: 25,
    PieceType.four: -10,
    PieceType.five: 35,
}

CENTER_PIECE_MULTIPLIER = 2.15
PIECE_ABOUT_TO_EXIT_MULTIPLIER = 0.75
FRESH_CORNER_PIECE_MULTIPLIER = 1.55

# Multiplier for pieces on the board based on their coordinates and direction #
TILE_SCORE_MULTIPLIER = {
    1: {
        1: {
            Direction.east: FRESH_CORNER_PIECE_MULTIPLIER,
            Direction.west: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.south: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.north: FRESH_CORNER_PIECE_MULTIPLIER,
        },
        2: {
            Direction.east: 1,
            Direction.west: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.south: 1,
            Direction.north: 1,
        },
        3: {
            Direction.east: FRESH_CORNER_PIECE_MULTIPLIER,
            Direction.west: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.south: FRESH_CORNER_PIECE_MULTIPLIER,
            Direction.north: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
        },
    },
    2: {
        1: {
            Direction.east: 1,
            Direction.west: 1,
            Direction.south: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.north: 1,
        },
        2: {
            Direction.east: CENTER_PIECE_MULTIPLIER,
            Direction.west: CENTER_PIECE_MULTIPLIER,
            Direction.south: CENTER_PIECE_MULTIPLIER,
            Direction.north: CENTER_PIECE_MULTIPLIER,
        },
        3: {
            Direction.east: 1,
            Direction.west: 1,
            Direction.south: 1,
            Direction.north: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
        },
    },
    3: {
        1: {
            Direction.east: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.west: FRESH_CORNER_PIECE_MULTIPLIER,
            Direction.south: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.north: FRESH_CORNER_PIECE_MULTIPLIER,
        },
        2: {
            Direction.east: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.west: 1,
            Direction.south: 1,
            Direction.north: 1,
        },
        3: {
            Direction.east: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
            Direction.west: FRESH_CORNER_PIECE_MULTIPLIER,
            Direction.south: FRESH_CORNER_PIECE_MULTIPLIER,
            Direction.north: PIECE_ABOUT_TO_EXIT_MULTIPLIER,
        },
    },
}
