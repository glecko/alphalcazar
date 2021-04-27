from game.piece import PieceType
from game.enums import GameResult
from game.board import Board

WIN_CONDITION_SCORE = 900
DEPTH_PENALTY = 10

TILE_SCORE_MULTIPLIER = {
    1: {1: 1, 2: 1, 3: 1},
    2: {1: 1, 2: 2, 3: 1},
    3: {1: 1, 2: 1, 3: 1},
}


PLACED_PIECE_SCORE = {
    PieceType.one: 20,
    PieceType.two: 30,
    PieceType.three: 30,
    PieceType.four: -10,
    PieceType.five: 30,
}


def game_result_to_score(result: GameResult) -> int:
    if result == GameResult.win:
        return WIN_CONDITION_SCORE
    elif result == GameResult.draw:
        return 0
    elif result == GameResult.loss:
        return -WIN_CONDITION_SCORE


def evaluate_board(board: Board, player_id: int, opponent_id: int) -> int:
    result = board.get_game_result(player_id, opponent_id)
    if result is None:
        score = 0
        pieces = board.get_board_pieces(exclude_perimeter=True)
        for piece in pieces:
            tile_multiplier = TILE_SCORE_MULTIPLIER[piece.tile.x][piece.tile.y]
            piece_score = PLACED_PIECE_SCORE[piece.type] * tile_multiplier
            if piece.owner_id == player_id:
                score += piece_score
            else:
                score -= piece_score
        return score
    else:
        return game_result_to_score(result)
