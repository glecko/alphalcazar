from game.player import Player
from strategies.tree_search.min_max import find_best_next_move, min, max
from strategies.tree_search.abstract_move import AbstractMove, get_legal_scored_moves
from strategies.tree_search.config import WIN_CONDITION_SCORE
from multiprocessing import Pool
from itertools import repeat

pool = None


def get_or_init_pool():
    global pool
    if pool is None:
        pool = Pool()
    return pool


def max_multiprocess(player: Player, opponent: Player, depth: int, is_first_move: bool, alpha: int, beta: int):
    moves = get_legal_scored_moves(player)
    best_moves, best_score = list(), -WIN_CONDITION_SCORE * 10
    pool_instance = get_or_init_pool()
    process_results = pool_instance.starmap(
        find_best_next_move,
        zip(
            moves,
            repeat(max), repeat(min),
            repeat(player), repeat(opponent), repeat(depth), repeat(is_first_move),
            repeat(alpha), repeat(beta), repeat(depth)
        ),
    )
    for index, result in enumerate(process_results):
        next_best_moves, next_best_score = result
        if next_best_score > best_score:
            best_moves = [moves[index]]
            best_score = next_best_score
        elif next_best_score == best_score:
            best_moves.append(moves[index])
    return best_moves, best_score
