from game.game import Game
from tree_search_strategy.strategy import build_tree_search_strategy
import sys

sys.setrecursionlimit(100000)
game = Game()
game.play_game(build_tree_search_strategy(depth=2))
