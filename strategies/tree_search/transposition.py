from game.player import Player
from strategies.tree_search.abstract_move import AbstractMove
from typing import Optional, Dict, List, Tuple


TRANSPOSITION_DICT: Dict[str, Tuple[List[AbstractMove], int, int]] = dict()


def get_board_depth_hash_key(player: Player) -> str:
    return f"{player.id}#{player.board.to_string_notation()}"


def get_best_move_from_transposition_dict(player: Player, remaining_depth: int, inverse_score: bool) -> Tuple[Optional[List[AbstractMove]], Optional[int]]:
    board_hash_key = get_board_depth_hash_key(player)
    if TRANSPOSITION_DICT.get(board_hash_key) is not None:
        best_moves, best_score, explored_depth = TRANSPOSITION_DICT[board_hash_key]
        if explored_depth >= remaining_depth:
            if inverse_score:
                return best_moves, -best_score
            return best_moves, best_score
    return None, None


def store_in_transposition_dict(best_moves: List[AbstractMove], best_score: int, player: Player, depth: int, inverse_score: bool):
    board_hash_key = get_board_depth_hash_key(player)

    # We only overwrite the value in cache if our depth value is higher than the max explored depth
    # already present in the cache
    if TRANSPOSITION_DICT.get(board_hash_key) is not None:
        _, _, explored_depth = TRANSPOSITION_DICT[board_hash_key]
        if explored_depth >= depth:
            return

    if inverse_score:
        TRANSPOSITION_DICT[board_hash_key] = best_moves, -best_score, depth
    else:
        TRANSPOSITION_DICT[board_hash_key] = best_moves, best_score, depth
