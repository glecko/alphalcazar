from game.enums import PieceType
from game.player import Player, PlacementMove
from typing import Optional
from ui.display import Display


def build_ui_input_strategy(display: Display):
    def ui_input_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
        display.move_made.wait()
        return display.consume_move()
    return ui_input_strategy
