from game.enums import PieceType
from game.placement_move import PlacementMove
from game.game import Game


class TestPlacementMove(object):
    def test_get_board_target_tile(self):
        game = Game()

        three = game.player_1.get_piece_by_type(PieceType.three)
        tile = game.board.get_tile(1, 4)

        placement_move = PlacementMove(piece=three, tile=tile, board=game.board)
        target_tile = placement_move.get_board_target_tile()

        assert target_tile.x == 1 and target_tile.y == 3

    def test_execute(self):
        game = Game()

        three = game.player_1.get_piece_by_type(PieceType.three)
        tile = game.board.get_tile(4, 2)

        placement_move = PlacementMove(piece=three, tile=tile, board=game.board)
        placement_move.execute()

        assert game.board.get_tile(4, 2).piece == three
