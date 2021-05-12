from game.board import Board
from game.constants import BOARD_SIZE, PLAY_AREA_SIZE, PLAYER_1_ID, PLAYER_2_ID
from game.piece import Piece
from game.enums import PieceType, GameResult


class TestBoard(object):

    def test_setup(self):
        board = Board()
        assert len(board.tiles) == PLAY_AREA_SIZE * PLAY_AREA_SIZE - 4
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None
        assert len(board.get_legal_tiles()) == BOARD_SIZE * 4
        assert board.has_complete_row(PLAYER_1_ID) is False

    def test_win_conditions(self):
        board = Board()
        board.get_tile(1, 1).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None

        board.get_tile(1, 2).place_piece(Piece(PLAYER_1_ID, PieceType.four))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None

        board.get_tile(2, 2).place_piece(Piece(PLAYER_1_ID, PieceType.five))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None

        board.get_tile(1, 3).place_piece(Piece(PLAYER_1_ID, PieceType.three))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is GameResult.win
        assert board.get_game_result(PLAYER_2_ID, PLAYER_1_ID) is GameResult.loss

        board.get_tile(3, 1).place_piece(Piece(PLAYER_2_ID, PieceType.three))
        board.get_tile(3, 2).place_piece(Piece(PLAYER_2_ID, PieceType.three))
        board.get_tile(3, 3).place_piece(Piece(PLAYER_2_ID, PieceType.three))
        assert board.get_game_result(PLAYER_2_ID, PLAYER_1_ID) is GameResult.draw

    def test_central_row_win_condition(self):
        board = Board()
        board.get_tile(2, 1).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        board.get_tile(2, 2).place_piece(Piece(PLAYER_1_ID, PieceType.one))
        board.get_tile(2, 3).place_piece(Piece(PLAYER_1_ID, PieceType.five))

        # Random pieces
        board.get_tile(3, 3).place_piece(Piece(PLAYER_2_ID, PieceType.four))
        board.get_tile(1, 1).place_piece(Piece(PLAYER_2_ID, PieceType.five))

        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is GameResult.win

    def test_diagonal_win_conditions(self):
        board = Board()
        board.get_tile(1, 1).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None
        board.get_tile(2, 2).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None
        board.get_tile(3, 3).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is GameResult.win

        board.get_tile(2, 2).remove_piece()
        board.get_tile(1, 3).place_piece(Piece(PLAYER_2_ID, PieceType.two))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None
        board.get_tile(2, 2).place_piece(Piece(PLAYER_2_ID, PieceType.two))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None
        board.get_tile(3, 1).place_piece(Piece(PLAYER_2_ID, PieceType.two))
        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is GameResult.loss

    def test_draw_by_multiple_wins(self):
        board = Board()

        board.get_tile(1, 2).place_piece(Piece(PLAYER_1_ID, PieceType.one))
        board.get_tile(2, 2).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        board.get_tile(3, 2).place_piece(Piece(PLAYER_1_ID, PieceType.three))

        board.get_tile(1, 1).place_piece(Piece(PLAYER_2_ID, PieceType.one))
        board.get_tile(2, 1).place_piece(Piece(PLAYER_2_ID, PieceType.two))
        board.get_tile(3, 1).place_piece(Piece(PLAYER_2_ID, PieceType.three))

        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is GameResult.draw

    def test_perimeter_exclusion_from_win_conditions(self):
        board = Board()
        board.get_tile(0, 1).place_piece(Piece(PLAYER_1_ID, PieceType.four))
        board.get_tile(0, 2).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        board.get_tile(0, 3).place_piece(Piece(PLAYER_1_ID, PieceType.one))

        board.get_tile(2, 1).place_piece(Piece(PLAYER_1_ID, PieceType.four))
        board.get_tile(3, 1).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        board.get_tile(4, 1).place_piece(Piece(PLAYER_1_ID, PieceType.one))

        assert board.get_game_result(PLAYER_1_ID, PLAYER_2_ID) is None

    def test_board_pieces(self):
        board = Board()
        assert board.get_board_pieces() == []

        board.get_tile(0, 1).place_piece(Piece(PLAYER_1_ID, PieceType.four))
        board.get_tile(2, 2).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        board.get_tile(3, 3).place_piece(Piece(PLAYER_1_ID, PieceType.one))

        assert len(board.get_board_pieces()) == 3
        assert len(board.get_board_pieces(exclude_perimeter=True)) == 2

    def test_tile_neighbors(self):
        board = Board()

        assert len(board.get_tile_neighbors(board.get_tile(2, 2), False)) == 4
        assert len(board.get_tile_neighbors(board.get_tile(2, 2), True)) == 8
        assert len(board.get_tile_perimeter_neightbors(board.get_tile(2, 2), True)) == 0
        assert len(board.get_tile_perimeter_neightbors(board.get_tile(2, 2), False)) == 0

        assert len(board.get_tile_perimeter_neightbors(board.get_tile(1, 1), False)) == 2
        assert len(board.get_tile_perimeter_neightbors(board.get_tile(1, 1), True)) == 4
