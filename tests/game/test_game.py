from game.game import Game
from game.enums import PieceType, GameResult, Direction


class TestBoard(object):

    def test_setup(self):
        game = Game()
        assert game.get_current_result() is None
        assert len(game.player_1.pieces) == len(list(PieceType))
        assert len(game.player_1.get_legal_placement_moves()) == len(game.board.get_legal_tiles()) * len(list(PieceType))
        assert len(game.player_2.get_legal_placement_moves()) == len(game.board.get_legal_tiles()) * len(list(PieceType))

    def test_random_games(self):
        for _ in range(50):
            game = Game()
            result = game.play_random_game()
            assert game.turns >= 3  # Minimum amount of moves by one player to win or draw
            assert game.turns == len(game.player_2_moves) == len(game.player_1_moves)
            assert result == GameResult.win or result == GameResult.draw or result == GameResult.loss

    def test_clone_game(self):
        game = Game()
        game.starting_player = game.player_2

        two = game.player_1.get_piece_by_type(PieceType.two)
        two.set_movement_direction(Direction.east)
        game.board.get_tile(2, 2).place_piece(two)

        three = game.player_2.get_piece_by_type(PieceType.three)
        three.set_movement_direction(Direction.north)
        game.board.get_tile(1, 0).place_piece(three)

        game_clone = game.clone()

        center_piece_clone = game_clone.board.get_tile(2, 2).piece
        assert center_piece_clone is not None
        assert center_piece_clone.type == PieceType.two
        assert center_piece_clone.owner_id == game_clone.player_1.id

        center_piece_clone = game_clone.board.get_tile(1, 0).piece
        assert center_piece_clone is not None
        assert center_piece_clone.type == PieceType.three
        assert center_piece_clone.owner_id == game_clone.player_2.id

        assert game_clone.starting_player == game_clone.player_2

