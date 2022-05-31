from game.player import Player
from game.placement_move import PlacementMove
from strategies.tree_search.abstract_move import AbstractMove, ScoredMove
from strategies.tree_search.config import WIN_CONDITION_SCORE, EvaluationType
from strategies.tree_search.min_max import max
from strategies.tree_search.persistence import hydrate_transposition_cache
from typing import Optional, Callable, Tuple, List
import random
import logging

logger = logging.getLogger(__name__)

MinMaxCallable = Callable[[Player, Player, int, bool, int, int, int], AbstractMove]


def build_tree_search_strategy(depth: int) -> Callable[[Player, Player, bool], Optional[PlacementMove]]:
    def tree_search_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
        if len(player.get_legal_placement_moves()) == 0:
            return None

        best_move = get_best_move(player, opponent, is_starting, depth)
        logger.info(f"{player!r} plays: {best_move!s}")
        placement_move = best_move.to_placement_move(player)
        return placement_move

    hydrate_transposition_cache()
    return tree_search_strategy


def get_best_moves(player: Player, opponent: Player, is_first_move: bool, depth: int) -> Tuple[List[AbstractMove], int, EvaluationType]:
    alpha_starting_value = -WIN_CONDITION_SCORE * 10
    beta_starting_value = WIN_CONDITION_SCORE * 10
    best_moves, score, eval_type = max(player, opponent, depth, is_first_move, alpha_starting_value, beta_starting_value)
    return best_moves, score, eval_type


def get_best_move(player: Player, opponent: Player, is_first_move: bool, depth: int) -> ScoredMove:
    best_moves, score, _ = get_best_moves(player, opponent, is_first_move, depth)
    logger.debug(f"Found {len(best_moves)} moves with score {score}.")
    best_move = random.choice(best_moves)
    return ScoredMove(best_move, player, score)

