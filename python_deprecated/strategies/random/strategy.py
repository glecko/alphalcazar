from game.player import Player
from game.placement_move import PlacementMove
from typing import Optional
import random


def random_move_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
    legal_moves = player.get_legal_placement_moves()
    if len(legal_moves) > 0:
        return random.choice(legal_moves)
    return None
