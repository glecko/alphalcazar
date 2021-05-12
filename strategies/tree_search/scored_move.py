from game.player import Player, PlacementMove
from game.enums import Direction
from game.piece import Piece
from game.constants import CENTER_COORDINATE
from typing import Optional, List, Tuple
import random


class ScoredMove(object):
    def __init__(self, placement_move: Optional[PlacementMove], score: Optional[int]):
        self.x = placement_move.tile.x if placement_move else None
        self.y = placement_move.tile.y if placement_move else None
        self.piece_type = placement_move.piece.type if placement_move else None
        self.score = score
        self.owner_id = placement_move.piece.owner_id if placement_move else None

    def __str__(self):
        return f"{self.piece_type!s} -> ({self.x}, {self.y}) "

    def __repr__(self):
        return f"<ScoredMove {self!s}>"

    def to_placement_move(self, player: Player) -> Optional[PlacementMove]:
        if self.x is None or self.y is None or self.piece_type is None:
            return None
        piece = player.get_piece_by_type(self.piece_type)
        tile = player.board.get_tile(self.x, self.y)
        return PlacementMove(piece, tile)

    def execute(self, player: Player):
        placement_move = self.to_placement_move(player)
        if placement_move is not None:
            placement_move.execute()


def filter_symmetric_movements(scored_moves: List[ScoredMove], player: Player) -> List[ScoredMove]:
    board_pieces = player.game.board.get_board_pieces(exclude_perimeter=False)
    center_x_symmetry, center_y_symmetry = True, True
    for piece in board_pieces:
        if piece.tile.y != CENTER_COORDINATE or piece.direction in [Direction.north, Direction.south]:
            center_x_symmetry = False
        if piece.tile.x != CENTER_COORDINATE or piece.direction in [Direction.east, Direction.west]:
            center_y_symmetry = False

    def filter_function(move: ScoredMove) -> bool:
        # If the board is empty, there are only 2 different tiles on which to play: center or corner
        # We simply select two arbitrary tiles of these types (4, 2) and (4, 3)
        empty_board_condition = move.x == 4 and (move.y == 2 or move.y == 3)
        if len(board_pieces) == 0:
            return empty_board_condition
        elif center_x_symmetry:
            return move.y >= CENTER_COORDINATE
        elif center_y_symmetry:
            return move.x >= CENTER_COORDINATE
        return True

    filtered_moves = list(filter(filter_function, scored_moves))
    return filtered_moves


def get_legal_scored_moves(player: Player) -> List[ScoredMove]:
    placement_moves = player.get_legal_placement_moves()
    scored_moves = list()
    for move in placement_moves:
        scored_moves.append(ScoredMove(move, None))
    filtered_moves = filter_symmetric_movements(scored_moves, player)
    random.shuffle(filtered_moves)
    return filtered_moves
