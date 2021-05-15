from game.player import Player
from strategies.tree_search.abstract_move import AbstractMove, get_legal_scored_moves
from strategies.tree_search.board_evaluation import evaluate_board
from strategies.tree_search.config import WIN_CONDITION_SCORE, DEPTH_PENALTY
from strategies.tree_search.transposition import store_in_transposition_dict, get_best_move_from_transposition_dict
from typing import Callable, Tuple, List


MinMaxCallable = Callable[[Player, Player, int, bool, int, int, int], Tuple[List[AbstractMove], int]]


def board_needs_to_be_evaluated(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool) -> bool:
    return is_first_move and (remaining_depth == 0 or player.board.get_game_result(player.id, opponent.id) is not None)


def get_depth_adjusted_board_evaluation(player: Player, opponent: Player, initial_depth: int, remaining_depth: int) -> int:
    score = evaluate_board(player.board, player.id, opponent.id)
    if score > 0:
        score -= (initial_depth - remaining_depth) * DEPTH_PENALTY
    else:
        score += (initial_depth - remaining_depth) * DEPTH_PENALTY
    return score


def max(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int, initial_depth: int) -> Tuple[List[AbstractMove], int]:
    if board_needs_to_be_evaluated(player, opponent, remaining_depth, is_first_move):
        score = get_depth_adjusted_board_evaluation(player, opponent, initial_depth, remaining_depth)
        return list(), score

    cached_best_moves, cached_best_score = get_best_move_from_transposition_dict(player, remaining_depth, False)
    if cached_best_moves is not None:
        return cached_best_moves, cached_best_score

    moves = get_legal_scored_moves(player)
    best_moves, best_score = list(), -WIN_CONDITION_SCORE * 10
    for move in moves:
        best_next_moves, best_next_score = find_best_next_move(move, max, min, player, opponent, remaining_depth, is_first_move, alpha, beta, initial_depth)

        if best_next_score > best_score:
            move.score = best_next_score
            best_score = best_next_score
            best_moves = [move]
        elif best_next_score == best_score:
            best_moves.append(move)

        if best_score >= beta:
            store_in_transposition_dict(best_moves, best_score, player, remaining_depth, False)
            return best_moves, best_score

        if best_score > alpha:
            alpha = best_score

    store_in_transposition_dict(best_moves, best_score, player, remaining_depth, False)
    return best_moves, best_score


def min(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int, initial_depth: int) -> Tuple[List[AbstractMove], int]:
    if board_needs_to_be_evaluated(player, opponent, remaining_depth, is_first_move):
        score = get_depth_adjusted_board_evaluation(player, opponent, initial_depth, remaining_depth)
        return list(), score

    cached_best_moves, cached_best_score = get_best_move_from_transposition_dict(opponent, remaining_depth, True)
    if cached_best_moves is not None:
        return cached_best_moves, cached_best_score

    moves = get_legal_scored_moves(opponent)
    best_moves, best_score = list(), WIN_CONDITION_SCORE * 10
    for move in moves:
        best_next_moves, best_next_score = find_best_next_move(move, min, max, player, opponent, remaining_depth, is_first_move, alpha, beta, initial_depth)

        if best_next_score < best_score:
            move.score = best_next_score
            best_score = best_next_score
            best_moves = [move]
        elif best_next_score == best_score:
            best_moves.append(move)

        if best_score <= alpha:
            store_in_transposition_dict(best_moves, best_score, opponent, remaining_depth, True)
            return best_moves, best_score

        if best_score < beta:
            beta = best_score

    store_in_transposition_dict(best_moves, best_score, opponent, remaining_depth, True)
    return best_moves, best_score


def find_best_next_move(
        move: AbstractMove,
        auto_callback: MinMaxCallable,
        opposite_callback: MinMaxCallable,
        player: Player,
        opponent: Player,
        remaining_depth: int,
        is_first_move: bool,
        alpha: int,
        beta: int,
        initial_depth: int
) -> Tuple[List[AbstractMove], int]:
    game_clone = player.game.clone()
    player_clone = game_clone.get_player_by_id(player.id)
    opponent_clone = game_clone.get_player_by_id(opponent.id)
    move.execute(game_clone.get_player_by_id(move.owner_id))
    if is_first_move:
        game_clone.first_move_executed = True
        best_next_moves, best_next_score = opposite_callback(
            player_clone,
            opponent_clone,
            remaining_depth,
            False,
            alpha,
            beta,
            initial_depth
        )
    else:
        game_clone.board.execute_board_movements(game_clone.starting_player.id)
        game_clone.switch_starting_player()
        game_clone.first_move_executed = False
        best_next_moves, best_next_score = auto_callback(
            player_clone,
            opponent_clone,
            remaining_depth - 1,
            True,
            alpha,
            beta,
            initial_depth
        )
    return best_next_moves, best_next_score
