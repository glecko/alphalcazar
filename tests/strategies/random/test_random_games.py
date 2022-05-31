from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.enums import GameResult
from game.game import Game
from strategies.random.strategy import random_move_strategy


class TestRandomGames(object):

    def test_random_games(self):
        for _ in range(50):
            game = Game()
            result = game.play_game(random_move_strategy, random_move_strategy)
            assert game.turns >= 3  # Minimum amount of moves by one player to win or draw
            assert game.turns == len(game.player_moves[PLAYER_1_ID]) == len(game.player_moves[PLAYER_2_ID])
            assert result == GameResult.win or result == GameResult.loss
