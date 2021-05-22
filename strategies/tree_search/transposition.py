from game.player import Player
from strategies.tree_search.abstract_move import AbstractMove
from strategies.tree_search.config import EvaluationType
from typing import Optional, Dict, List, Tuple


TRANSPOSITION_DICT: Dict[str, Tuple[List[AbstractMove], int, int, EvaluationType]] = dict()


def evaluation_type_valid(eval_type: EvaluationType, score: int, alpha: int, beta: int) -> bool:
    if eval_type == EvaluationType.exact:
        return True

    if eval_type == EvaluationType.beta_cutoff:
        # We know that the node was not evaluated completely because 1 of the nodes had a value higher than beta
        # Which means the "real" value of the node is "score" or higher
        # We can use this value if we are under similar circumstances (score is higher than our beta)
        return score > beta
    if eval_type == EvaluationType.alpha_cutoff:
        # We know that the node was not evaluated completely because 1 of the nodes had a value lower than alpha
        # Which means the "real" value of the node is "score" or lower
        # We can use this value if we are under similar circumstances (score is lower than our alpha)
        return score < alpha
    return False


def get_board_depth_hash_key(player: Player) -> str:
    return f"{player.id}#{player.board.to_string_notation()}"


def get_best_move_from_transposition_dict(player: Player, remaining_depth: int, inverse_score: bool, alpha: int, beta: int) -> Tuple[Optional[List[AbstractMove]], Optional[int], Optional[EvaluationType]]:
    board_hash_key = get_board_depth_hash_key(player)
    if TRANSPOSITION_DICT.get(board_hash_key) is not None:
        best_moves, best_score, explored_depth, evaluation_type = TRANSPOSITION_DICT[board_hash_key]
        if explored_depth >= remaining_depth and evaluation_type_valid(evaluation_type, best_score, alpha, beta):
            if inverse_score:
                return best_moves, -best_score, evaluation_type
            return best_moves, best_score, evaluation_type
    return None, None, None


def store_in_transposition_dict(best_moves: List[AbstractMove], best_score: int, evaluation_type: EvaluationType, player: Player, depth: int, inverse_score: bool):
    board_hash_key = get_board_depth_hash_key(player)

    if TRANSPOSITION_DICT.get(board_hash_key) is not None:
        _, _, explored_depth, stored_eval_type = TRANSPOSITION_DICT[board_hash_key]
        # We only overwrite the value in cache if our depth value is higher than the max explored depth
        # already present in the cache
        if explored_depth > depth:
            return
        # In case of equal depths, we only overwrite alpha/beta cutoff evaluations with exact evaluations
        if explored_depth <= depth and evaluation_type != EvaluationType.exact:
            return

    if inverse_score:
        TRANSPOSITION_DICT[board_hash_key] = best_moves, -best_score, depth, evaluation_type
    else:
        TRANSPOSITION_DICT[board_hash_key] = best_moves, best_score, depth, evaluation_type
