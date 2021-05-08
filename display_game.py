from game.game import Game
from game.enums import PieceType, Direction
from ui.display import Display
from strategies.ui_input.strategy import build_ui_input_strategy
from strategies.tree_search.strategy import build_tree_search_strategy
import threading


def thread_function(display: Display):
    print("Game starting!")
    game_result = game.play_game(build_ui_input_strategy(display), build_tree_search_strategy(depth=2))
    print(game_result)


print("Opening game...")
game = Game()
ui = Display(game)
game_logic = threading.Thread(target=thread_function, args=(ui,))
game_logic.daemon = True
game_logic.start()
ui.create_board()



