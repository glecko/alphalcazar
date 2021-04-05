from game.piece import Piece
from game.enums import PieceType
from game.tile import Tile
from game.board import Board
from typing import List, Optional
import random


class PlacementMove(object):
    def __init__(self, piece: Piece, tile: Tile):
        self.piece = piece
        self.tile = tile

    def execute(self):
        self.tile.place_piece(self.piece)
        self.piece.set_movement_direction(self.tile.legal_placement_direction)


class Player(object):
    def __init__(self, player_id: int, board: Board):
        self.id = player_id
        self.board = board
        self.pieces = self.setup_pieces()

    def setup_pieces(self) -> List[Piece]:
        pieces = list()
        for order in PieceType:
            piece = Piece(owner_id=self.id, type=order)
            pieces.append(piece)
        return pieces

    def get_available_pieces(self):
        return [piece for piece in self.pieces if not piece.is_on_board()]

    def play_random_piece(self) -> Optional[PlacementMove]:
        legal_tiles = self.board.get_legal_tiles()
        available_pieces = self.get_available_pieces()
        if len(legal_tiles) > 0 and len(available_pieces) > 0:
            move = PlacementMove(piece=random.choice(available_pieces), tile=random.choice(legal_tiles))
            move.execute()
            return move
        return None

