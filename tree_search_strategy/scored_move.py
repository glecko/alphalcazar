from game.player import Player, PlacementMove
from typing import Optional, List


class ScoredMove(object):
    def __init__(self, placement_move: Optional[PlacementMove], score: Optional[int]):
        self.x = placement_move.tile.x if placement_move else None
        self.y = placement_move.tile.y if placement_move else None
        self.piece_type = placement_move.piece.type if placement_move else None
        self.score = score

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


def get_legal_scored_moves(player: Player) -> List[ScoredMove]:
    placement_moves = player.get_legal_placement_moves()
    scored_moves = list()
    for move in placement_moves:
        scored_moves.append(ScoredMove(move, None))
    return scored_moves
