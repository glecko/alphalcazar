from __future__ import annotations
from game.piece import Piece
from game.enums import PieceType
from game.tile import Tile
from game.board import Board
from typing import List

# To avoid the cyclic import with Game, only needed for type checking
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from game.game import Game


class PlacementMove(object):
    def __init__(self, piece: Piece, tile: Tile):
        self.piece = piece
        self.tile = tile

    def __str__(self):
        return f"{self.piece!r} -> {self.tile!r}"

    def __repr__(self):
        return f"<PlacementMove {self!s}>"

    def __eq__(self, other):
        return self.piece == other.piece and self.tile == other.tile

    def execute(self):
        self.tile.place_piece(self.piece)
        self.piece.set_movement_direction(self.tile.legal_placement_direction)


class Player(object):
    def __init__(self, player_id: int, board: Board, game: Game):
        self.id = player_id
        self.board = board
        self.game = game
        self.pieces = self.setup_pieces()

    def __str__(self):
        return f"{self.id}"

    def __repr__(self):
        return f"<Player {self!s}>"

    def setup_pieces(self) -> List[Piece]:
        pieces = list()
        for piece_type in PieceType:
            piece = Piece(owner_id=self.id, type=piece_type)
            pieces.append(piece)
        return pieces

    def get_available_pieces(self):
        return [piece for piece in self.pieces if not piece.is_on_board()]

    def get_piece_by_type(self, type: int) -> Piece:
        return next((piece for piece in self.pieces if piece.type == type), None)

    def get_legal_placement_moves(self) -> List[PlacementMove]:
        legal_tiles = self.board.get_legal_tiles()
        available_pieces = self.get_available_pieces()
        legal_moves: List[PlacementMove] = list()
        for tile in legal_tiles:
            for piece in available_pieces:
                move = PlacementMove(piece, tile)
                legal_moves.append(move)
        return legal_moves

