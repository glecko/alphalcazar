from game.enums import PieceType, Direction
from game.game import Game
from strategies.tree_search.strategy import get_best_move
from strategies.tree_search.transposition import get_best_move_from_transposition_dict


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


class TestTransposition(object):

    def test_cached_movement_recovery(self):
        game = Game()
        game.starting_player = game.player_1

        five = game.player_1.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.west)
        game.board.get_tile(3, 1).place_piece(five)

        four = game.player_2.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.west)
        game.board.get_tile(3, 2).place_piece(four)

        one = game.player_2.get_piece_by_type(PieceType.one)
        one.set_movement_direction(Direction.north)
        game.board.get_tile(1, 3).place_piece(one)

        best_move = get_best_move(game.player_1, game.player_2, is_first_move=True, depth=1)

        cached_moves, cached_score = get_best_move_from_transposition_dict(game.player_1, 1, False)
        assert best_move.abstract_move in cached_moves

        best_opponent_move = get_best_move(game.player_2, game.player_1, is_first_move=True, depth=1)

        assert best_move != best_opponent_move

        cached_moves_p2, cached_score_p2 = get_best_move_from_transposition_dict(game.player_2, 1, False)
        assert best_opponent_move.abstract_move in cached_moves_p2

    def test_no_blunders_due_to_inversed_cache(self):
        # Here we are going to set up a test where player 2 goes second, and player 1 is about to win (he played first)
        # The only way to postpone an immediate loss for player 2 is to play a 4 on (2, 4).
        first_game = setup_game_with_forced_player_2_movement()
        first_game.starting_player = first_game.player_2

        # This movement shouldn't matter, player 1 is about to win anyways
        best_move_player_1 = get_best_move(first_game.player_1, first_game.player_2, is_first_move=False, depth=1)

        second_game = setup_game_with_forced_player_2_movement()
        second_game.starting_player = first_game.player_1

        best_move_player_2 = get_best_move(second_game.player_2, second_game.player_1, is_first_move=False, depth=1)

        assert best_move_player_2.abstract_move.x == 2
        assert best_move_player_2.abstract_move.y == 4
        assert best_move_player_2.abstract_move.piece_type == PieceType.four
        assert best_move_player_2.score < 0

        assert best_move_player_2 != best_move_player_1
