from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.piece import Piece
from game.enums import PieceType, Direction
from game.board import Board
from tree_search_strategy.board_evaluation import evaluate_board
from tree_search_strategy.config import WIN_CONDITION_SCORE, TILE_SCORE_MULTIPLIER, PLACED_PIECE_SCORE


class TestBoardEvaluation(object):

    def test_piece_board_evaluation(self):
        board = Board()

        piece_two = Piece(PLAYER_1_ID, PieceType.two)
        piece_two.set_movement_direction(Direction.west)
        board.get_tile(2, 2).place_piece(piece_two)

        piece_two_score = PLACED_PIECE_SCORE[PieceType.two] * TILE_SCORE_MULTIPLIER[2][2][Direction.west]
        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == piece_two_score

        piece_two_enemy = Piece(PLAYER_2_ID, PieceType.two)
        piece_two_enemy.set_movement_direction(Direction.east)
        board.get_tile(2, 1).place_piece(piece_two_enemy)

        enemy_two_score = PLACED_PIECE_SCORE[PieceType.two] * TILE_SCORE_MULTIPLIER[2][1][Direction.east]
        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == piece_two_score - enemy_two_score

        piece_three = Piece(PLAYER_2_ID, PieceType.three)
        piece_three.set_movement_direction(Direction.south)
        board.get_tile(1, 1).place_piece(piece_three)

        enemy_three_score = PLACED_PIECE_SCORE[PieceType.three] * TILE_SCORE_MULTIPLIER[1][1][Direction.south]
        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == piece_two_score - enemy_two_score - enemy_three_score

    def test_piece_value_relationships(self):
        # We want to test that pieces in squares where they are more valuable
        # are valued better than if placed somewhere else #
        board = Board()

        piece_two = Piece(PLAYER_1_ID, PieceType.two)
        piece_two.set_movement_direction(Direction.north)
        board.get_tile(1, 1).place_piece(piece_two)

        just_entered_score = evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID)

        board.get_tile(1, 1).remove_piece()
        board.get_tile(2, 2).place_piece(piece_two)

        center_square_score = evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID)

        board.get_tile(2, 2).remove_piece()
        piece_two.set_movement_direction(Direction.east)
        board.get_tile(3, 3).place_piece(piece_two)

        about_to_exit_score = evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID)

        board.get_tile(3, 3).remove_piece()
        piece_two.set_movement_direction(Direction.west)
        board.get_tile(2, 3).place_piece(piece_two)

        neutral_score = evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID)

        assert center_square_score > neutral_score
        assert just_entered_score > neutral_score
        assert neutral_score > about_to_exit_score


    def test_win_condition_board_evaluation(self):
        board = Board()

        piece_one = Piece(PLAYER_1_ID, PieceType.one)
        piece_one.set_movement_direction(Direction.north)
        board.get_tile(1, 1).place_piece(piece_one)

        piece_two = Piece(PLAYER_1_ID, PieceType.two)
        piece_two.set_movement_direction(Direction.north)
        board.get_tile(2, 2).place_piece(piece_two)

        piece_three = Piece(PLAYER_1_ID, PieceType.three)
        piece_three.set_movement_direction(Direction.south)
        board.get_tile(3, 3).place_piece(piece_three)

        assert evaluate_board(board, PLAYER_1_ID, PLAYER_2_ID) == WIN_CONDITION_SCORE
        assert evaluate_board(board, PLAYER_2_ID, PLAYER_1_ID) == -WIN_CONDITION_SCORE
