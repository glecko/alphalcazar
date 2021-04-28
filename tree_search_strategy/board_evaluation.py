from game.enums import GameResult
from game.board import Board
from tree_search_strategy.config import WIN_CONDITION_SCORE, TILE_SCORE_MULTIPLIER, PLACED_PIECE_SCORE

BOARD_SCORE_CACHE = dict()


def game_result_to_score(result: GameResult) -> int:
    if result == GameResult.win:
        return WIN_CONDITION_SCORE
    elif result == GameResult.draw:
        return 0
    elif result == GameResult.loss:
        return -WIN_CONDITION_SCORE


def evaluate_board(board: Board, player_id: int, opponent_id: int) -> int:
    # First check if we have this score in the cache already #
    cache_key = f"{player_id}#{board.to_string_notation()}"
    if BOARD_SCORE_CACHE.get(cache_key):
        return BOARD_SCORE_CACHE[cache_key]

    result = board.get_game_result(player_id, opponent_id)
    score = 0
    if result is not None:
        score = game_result_to_score(result)
    else:
        pieces = board.get_board_pieces(exclude_perimeter=True)
        for piece in pieces:
            tile_multiplier = TILE_SCORE_MULTIPLIER[piece.tile.x][piece.tile.y]
            piece_score = PLACED_PIECE_SCORE[piece.type] * tile_multiplier
            if piece.owner_id == player_id:
                score += piece_score
            else:
                score -= piece_score
    BOARD_SCORE_CACHE[cache_key] = score
    return score
