from strategies.tree_search.transposition import TRANSPOSITION_DICT, TranspositionCache
from strategies.tree_search.abstract_move import AbstractMove
from strategies.tree_search.config import EvaluationType
from decouple import config
from psycopg2.extras import execute_values
import json
import psycopg2
import logging

logger = logging.getLogger(__name__)


QUERY_PERSIST_TRANSPOSITION_CACHE = """
INSERT INTO transposition_cache(board_hash_key, best_moves, score, depth, evaluation_type) VALUES %s 
ON CONFLICT(board_hash_key) DO UPDATE SET best_moves = excluded.best_moves, score = excluded.score, 
depth = excluded.depth, evaluation_type = excluded.evaluation_type;
"""

QUERY_LOAD_TRANSPOSITION_CACHE = """
SELECT board_hash_key, best_moves, score, depth, evaluation_type FROM transposition_cache;
"""


class DatabaseConnection(object):

    def __init__(self):
        self.dsn = config("TRANSPOSITION_CACHE_PERSISTENCE_DSN")

    def __enter__(self):
        # Connect to an existing database
        self.database_connection = psycopg2.connect(self.dsn)
        self.cursor = self.database_connection.cursor()
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


def persist_transposition_cache():
    logger.info(f"Persisting transposition cache with {len(TRANSPOSITION_DICT)} entries.")
    try:
        with DatabaseConnection() as db_conn:
            values = list()
            for key, value in TRANSPOSITION_DICT.items():
                best_moves, score, depth, evaluation_type = value
                # For scalability purposes, for now we only store exact evaluations
                if evaluation_type != EvaluationType.exact:
                    continue
                best_moves_json = [move.to_json() for move in best_moves]
                entry_values = [key, json.dumps(best_moves_json), score, depth, int(evaluation_type)]
                values.append(entry_values)
            db_conn.execute_values(QUERY_PERSIST_TRANSPOSITION_CACHE, values)
    except Exception as e:
        logger.warning(f"Error while persisting transposition cache: {e}.")


def load_transposition_cache_data() -> TranspositionCache:
    result = {}
    try:
        with DatabaseConnection() as db_conn:
            query_result = db_conn.fetch_query(QUERY_LOAD_TRANSPOSITION_CACHE, {})
            for board_hash_key, best_moves_json, score, depth, evaluation_type in query_result:
                best_moves = list()
                for move_json in best_moves_json:
                    best_moves.append(AbstractMove.from_json(move_json))
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

