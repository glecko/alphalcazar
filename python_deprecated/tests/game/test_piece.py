from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.piece import Piece
from game.enums import PieceType, Direction
from game.board import Board


class TestPiece(object):
    def test_piece_equality(self):
        assert Piece(PLAYER_1_ID, PieceType.two) == Piece(PLAYER_1_ID, PieceType.two)
        assert Piece(PLAYER_1_ID, PieceType.two) != Piece(PLAYER_2_ID, PieceType.two)
        assert Piece(PLAYER_1_ID, PieceType.two) != Piece(PLAYER_1_ID, PieceType.four)

    def test_piece_about_to_exit(self):
        board = Board()

        center_piece = Piece(PLAYER_1_ID, PieceType.two)
        center_piece.set_movement_direction(Direction.east)
        board.get_tile(2, 2).place_piece(center_piece)
        assert center_piece.is_exiting_board() is False

        top_piece = Piece(PLAYER_1_ID, PieceType.two)
        top_piece.set_movement_direction(Direction.north)
        board.get_tile(2, 3).place_piece(top_piece)
        assert top_piece.is_exiting_board() is True

        top_piece.set_movement_direction(Direction.west)
        assert top_piece.is_exiting_board() is False

        east_piece = Piece(PLAYER_1_ID, PieceType.two)
        east_piece.set_movement_direction(Direction.east)
        board.get_tile(3, 1).place_piece(east_piece)
        assert east_piece.is_exiting_board() is True

        east_piece.set_movement_direction(Direction.west)
        assert east_piece.is_exiting_board() is False





