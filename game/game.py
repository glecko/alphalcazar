from __future__ import annotations
from game.board import Board
from game.player import Player, PlacementMove
from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.enums import GameResult
from typing import Callable, Optional

StrategyFunction = Callable[[Player, Player, bool], Optional[PlacementMove]]


class Game(object):
    def __init__(self, on_player_moves_executed: Optional[Callable] = None):
        self.on_player_moves_executed = on_player_moves_executed
        self.board = Board()
        self.player_1 = Player(PLAYER_1_ID, self.board, self)
        self.player_2 = Player(PLAYER_2_ID, self.board, self)
        self.starting_player = self.player_1
        self.first_move_executed = False
        self.player_moves = {
            PLAYER_1_ID: list(),
            PLAYER_2_ID: list()
        }
        self.turns = 0
        self.result = None

    def play_game(self, player_1_strategy_fn: StrategyFunction, player_2_strategy_fn: StrategyFunction) -> GameResult:
        while self.result is None:
            self.first_move_executed = False
            self.execute_player_moves(player_1_strategy_fn, player_2_strategy_fn)
            if self.on_player_moves_executed is not None:
                self.on_player_moves_executed()
            executed_movements = self.board.execute_board_movements(self.starting_player.id)
            self.turns += 1
            if executed_movements == 0 and self.board.is_full():
                self.result = GameResult.draw
                return self.result
            self.switch_starting_player()
            self.result = self.get_current_result()
        return self.result

    def execute_player_moves(self, player_1_strategy_fn: StrategyFunction, player_2_strategy_fn: StrategyFunction):
        starting_strategy_fn = player_1_strategy_fn if self.starting_player.id == self.player_1.id else player_2_strategy_fn
        starting_move = starting_strategy_fn(self.starting_player, self.get_secondary_player(), True)
        if starting_move is not None:
            starting_move.execute()
        self.first_move_executed = True
        second_strategy_fn = player_1_strategy_fn if self.get_secondary_player().id == self.player_1.id else player_2_strategy_fn
        second_move = second_strategy_fn(self.get_secondary_player(), self.starting_player, False)
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
            raise ValueError(f"Unknown player ID specified: {player_id}.")

    # Returns the player that is next to play
    def get_active_player(self):
        return self.get_secondary_player() if self.first_move_executed else self.starting_player

    def switch_starting_player(self):
        self.starting_player = self.get_secondary_player()

    def get_secondary_player(self) -> Player:
        if self.starting_player is self.player_1:
            return self.player_2
        return self.player_1

    def clone(self) -> Game:
        clone_game = Game()
        clone_game.starting_player = clone_game.get_player_by_id(self.starting_player.id)
        board_pieces = self.board.get_board_pieces()
        for board_piece in board_pieces:
            owner = clone_game.get_player_by_id(board_piece.owner_id)
            target_piece = owner.get_piece_by_type(board_piece.type)
            target_piece.set_movement_direction(board_piece.direction)
            target_tile = clone_game.board.get_tile(board_piece.tile.x, board_piece.tile.y)
            target_tile.place_piece(target_piece)
        return clone_game
