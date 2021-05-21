from game.player import Player
from game.placement_move import PlacementMove
from strategies.tree_search.abstract_move import AbstractMove, ScoredMove
from strategies.tree_search.config import WIN_CONDITION_SCORE
from strategies.tree_search.min_max import max
from typing import Optional, Callable, Tuple, List
import random

MinMaxCallable = Callable[[Player, Player, int, bool, int, int, int], AbstractMove]


def build_tree_search_strategy(depth: int) -> Callable[[Player, Player, bool], Optional[PlacementMove]]:
    def tree_search_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
        if len(player.get_legal_placement_moves()) == 0:
            return None

        best_move = get_best_move(player, opponent, is_starting, depth)
        print(f"{player!r} plays: {best_move!s}")
        placement_move = best_move.to_placement_move(player)
        return placement_move
    return tree_search_strategy


def get_best_moves(player: Player, opponent: Player, is_first_move: bool, depth: int) -> Tuple[List[AbstractMove], int]:
    alpha_starting_value = -WIN_CONDITION_SCORE * 10
    beta_starting_value = WIN_CONDITION_SCORE * 10
    best_moves, score, _ = max(player, opponent, depth, is_first_move, alpha_starting_value, beta_starting_value, depth)
    return best_moves, score


def get_best_move(player: Player, opponent: Player, is_first_move: bool, depth: int) -> ScoredMove:
    best_moves, score = get_best_moves(player, opponent, is_first_move, depth)
    best_move = None
    if len(best_moves) > 0:
        best_move = random.choice(best_moves)
    return ScoredMove(best_move, player, score)

