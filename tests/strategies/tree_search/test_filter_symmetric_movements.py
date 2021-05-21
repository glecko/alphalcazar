from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.player import PlacementMove
from game.enums import PieceType, Direction
from game.game import Game
from strategies.tree_search.abstract_move import get_legal_scored_moves


class TestFilterSymmetricMovements(object):

    def test_symmetric_movements_removal(self):
        game = Game()

        # With an empty board, there should only be 2 valid tiles on which to play
        assert len(get_legal_scored_moves(game.player_1)) == 2 * len(game.player_1.pieces)

        PlacementMove(
            piece=game.player_1.get_piece_by_type(PieceType.two),
            tile=game.board.get_tile(0, 2)
        ).execute()
        PlacementMove(
            piece=game.player_2.get_piece_by_type(PieceType.four),
            tile=game.board.get_tile(4, 2)
        ).execute()
        game.board.execute_board_movements(PLAYER_1_ID)

        # Now both players should have 7 legal tiles at which to place their pieces
        # as the board now has x-axis symmetry
        assert len(get_legal_scored_moves(game.player_1)) == 7 * len(game.player_1.get_available_pieces())
        assert len(get_legal_scored_moves(game.player_2)) == 7 * len(game.player_2.get_available_pieces())

        game.board.execute_board_movements(PLAYER_2_ID)

        # The game still has x-axis symmetry
        assert len(get_legal_scored_moves(game.player_1)) == 7 * len(game.player_1.get_available_pieces())
        assert len(get_legal_scored_moves(game.player_2)) == 7 * len(game.player_2.get_available_pieces())

    def test_non_symmetric_center_row(self):
        game = Game()

        two = game.player_1.get_piece_by_type(PieceType.two)
        two.set_movement_direction(Direction.north)
        game.board.get_tile(2, 0).place_piece(two)

        three = game.player_1.get_piece_by_type(PieceType.three)
        three.set_movement_direction(Direction.south)
        game.board.get_tile(2, 1).place_piece(three)

        five = game.player_1.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.east)
        game.board.get_tile(2, 2).place_piece(five)

        # Game should not have y-axis symmetry because the center piece is facing east
        assert len(get_legal_scored_moves(game.player_2)) == 11 * len(game.player_2.pieces)

    def test_non_symmetric_movements(self):
        game = Game()

        PlacementMove(
            piece=game.player_1.get_piece_by_type(PieceType.two),
            tile=game.board.get_tile(0, 1)
        ).execute()

        # All tiles except 1 (the one occupied by player 1) should need to be considered
        assert len(get_legal_scored_moves(game.player_2)) == 11 * len(game.player_2.pieces)

        # No after executing the movement, all possible movements need to be explored
        game.board.execute_board_movements(PLAYER_1_ID)
        assert len(get_legal_scored_moves(game.player_2)) == 12 * len(game.player_2.pieces)
