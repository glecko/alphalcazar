from game.piece import PieceType
from game.enums import GameResult
from game.board import Board

WIN_CONDITION_SCORE = 900
DEPTH_PENALTY = 10


PLACED_PIECE_SCORE = {
    PieceType.one: 20,
    PieceType.two: 20,
    PieceType.three: 20,
    PieceType.four: -10,
    PieceType.five: 20,
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
        pieces = board.get_board_pieces()
        for piece in pieces:
            if piece.owner_id == player_id:
                score += PLACED_PIECE_SCORE[piece.type]
            else:
                score -= PLACED_PIECE_SCORE[piece.type]
        return score
    else:
        return game_result_to_score(result)
