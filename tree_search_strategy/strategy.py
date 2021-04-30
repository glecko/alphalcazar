from game.player import Player, PlacementMove
from game.game import Game
from tree_search_strategy.scored_move import ScoredMove, get_legal_scored_moves
from tree_search_strategy.board_evaluation import evaluate_board
from tree_search_strategy.config import WIN_CONDITION_SCORE, DEPTH_PENALTY
from typing import Optional, Callable

MinMaxCallable = Callable[[Player, Player, int, bool, int, int, int], ScoredMove]


def build_tree_search_strategy(depth: int) -> Callable[[Player, Player, bool], Optional[PlacementMove]]:
    def tree_search_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
        if len(player.get_legal_placement_moves()) == 0:
            return None

        best_move = get_best_move(player, opponent, is_starting, depth)
        return best_move.to_placement_move(player)
    return tree_search_strategy


def get_best_move(player: Player, opponent: Player, is_first_move: bool, depth: int) -> ScoredMove:
    alpha_starting_value = -WIN_CONDITION_SCORE - 1
    beta_starting_value = WIN_CONDITION_SCORE + 1
    return max(player, opponent, depth, is_first_move, alpha_starting_value, beta_starting_value, depth)


def find_best_next_move(
        auto_callback: MinMaxCallable,
        opposite_callback: MinMaxCallable,
        move: ScoredMove,
        game: Game,
        player_id: int,
        opponent_id: int,
        remaining_depth: int,
        is_first_move: bool,
        alpha: int, beta: int,
        initial_depth: int
) -> ScoredMove:
    player = game.get_player_by_id(player_id)
    opponent = game.get_player_by_id(opponent_id)
    move.execute(game.get_player_by_id(move.owner_id))
    if is_first_move:
        best_next_move = opposite_callback(
            player,
            opponent,
            remaining_depth,
            False,
            alpha,
            beta,
            initial_depth
        )
    else:
        game.board.execute_board_movements(game.starting_player.id)
        game.switch_starting_player()
        best_next_move = auto_callback(
            player,
            opponent,
            remaining_depth - 1,
            True,
            alpha,
            beta,
            initial_depth
        )
    return best_next_move


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
        game_clone = player.game.clone()
        best_next_move = find_best_next_move(max, min, move, game_clone, player.id, opponent.id, remaining_depth, is_first_move, alpha, beta, initial_depth)

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
        game_clone = player.game.clone()
        best_next_move = find_best_next_move(min, max, move, game_clone, player.id, opponent.id, remaining_depth, is_first_move, alpha, beta, initial_depth)

        if best_next_move.score < best_move.score:
            move.score = best_next_move.score
            best_move = move

        if best_move.score <= alpha:
            return best_move

        if best_move.score < beta:
            beta = best_move.score
    return best_move
