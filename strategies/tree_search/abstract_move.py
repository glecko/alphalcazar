from game.player import Player, PlacementMove
from game.enums import Direction, PieceType
from game.constants import CENTER_COORDINATE
from strategies.tree_search.config import PIECE_TYPE_COORDINATE_SORTING_ORDER, PIECE_ENTRY_APPEARS_BLOCKED_ORDER
from typing import Optional, List


class AbstractMove(object):
    def __init__(self, placement_move: Optional[PlacementMove], player: Optional[Player]):
        self.x = placement_move.tile.x if placement_move is not None else None
        self.y = placement_move.tile.y if placement_move is not None else None
        self.piece_type = placement_move.piece.type if placement_move is not None else None
        self.owner_id = player.id if player else None

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y and self.owner_id == other.owner_id and self.piece_type == other.piece_type

    def __str__(self):
        return f"{self.piece_type!s} -> ({self.x}, {self.y})"

    def __repr__(self):
        return f"<AbstractMove {self!s}>"

    @classmethod
    def from_raw_values(cls, x: int, y: int, piece_type: PieceType, owner_id: int):
        move = cls(None, None)
        move.x, move.y, move.piece_type, move.owner_id = x, y, piece_type, owner_id
        return move

    def is_empty_movement(self):
        return self.x is None or self.y is None or self.piece_type is None

    def to_sorting_order(self, player: Player) -> int:
        if self.is_empty_movement():
            return 0

        # We check for movements that appear would lead to our piece not entering and rank those last
        placement_move = self.to_placement_move(player)
        target_tile = placement_move.get_board_target_tile()
        if target_tile.piece is not None and not placement_move.piece.is_pusher() and not target_tile.piece.is_pushable():
            if target_tile.piece.is_exiting_board() or target_tile.piece.type > self.piece_type:
                # It appears that our piece would not be able to enter
                return PIECE_ENTRY_APPEARS_BLOCKED_ORDER

        return PIECE_TYPE_COORDINATE_SORTING_ORDER[self.piece_type][self.x][self.y]

    def to_placement_move(self, player: Player) -> Optional[PlacementMove]:
        if self.is_empty_movement():
            return None
        piece = player.get_piece_by_type(self.piece_type)
        tile = player.board.get_tile(self.x, self.y)
        return PlacementMove(piece, tile, player.board)

    def execute(self, player: Player):
        placement_move = self.to_placement_move(player)
        if placement_move is not None:
            placement_move.execute()


class ScoredMove(AbstractMove):
    def __init__(self, abstract_move: AbstractMove, player: Player, score: int):
        super().__init__(abstract_move.to_placement_move(player), player)
        self.score = score

    def __repr__(self):
        return f"<ScoredMove {self!s}>"

    def __str__(self):
        return f"{super().__str__()!s} ({self.score})"


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


def get_legal_abstract_moves(player: Player, filter_symmetric_moves: bool) -> List[AbstractMove]:
    placement_moves = player.get_legal_placement_moves()
    moves = list()
    for move in placement_moves:
        moves.append(AbstractMove(move, player))
    if filter_symmetric_moves:
        moves = filter_symmetric_movements(moves, player)
    if len(moves) == 0:
        moves.append(AbstractMove(None, player))
    moves.sort(key=lambda m: m.to_sorting_order(player), reverse=True)
    return moves
