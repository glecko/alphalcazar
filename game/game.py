from __future__ import annotations
from game.board import Board
from game.player import Player, PlacementMove
from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.enums import GameResult, Direction


class Game(object):
    def __init__(self):
        self.board = Board()
        self.player_1 = Player(PLAYER_1_ID, self.board, self)
        self.player_2 = Player(PLAYER_2_ID, self.board, self)
        self.starting_player = self.player_1
        self.player_moves = {
            PLAYER_1_ID: list(),
            PLAYER_2_ID: list()
        }
        self.turns = 0
        self.result = None

    @classmethod
    def from_string_notation(cls, notation: str) -> Game:
        game = cls()
        starting_player, board_notation = notation.split("#")
        if int(starting_player) == PLAYER_1_ID:
            game.starting_player = game.player_1
        elif int(starting_player) == PLAYER_2_ID:
            game.starting_player = game.player_2
        else:
            raise ValueError(f"Unknown starting player ID found: {starting_player!r}.")

        tiles = board_notation.split(",")
        for index, tile_notation in enumerate(tiles):
            if tile_notation == "":
                continue
            piece_type, owner_id, direction = tile_notation.split("|")
            owner = game.get_player_by_id(int(owner_id))
            piece = owner.get_piece_by_type(int(piece_type))
            piece.set_movement_direction(Direction[direction])
            game.board.tiles[index].place_piece(piece)
        return game

    @staticmethod
    def play_random_piece_strategy(player: Player, opponent: Player, is_starting: bool) -> PlacementMove:
        return player.play_random_piece()

    def play_random_game(self) -> GameResult:
        return self.play_game(self.play_random_piece_strategy)

    def play_game(self, player_strategy_fn) -> GameResult:
        while self.result is None:
            self.execute_player_moves(player_strategy_fn)
            executed_movements = self.board.execute_board_movements(self.starting_player.id)
            self.turns += 1
            if executed_movements == 0 and self.board.is_full():
                self.result = GameResult.draw
                return self.result
            self.switch_starting_player()
            self.result = self.get_current_result()
        print(self.result)
        return self.result

    def execute_player_moves(self, player_strategy_fn):
        starting_move = player_strategy_fn(self.starting_player, self.get_secondary_player(), is_starting=True)
        print(starting_move)
        if starting_move is not None:
            starting_move.execute()
        second_move = player_strategy_fn(self.get_secondary_player(), self.starting_player, is_starting=False)
        print(second_move)
        if second_move is not None:
            second_move.execute()
        self.player_moves[self.starting_player.id].append(starting_move)
        self.player_moves[self.get_secondary_player().id].append(second_move)

    def get_current_result(self) -> GameResult:
        return self.board.get_game_result(self.player_1.id, self.player_2.id)

    def get_player_by_id(self, player_id) -> Player:
        if player_id == PLAYER_1_ID:
            return self.player_1
        elif player_id == PLAYER_2_ID:
            return self.player_2
        else:
            raise ValueError(f"Unknown player ID specified: {player_id}")

    def switch_starting_player(self):
        self.starting_player = self.get_secondary_player()

    def get_secondary_player(self) -> Player:
        if self.starting_player is self.player_1:
            return self.player_2
        return self.player_1

    def to_string_notation(self):
        return f"{self.starting_player.id}#{self.board.to_string_notation()}"

    def clone(self) -> Game:
        return self.from_string_notation(f"{self.starting_player.id}#{self.board.to_string_notation()}")
