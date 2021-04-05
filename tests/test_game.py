from game.game import Game
from game.enums import PieceType, GameResult


class TestBoard(object):

    def test_setup(self):
        game = Game()
        assert game.get_current_result() is None
        assert len(game.player_1.pieces) == len(list(PieceType))

    def test_random_games(self):
        for _ in range(50):
            game = Game()
            result = game.play_random_game()
            assert game.turns >= 3  # Minimum amount of moves by one player to win or draw
            assert game.turns == len(game.player_2_moves) == len(game.player_1_moves)
            assert result == GameResult.win or result == GameResult.draw or result == GameResult.loss



