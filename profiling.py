from game.game import Game
from tree_search_strategy.strategy import build_tree_search_strategy
import cProfile, io
import pstats
from pstats import SortKey

game = Game()
strategy_fn = build_tree_search_strategy(depth=2)
pr = cProfile.Profile()
pr.enable()
best_first_move = strategy_fn(game.player_1, game.player_2, True)
pr.disable()
stream = io.StringIO()
profile_stream = pstats.Stats(pr, stream=stream).strip_dirs().sort_stats(SortKey.CUMULATIVE)
profile_stream.print_stats()
print(stream.getvalue())
