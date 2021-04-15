from game.constants import PLAYER_1_ID
from game.player import PlacementMove
from game.enums import PieceType, Direction
from game.game import Game
from tree_search_strategy.strategy import MinMaxState
from tree_search_strategy.board_evaluation import WIN_CONDITION_SCORE, DEPTH_PENALTY, PLACED_PIECE_SCORE


class TestTreeSearch(object):

    def test_obvious_second_placement_move(self):
        # Here we are going to set up a game where player 1 has a single option to win immediately,
        # and he's going to play second (he doesn't need to account for a move of the opponent before the
        # board execution)
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

        # Player 1 can win the game by placing a one in tile (2, 0) or a three/four on (3, 0)
        # But he only has piece one available in hand
        best_move = MinMaxState(game.player_1, game.player_2, is_first_move=False, depth=1).get_best_move()
        assert best_move.x == 2
        assert best_move.y == 0
        assert best_move.piece_type == PieceType.one
        assert best_move.score == WIN_CONDITION_SCORE - DEPTH_PENALTY

    def test_obvious_first_movement(self):
        # Here we are going to set up a game where player 1 is about to win the game, except if
        # player 2 (who goes first) prevents it by placing a piece on the square player 1 needs to use
        # to win
        game = Game()

        five = game.player_1.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.east)
        game.board.get_tile(1, 1).place_piece(five)

        four = game.player_1.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.west)
        game.board.get_tile(3, 2).place_piece(four)

        # Player 2 can only avoid losing this turn by playing anything on tile (2, 4)
        best_move = MinMaxState(game.player_2, game.player_1, is_first_move=True, depth=1).get_best_move()

        assert best_move.x == 2
        assert best_move.y == 4

        # Should be true for any depth level (using 2 for test speed reasons)
        best_move_depth_2 = MinMaxState(game.player_2, game.player_1, is_first_move=True, depth=2).get_best_move()

        assert best_move_depth_2.x == 2
        assert best_move_depth_2.y == 4

    def test_player_must_use_four_piece(self):
        game = Game()

        five = game.player_2.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.south)
        game.board.get_tile(2, 3).place_piece(five)

        two = game.player_2.get_piece_by_type(PieceType.two)
        two.set_movement_direction(Direction.south)
        game.board.get_tile(3, 3).place_piece(two)

        three = game.player_2.get_piece_by_type(PieceType.three)
        three.set_movement_direction(Direction.north)
        game.board.get_tile(1, 1).place_piece(three)

        player_two = game.player_1.get_piece_by_type(PieceType.two)
        player_two.set_movement_direction(Direction.west)
        game.board.get_tile(1, 3).place_piece(player_two)

        # Player 1 can only avoid losing this turn by player the four piece on either
        # the (0, 2) or the (4, 2) tile or on the (0, 3) tile
        best_move = MinMaxState(game.player_1, game.player_2, is_first_move=False, depth=1).get_best_move()

        assert (best_move.x == 0 and best_move.y == 2) or (best_move.x == 4 and best_move.y == 2) or (best_move.x == 0 and best_move.y == 3)
        assert best_move.piece_type == PieceType.four

    def test_hopeless_situation(self):
        # In this test we are going to set up a situation where player_1, moving second
        # cannot postpone his loss regardless of what he plays
        game = Game()

        five = game.player_2.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.west)
        game.board.get_tile(3, 2).place_piece(five)

        two = game.player_2.get_piece_by_type(PieceType.two)
        two.set_movement_direction(Direction.west)
        game.board.get_tile(3, 3).place_piece(two)

        four = game.player_2.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.east)
        game.board.get_tile(1, 1).place_piece(four)

        three = game.player_2.get_piece_by_type(PieceType.three)
        three.set_movement_direction(Direction.north)
        game.board.get_tile(1, 0).place_piece(three)

        one = game.player_2.get_piece_by_type(PieceType.one)
        one.set_movement_direction(Direction.south)
        game.board.get_tile(3, 4).place_piece(one)

        opponent_four = game.player_1.get_piece_by_type(PieceType.four)
        opponent_four.set_movement_direction(Direction.east)
        game.board.get_tile(3, 1).place_piece(opponent_four)

        # Player 1 cannot do anything, there are 2 winning rows coming into play (center vertical line and diagonal)
        # and he can only prevent the diagonal.
        # Setting the depth to a ridiculous number should not make this test run forever, as on depth 1 it should
        # already return a score for all movements
        best_move = MinMaxState(game.player_1, game.player_2, is_first_move=False, depth=10).get_best_move()

        assert best_move.score == -WIN_CONDITION_SCORE - DEPTH_PENALTY
