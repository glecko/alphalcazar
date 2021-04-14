from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.piece import Piece
from game.enums import PieceType


class TestPiece(object):
    def test_piece_equality(self):
        assert Piece(PLAYER_1_ID, PieceType.two) == Piece(PLAYER_1_ID, PieceType.two)
        assert Piece(PLAYER_1_ID, PieceType.two) != Piece(PLAYER_2_ID, PieceType.two)
        assert Piece(PLAYER_1_ID, PieceType.two) != Piece(PLAYER_1_ID, PieceType.four)





