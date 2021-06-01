from game.enums import PieceType, Direction
from enum import IntEnum

MIN_DEPTH_TO_PERSIST = 2


class EvaluationType(IntEnum):
    exact = 0
    alpha_cutoff = 1
    beta_cutoff = 2


WIN_CONDITION_SCORE = 1000

# Depth penalty should be so low as to be guaranteed to never alter the result of a position
DEPTH_PENALTY = 1

# How much is each piece worth if on the board #
PLACED_PIECE_SCORE = {
    PieceType.one: 80,
    PieceType.two: 120,
    PieceType.three: 140,
    PieceType.four: -80,
    PieceType.five: 100,
}

CENTER_PIECE_MULTIPLIER = 2
PIECE_ABOUT_TO_EXIT_MULTIPLIER = 0.70
FRESH_CORNER_PIECE_MULTIPLIER = 1.55
FRESH_CENTER_LANE_PIECE_MULTIPLIER = 1.70

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
            Direction.east: FRESH_CENTER_LANE_PIECE_MULTIPLIER,
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
            Direction.north: FRESH_CENTER_LANE_PIECE_MULTIPLIER,
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
            Direction.south: FRESH_CENTER_LANE_PIECE_MULTIPLIER,
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
            Direction.west: FRESH_CENTER_LANE_PIECE_MULTIPLIER,
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

PIECE_ENTRY_APPEARS_BLOCKED_ORDER = 1
ONE_CORNER_ROW_ORDER = 2
CORNER_ROW_ORDER = 3
CENTRAL_ROW_ORDER = 4
FOUR_CORNER_ROW_ORDER = 5
FOUR_CENTRAL_ROW_ORDER = 6

PIECE_TYPE_COORDINATE_SORTING_ORDER = {
    PieceType.one: {
        0: {
            1: ONE_CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: ONE_CORNER_ROW_ORDER,
        },
        1: {
            0: ONE_CORNER_ROW_ORDER,
            4: ONE_CORNER_ROW_ORDER,
        },
        2: {
            0: CENTRAL_ROW_ORDER,
            4: CENTRAL_ROW_ORDER,
        },
        3: {
            0: ONE_CORNER_ROW_ORDER,
            4: ONE_CORNER_ROW_ORDER,
        },
        4: {
            1: ONE_CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: ONE_CORNER_ROW_ORDER,
        },
    },
    PieceType.two: {
        0: {
            1: CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: CORNER_ROW_ORDER,
        },
        1: {
            0: CORNER_ROW_ORDER,
            4: CORNER_ROW_ORDER,
        },
        2: {
            0: CENTRAL_ROW_ORDER,
            4: CENTRAL_ROW_ORDER,
        },
        3: {
            0: CORNER_ROW_ORDER,
            4: CORNER_ROW_ORDER,
        },
        4: {
            1: CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: CORNER_ROW_ORDER,
        },
    },
    PieceType.three: {
        0: {
            1: CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: CORNER_ROW_ORDER,
        },
        1: {
            0: CORNER_ROW_ORDER,
            4: CORNER_ROW_ORDER,
        },
        2: {
            0: CENTRAL_ROW_ORDER,
            4: CENTRAL_ROW_ORDER,
        },
        3: {
            0: CORNER_ROW_ORDER,
            4: CORNER_ROW_ORDER,
        },
        4: {
            1: CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: CORNER_ROW_ORDER,
        },
    },
    PieceType.four: {
        0: {
            1: FOUR_CORNER_ROW_ORDER,
            2: FOUR_CENTRAL_ROW_ORDER,
            3: FOUR_CORNER_ROW_ORDER,
        },
        1: {
            0: FOUR_CORNER_ROW_ORDER,
            4: FOUR_CORNER_ROW_ORDER,
        },
        2: {
            0: FOUR_CENTRAL_ROW_ORDER,
            4: FOUR_CENTRAL_ROW_ORDER,
        },
        3: {
            0: FOUR_CORNER_ROW_ORDER,
            4: FOUR_CORNER_ROW_ORDER,
        },
        4: {
            1: FOUR_CORNER_ROW_ORDER,
            2: FOUR_CENTRAL_ROW_ORDER,
            3: FOUR_CORNER_ROW_ORDER,
        },
    },
    PieceType.five: {
        0: {
            1: CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: CORNER_ROW_ORDER,
        },
        1: {
            0: CORNER_ROW_ORDER,
            4: CORNER_ROW_ORDER,
        },
        2: {
            0: CENTRAL_ROW_ORDER,
            4: CENTRAL_ROW_ORDER,
        },
        3: {
            0: CORNER_ROW_ORDER,
            4: CORNER_ROW_ORDER,
        },
        4: {
            1: CORNER_ROW_ORDER,
            2: CENTRAL_ROW_ORDER,
            3: CORNER_ROW_ORDER,
        },
    }
}
