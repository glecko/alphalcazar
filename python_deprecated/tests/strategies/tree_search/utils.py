from game.enums import PieceType, Direction
from game.game import Game
from strategies.tree_search.transposition import TRANSPOSITION_DICT
from strategies.tree_search.board_evaluation import BOARD_SCORE_CACHE
import pytest


@pytest.fixture()
def clean_tree_search_caches_before_tests():
    TRANSPOSITION_DICT.clear()
    BOARD_SCORE_CACHE.clear()
    yield


def setup_game_with_forced_player_2_movement():
    game = Game()

    five = game.player_1.get_piece_by_type(PieceType.five)
    five.set_movement_direction(Direction.north)
    game.board.get_tile(2, 2).place_piece(five)

    three = game.player_1.get_piece_by_type(PieceType.three)
    three.set_movement_direction(Direction.south)
    game.board.get_tile(2, 3).place_piece(three)

    one = game.player_1.get_piece_by_type(PieceType.one)
    one.set_movement_direction(Direction.north)
    game.board.get_tile(2, 0).place_piece(one)

    return game
