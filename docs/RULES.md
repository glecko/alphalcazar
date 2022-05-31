# Rules

Advanced tic tac toe is played on a 3x3 grid, similar to regular tic tac toe. Players take turns to place pieces, but cannot do so on the board itself. Instead, they can only place their pieces on the edge of the board, facing towards the board.

![Pygame board](images/pygame_ui.png?raw=true)


### Pieces

Each player has 5 pieces, numbered 1-5. The number on each piece represents the order in which that piece will move.

### Placement phase

The player who has the crown goes first. He places a piece anywhere on the edge of the board, facing towards it. Next, the second player places a piece on the edge of the board. He cannot place a piece at the position where player 1 played.

### Movement phase

Once both players have played, all pieces (the just recently placed and those that were already on the board) move in the direction they are facing.

They do so in the order of the numbers that are printed on them. Pieces with number 1 move first, then pieces with number 2, etc. If there are 2 pieces with the same number on the board, the one that belongs to the player with the crown moves first.

In general, pieces cannot move if the tile they are facing towards is blocked by another piece. When it's the turn of a piece to move, and its movement is blocked, the pieces loses its turn and the next piece is evaluated.

If a piece was placed on the edge of the board and cannot enter because its movement is blocked, it is immediately returned to its owner's hand. Pieces that move outside of the board are also returned to their owner's hand.

Pieces returned to the hand this way do so right after they move (or try to move). They do **not** remain in place blocking the movement of other pieces until the other movements of that round have finished. 

#### Special movement rule: Piece type 1

The piece with number 1 moves like any other piece, but it is lightweight and can get pushed around by other pieces. If a piece would normally move to a tile blocked by Piece 1, it still moves, pushing Piece 1 back a square.

If the tile behind a piece of type 1 is blocked by another piece, then it cannot be pushed (even if there is another Piece of type 1 behind it).

A Piece of type 1 **cannot** push another Piece of type 1.

#### Special movement rule: Piece type 4

Piece with number 4, aka. the bulldozer, **always** moves when its turn arrives. Any chain of pieces in its path (including another Piece of type 4) will get pushed back when it moves.

Pieces that are pushed outside the board are returned to their owner's hand (at the very moment they are moved).

Pieces that were on the edge of the board (awaiting entry on their turn) and are pushed are also returned to their owner's hand.
They cannot be pushed beyond the first layer of the edge of the board.

### Winning condition

If after all pieces have moved one of the players has a line of 3 pieces (horizontal, vertical or diagonal) he wins the game.

If no player makes a line of 3, or if **both** players make a line of 3 at the same time, the game continues. The crown goes to the other player, and a new turn begins.

_Intermediate_ lines of 3 (that happen while there are still pieces left to move) do not grant a victory.
Only after all pieces have moved the board is evaluated for a victory condition.

