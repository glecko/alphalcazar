from game.player import Player, PlacementMove
from strategies.tree_search.abstract_move import AbstractMove, ScoredMove
from strategies.tree_search.config import WIN_CONDITION_SCORE
from strategies.tree_search.min_max import max
from strategies.tree_search.multiprocessing import max_multiprocess
from typing import Optional, Callable

MinMaxCallable = Callable[[Player, Player, int, bool, int, int, int], AbstractMove]


def build_tree_search_strategy(depth: int, multiprocessing=False) -> Callable[[Player, Player, bool], Optional[PlacementMove]]:
    def tree_search_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
        if len(player.get_legal_placement_moves()) == 0:
            return None

        best_move = get_best_move(player, opponent, is_starting, depth, multiprocessing)
        placement_move = best_move.to_placement_move(player)
        print(f"{player!r} plays: {placement_move!s}")
        return placement_move
    return tree_search_strategy


def get_best_move(player: Player, opponent: Player, is_first_move: bool, depth: int, multiprocessing=False) -> ScoredMove:
    alpha_starting_value = -WIN_CONDITION_SCORE - 1
    beta_starting_value = WIN_CONDITION_SCORE + 1
    if multiprocessing:
        best_moves, score = max_multiprocess(player, opponent, depth, is_first_move, alpha_starting_value, beta_starting_value)
    else:
        best_moves, score = max(player, opponent, depth, is_first_move, alpha_starting_value, beta_starting_value, depth)
    return ScoredMove(best_moves, score)

