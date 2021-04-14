from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.piece import Piece
from game.enums import PieceType
from game.board import Board
from tree_search_strategy.board_evaluation import evaluate_board, PLACED_PIECE_SCORE, WIN_CONDITION_SCORE


class TestBoardEvaluation(object):

    def test_piece_board_evaluation(self):
        board = Board()

        board.get_tile(2, 2).place_piece(Piece(PLAYER_1_ID, PieceType.two))

        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == PLACED_PIECE_SCORE[PieceType.two]

        board.get_tile(2, 1).place_piece(Piece(PLAYER_2_ID, PieceType.two))

        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == 0

        board.get_tile(1, 1).place_piece(Piece(PLAYER_2_ID, PieceType.three))

        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == -PLACED_PIECE_SCORE[PieceType.three]

    def test_win_condition_board_evaluation(self):
        board = Board()

        board.get_tile(1, 1).place_piece(Piece(PLAYER_1_ID, PieceType.one))
        board.get_tile(2, 2).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        board.get_tile(3, 3).place_piece(Piece(PLAYER_1_ID, PieceType.three))

        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == WIN_CONDITION_SCORE
        assert evaluate_board(board, PLAYER_2_ID, PLAYER_1_ID) == -WIN_CONDITION_SCORE
