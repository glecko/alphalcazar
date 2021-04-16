from game.player import Player, PlacementMove
from game.game import Game
from tree_search_strategy.scored_move import ScoredMove, get_legal_scored_moves
from tree_search_strategy.board_evaluation import evaluate_board, WIN_CONDITION_SCORE, DEPTH_PENALTY
from typing import Optional, Callable


def build_tree_search_strategy(depth: int) -> Callable[[Player, Player, bool], Optional[PlacementMove]]:
    def tree_search_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
        if len(player.get_legal_placement_moves()) == 0:
            return None

        best_move = MinMaxState(player, opponent, is_starting, depth).get_best_move()
        return best_move.to_placement_move(player)
    return tree_search_strategy


class MinMaxState(object):
    def __init__(self, player: Player, opponent: Player, is_first_move: bool, depth: int):
        self.player = player
        self.opponent = opponent
        self.is_first_move = is_first_move
        self.depth = depth

    def get_best_move(self) -> ScoredMove:
        alpha_starting_value = -WIN_CONDITION_SCORE - 1
        beta_starting_value = WIN_CONDITION_SCORE + 1
        return self.max(self.player, self.opponent, self.depth, self.is_first_move, alpha_starting_value, beta_starting_value)

    def max(self, player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int) -> ScoredMove:
        if is_first_move and (remaining_depth == 0 or player.board.get_game_result(player.id, opponent.id) is not None):
            score = evaluate_board(player.board, player.id, opponent.id)
            score -= (self.depth - remaining_depth) * DEPTH_PENALTY
            return ScoredMove(None, score)
        moves = get_legal_scored_moves(player)
        best_move = ScoredMove(None, -WIN_CONDITION_SCORE * 10)
        for move in moves:
            game_clone = player.game.clone()
            move.execute(game_clone.get_player_by_id(player.id))
            if is_first_move:
                best_next_move = self.min(
                    game_clone.get_player_by_id(player.id),
                    game_clone.get_player_by_id(opponent.id),
                    remaining_depth,
                    is_first_move=False,
                    alpha=alpha,
                    beta=beta
                )
            else:
                game_clone.board.execute_board_movements(opponent.id)
                best_next_move = self.max(
                    game_clone.get_player_by_id(player.id),
                    game_clone.get_player_by_id(opponent.id),
                    remaining_depth - 1,
                    is_first_move=True,
                    alpha=alpha,
                    beta=beta
                )

            if best_next_move.score > best_move.score:
                move.score = best_next_move.score
                best_move = move

            if best_move.score >= beta:
                return best_move

            if best_move.score > alpha:
                alpha = best_move.score
        return best_move

    def min(self, player: Player, opponent: Player, remaining_depth: int, is_first_move: bool, alpha: int, beta: int) -> ScoredMove:
        if is_first_move and (remaining_depth == 0 or player.board.get_game_result(player.id, opponent.id) is not None):
            score = evaluate_board(player.board, player.id, opponent.id)
            score += (self.depth - remaining_depth) * DEPTH_PENALTY
            return ScoredMove(None, score)
        moves = get_legal_scored_moves(opponent)
        best_move = ScoredMove(None, WIN_CONDITION_SCORE * 10)
        for move in moves:
            game_clone = player.game.clone()
            move.execute(game_clone.get_player_by_id(opponent.id))
            if is_first_move:
                best_next_move = self.max(
                    game_clone.get_player_by_id(player.id),
                    game_clone.get_player_by_id(opponent.id),
                    remaining_depth,
                    is_first_move=False,
                    alpha=alpha,
                    beta=beta
                )
            else:
                game_clone.board.execute_board_movements(player.id)
                best_next_move = self.min(
                    game_clone.get_player_by_id(player.id),
                    game_clone.get_player_by_id(opponent.id),
                    remaining_depth - 1,
                    is_first_move=True,
                    alpha=alpha,
                    beta=beta
                )

            if best_next_move.score < best_move.score:
                move.score = best_next_move.score
                best_move = move

            if best_move.score <= alpha:
                return best_move

            if best_move.score < beta:
                beta = best_move.score
        return best_move
