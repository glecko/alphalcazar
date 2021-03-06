from game.game import Game
from ui.display import Display
from strategies.ui_input.strategy import build_ui_input_strategy
from strategies.tree_search.strategy import build_tree_search_strategy
import threading
from time import sleep
import logging

FORMAT = "[%(asctime)s] [%(levelname)s] %(message)s"
logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO, format=FORMAT)


def on_player_moves_executed():
    sleep(2)


def thread_function(display: Display):
    game.play_game(build_ui_input_strategy(display), build_ui_input_strategy(display))


logger.info("Opening game...")
game = Game(on_player_moves_executed)
ui = Display(game)
game_logic = threading.Thread(target=thread_function, args=(ui,))
game_logic.daemon = True
game_logic.start()
ui.create_board()



