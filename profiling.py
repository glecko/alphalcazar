from game.game import Game
from tree_search_strategy.strategy import build_tree_search_strategy
import cProfile, io
import pstats
from pstats import SortKey
from typing import Callable


def profile_function(callback: Callable):
    pr = cProfile.Profile()
    pr.enable()
    callback()
    pr.disable()
    stream = io.StringIO()
    profile_stream = pstats.Stats(pr, stream=stream).strip_dirs().sort_stats(SortKey.TIME)
    profile_stream.print_stats()
    print(stream.getvalue())


game = Game()
strategy_fn = build_tree_search_strategy(depth=2)


def function_to_profile():
    strategy_fn(game.player_1, game.player_2, True)
    # for _ in range(0, 50000):
        # game.clone()


profile_function(function_to_profile)
