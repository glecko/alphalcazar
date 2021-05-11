from game.player import Player
from strategies.tree_search.scored_move import ScoredMove, get_legal_scored_moves
from strategies.tree_search.board_evaluation import evaluate_board
from strategies.tree_search.config import WIN_CONDITION_SCORE, DEPTH_PENALTY
from typing import Callable


MinMaxCallable = Callable[[Player, Player, int, bool, int, int, int], ScoredMove]


def board_needs_to_be_evaluated(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool) -> bool:
    return is_first_move and (remaining_depth == 0 or player.board.get_game_result(player.id, opponent.id) is not None)


def get_depth_adjusted_board_evaluation(player: Player, opponent: Player, initial_depth: int, remaining_depth: int) -> int:
    score = evaluate_board(player.board, player.id, opponent.id)
    if score > 0:
        score -= (initial_depth - remaining_depth) * DEPTH_PENALTY
    else:
        score += (initial_depth - remaining_depth) * DEPTH_PENALTY
    return score


def max(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int, initial_depth: int) -> ScoredMove:
    if board_needs_to_be_evaluated(player, opponent, remaining_depth, is_first_move):
        score = get_depth_adjusted_board_evaluation(player, opponent, initial_depth, remaining_depth)
        return ScoredMove(None, score)
    moves = get_legal_scored_moves(player)
    best_move = ScoredMove(None, -WIN_CONDITION_SCORE * 10)
    for move in moves:
        best_next_move = find_best_next_move(move, max, min, player, opponent, remaining_depth, is_first_move, alpha, beta, initial_depth)

        if best_next_move.score > best_move.score:
            move.score = best_next_move.score
            best_move = move

        if best_move.score >= beta:
            return best_move

        if best_move.score > alpha:
            alpha = best_move.score
    return best_move


def min(player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int, initial_depth: int) -> ScoredMove:
    if board_needs_to_be_evaluated(player, opponent, remaining_depth, is_first_move):
        score = get_depth_adjusted_board_evaluation(player, opponent, initial_depth, remaining_depth)
        return ScoredMove(None, score)
    moves = get_legal_scored_moves(opponent)
    best_move = ScoredMove(None, WIN_CONDITION_SCORE * 10)
    for move in moves:
        best_next_move = find_best_next_move(move, min, max, player, opponent, remaining_depth, is_first_move, alpha, beta, initial_depth)

        if best_next_move.score < best_move.score:
            move.score = best_next_move.score
            best_move = move

        if best_move.score <= alpha:
            return best_move

        if best_move.score < beta:
            beta = best_move.score
    return best_move


def find_best_next_move(
        move: ScoredMove,
        auto_callback: MinMaxCallable,
        opposite_callback: MinMaxCallable,
        player: Player,
        opponent: Player,
        remaining_depth: int,
        is_first_move: bool,
        alpha: int,
        beta: int,
        initial_depth: int
) -> ScoredMove:
    game_clone = player.game.clone()
    player_clone = game_clone.get_player_by_id(player.id)
    opponent_clone = game_clone.get_player_by_id(opponent.id)
    move.execute(game_clone.get_player_by_id(move.owner_id))
    if is_first_move:
        best_next_move = opposite_callback(
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
        best_next_move = auto_callback(
            player_clone,
            opponent_clone,
            remaining_depth - 1,
            True,
            alpha,
            beta,
            initial_depth
        )
    return best_next_move
