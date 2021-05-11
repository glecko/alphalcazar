from game.player import Player
from strategies.tree_search.min_max import find_best_next_move, min, max
from strategies.tree_search.scored_move import ScoredMove, get_legal_scored_moves
from strategies.tree_search.config import WIN_CONDITION_SCORE
from multiprocessing import Pool
from itertools import repeat

pool = Pool()


def max_multiprocess(player: Player, opponent: Player, depth: int, is_first_move: bool, alpha: int, beta: int):
    moves = get_legal_scored_moves(player)
    best_move = ScoredMove(None, -WIN_CONDITION_SCORE * 10)
    next_scored_movements = pool.starmap(
        find_best_next_move,
        zip(
            moves,
            repeat(max), repeat(min),
            repeat(player), repeat(opponent), repeat(depth), repeat(is_first_move),
            repeat(alpha), repeat(beta), repeat(depth)
        ),
    )
    for index, next_move in enumerate(next_scored_movements):
        if next_move.score > best_move.score:
            best_move = moves[index]
            best_move.score = next_move.score
    return best_move
