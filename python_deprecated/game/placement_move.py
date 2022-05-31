from game.board import Board
from game.constants import DIRECTION_MOVEMENT_OFFSETS
from game.piece import Piece
from game.tile import Tile


class PlacementMove(object):
    def __init__(self, piece: Piece, tile: Tile, board: Board):
        self.piece = piece
        self.tile = tile
        self.board = board

    def __str__(self):
        return f"{self.piece!r} -> {self.tile!r}"

    def __repr__(self):
        return f"<PlacementMove {self!s}>"

    def __eq__(self, other):
        return self.piece == other.piece and self.tile == other.tile

    def get_board_target_tile(self) -> Tile:
        x, y = DIRECTION_MOVEMENT_OFFSETS.get(self.tile.legal_placement_direction)
        return self.board.get_tile(self.tile.x + x, self.tile.y + y)

    def execute(self):
        self.tile.place_piece(self.piece)
        self.piece.set_movement_direction(self.tile.legal_placement_direction)
