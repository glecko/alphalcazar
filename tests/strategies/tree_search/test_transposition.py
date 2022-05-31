from game.enums import PieceType, Direction
from game.game import Game
from strategies.tree_search.strategy import get_best_move, get_best_moves
from strategies.tree_search.config import EvaluationType, DEPTH_PENALTY, WIN_CONDITION_SCORE
from strategies.tree_search.abstract_move import get_legal_abstract_moves
from strategies.tree_search.board_evaluation import BOARD_SCORE_CACHE
from strategies.tree_search.transposition import get_best_move_from_transposition_dict, store_in_transposition_dict, \
    TRANSPOSITION_DICT
from tests.strategies.tree_search.utils import clean_tree_search_caches_before_tests, setup_game_with_forced_player_2_movement


class TestTransposition(object):

    def test_cached_movement_recovery(self, clean_tree_search_caches_before_tests):
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

        cached_moves, cached_score, cached_eval_type = get_best_move_from_transposition_dict(game.player_1, 1, False, 0, 0)
        assert cached_moves is not None
        assert best_move in cached_moves
        assert cached_eval_type == EvaluationType.exact

        best_opponent_move = get_best_move(game.player_2, game.player_1, is_first_move=True, depth=1)

        assert best_move != best_opponent_move

        cached_moves_p2, cached_score_p2, cached_eval_type_p2 = get_best_move_from_transposition_dict(game.player_2, 1, False, 0, 0)
        assert best_opponent_move in cached_moves_p2

    def test_no_blunders_due_to_inversed_cache(self, clean_tree_search_caches_before_tests):
        # Here we are going to set up a test where player 2 goes second, and player 1 is about to win (he played first)
        # The only way to postpone an immediate loss for player 2 is to play a 4 on (2, 4).
        first_game = setup_game_with_forced_player_2_movement()
        first_game.starting_player = first_game.player_2

        # This movement shouldn't matter, player 1 is about to win anyways
        best_move_player_1 = get_best_move(first_game.player_1, first_game.player_2, is_first_move=False, depth=1)

        second_game = setup_game_with_forced_player_2_movement()
        second_game.starting_player = first_game.player_1

        best_move_player_2 = get_best_move(second_game.player_2, second_game.player_1, is_first_move=False, depth=1)

        assert best_move_player_2.x == 2
        assert best_move_player_2.y == 4
        assert best_move_player_2.piece_type == PieceType.four
        assert best_move_player_2.score < 0

        assert best_move_player_2 != best_move_player_1

    def test_overwriting_transposition_dict(self, clean_tree_search_caches_before_tests):
        game = Game()
        best_moves = get_legal_abstract_moves(game.player_1, False)[0:1]

        store_in_transposition_dict(best_moves, 20, EvaluationType.exact, game.player_1, 2, False)

        # We should never overwrite an exact evaluation
        store_in_transposition_dict([], 50, EvaluationType.beta_cutoff, game.player_1, 3, False)

        # We should never overwrite an evaluation with higher depth
        store_in_transposition_dict([], 50, EvaluationType.beta_cutoff, game.player_1, 1, False)

        stored_moves, stored_score, _ = get_best_move_from_transposition_dict(game.player_1, 1, False, 0, 0)
        assert len(stored_moves) == 1
        assert stored_score == 20

        store_in_transposition_dict([], 30, EvaluationType.exact, game.player_1, 4, False)

        _, stored_score, _ = get_best_move_from_transposition_dict(game.player_1, 1, False, 0, 0)
        assert stored_score == 30

    def test_using_valid_transposition_cache_cutoffs(self, clean_tree_search_caches_before_tests):
        game = Game()
        best_moves = get_legal_abstract_moves(game.player_1, False)[0:1]

        # We were searching a node with beta = 10 and found a move with value 35, which means the current node would
        # never happen in practice and we stored the lastly explored best value, 35
        store_in_transposition_dict(best_moves, 35, EvaluationType.beta_cutoff, game.player_1, 2, False)

        # Now we are exploring the same position at depth 2 but with a beta value of 50.
        # Since the node is a beta_cutoff, we know that the value is 35 or higher.
        # The beta player is only guaranteed a move of 50 or higher, which means this node could be a suitable candidate
        # We cannot reuse it, as it will not be discarded by beta cutoff and we don't know its real score
        _, stored_score, _ = get_best_move_from_transposition_dict(game.player_1, 2, False, 0, 50)
        assert stored_score is None

        # However, when exploring it with a beta value of 30, we can re-use this value as we will later discard it
        # in a beta cutoff, as the beta player is guaranteed a move that is better than "35 or higher"
        _, stored_score_2, _ = get_best_move_from_transposition_dict(game.player_1, 2, False, 0, 30)
        assert stored_score_2 is 35

    def test_transposition_does_not_use_wrong_alphabeta_cutoffs(self, clean_tree_search_caches_before_tests):
        game = Game()
        game.starting_player = game.player_2
        game.first_move_executed = True

        one_p1 = game.player_1.get_piece_by_type(PieceType.one)
        one_p1.set_movement_direction(Direction.south)
        game.board.get_tile(2, 3).place_piece(one_p1)

        one_p2 = game.player_2.get_piece_by_type(PieceType.one)
        one_p2.set_movement_direction(Direction.west)
        game.board.get_tile(3, 3).place_piece(one_p2)

        five_p2 = game.player_2.get_piece_by_type(PieceType.five)
        five_p2.set_movement_direction(Direction.west)
        game.board.get_tile(4, 2).place_piece(five_p2)

        best_move_p1_r1 = get_best_move(game.player_1, game.player_2, is_first_move=False, depth=2)

        assert best_move_p1_r1.piece_type != PieceType.four or best_move_p1_r1.x != 0 or best_move_p1_r1.y != 3

        # We ignore the correct movement and play a losing move (4 piece to (0, 3))

        four_p1 = game.player_1.get_piece_by_type(PieceType.four)
        four_p1.set_movement_direction(Direction.east)
        game.board.get_tile(0, 3).place_piece(four_p1)

        game.board.execute_board_movements(game.player_2.id)
        game.switch_starting_player()

        _, score, eval_type = get_best_moves(game.player_1, game.player_2, is_first_move=True, depth=2)

        assert score == -WIN_CONDITION_SCORE + DEPTH_PENALTY
        assert eval_type == EvaluationType.exact

    def test_using_inverted_alpha_beta_cutoffs(self, clean_tree_search_caches_before_tests):
        game = Game()

        # Player 1 pieces

        one_p1 = game.player_1.get_piece_by_type(PieceType.one)
        one_p1.set_movement_direction(Direction.east)
        game.board.get_tile(1, 3).place_piece(one_p1)

        four_p1 = game.player_1.get_piece_by_type(PieceType.four)
        four_p1.set_movement_direction(Direction.south)
        game.board.get_tile(3, 2).place_piece(four_p1)

        five_p1 = game.player_1.get_piece_by_type(PieceType.five)
        five_p1.set_movement_direction(Direction.south)
        game.board.get_tile(2, 2).place_piece(five_p1)

        # Player 2 pieces

        two_p2 = game.player_2.get_piece_by_type(PieceType.two)
        two_p2.set_movement_direction(Direction.west)
        game.board.get_tile(3, 1).place_piece(two_p2)

        three_p2 = game.player_2.get_piece_by_type(PieceType.three)
        three_p2.set_movement_direction(Direction.east)
        game.board.get_tile(2, 3).place_piece(three_p2)

        four_p2 = game.player_2.get_piece_by_type(PieceType.four)
        four_p2.set_movement_direction(Direction.east)
        game.board.get_tile(2, 1).place_piece(four_p2)

        five_p2 = game.player_2.get_piece_by_type(PieceType.five)
        five_p2.set_movement_direction(Direction.east)
        game.board.get_tile(1, 2).place_piece(five_p2)

        _, score, eval_type = get_best_moves(game.player_1, game.player_2, is_first_move=True, depth=2)

        assert eval_type == EvaluationType.exact

    def test_cache_dictionaries_cleared_before_tests(self, clean_tree_search_caches_before_tests):
        assert len(TRANSPOSITION_DICT) == 0
        assert len(BOARD_SCORE_CACHE) == 0
