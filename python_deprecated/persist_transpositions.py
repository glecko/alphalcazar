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

# First, play a couple of normal games #
for x in range(0, 25):
    logger.info(f"Generating transposition cache for normal game {x}.")
    normal_game = Game()
    normal_game.play_game(strategy_fn, strategy_fn)
    persist_transposition_cache()

# Once "typical" games have been persisted, start mapping all possible first-turn positions
for move_p1 in get_legal_abstract_moves(game.player_1, filter_symmetric_moves=False):
    game_2 = Game()
    move_p1.execute(game_2.player_1)
    for move_p2 in get_legal_abstract_moves(game_2.player_2, filter_symmetric_moves=False):
        move_game = game_2.clone()
        logger.info(f"Generating transposition cache for starting move {move_p1!r} and follow up move {move_p2!r}.")
        move_p2.execute(move_game.player_2)
        move_game.board.execute_board_movements(move_game.starting_player.id)
        move_game.switch_starting_player()
        move_game.play_game(strategy_fn, strategy_fn)
        persist_transposition_cache()
