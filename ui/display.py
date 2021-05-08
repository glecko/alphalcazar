import sys, pygame
from game.game import Game
from game.piece import Piece
from game.enums import PieceType
from game.player import PlacementMove
from ui.selectables import SelectableEntity
from ui.constants import *
import threading


class Display(object):

    def __init__(self, game: Game):
        pygame.init()
        self.move_made = threading.Event()

        self.screen = pygame.display.set_mode(SIZE)
        self.screen.fill(BACKGROUND_COLOR)
        font = pygame.font.SysFont(None, 60)
        self.piece_type_image = {
            PieceType.one: font.render('1', True, BACKGROUND_COLOR),
            PieceType.two: font.render('2', True, BACKGROUND_COLOR),
            PieceType.three: font.render('3', True, BACKGROUND_COLOR),
            PieceType.four: font.render('4', True, BACKGROUND_COLOR),
            PieceType.five: font.render('5', True, BACKGROUND_COLOR),
        }
        self.game = game
        self.selected_entity = None
        self.selectable_pieces = []
        self.selectable_tiles = []
        self.move = None

    def create_board(self):
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    sys.exit()
                if event.type == pygame.MOUSEBUTTONUP:
                    self.evaluate_drop()
                if event.type == pygame.MOUSEBUTTONDOWN:
                    self.evaluate_collisions()

            self.draw_board()

    def evaluate_collisions(self):
        for ent in self.selectable_pieces:
            if ent.area.collidepoint(pygame.mouse.get_pos()):
                self.selected_entity = ent
                return

    def evaluate_drop(self):
        if self.selected_entity is None:
            return
        for ent in self.selectable_tiles:
            if ent.area.collidepoint(pygame.mouse.get_pos()):
                if not self.game.first_move_executed:
                    legal_moves = self.game.starting_player.get_legal_placement_moves()
                else:
                    legal_moves = self.game.get_secondary_player().get_legal_placement_moves()
                move = PlacementMove(self.selected_entity.object, ent.object)
                if move in legal_moves:
                    self.move = move
                    self.move_made.set()

                self.selected_entity = None
                return
        self.selected_entity = None

    def consume_move(self):
        self.move_made.clear()
        return self.move

    def draw_board(self):
        self.selectable_pieces = []
        self.selectable_tiles = []

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
                DY - 2 * TILES_BORDER), border_radius=3)

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
        if self.selected_entity is not None and self.selected_entity.object == piece:
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
        return [(x + dx / 2 + dx * 0.3 / SQRT_2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 + dir_y * (dy * 0.3 / SQRT_2)),
         (x + dx / 2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 + dy * 0.3 / SQRT_2 + dir_y * (dy * 0.3 / SQRT_2)),
         (x + dx / 2 - dx * 0.3 / SQRT_2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 + dir_y * (dy * 0.3 / SQRT_2)),
         (x + dx / 2 + dir_x * (dx * 0.3 / SQRT_2), y + dy / 2 - dy * 0.3 / SQRT_2 + dir_y * (dy * 0.3 / SQRT_2))]