from game.game import Game
from strategies.tree_search.strategy import build_tree_search_strategy
import cProfile, io
import pstats
from pstats import SortKey
from typing import Callable
from game_analysis import analyze_games


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
    #strategy_fn(game.player_1, game.player_2, True)
    results = list()
    for _ in range(0, 10):
        game = Game()
        game.play_game(strategy_fn, strategy_fn)
        results.append(game)
        print(game.result)
    analyze_games(results)


profile_function(function_to_profile)
