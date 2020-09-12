# Poker User DLL

`Poker User DLL` is a DLL used to take a decision for 6-max Holdem Poker, in a timely manner. The decisions it takes are based on GTO theory and personal poker experience. At the moment, it can only be used for cash games. Multiple tables are supported.

---

## How to use

`User.dll` exposes 2 functions that make decision making possible:
```C
int update_symbols(const char* psymbols);

double process_query(const char* pquery);
```

These functions are used by [ajecc/poker-table-controller](https://github.com/ajecc/poker-table-controller). You can use that if you don't want to interface directly with the DLL. 

---

### Update Symbols 

`update_symbols` takes as input the symbols that define the table's state. Each symbol must lie on its own line. The name of the symbol and its value must be separated by `:`. An example on how `psymbols` should look like, as well as all the symbols requiered, can be found below:
<details>
    <summary>Symbols</summary>

```json
c0cardface0:Ah
c0cardface1:2c
c0cardface2:Td
c0cardface3:0 
c0cardface4:0 
c0pot0:20
c0pot1:0
p0active:0
p0balance:0
p0bet:0
p0dealer:0
p0name:NOT_SEATED
p0seated:0
p1active:0
p1balance:33
p1bet:0
p1dealer:0
p1name:SEATED_BUT_NOT_ACTIVE
p1seated:1
p2active:1
p2balance:100
p2bet:3
p2cardface0:Ac
p2cardface1:Kc
p2dealer:1
p2name:HERO
p2seated:1
p3active:1
p3balance:78
p3bet:0
p3dealer:0
p3name:some_player_name
p3seated:1
p4active:0
p4balance:32
p4bet:0
p4dealer:0
p4name:just_another_name
p4seated:1
p5active:1
p5balance:33
p5bet:3
p5dealer:0
p5name:one_more_name
p5seated:1
bblind:2
```
</details>

Note that the hero is defined by `p2*` symbols.

---

### Process Query

`process_query` takes as input a string that defines a query. These strings are:
- `dll$beep`
    - the DLL takes a decision based on the symbols used in `update_symbols` 
- `dll$check`
    - returns 1 if the decision is a check and 0 otherwise
- `dll$call`
    - same as `dll$check`, but for calls
- `dll$bet`
    - same as `dll$check`, but for bets
- `dll$raise`
    - same as `dll$check`, but for raises
- `dll$betsize` 
    - the betsize (a double)
    - should be queried if `dll$bet` or `dll$raise` returned 1

---

## Project Structure

The decision taken by the DLL is based on 2 main factors:
- the program tries to guess the range of the villain based on some observations
    - `villain_range.h` tries to derive the range.
- the program then tries to take a decision based on this range
    - if the hero is in an opening position, it will bet according to how often and profitable the villain calls
    - if the hero is in a defending position, it will call/raise based on the pot odds and implied odds derived from the range of the opponent
    - the logic for this can be found in `decision.h` 

The probabilities needed to take a decision are partly precalculated. The ones that are not are easy to calculate. `hand_board_result.h` is responsible for these calculations.

The other headers implemented are to support the above headers and should not need changing.

For more info, read the [docs](https://github.com/ajecc/poker-user-dll/tree/master/docs).
