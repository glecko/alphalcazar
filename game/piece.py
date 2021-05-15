from game.enums import Direction, PieceType
from game.constants import BOARD_SIZE
from typing import Tuple


class Piece(object):
    def __init__(self, owner_id: int, type: PieceType):
        self.owner_id = owner_id
        self.type = type
        self.tile = None
        self.direction = None

    def __str__(self):
        direction_string = self.direction.name[0] if self.direction is not None else ""
        return f"{self.type}{direction_string} ({self.owner_id})"

    def __repr__(self):
        return f"<Piece {self!s}>"

    def __eq__(self, other):
        if other is None: return False
        return self.owner_id == other.owner_id and self.type == other.type

    def get_movement_order(self, starting_player_id) -> int:
        if self.owner_id == starting_player_id:
            return self.type * 10
        return self.type * 10 + 1

    def get_movement_offsets(self) -> Tuple[int, int]:
        if self.direction == Direction.north:
            return 0, 1
        if self.direction == Direction.south:
            return 0, -1
        if self.direction == Direction.east:
            return 1, 0
        if self.direction == Direction.west:
            return -1, 0
        raise ValueError(f"Invalid direction ({self.direction}) was specified in {self!r}.")

    def get_target_coordinates(self) -> Tuple[int, int]:
        x, y = self.get_movement_offsets()
        return self.tile.x + x, self.tile.y + y

    def is_on_board(self) -> bool:
        return self.tile is not None

    def remove_from_play(self):
        self.direction = None
        self.tile = None

    def set_movement_direction(self, direction):
        self.direction = direction

    def is_pushable(self) -> bool:
        return self.type == PieceType.one

    def is_pusher(self) -> bool:
        return self.type == PieceType.four

    def is_exiting_board(self) -> bool:
        if self.tile is None:
            return False
        return (
                (self.tile.x == 1 and self.direction == Direction.west) or
                (self.tile.x == BOARD_SIZE and self.direction == Direction.east) or
                (self.tile.y == 1 and self.direction == Direction.south) or
                (self.tile.y == BOARD_SIZE and self.direction == Direction.north)
        )

