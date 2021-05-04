from game.game import Game
from game.constants import PLAYER_1_ID, PLAYER_2_ID
from strategies.tree_search.strategy import build_tree_search_strategy
from strategies.human_input.strategy import human_input_strategy
from strategies.random.strategy import random_move_strategy
import sys

strategy_options = [
    {
        "id": 1,
        "description": "Tree search (Depth 2)",
        "strategy": build_tree_search_strategy(depth=2),
    },
    {
        "id": 2,
        "description": "Tree search (Depth 3)",
        "strategy": build_tree_search_strategy(depth=3),
    },
    {
        "id": 3,
        "description": "Human input",
        "strategy": human_input_strategy,
    },
    {
        "id": 4,
        "description": "Random",
        "strategy": random_move_strategy,
    },
]


def get_strategy_options_string():
    strategy_options_string = ""
    for option in strategy_options:
        strategy_options_string += f"\n{option['id']}) {option['description']}"
    return strategy_options_string


def select_strategy_by_id_input(player_id: int):
    matching_strategy_option = None
    while matching_strategy_option is None:
        try:
            strategy_id = input(f"Select a strategy for player {player_id}. Options: {get_strategy_options_string()}\nSelect: ")
            matching_strategy_option = next((option for option in strategy_options if option["id"] == int(strategy_id)), None)
            if matching_strategy_option is None:
                print(f"Invalid strategy option selected ({strategy_id}), please try again.")
        except ValueError:
            print(f"Invalid strategy option selected ({strategy_id}), please try again.")
    return matching_strategy_option["strategy"]


player_1_strategy_id = select_strategy_by_id_input(PLAYER_1_ID)
player_2_strategy_id = select_strategy_by_id_input(PLAYER_2_ID)


sys.setrecursionlimit(100000)
game = Game()
print("Game starting!")
game_result = game.play_game(player_1_strategy_id, player_2_strategy_id)
print(game_result)
