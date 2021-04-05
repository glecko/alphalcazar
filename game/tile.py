from game.constants import PLAY_AREA_SIZE, PERIMETER_COORDINATES
from game.piece import Piece
from game.enums import Direction
from typing import Optional


class Tile(object):
    def __init__(self, x, y):
        self.piece: Piece = None
        self.x = x
        self.y = y
        self.legal_placement_direction = self.setup_legal_placement_directions()

    def __eq__(self, other):
        if other is None:
            return False
        return self.x == other.x and self.y == other.y

    def __str__(self):
        return f"{self.x},{self.y}"

    def __repr__(self):
        return f"<Tile {self!s}>"

    def setup_legal_placement_directions(self) -> Optional[Direction]:
        if self.is_perimeter():
            if self.x == 0:
                return Direction.east
            if self.x == PLAY_AREA_SIZE - 1:
                return Direction.west
            if self.y == 0:
                return Direction.north
            if self.y == PLAY_AREA_SIZE - 1:
                return Direction.south
        return None

    def place_piece(self, piece: Piece):
        self.piece = piece
        piece.tile = self

    def remove_piece(self):
        self.piece = None

    def is_perimeter(self) -> bool:
        return self.x in PERIMETER_COORDINATES or self.y in PERIMETER_COORDINATES

    def is_empty(self) -> bool:
        return self.piece is None

    def is_placement_legal(self) -> bool:
        return self.piece is None and self.is_perimeter()

    def has_piece_of_player(self, owner_id: int) -> bool:
        return self.piece is not None and self.piece.owner_id == owner_id
