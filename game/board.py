from game.constants import BOARD_SIZE, PLAY_AREA_SIZE, PERIMETER_COORDINATES, CENTER_COORDINATE
from game.enums import GameResult
from game.constants import STRAIGHT_OFFSETS, DIAGONAL_OFFSETS, NORTHEAST_DIAGONAL_OFFSETS, SOUTHEAST_DIAGONAL_OFFSETS
from game.tile import Tile
from game.piece import Piece
from typing import List, Tuple, Optional, Dict


class Board(object):
    def __init__(self):
        self.tiles, self.tiles_by_coordinates_dict = self.setup_tiles()

    def __repr__(self):
        return f"""
            {self.get_tile(1, 3).piece} | {self.get_tile(2, 3).piece} | {self.get_tile(3, 3).piece}
            ---------------------
            {self.get_tile(1, 2).piece} | {self.get_tile(2, 2).piece} | {self.get_tile(3, 2).piece}
            ---------------------
            {self.get_tile(1, 1).piece} | {self.get_tile(2, 1).piece} | {self.get_tile(3, 1).piece}
        """

    @staticmethod
    def setup_tiles() -> Tuple[List[Tile], Dict[int, Dict[int, Tile]]]:
        tiles = list()
        coordinates_dict = dict()
        for x in range(PLAY_AREA_SIZE):
            for y in range(PLAY_AREA_SIZE):
                if x in PERIMETER_COORDINATES and y in PERIMETER_COORDINATES:
                    # It's a corner, ignore
                    continue
                tile = Tile(x, y)
                tiles.append(tile)

                # Add to coordinates dict
                if coordinates_dict.get(x) is None:
                    coordinates_dict[x] = dict()
                coordinates_dict[x][y] = tile

        return tiles, coordinates_dict

    def get_game_result(self, player_id, opponent_id):
        player_complete_row = self.has_complete_row(player_id)
        opponent_complete_row = self.has_complete_row(opponent_id)
        if player_complete_row and not opponent_complete_row:
            return GameResult.win
        if opponent_complete_row and not player_complete_row:
            return GameResult.loss
        return None

    def has_complete_row(self, player_id):
        # Check all rows
        for x in range(1, BOARD_SIZE + 1):
            if self.check_row_completeness(player_id, x, vertical=False):
                return True
        # Check all columns
        for y in range(1, BOARD_SIZE + 1):
            if self.check_row_completeness(player_id, y, vertical=True):
                return True

        # Diagonals
        center_tile = self.get_tile(x=CENTER_COORDINATE, y=CENTER_COORDINATE)
        if center_tile.has_piece_of_player(player_id):
            for direction_offsets in [NORTHEAST_DIAGONAL_OFFSETS, SOUTHEAST_DIAGONAL_OFFSETS]:
                if self.check_diagonal_completness(player_id, direction_offsets):
                    return True

        return False

    def check_diagonal_completness(self, player_id, direction_offsets):
        for x, y in direction_offsets:
            tile = self.get_tile(x=CENTER_COORDINATE + x, y=CENTER_COORDINATE + y)
            if not tile.has_piece_of_player(player_id):
                return False
        return True

    def check_row_completeness(self, player_id, main_coordinate, vertical) -> bool:
        for secondary_coordinate in range(1, BOARD_SIZE + 1):
            if vertical:
                tile = self.get_tile(x=secondary_coordinate, y=main_coordinate)
            else:
                tile = self.get_tile(x=main_coordinate, y=secondary_coordinate)
            if not tile.has_piece_of_player(player_id):
                return False
        return True

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
        target_x, target_y = piece.get_target_coordinates()
        target_tile = self.get_tile(target_x, target_y)
        if target_tile.piece is None:
            self.commit_piece_movement(piece, piece.tile, target_tile)
            return 1
        elif piece.is_pusher():
            push_movements = self.get_chained_push_movements(piece.tile, target_tile)
            for push_source_tile, push_target_tile in reversed(push_movements):
                if push_target_tile is None:
                    push_source_tile.piece.remove_from_play()
                    push_source_tile.remove_piece()
                else:
                    self.commit_piece_movement(push_source_tile.piece, push_source_tile, push_target_tile)
            return len(push_movements)
        elif target_tile.piece.is_pushable() and not piece.is_pushable():
            dx, dy = piece.get_movement_offsets()
            push_target_tile = self.get_tile(x=target_tile.x + dx, y=target_tile.y + dy)
            if push_target_tile is None or push_target_tile.piece is None:
                self.commit_piece_movement(target_tile.piece, target_tile, push_target_tile)
                self.commit_piece_movement(piece, piece.tile, target_tile)
                return 2
            else:
                self.remove_piece_stuck_in_perimeter(piece.tile, piece)
        else:
            self.remove_piece_stuck_in_perimeter(piece.tile, piece)
        return 0

    @staticmethod
    def remove_piece_stuck_in_perimeter(tile: Tile, piece: Piece):
        # Piece could not move, remove it from play if it was stuck in the perimeter
        if tile.is_perimeter():
            tile.remove_piece()
            piece.remove_from_play()

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

    def get_board_pieces(self, exclude_perimeter=False):
        pieces = list()
        for tile in self.tiles:
            if tile.piece and not (exclude_perimeter and tile.is_perimeter()):
                pieces.append(tile.piece)
        return pieces

    def is_full(self):
        for tile in self.tiles:
            if not tile.is_perimeter() and tile.piece is None:
                return False
        return True

    def get_tile(self, x, y) -> Optional[Tile]:
        x_dict = self.tiles_by_coordinates_dict.get(x)
        return x_dict.get(y) if x_dict is not None else None

    def get_board_tiles(self):
        return [tile for tile in self.tiles if not tile.is_perimeter()]

    def get_legal_tiles(self):
        return [tile for tile in self.tiles if tile.is_placement_legal()]

    def get_tile_neighbors(self, tile: Tile, include_diagonals: bool) -> List[Tile]:
        neighbor_tiles, target_offsets = list(), list()
        target_offsets += STRAIGHT_OFFSETS
        if include_diagonals:
            target_offsets += DIAGONAL_OFFSETS
        for target_tile in self.tiles:
            offsets = target_tile.x - tile.x, target_tile.y - tile.y
            if offsets in target_offsets:
                neighbor_tiles.append(target_tile)
        return neighbor_tiles

    def get_tile_perimeter_neightbors(self, tile: Tile, include_diagonals: bool):
        return [tile for tile in self.get_tile_neighbors(tile, include_diagonals) if tile.is_perimeter()]

    def to_string_notation(self) -> str:
        result = ""
        for index, tile in enumerate(self.tiles):
            if index > 0:
                result += ","
            result += tile.to_string_notation()
        return result
