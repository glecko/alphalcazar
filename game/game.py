from game.board import Board
from game.player import Player, PlacementMove
from game.constants import PLAYER_1_ID, PLAYER_2_ID
from game.enums import GameResult


class Game(object):
    def __init__(self):
        self.board = Board()
        self.player_1 = Player(PLAYER_1_ID, self.board)
        self.player_2 = Player(PLAYER_2_ID, self.board)
        self.starting_player = self.player_1
        self.player_1_moves, self.player_2_moves = list(), list()
        self.turns = 0
        self.result = None

    @staticmethod
    def play_random_piece_strategy(player: Player, is_starting: bool) -> PlacementMove:
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
            self.starting_player = self.get_secondary_player()
            self.result = self.get_current_result()
        return self.result

    def execute_player_moves(self, player_strategy_fn):
        starting_move = player_strategy_fn(self.starting_player, is_starting=True)
        second_move = player_strategy_fn(self.get_secondary_player(), is_starting=False)
        if self.starting_player.id == PLAYER_1_ID:
            self.player_1_moves.append(starting_move)
            self.player_2_moves.append(second_move)
        else:
            self.player_1_moves.append(second_move)
            self.player_2_moves.append(starting_move)

    def get_current_result(self) -> GameResult:
        return self.board.get_game_result(self.player_1.id, self.player_2.id)

    def get_secondary_player(self) -> Player:
        if self.starting_player is self.player_1:
            return self.player_2
        return self.player_1
