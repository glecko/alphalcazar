from game.player import Player, PlacementMove
from game.game import Game
from tree_search_strategy.scored_move import ScoredMove, get_legal_scored_moves
from tree_search_strategy.board_evaluation import evaluate_board, WIN_CONDITION_SCORE
from typing import Optional


def tree_search_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
    if len(player.get_legal_placement_moves()) == 0:
        return None

    best_move = max_move(player, opponent, 2, is_starting)
    return best_move.to_placement_move(player)


def max_move(player: Player, opponent: Player, depth: int, is_first_move: bool) -> ScoredMove:
    if depth == 0 and is_first_move:
        score = evaluate_board(player.board, player.id, opponent.id)
        return ScoredMove(None, score)
    moves = get_legal_scored_moves(player)
    best_move = ScoredMove(None, -WIN_CONDITION_SCORE - 1)
    for move in moves:
        game_clone = Game.from_string_notation(f"{player.id}#{player.board.to_string_notation()}")
        move.execute(game_clone.get_player_by_id(player.id))
        if is_first_move:
            best_next_move = min_move(game_clone.get_player_by_id(player.id), game_clone.get_player_by_id(opponent.id), depth, False)
        else:
            game_clone.board.execute_board_movements(opponent.id)
            best_next_move = max_move(game_clone.get_player_by_id(player.id), game_clone.get_player_by_id(opponent.id), depth - 1, True)

        if best_next_move.score > best_move.score:
            move.score = best_next_move.score
            best_move = move
    return best_move


def min_move(player: Player, opponent: Player, depth: int, is_first_move: bool) -> ScoredMove:
    if depth == 0 and is_first_move:
        score = evaluate_board(player.board, player.id, opponent.id)
        return ScoredMove(None, score)
    moves = get_legal_scored_moves(opponent)
    best_move = ScoredMove(None, WIN_CONDITION_SCORE + 1)
    for move in moves:
        game_clone = Game.from_string_notation(f"{player.id}#{player.board.to_string_notation()}")
        move.execute(game_clone.get_player_by_id(opponent.id))
        if is_first_move:
            best_next_move = max_move(game_clone.get_player_by_id(player.id), game_clone.get_player_by_id(opponent.id), depth, False)
        else:
            game_clone.board.execute_board_movements(player.id)
            best_next_move = min_move(game_clone.get_player_by_id(player.id), game_clone.get_player_by_id(opponent.id), depth - 1, True)

        if best_next_move.score < best_move.score:
            move.score = best_next_move.score
            best_move = move
    return best_move
