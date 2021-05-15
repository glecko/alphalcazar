from game.player import Player, PlacementMove
from game.enums import Direction
from game.constants import CENTER_COORDINATE
from typing import Optional, List
import random


class AbstractMove(object):
    def __init__(self, placement_move: Optional[PlacementMove]):
        self.x = placement_move.tile.x
        self.y = placement_move.tile.y
        self.piece_type = placement_move.piece.type
        self.owner_id = placement_move.piece.owner_id

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y and self.owner_id == other.owner_id and self.piece_type == other.piece_type

    def __str__(self):
        return f"{self.piece_type!s} -> ({self.x}, {self.y})"

    def __repr__(self):
        return f"<AbstractMove {self!s}>"

    def to_placement_move(self, player: Player) -> Optional[PlacementMove]:
        piece = player.get_piece_by_type(self.piece_type)
        tile = player.board.get_tile(self.x, self.y)
        return PlacementMove(piece, tile)

    def execute(self, player: Player):
        placement_move = self.to_placement_move(player)
        if placement_move is not None:
            placement_move.execute()


class ScoredMove(object):
    def __init__(self, best_moves: List[AbstractMove], score: int):
        self.abstract_move = None
        if len(best_moves) > 0:
            self.abstract_move: Optional[AbstractMove] = random.choice(best_moves)
        self.score = score

    def __eq__(self, other):
        return self.abstract_move == other.abstract_move and self.score == other.score

    def to_placement_move(self, player: Player) -> Optional[PlacementMove]:
        return self.abstract_move.to_placement_move(player) if self.abstract_move is not None else None


def filter_symmetric_movements(scored_moves: List[AbstractMove], player: Player) -> List[AbstractMove]:
    board_pieces = player.game.board.get_board_pieces(exclude_perimeter=False)
    center_x_symmetry, center_y_symmetry = True, True
    for piece in board_pieces:
        if piece.tile.y != CENTER_COORDINATE or piece.direction in [Direction.north, Direction.south]:
            center_x_symmetry = False
        if piece.tile.x != CENTER_COORDINATE or piece.direction in [Direction.east, Direction.west]:
            center_y_symmetry = False

    def filter_function(move: AbstractMove) -> bool:
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


def get_legal_scored_moves(player: Player) -> List[AbstractMove]:
    placement_moves = player.get_legal_placement_moves()
    scored_moves = list()
    for move in placement_moves:
        scored_moves.append(AbstractMove(move))
    filtered_moves = filter_symmetric_movements(scored_moves, player)
    random.shuffle(filtered_moves)
    return filtered_moves
