from game.game import Game
from game.enums import GameResult
from typing import List


def analyze_games(games: List[Game]):
    wins = [game for game in games if game.result == GameResult.win]
    wins_average_turns = sum(game.turns for game in wins) / len(wins) if len(wins) > 0 else 0

    losses = [game for game in games if game.result == GameResult.loss]
    losses_average_turns = sum(game.turns for game in losses) / len(losses) if len(losses) > 0 else 0

    print(f"Total games: {len(games)}.")
    print(f"Starting player wins: {len(wins)}. Average turns: {wins_average_turns}.")
    print(f"Second player wins: {len(losses)}. Average turns: {losses_average_turns}.")
