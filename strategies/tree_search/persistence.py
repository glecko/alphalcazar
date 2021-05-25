from strategies.tree_search.transposition import TRANSPOSITION_DICT, TranspositionCache
from strategies.tree_search.abstract_move import AbstractMove, get_legal_abstract_moves
from strategies.tree_search.config import EvaluationType
from game.game import Game
from game.enums import PieceType
from decouple import config
from psycopg2.extras import execute_values
from typing import Dict
import json
import psycopg2
import logging

logger = logging.getLogger(__name__)

QUERY_STORE_LEGAL_MOVES = """
INSERT INTO moves(x, y, piece_type, owner_id) VALUES %s;
"""

QUERY_PERSIST_TRANSPOSITION_CACHE = """
INSERT INTO transposition_cache(board_hash_key, best_move_ids, score, depth, evaluation_type) VALUES %s 
ON CONFLICT(board_hash_key) DO UPDATE SET best_move_ids = excluded.best_move_ids, score = excluded.score, 
depth = excluded.depth, evaluation_type = excluded.evaluation_type;
"""

QUERY_LOAD_TRANSPOSITION_CACHE = """
SELECT board_hash_key, best_move_ids, score, depth, evaluation_type FROM transposition_cache;
"""


class DatabaseConnection(object):

    def __init__(self):
        self.dsn = config("TRANSPOSITION_CACHE_PERSISTENCE_DSN")

    def __enter__(self):
        # Connect to an existing database
        self.database_connection = psycopg2.connect(self.dsn)
        self.cursor = self.database_connection.cursor()
        self.populate_moves_table()
        self.moves: Dict[int, AbstractMove] = self.load_moves_dictionary()
        return self

    def __exit__(self, *args):
        self.cursor.close()
        self.database_connection.close()

    def fetch_query(self, query, parameters):
        self.cursor.execute(query, parameters)
        logger.debug(f"Executed query: {self.cursor.query!r}")
        result = self.cursor.fetchall()
        return result

    def execute_values(self, query, values):
        execute_values(self.cursor, query, values)
        self.database_connection.commit()

    def load_moves_dictionary(self) -> Dict[int, AbstractMove]:
        moves_dict = dict()
        self.cursor.execute("SELECT id, x, y, piece_type, owner_id from moves;")
        for move_id, x, y, piece_type, owner_id in self.cursor.fetchall():
            move = AbstractMove.from_raw_values(x, y, PieceType(piece_type) if piece_type is not None else None, owner_id)
            moves_dict[move_id] = move
        return moves_dict

    def populate_moves_table(self):
        self.cursor.execute("SELECT COUNT(*) from moves;")
        if self.cursor.fetchone()[0] > 0:
            return
        game = Game()
        moves = get_legal_abstract_moves(game.player_1, filter_symmetric_moves=False)
        moves += get_legal_abstract_moves(game.player_2, filter_symmetric_moves=False)
        moves.append(AbstractMove(None, game.player_1))
        moves.append(AbstractMove(None, game.player_2))
        move_values = [[move.x, move.y, int(move.piece_type) if move.piece_type is not None else None, move.owner_id] for move in moves]
        self.execute_values(QUERY_STORE_LEGAL_MOVES, move_values)
        self.database_connection.commit()


def build_move_properties_dict(moves_dict: Dict[int, AbstractMove]):
    properties_dict = dict()
    for move_id, move in moves_dict.items():
        if properties_dict.get(move.owner_id) is None:
            properties_dict[move.owner_id] = dict()
        if properties_dict[move.owner_id].get(move.piece_type) is None:
            properties_dict[move.owner_id][move.piece_type] = dict()
        if properties_dict[move.owner_id][move.piece_type].get(move.x) is None:
            properties_dict[move.owner_id][move.piece_type][move.x] = dict()
        properties_dict[move.owner_id][move.piece_type][move.x][move.y] = move_id
    return properties_dict


def persist_transposition_cache():
    logger.info(f"Persisting transposition cache with {len(TRANSPOSITION_DICT)} entries.")
    try:
        with DatabaseConnection() as db_conn:
            values = list()
            moves_by_props = build_move_properties_dict(db_conn.moves)
            for key, value in TRANSPOSITION_DICT.items():
                best_moves, score, depth, evaluation_type = value
                # For scalability purposes, for now we only store exact evaluations of depth >= 2
                if evaluation_type != EvaluationType.exact or depth < 2:
                    continue
                best_move_ids = [moves_by_props[move.owner_id][move.piece_type][move.x][move.y] for move in best_moves]
                entry_values = [key, best_move_ids, score, depth, int(evaluation_type)]
                values.append(entry_values)
            db_conn.execute_values(QUERY_PERSIST_TRANSPOSITION_CACHE, values)
    except Exception as e:
        logger.warning(f"Error while persisting transposition cache: {e}.")


def load_transposition_cache_data() -> TranspositionCache:
    result = {}
    try:
        with DatabaseConnection() as db_conn:
            query_result = db_conn.fetch_query(QUERY_LOAD_TRANSPOSITION_CACHE, {})
            for board_hash_key, best_move_ids, score, depth, evaluation_type in query_result:
                best_moves = [db_conn.moves[move_id] for move_id in best_move_ids]
                result[board_hash_key] = best_moves, score, depth, EvaluationType(evaluation_type)
    except Exception as e:
        logger.warning(f"Error while loading transposition cache: {e}.")
    return result


def hydrate_transposition_cache():
    # Skip this step if the transposition cache was already hydrated this execution
    if bool(TRANSPOSITION_DICT):
        logger.info("Transposition cache was already hydrated, skipping.")
        return

    logger.info("Hydrating transposition cache from persistence...")
    data = load_transposition_cache_data()
    TRANSPOSITION_DICT.update(data)
    logger.info(f"Hydrated transposition cache with {len(TRANSPOSITION_DICT)} entries.")

