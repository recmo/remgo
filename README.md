# RemGo


## Rules of Go

The online competition server use what is known as *chinese rules* with
*positional superko*.

https://en.wikipedia.org/wiki/Rules_of_go

### Board

Go is a two player game played on a *board* of *N × N* *positions*.
*N ∊ {9, 13, 19}*, the *N < 19* variants are simplified versions. Proper Go has
a *19 × 19* grid.

Two positions are *adjacent* if they are horizontally or vertically neighbors.

### Positions

Each *position* on the board is always in one of three states:
*{Empty, Black, White}*.

Two positions of the same state are *connected* when there is a path of
adjacent positions of the same state.

A *liberty* of a non-empty position is an empty position adjacent to a connected
position.

### Play

Initially all positions in the board are empty.

Black moves first. After that the players take turns.

A player may *pass* or *play*. If the player *plays*, the following is done in
order:

1. Change an empty position to the player colour, subject to two rules below.
2. Change all positions in the opponent colour that have no liberties to empty.
3. Change all positions in the player colour that have no liberties to empty.

Addendum 1: *Prohibition of suicide* is an optional rule forbids any position
in step one that will result in a non-empty set of positions in step three.

Addendum 2: *Prohibition of repetition*: any position in step one that, after
steps two and three, results in a board state that was the result of an earlier
play is illegal.

### End

The game ends when both players have passed consecutively. The *final* board
state is used to calculate player scores and determine the winner.

1. All stones are removed that players agree are inevitably captured if the
   game continued. On dispute the game continues.
2. An empty position belongs to a players *territory* when all stones adjacent
   to the postion or connected empty positions are of the players colour.
3. The *area* of a player is the union of its stones and its territory.
4. A players *score* is the number of positions in its area.

The winner is the player with the highest score if there is one, or else the
game is a draw.

## Sources

* http://www.cs.cmu.edu/~wjh/go/rules/AGA.html
* https://en.wikipedia.org/wiki/Rules_of_go#Reference_statement

## Protocol

[GTP — Go Text Protocol version 2](http://www.lysator.liu.se/~gunnar/gtp/gtp2-spec-draft2/gtp2-spec.html)


## Testing

Using the `twogtp.py` script:

> ./twogtp.py --white 'gnugo --mode gtp' --black 'gnugo --mode gtp' --verbose 2



## Online competitions

* http://cgos.boardspace.net/

* http://www.computer-go.info/events/future.html

* http://www.gokgs.com/


