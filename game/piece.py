from game.enums import Direction, PieceType


class Piece(object):
    def __init__(self, owner_id: int, type: PieceType):
        self.owner_id = owner_id
        self.type = type
        self.tile = None
        self.direction = None

    def __str__(self):
        return f"{self.type} ({self.owner_id})"

    def __repr__(self):
        return f"<Piece {self!s}>"

    def __eq__(self, other):
        if other is None: return False
        return self.owner_id == other.owner_id and self.type == other.type

    def get_movement_order(self, starting_player_id):
        if self.owner_id == starting_player_id:
            return self.type * 10
        return self.type * 10 + 1

    def get_movement_offsets(self):
        if self.direction == Direction.north:
            return 0, 1
        if self.direction == Direction.south:
            return 0, -1
        if self.direction == Direction.east:
            return 1, 0
        if self.direction == Direction.west:
            return -1, 0
        raise ValueError(f"Invalid direction ({self.direction}) was specified in {self!r}.")

    def is_on_board(self):
        return self.tile is not None

    def remove_from_play(self):
        self.tile = None

    def set_movement_direction(self, direction):
        self.direction = direction

    def is_pushable(self):
        return self.type == PieceType.one

    def is_pusher(self):
        return self.type == PieceType.four

