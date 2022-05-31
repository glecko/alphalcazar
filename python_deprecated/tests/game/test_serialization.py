from game.board import Board
from game.constants import PLAYER_1_ID
from game.piece import Piece
from game.enums import PieceType, Direction


EMPTY_BOARD_SERIALIZATION = ",,,,,,,,,,,,,,,,,,,,"


class TestSerialization(object):

    def test_board_serialization(self):
        board = Board()
        assert board.to_string_notation() == EMPTY_BOARD_SERIALIZATION
        piece_two = Piece(PLAYER_1_ID, PieceType.two)
        piece_two.set_movement_direction(Direction.east)
        board.get_tile(1, 1).place_piece(piece_two)
        assert board.get_tile(1, 1).to_string_notation() == "2|1|east"
        assert board.to_string_notation() == ",,,,2|1|east,,,,,,,,,,,,,,,,"
