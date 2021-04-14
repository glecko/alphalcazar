from game.constants import BOARD_SIZE, PLAY_AREA_SIZE, PERIMETER_COORDINATES
from game.enums import GameResult
from game.tile import Tile
from game.piece import Piece
from typing import List, Tuple, Optional


class Board(object):
    def __init__(self):
        self.tiles = self.setup_tiles()

    def __repr__(self):
        return f"""
            {self.get_tile(1, 3).piece} | {self.get_tile(2, 3).piece} | {self.get_tile(3, 3).piece}
            ---------------------
            {self.get_tile(1, 2).piece} | {self.get_tile(2, 2).piece} | {self.get_tile(3, 2).piece}
            ---------------------
            {self.get_tile(1, 1).piece} | {self.get_tile(2, 1).piece} | {self.get_tile(3, 1).piece}
        """

    @staticmethod
    def setup_tiles():
        tiles = list()
        for x in range(PLAY_AREA_SIZE):
            for y in range(PLAY_AREA_SIZE):
                if x in PERIMETER_COORDINATES and y in PERIMETER_COORDINATES:
                    # It's a corner, ignore
                    continue
                tile = Tile(x, y)
                tiles.append(tile)
        return tiles

    def get_game_result(self, player, opponent):
        if self.has_complete_row(player) and self.has_complete_row(opponent):
            return GameResult.draw
        if self.has_complete_row(player):
            return GameResult.win
        if self.has_complete_row(opponent):
            return GameResult.loss
        return None

    def has_complete_row(self, player):
        # Check all rows
        for x in range(1, BOARD_SIZE + 1):
            if self.check_row_completeness(player, x, vertical=False):
                return True
        # Check all columns
        for y in range(1, BOARD_SIZE + 1):
            if self.check_row_completeness(player, y, vertical=True):
                return True

        # Diagonals
        center_coordinate = (BOARD_SIZE + 1) / 2
        center_tile = self.get_tile(x=center_coordinate, y=center_coordinate)
        if center_tile.has_piece_of_player(player):
            diagonal_direction_offsets = [
                [(-1, -1), (1, 1)],
                [(-1, 1), (1, -1)]
            ]
            for direction_offsets in diagonal_direction_offsets:
                pieces = 1
                for x, y in direction_offsets:
                    tile = self.get_tile(x=center_coordinate + x, y=center_coordinate + y)
                    if tile.has_piece_of_player(player):
                        pieces += 1
                if pieces == BOARD_SIZE:
                    return True

        return False

    def check_row_completeness(self, player, main_coordinate, vertical) -> bool:
        pieces = 0
        for secondary_coordinate in range(1, BOARD_SIZE + 1):
            if vertical:
                tile = self.get_tile(x=secondary_coordinate, y=main_coordinate)
            else:
                tile = self.get_tile(x=main_coordinate, y=secondary_coordinate)
            if tile.has_piece_of_player(player):
                pieces += 1
        if pieces == BOARD_SIZE:
            return True
        return False

    def get_movement_ordered_pieces(self, starting_player_id: int):
        pieces = self.get_board_pieces()

        def get_piece_movement_order(piece: Piece):
            return piece.get_movement_order(starting_player_id)

        return sorted(pieces, key=get_piece_movement_order)

    def execute_board_movements(self, starting_player_id: int) -> int:
        pieces = self.get_movement_ordered_pieces(starting_player_id)
        executed_movements = 0
        for piece in pieces:
            # Piece may have been removed from board by other pieces #
            if not piece.is_on_board():
                continue
            executed_movements += self.execute_piece_movement(piece)
        return executed_movements

    def execute_piece_movement(self, piece: Piece) -> int:
        source_tile = self.get_piece_tile(piece)
        x, y = piece.get_movement_offsets()
        target_tile = self.get_tile(x=source_tile.x + x, y=source_tile.y + y)
        if target_tile.piece is None:
            self.commit_piece_movement(piece, source_tile, target_tile)
            return 1
        elif piece.is_pusher():
            push_movements = self.get_chained_push_movements(source_tile, target_tile)
            for push_source_tile, push_target_tile in reversed(push_movements):
                if push_target_tile is None:
                    push_source_tile.piece.remove_from_play()
                    push_source_tile.remove_piece()
                else:
                    self.commit_piece_movement(push_source_tile.piece, push_source_tile, push_target_tile)
            return len(push_movements)
        elif target_tile.piece.is_pushable() and not piece.is_pushable():
            push_target_tile = self.get_tile(x=target_tile.x + x, y=target_tile.y + y)
            if push_target_tile is None or push_target_tile.piece is None:
                self.commit_piece_movement(target_tile.piece, target_tile, push_target_tile)
                self.commit_piece_movement(piece, source_tile, target_tile)
                return 2
        return 0

    def get_chained_push_movements(self, source_tile: Tile, target_tile: Tile) -> List[Tuple[Tile, Tile]]:
        x_direction = target_tile.x - source_tile.x
        y_direction = target_tile.y - source_tile.y
        push_movements = list()
        push_target_tile = target_tile
        push_source_tile = source_tile
        while push_source_tile.piece is not None:
            push_movements.append((push_source_tile, push_target_tile))
            push_source_tile = push_target_tile
            push_target_tile = self.get_tile(x=push_source_tile.x + x_direction, y=push_source_tile.y + y_direction)
            if push_target_tile is None and push_source_tile.piece is not None:
                push_movements.append((push_source_tile, None))
                break
        return push_movements

    @staticmethod
    def commit_piece_movement(piece: Piece, source_tile: Tile, target_tile: Optional[Tile]):
        source_tile.remove_piece()
        if target_tile is None or target_tile.is_perimeter():
            piece.remove_from_play()
        else:
            target_tile.place_piece(piece)

    def get_board_pieces(self):
        pieces = list()
        for tile in self.tiles:
            if tile.piece:
                pieces.append(tile.piece)
        return pieces

    def is_full(self):
        for tile in self.tiles:
            if not tile.is_perimeter() and tile.piece is None:
                return False
        return True

    def get_piece_tile(self, piece: Piece):
        for tile in self.tiles:
            if tile.piece == piece:
                return tile
        return None

    def get_tile(self, x, y) -> Tile:
        return next((tile for tile in self.tiles if tile.x == x and tile.y == y), None)

    def get_board_tiles(self):
        return [tile for tile in self.tiles if not tile.is_perimeter()]

    def get_legal_tiles(self):
        return [tile for tile in self.tiles if tile.is_placement_legal()]

    def to_string_notation(self) -> str:
        result = ""
        for index, tile in enumerate(self.tiles):
            if index > 0:
                result += ","
            result += tile.to_string_notation()
        return result
