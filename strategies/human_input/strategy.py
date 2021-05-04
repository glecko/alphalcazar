from game.enums import PieceType
from game.player import Player, PlacementMove
from typing import Optional


def human_input_strategy(player: Player, opponent: Player, is_starting: bool) -> Optional[PlacementMove]:
    legal_moves = player.get_legal_placement_moves()
    if len(legal_moves) == 0:
        print("You have no valid legal moves, skipping")
        return None
    matching_legal_move = None
    while matching_legal_move is None:
        piece_type = PieceType(int(input("Piece type: ")))
        x = int(input("X coordinate: "))
        y = int(input("Y coordinate: "))
        matching_legal_move = next((move for move in legal_moves if move.piece.type == piece_type and move.tile.x == x and move.tile.y == y), None)
        if matching_legal_move is None:
            print(f"Invalid legal move specified (piece: {piece_type}, x: {x}, y: {y}), try again.")

    return matching_legal_move
