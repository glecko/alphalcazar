from game.game import Game
from strategies.tree_search.strategy import build_tree_search_strategy
from strategies.tree_search.persistence import persist_transposition_cache
from strategies.tree_search.abstract_move import get_legal_abstract_moves
import logging

FORMAT = "[%(asctime)s] [%(levelname)s] %(message)s"
logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO, format=FORMAT)

game = Game()
strategy_fn = build_tree_search_strategy(depth=2)

for move in get_legal_abstract_moves(game.player_1):
    move_game = Game()
    move.execute(move_game.player_1)
    move_game.first_move_executed = True
    move_game.play_game(strategy_fn, strategy_fn)
    persist_transposition_cache()


