from game.player import Player
from strategies.tree_search.abstract_move import AbstractMove, get_legal_abstract_moves
from strategies.tree_search.board_evaluation import evaluate_board, get_depth_adjusted_score
from strategies.tree_search.config import WIN_CONDITION_SCORE, EvaluationType
from strategies.tree_search.transposition import store_in_transposition_dict, get_best_move_from_transposition_dict
from typing import Callable, Tuple, List


MinMaxCallable = Callable[[Player, Player, int, bool, int, int], Tuple[List[AbstractMove], int, EvaluationType]]


def board_needs_to_be_evaluated(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool) -> bool:
    return is_first_move and (remaining_depth == 0 or player.board.get_game_result(player.id, opponent.id) is not None)


def max(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int) -> Tuple[List[AbstractMove], int, EvaluationType]:
    if board_needs_to_be_evaluated(player, opponent, remaining_depth, is_first_move):
        score = evaluate_board(player.board, player.id, opponent.id)
        return list(), score, EvaluationType.exact

    cached_best_moves, cached_best_score, cached_eval_type = get_best_move_from_transposition_dict(player, remaining_depth, False, alpha, beta)
    if cached_best_moves is not None:
        return cached_best_moves, cached_best_score, cached_eval_type

    moves = get_legal_abstract_moves(player, filter_symmetric_moves=True)
    eval_type = EvaluationType.exact
    best_moves, best_score = list(), -WIN_CONDITION_SCORE * 10
    for move in moves:
        best_next_moves, best_next_score, next_eval_type = find_best_next_move(move, max, min, player, opponent, remaining_depth, is_first_move, alpha, beta)

        if best_next_score > best_score:
            move.score = best_next_score
            best_moves, best_score, eval_type = [move], best_next_score, next_eval_type
        elif best_next_score == best_score and next_eval_type == EvaluationType.exact:
            best_moves.append(move)

        alpha = best_score if best_score > alpha else alpha
        if alpha > beta:
            eval_type = EvaluationType.beta_cutoff
            break

    store_in_transposition_dict(best_moves, best_score, eval_type, player, remaining_depth, False)
    return best_moves, best_score, eval_type


def min(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int) -> Tuple[List[AbstractMove], int, EvaluationType]:
    if board_needs_to_be_evaluated(player, opponent, remaining_depth, is_first_move):
        score = evaluate_board(player.board, player.id, opponent.id)
        return list(), score, EvaluationType.exact

    cached_best_moves, cached_best_score, cached_eval_type = get_best_move_from_transposition_dict(opponent, remaining_depth, True, alpha, beta)
    if cached_best_moves is not None:
        return cached_best_moves, cached_best_score, cached_eval_type

    moves = get_legal_abstract_moves(opponent, filter_symmetric_moves=False)
    eval_type = EvaluationType.exact
    best_moves, best_score = list(), WIN_CONDITION_SCORE * 10
    for move in moves:
        best_next_moves, best_next_score, next_eval_type = find_best_next_move(move, min, max, player, opponent, remaining_depth, is_first_move, alpha, beta)

        if best_next_score < best_score:
            move.score = best_next_score
            best_moves, best_score, eval_type = [move], best_next_score, next_eval_type
        elif best_next_score == best_score and next_eval_type == EvaluationType.exact:
            best_moves.append(move)

        beta = best_score if best_score < beta else beta
        if beta < alpha:
            eval_type = EvaluationType.alpha_cutoff
            break

    store_in_transposition_dict(best_moves, best_score, eval_type, opponent, remaining_depth, True)
    return best_moves, best_score, eval_type


def find_best_next_move(
        move: AbstractMove,
        auto_callback: MinMaxCallable,
        opposite_callback: MinMaxCallable,
        player: Player,
        opponent: Player,
        remaining_depth: int,
        is_first_move: bool,
        alpha: int,
        beta: int
) -> Tuple[List[AbstractMove], int, EvaluationType]:
    game_clone = player.game.clone()
    player_clone = game_clone.get_player_by_id(player.id)
    opponent_clone = game_clone.get_player_by_id(opponent.id)
    move.execute(game_clone.get_player_by_id(move.owner_id))
    if is_first_move:
        game_clone.first_move_executed = True
        best_next_moves, best_next_score, eval_type = opposite_callback(
            player_clone,
            opponent_clone,
            remaining_depth,
            False,
            alpha,
            beta
        )
    else:
        game_clone.board.execute_board_movements(game_clone.starting_player.id)
        game_clone.switch_starting_player()
        game_clone.first_move_executed = False
        best_next_moves, best_next_score, eval_type = auto_callback(
            player_clone,
            opponent_clone,
            remaining_depth - 1,
            True,
            alpha,
            beta
        )
        best_next_score = get_depth_adjusted_score(best_next_score)
    return best_next_moves, best_next_score, eval_type
