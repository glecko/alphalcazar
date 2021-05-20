from game.constants import PLAYER_1_ID
from game.player import PlacementMove
from game.enums import PieceType, Direction, GameResult
from game.game import Game
from strategies.tree_search.strategy import get_best_move, get_best_moves
from strategies.tree_search.config import WIN_CONDITION_SCORE, DEPTH_PENALTY
from tests.strategies.tree_search.utils import clean_tree_search_caches_before_tests


class TestTreeSearch(object):

    def test_obvious_second_placement_move(self, clean_tree_search_caches_before_tests):
        # Here we are going to set up a game where player 1 has a single option to win immediately,
        # and he's going to play second (he doesn't need to account for a move of the opponent before the
        # board execution)
        game = Game()
        game.starting_player = game.player_2

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
        best_move = get_best_move(game.player_1, game.player_2, is_first_move=False, depth=1)
        assert best_move.x == 2
        assert best_move.y == 0
        assert best_move.piece_type == PieceType.one
        assert best_move.score == WIN_CONDITION_SCORE - DEPTH_PENALTY

    def test_obvious_first_movement(self, clean_tree_search_caches_before_tests):
        # Here we are going to set up a game where player 1 is about to win the game, except if
        # player 2 (who goes first) prevents it by placing a piece on the square player 1 needs to use
        # to win
        game = Game()
        game.starting_player = game.player_2

        five = game.player_1.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.east)
        game.board.get_tile(1, 1).place_piece(five)

        four = game.player_1.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.west)
        game.board.get_tile(3, 2).place_piece(four)

        opponent_four = game.player_2.get_piece_by_type(PieceType.four)
        opponent_four.set_movement_direction(Direction.west)
        game.board.get_tile(1, 3).place_piece(opponent_four)

        best_moves, _ = get_best_moves(game.player_2, game.player_1, is_first_move=True, depth=1)
        assert len(best_moves) == 1

        # Player 2 can only avoid losing this turn by playing anything on tile (2, 4)
        best_move = get_best_move(game.player_2, game.player_1, is_first_move=True, depth=1)

        assert best_move.x == 2
        assert best_move.y == 4

        # Should be true for any depth level (using 2 for test speed reasons)
        best_move_depth_2 = get_best_move(game.player_2, game.player_1, is_first_move=True, depth=2)

        assert best_move_depth_2.x == 2
        assert best_move_depth_2.y == 4

    def test_player_must_use_four_piece(self, clean_tree_search_caches_before_tests):
        game = Game()
        game.starting_player = game.player_2

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
        # the (2, 0) or the (2, 4) tile or on the (0, 3) tile
        best_move = get_best_move(game.player_1, game.player_2, is_first_move=False, depth=1)

        assert (best_move.x == 2 and best_move.y == 0) or\
               (best_move.x == 2 and best_move.y == 4) or\
               (best_move.x == 0 and best_move.y == 3)
        assert best_move.piece_type == PieceType.four

    def test_hopeless_situation(self, clean_tree_search_caches_before_tests):
        # In this test we are going to set up a situation where player_1, moving second
        # cannot postpone his loss regardless of what he plays
        game = Game()
        game.starting_player = game.player_2

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
        best_move = get_best_move(game.player_1, game.player_2, is_first_move=False, depth=100)

        assert best_move.score == -WIN_CONDITION_SCORE + DEPTH_PENALTY

    def test_player_must_use_last_piece(self, clean_tree_search_caches_before_tests):
        # Here we set up a scenario where player 1 can win by playing his last available piece
        # If he plays a 3 at (2, 4), he wins
        game = Game()
        game.starting_player = game.player_2

        one = game.player_1.get_piece_by_type(PieceType.one)
        one.set_movement_direction(Direction.north)
        game.board.get_tile(3, 3).place_piece(one)

        two = game.player_1.get_piece_by_type(PieceType.two)
        two.set_movement_direction(Direction.east)
        game.board.get_tile(3, 2).place_piece(two)

        four = game.player_1.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.north)
        game.board.get_tile(2, 0).place_piece(four)

        five = game.player_1.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.south)
        game.board.get_tile(2, 2).place_piece(five)

        enemy_two = game.player_2.get_piece_by_type(PieceType.two)
        enemy_two.set_movement_direction(Direction.west)
        game.board.get_tile(4, 1).place_piece(enemy_two)

        best_move = get_best_move(game.player_1, game.player_2, is_first_move=False, depth=1)

        assert best_move is not None
        assert best_move.piece_type == PieceType.three
        assert best_move.x == 2
        assert best_move.y == 4

    def test_player_has_no_legal_moves(self, clean_tree_search_caches_before_tests):
        game = Game()
        game.starting_player = game.player_2

        one = game.player_1.get_piece_by_type(PieceType.one)
        one.set_movement_direction(Direction.north)
        game.board.get_tile(3, 3).place_piece(one)

        two = game.player_1.get_piece_by_type(PieceType.two)
        two.set_movement_direction(Direction.east)
        game.board.get_tile(3, 2).place_piece(two)

        three = game.player_1.get_piece_by_type(PieceType.three)
        three.set_movement_direction(Direction.north)
        game.board.get_tile(1, 3).place_piece(three)

        four = game.player_1.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.north)
        game.board.get_tile(2, 0).place_piece(four)

        five = game.player_1.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.south)
        game.board.get_tile(2, 2).place_piece(five)

        best_move = get_best_move(game.player_1, game.player_2, is_first_move=False, depth=1)

        assert best_move.to_placement_move(game.player_1) is None
        assert best_move.score > 0

    def test_game_is_lost_on_depth_2(self, clean_tree_search_caches_before_tests):
        game = Game()
        game.starting_player = game.player_1

        two = game.player_1.get_piece_by_type(PieceType.two)
        two.set_movement_direction(Direction.south)
        game.board.get_tile(2, 3).place_piece(two)

        three = game.player_1.get_piece_by_type(PieceType.three)
        three.set_movement_direction(Direction.east)
        game.board.get_tile(1, 2).place_piece(three)

        four = game.player_1.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.east)
        game.board.get_tile(0, 2).place_piece(four)

        one_p2 = game.player_2.get_piece_by_type(PieceType.one)
        one_p2.set_movement_direction(Direction.north)
        game.board.get_tile(2, 2).place_piece(one_p2)

        two_p2 = game.player_2.get_piece_by_type(PieceType.two)
        two_p2.set_movement_direction(Direction.west)
        game.board.get_tile(3, 2).place_piece(two_p2)

        # At this position, player 2 is completely losing at depth 2
        # He can avoid losing this round by playing the 4 piece at (2, 0), but next round he will
        # be unable to block both squares where P1 can mate, and will not have the 4 in hand

        best_moves_p2_t1, score = get_best_moves(game.player_2, game.player_1, is_first_move=False, depth=2)
        assert len(best_moves_p2_t1) == 1
        best_move_p2 = get_best_move(game.player_2, game.player_1, is_first_move=False, depth=2)

        assert best_move_p2.x == 2 and best_move_p2.y == 0
        assert best_move_p2.piece_type == PieceType.four
        assert best_move_p2.score == -WIN_CONDITION_SCORE + DEPTH_PENALTY * 2

    def test_game_is_lost_on_depth_2_alternative(self, clean_tree_search_caches_before_tests):
        game = Game()
        game.starting_player = game.player_2

        three = game.player_1.get_piece_by_type(PieceType.three)
        three.set_movement_direction(Direction.east)
        game.board.get_tile(2, 1).place_piece(three)

        four = game.player_1.get_piece_by_type(PieceType.four)
        four.set_movement_direction(Direction.south)
        game.board.get_tile(2, 3).place_piece(four)

        five = game.player_1.get_piece_by_type(PieceType.five)
        five.set_movement_direction(Direction.south)
        game.board.get_tile(1, 2).place_piece(five)

        four_p2 = game.player_2.get_piece_by_type(PieceType.four)
        four_p2.set_movement_direction(Direction.west)
        game.board.get_tile(3, 2).place_piece(four_p2)

        five_p2 = game.player_2.get_piece_by_type(PieceType.five)
        five_p2.set_movement_direction(Direction.west)
        game.board.get_tile(2, 2).place_piece(five_p2)

        # At this position, player 2 is losing at depth 2
        # He can avoid losing this round by playing the 1 piece at (0, 3), but next round he will
        # be unable to block both squares where P1 can mate, and will not have the 4 in hand

        best_moves_p2_t1, score = get_best_moves(game.player_2, game.player_1, is_first_move=True, depth=2)
        assert len(best_moves_p2_t1) == 1
        best_move_p2 = get_best_move(game.player_2, game.player_1, is_first_move=True, depth=2)

        assert best_move_p2.x == 0 and best_move_p2.y == 3
        assert best_move_p2.piece_type == PieceType.one
        assert best_move_p2.score == -WIN_CONDITION_SCORE + DEPTH_PENALTY * 2

    def test_black_widow(self, clean_tree_search_caches_before_tests):
        # The following situation is hard to see for a human, but completely winning for player 2
        # He only has piece 5 in hand, and as long as he keeps it for the next move (makes it not enter the board)
        # He will be able to use it next round to win the game, no reply from player 1 possible.

        game = Game()
        game.starting_player = game.player_1

        # Player 1 pieces
        player_1_one = game.player_1.get_piece_by_type(PieceType.one)
        player_1_one.set_movement_direction(Direction.east)
        game.board.get_tile(2, 3).place_piece(player_1_one)

        player_1_two = game.player_1.get_piece_by_type(PieceType.two)
        player_1_two.set_movement_direction(Direction.west)
        game.board.get_tile(3, 2).place_piece(player_1_two)

        player_1_four = game.player_1.get_piece_by_type(PieceType.four)
        player_1_four.set_movement_direction(Direction.east)
        game.board.get_tile(1, 3).place_piece(player_1_four)

        player_1_five = game.player_1.get_piece_by_type(PieceType.five)
        player_1_five.set_movement_direction(Direction.east)
        game.board.get_tile(1, 1).place_piece(player_1_five)

        player_1_three = game.player_1.get_piece_by_type(PieceType.three)
        player_1_three.set_movement_direction(Direction.east)
        game.board.get_tile(0, 3).place_piece(player_1_three)

        # Player 2 pieces
        player_2_one = game.player_2.get_piece_by_type(PieceType.one)
        player_2_one.set_movement_direction(Direction.north)
        game.board.get_tile(2, 1).place_piece(player_2_one)

        player_2_two = game.player_2.get_piece_by_type(PieceType.two)
        player_2_two.set_movement_direction(Direction.west)
        game.board.get_tile(3, 1).place_piece(player_2_two)

        player_2_three = game.player_2.get_piece_by_type(PieceType.three)
        player_2_three.set_movement_direction(Direction.west)
        game.board.get_tile(3, 3).place_piece(player_2_three)

        player_2_four = game.player_2.get_piece_by_type(PieceType.four)
        player_2_four.set_movement_direction(Direction.east)
        game.board.get_tile(2, 2).place_piece(player_2_four)

        best_moves_player_2_round_1, player_2_round_1_score = get_best_moves(game.player_2, game.player_1, is_first_move=False, depth=2)

        for candidate_move in best_moves_player_2_round_1:
            clone_game = game.clone()

            candidate_move.execute(clone_game.player_2)

            clone_game.board.execute_board_movements(starting_player_id=clone_game.player_1.id)
            clone_game.switch_starting_player()

            # Play the second round
            clone_move_p2_r2 = get_best_move(clone_game.player_2, clone_game.player_1, is_first_move=True, depth=2)
            clone_move_p2_r2.execute(clone_game.player_2)
            clone_move_p1_r2 = get_best_move(clone_game.player_1, clone_game.player_2, is_first_move=False, depth=2)
            clone_move_p1_r2.execute(clone_game.player_1)

            clone_game.board.execute_board_movements(starting_player_id=clone_game.player_2.id)
            assert clone_game.board.get_game_result(clone_game.player_2.id, clone_game.player_1.id) == GameResult.win

            assert clone_move_p1_r2.score == -WIN_CONDITION_SCORE + DEPTH_PENALTY
            assert clone_move_p2_r2.score == WIN_CONDITION_SCORE - DEPTH_PENALTY

        # Play the first round
        best_move_player_2_round_1 = get_best_move(game.player_2, game.player_1, is_first_move=False, depth=2)
        best_move_player_2_round_1.execute(game.player_2)

        game.board.execute_board_movements(starting_player_id=game.player_1.id)
        game.switch_starting_player()

        best_moves_player_2_round_2, _ = get_best_moves(game.player_2, game.player_1, is_first_move=True, depth=2)

        # Play the second round
        best_move_player_2_round_2 = get_best_move(game.player_2, game.player_1, is_first_move=True, depth=2)
        best_move_player_2_round_2.execute(game.player_2)
        best_move_player_1_round_2 = get_best_move(game.player_1, game.player_2, is_first_move=False, depth=2)
        best_move_player_1_round_2.execute(game.player_1)

        game.board.execute_board_movements(starting_player_id=game.player_2.id)

        # On the first round, player 2 must play the 5 piece (forced, only piece in hand) on any tile where it cannot
        # enter the board, as he needs it to mate in the next round.
        tiles_where_5_would_enter = [(0, 2), (1, 4), (0, 3), (2, 0)]
        for candidate_move in best_moves_player_2_round_1:
            assert candidate_move.piece_type == PieceType.five
            assert (candidate_move.x, candidate_move.y) not in tiles_where_5_would_enter
        assert best_move_player_2_round_1.piece_type == PieceType.five
        assert (best_move_player_2_round_1.x, best_move_player_2_round_1.y) not in tiles_where_5_would_enter

        # On the second round, player 2 should play the 5 on (2, 4), as its the winning move
        assert len(best_moves_player_2_round_2) == 1
        assert best_move_player_2_round_2.piece_type == PieceType.five
        assert best_move_player_2_round_2.x == 2 and best_move_player_2_round_2.y == 4

        # Both players should know at any moment they have already won/lost
        assert best_move_player_2_round_1.score == player_2_round_1_score == WIN_CONDITION_SCORE - DEPTH_PENALTY * 2
        assert best_move_player_1_round_2.score == -WIN_CONDITION_SCORE + DEPTH_PENALTY
        assert best_move_player_2_round_2.score == WIN_CONDITION_SCORE - DEPTH_PENALTY

        assert game.board.get_game_result(game.player_2.id, game.player_1.id) == GameResult.win
