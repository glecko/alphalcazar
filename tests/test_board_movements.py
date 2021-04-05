from game.board import Board
from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.piece import Piece
from game.tile import Tile
from game.enums import PieceType, Direction


class TestBoardMovements(object):
    def test_pieces_movement_order(self):
        board = Board()
        board.get_tile(1, 1).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        assert board.get_movement_ordered_pieces(PLAYER_1_ID) == [
            Piece(PLAYER_1_ID, PieceType.two)
        ]

        board.get_tile(1, 2).place_piece(Piece(PLAYER_2_ID, PieceType.one))
        board.get_tile(1, 3).place_piece(Piece(PLAYER_2_ID, PieceType.two))
        board.get_tile(2, 1).place_piece(Piece(PLAYER_1_ID, PieceType.four))

        assert board.get_movement_ordered_pieces(PLAYER_1_ID) == [
            Piece(PLAYER_2_ID, PieceType.one),
            Piece(PLAYER_1_ID, PieceType.two),
            Piece(PLAYER_2_ID, PieceType.two),
            Piece(PLAYER_1_ID, PieceType.four)
        ]

        assert board.get_movement_ordered_pieces(PLAYER_2_ID) == [
            Piece(PLAYER_2_ID, PieceType.one),
            Piece(PLAYER_2_ID, PieceType.two),
            Piece(PLAYER_1_ID, PieceType.two),
            Piece(PLAYER_1_ID, PieceType.four)
        ]

    def test_get_chained_push_movements(self):
        board = Board()
        board.get_tile(0, 3).place_piece(Piece(PLAYER_1_ID, PieceType.four))
        board.get_tile(1, 3).place_piece(Piece(PLAYER_1_ID, PieceType.two))
        board.get_tile(2, 3).place_piece(Piece(PLAYER_2_ID, PieceType.three))
        board.get_tile(4, 3).place_piece(Piece(PLAYER_2_ID, PieceType.one))
        movements = board.get_chained_push_movements(board.get_tile(0, 3), board.get_tile(1, 3))
        assert len(movements) == 3
        assert movements == [
            (Tile(0, 3), Tile(1, 3)),
            (Tile(1, 3), Tile(2, 3)),
            (Tile(2, 3), Tile(3, 3)),
        ]

    def test_get_chained_push_movements_empty(self):
        board = Board()
        board.get_tile(1, 3).place_piece(Piece(PLAYER_1_ID, PieceType.four))
        movements = board.get_chained_push_movements(board.get_tile(1, 3), board.get_tile(1, 2))
        assert movements == [
            (Tile(1, 3), Tile(1, 2)),
        ]

    def test_get_chained_push_movements_edge(self):
        board = Board()
        board.get_tile(3, 3).place_piece(Piece(PLAYER_1_ID, PieceType.four))
        board.get_tile(3, 4).place_piece(Piece(PLAYER_1_ID, PieceType.one))
        movements = board.get_chained_push_movements(board.get_tile(3, 3), board.get_tile(3, 4))
        assert movements == [
            (Tile(3, 3), Tile(3, 4)),
            (Tile(3, 4), None),
        ]

    def test_simple_movement_execution(self):
        board = Board()
        piece_two = Piece(PLAYER_1_ID, PieceType.two)
        piece_two.set_movement_direction(Direction.south)
        board.get_tile(1, 2).place_piece(piece_two)
        assert board.execute_board_movements(PLAYER_1_ID) == 1
        assert board.get_tile(1, 1).piece == piece_two

    def test_push_movements_execution(self):
        board = Board()

        piece_four = Piece(PLAYER_1_ID, PieceType.four)
        piece_four.set_movement_direction(Direction.north)
        board.get_tile(2, 1).place_piece(piece_four)

        piece_three = Piece(PLAYER_1_ID, PieceType.three)
        piece_three.set_movement_direction(Direction.east)
        board.get_tile(1, 2).place_piece(piece_three)

        piece_three_2 = Piece(PLAYER_2_ID, PieceType.three)
        piece_three_2.set_movement_direction(Direction.west)
        board.get_tile(2, 2).place_piece(piece_three_2)

        piece_five = Piece(PLAYER_2_ID, PieceType.five)
        piece_five.set_movement_direction(Direction.east)
        board.get_tile(2, 3).place_piece(piece_five)

        piece_two = Piece(PLAYER_1_ID, PieceType.two)
        piece_two.set_movement_direction(Direction.south)
        board.get_tile(3, 3).place_piece(piece_two)

        piece_one = Piece(PLAYER_2_ID, PieceType.one)
        piece_one.set_movement_direction(Direction.west)
        board.get_tile(3, 2).place_piece(piece_one)

        assert board.execute_board_movements(PLAYER_1_ID) == 5
        assert board.get_tile(1, 1).piece is None
        assert board.get_tile(2, 1).piece is None
        assert board.get_tile(3, 1).piece == piece_one
        assert board.get_tile(1, 2).piece == piece_three
        assert board.get_tile(2, 2).piece == piece_four
        assert board.get_tile(3, 2).piece == piece_two
        assert board.get_tile(1, 3).piece is None
        assert board.get_tile(2, 3).piece == piece_three_2
        assert board.get_tile(3, 3).piece is None
        assert piece_five.tile is None



