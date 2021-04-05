from game.game import Game
from game_analysis import analyze_games

games = list()
for _ in range(500):
    game = Game()
    game.play_random_game()
    games.append(game)

analyze_games(games)
