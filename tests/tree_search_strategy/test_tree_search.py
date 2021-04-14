from game.constants import PLAYER_1_ID
from game.player import PlacementMove
from game.enums import PieceType
from game.game import Game
from tree_search_strategy.strategy import max_move, min_move
from tree_search_strategy.board_evaluation import WIN_CONDITION_SCORE


class TestTreeSearch(object):

    def test_obvious_second_placement_move(self):
        game = Game()

        PlacementMove(
            piece=game.player_1.get_piece_by_type(PieceType.four),
            tile=game.board.get_tile(0, 2)
        ).execute()
        PlacementMove(
            piece=game.player_1.get_piece_by_type(PieceType.three),
            tile=game.board.get_tile(0, 3)
        ).execute()
        game.board.execute_board_movements(PLAYER_1_ID)
        game.board.execute_board_movements(PLAYER_1_ID)

        PlacementMove(
            piece=game.player_1.get_piece_by_type(PieceType.two),
            tile=game.board.get_tile(0, 1)
        ).execute()
        PlacementMove(
            piece=game.player_1.get_piece_by_type(PieceType.five),
            tile=game.board.get_tile(4, 1)
        ).execute()

        game.board.execute_board_movements(PLAYER_1_ID)
        game.starting_player = game.player_2

        PlacementMove(
            piece=game.player_2.get_piece_by_type(PieceType.five),
            tile=game.board.get_tile(4, 3)
        ).execute()

        best_move = max_move(game.player_1, game.player_2, 1, False)
        assert best_move.x == 2
        assert best_move.y == 0
        assert best_move.piece_type == PieceType.one
        assert best_move.score == WIN_CONDITION_SCORE
