import sys, pygame
from game.game import Game
from game.tile import Tile
from game.piece import Piece
from game.enums import PieceType
from game.player import PlacementMove
from ui.selectables import SelectableEntity
from ui.constants import *
from typing import List
import threading


class Display(object):

    def __init__(self, game: Game):
        pygame.init()
        self.move_input_event = threading.Event()

        self.screen = pygame.display.set_mode(SIZE)
        self.screen.fill(BACKGROUND_COLOR)

        self.piece_type_image = self.build_piece_type_images()
        self.game = game
        self.selected_piece: Piece = None
        self.selectable_pieces: List[SelectableEntity] = list()
        self.selectable_tiles: List[SelectableEntity] = list()
        self.consumable_move = None

    @staticmethod
    def build_piece_type_images():
        piece_type_image = dict()
        font = pygame.font.SysFont(None, 60)
        for piece_type in PieceType:
            piece_type_image[piece_type] = font.render(str(int(piece_type)), True, BACKGROUND_COLOR)
        return piece_type_image

    @staticmethod
    def get_mouse_position_object(selectable_list: List[SelectableEntity]):
        x, y = pygame.mouse.get_pos()
        for ent in selectable_list:
            if ent.area.collidepoint(x, y):
                return ent.object
        return None

    def create_board(self):
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    sys.exit()
                if event.type == pygame.MOUSEBUTTONUP:
                    self.evaluate_drop()
                if event.type == pygame.MOUSEBUTTONDOWN:
                    self.evaluate_click_collisions()

            self.draw_board()

    def evaluate_click_collisions(self):
        self.selected_piece = self.get_mouse_position_object(self.selectable_pieces)

    def evaluate_drop(self):
        if self.selected_piece is None:
            return
        drop_tile: Tile = self.get_mouse_position_object(self.selectable_tiles)
        if drop_tile is not None:
            legal_moves = self.game.get_active_player().get_legal_placement_moves()
            move = PlacementMove(self.selected_piece, drop_tile)
            if move in legal_moves:
                self.consumable_move = move
                self.move_input_event.set()
        self.selected_piece = None

    def consume_move(self):
        self.move_input_event.clear()
        return self.consumable_move

    def draw_board(self):
        self.selectable_pieces.clear()
        self.selectable_tiles.clear()

        self.screen.fill(BACKGROUND_COLOR)

        for tile in self.game.board.tiles:
            x = WIDTH * 0.15 + tile.x * WIDTH * 0.7 / PLAY_AREA_SIZE
            y = HEIGHT * 0.05 + tile.y * HEIGHT * 0.9 / PLAY_AREA_SIZE
            if tile.is_perimeter():
                color = PERIMETER_TILE_COLOR
            else:
                color = TILE_COLOR
            rect = pygame.draw.rect(self.screen, color, (
                x + TILES_BORDER,
                y + TILES_BORDER,
                DX - 2 * TILES_BORDER,
                DY - 2 * TILES_BORDER
            ), border_radius=3)

            self.selectable_tiles.append(SelectableEntity(rect, tile))
            if tile.piece is not None:
                self.draw_piece(tile.piece, x, y)

        for available_piece in self.game.player_1.get_available_pieces():
            self.draw_available_piece(available_piece)
        for available_piece in self.game.player_2.get_available_pieces():
            self.draw_available_piece(available_piece)

        pygame.display.flip()

    def draw_piece(self, piece: Piece, x, y):
        pygame.draw.circle(self.screen, PLAYER_COLOR[piece.owner_id], (x + DX/2, y + DY/2), DX * 0.3)
        dir_x, dir_y = piece.get_movement_offsets()
        pygame.draw.polygon(
            self.screen,
            PLAYER_COLOR[piece.owner_id],
            points=self.get_poli_points(x, y, DX, DY, dir_x, dir_y)
        )
        self.screen.blit(self.piece_type_image[piece.type], (x + DX / 2.5, y + DY / 3))

    def draw_available_piece(self, piece: Piece):
        if self.selected_piece == piece:
            x, y = pygame.mouse.get_pos()
            x -= DX/2
            y -= DY/2
        else:
            x = 0 if piece.owner_id == PLAYER_1_ID else WIDTH * 0.86
            y = HEIGHT * 0.05 + (int(piece.type)-1) * HEIGHT * 0.9 / PLAY_AREA_SIZE
        circle = pygame.draw.circle(self.screen, PLAYER_COLOR[piece.owner_id], (x + DX/2, y + DY/2), DX * 0.3)
        pygame.draw.polygon(
            self.screen,
            PLAYER_COLOR[piece.owner_id],
            points=self.get_poli_points(x, y, DX, DY, 1 if piece.owner_id == PLAYER_1_ID else -1, 0)
        )
        self.screen.blit(self.piece_type_image[piece.type], (x + DX / 2.5, y + DY / 3))

        self.selectable_pieces.append(SelectableEntity(circle, piece))

    @staticmethod
    def get_poli_points(x, y, dx, dy, dir_x, dir_y):
        return [
            (x + dx / 2 + dx * 0.3 / SQRT_2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 + dir_y * (dy * 0.3 / SQRT_2)),
            (x + dx / 2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 + dy * 0.3 / SQRT_2 + dir_y * (dy * 0.3 / SQRT_2)),
            (x + dx / 2 - dx * 0.3 / SQRT_2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 + dir_y * (dy * 0.3 / SQRT_2)),
            (x + dx / 2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 - dy * 0.3 / SQRT_2 + dir_y * (dy * 0.3 / SQRT_2))
        ]
