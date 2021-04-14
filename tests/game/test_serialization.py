from game.board import Board
from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.piece import Piece
from game.enums import PieceType, Direction
from game.game import Game


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

    def test_game_two_way_serialization(self):
        game = Game()
        assert game.to_string_notation() == f"1#{EMPTY_BOARD_SERIALIZATION}"

        piece_two = Piece(PLAYER_1_ID, PieceType.two)
        piece_two.set_movement_direction(Direction.west)
        game.board.get_tile(2, 2).place_piece(piece_two)

        piece_two_2 = Piece(PLAYER_2_ID, PieceType.two)
        piece_two_2.set_movement_direction(Direction.east)
        game.board.get_tile(2, 3).place_piece(piece_two_2)

        piece_five = Piece(PLAYER_1_ID, PieceType.five)
        piece_five.set_movement_direction(Direction.south)
        game.board.get_tile(0, 2).place_piece(piece_five)

        clone_game = Game.from_string_notation(game.to_string_notation())
        assert clone_game.starting_player.id == PLAYER_1_ID

        assert clone_game.board.get_tile(2, 2).piece.type == PieceType.two
        assert clone_game.board.get_tile(2, 2).piece.owner_id == PLAYER_1_ID
        assert clone_game.board.get_tile(2, 2).piece.direction == Direction.west

        assert clone_game.board.get_tile(2, 3).piece.type == PieceType.two
        assert clone_game.board.get_tile(2, 3).piece.owner_id == PLAYER_2_ID
        assert clone_game.board.get_tile(2, 3).piece.direction == Direction.east

        assert clone_game.board.get_tile(0, 2).piece.type == PieceType.five
        assert clone_game.board.get_tile(0, 2).piece.owner_id == PLAYER_1_ID
        assert clone_game.board.get_tile(0, 2).piece.direction == Direction.south
