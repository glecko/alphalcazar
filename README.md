# Alphalcazar

Python implementation of the advanced tic tac toe board game, invented by a friend of mine (Adrià Balcazar), together with several implementations of an AI for said game.

## Rules

The rules of the game can be found in the [rules document](docs/RULES.md).

## Setting up the project

### Installing dependencies

Install the requirements of the project:

`pip install -r requirements.txt`

### Setting up transposition persistence (Optional)

The `tree_search` strategy uses PostgreSQL to store the transposition cache.

#### Installing PostgreSQL

Follow the [official Getting started tutorial](https://www.postgresqltutorial.com/postgresql-getting-started/) to install and setup PostgreSQL on your local machine.

#### Create the database

Log into the PostgreSQL database server with any client tool (pgAdmin, Datagrip) and run:

```
CREATE DATABASE alphalcazar;

CREATE TABLE transposition_cache (
	board_hash_key VARCHAR(255) PRIMARY KEY,
	best_move_ids INTEGER[],
	score INTEGER,
	depth INTEGER,
	evaluation_type INTEGER
);

CREATE TABLE moves (
    id SERIAL PRIMARY KEY,
	x INTEGER,
	y INTEGER,
	piece_type INTEGER,
	owner_id INTEGER
);
```

#### Setting up .env file

You need to create a `.env` file in your project's root folder with the following schema:

```
TRANSPOSITION_CACHE_PERSISTENCE_DSN=postgres://<postgres_user>:<postgres_password>@localhost/alphalcazar
```


## Tests

Run the tests to make sure everything is in order:

`pytest`