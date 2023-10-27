------------------------------ MODULE CaveCell ------------------------------
(***************************************************************************)
(* This specification specifies states of the cells in the cave.           *)
(***************************************************************************)

EXTENDS FiniteSets, Integers

CONSTANT Cells              \* Represents the set of all cells that are already
                            \* in the cave or may enter the cave as sand cells.

VARIABLES SpawnCell,        \* Cell that represents the spawn point.
          SpawnWithSand,    \* Cell that represents the sand blocking the spawn.
          SandCells,        \* Set of all sand cells that are in the cave minus any
                            \* blocking the spawn point.
          Falling,          \* Sand cell that is falling.
          Sliding           \* Sand cell that is sliding left or right.
vars == << SpawnCell, SpawnWithSand, SandCells, Falling, Sliding >>

TypeOK ==
    /\ SpawnCell \subseteq Cells
    /\ SpawnWithSand \subseteq Cells
    /\ SandCells \subseteq Cells
    /\ Falling \subseteq (SpawnWithSand \cup SandCells)
    /\ Sliding \subseteq (SpawnWithSand \cup SandCells)

Invariants ==
    /\ Cardinality(SpawnCell \cup SpawnWithSand) = 1
    \* Only one spawn cell
    /\ Cardinality(Falling \cup Sliding) <= 1
    \* Only one active sand cell at a time.
    /\ Falling \cup Sliding \subseteq SandCells \cup SpawnWithSand
    \* An active cell is always a sand cell.
    /\ Falling \cap Sliding = {}
    \* An active sand cell must be in exactly one sand cell state.

MaterialCells == SpawnCell \cup SpawnWithSand \cup SandCells
(***************************************************************************)
(* Represents the cells that have materialised and is part of the cave.    *)
(***************************************************************************)

GetNewCell == { CHOOSE x \in Cells \ MaterialCells: TRUE }
(***************************************************************************)
(* Gets a new cell materialised in the cave.                               *)
(***************************************************************************)
-----------------------------------------------------------------------------
SpawnSandCell ==
(***************************************************************************)
(* Spawns a new sand cell at the spawn point.                              *)
(***************************************************************************)
    /\ SpawnCell # {}
    /\ SpawnWithSand = {}
    /\ Falling \cup Sliding = {}    \* No active sand cell
    /\ SpawnWithSand' = SpawnCell
    /\ SpawnCell' = {}
    /\ Falling' = SpawnCell
    /\ UNCHANGED << SandCells, Sliding >>

SeparateSandCellFromSpawn ==
(***************************************************************************)
(* Separates the spawned sand cell from the spawn.  This action requires   *)
(* that nothing is blocking the sand cell from falling or sliding.         *)
(***************************************************************************)
    /\ Cells # MaterialCells        \* Limited to total available cells
    /\ SpawnCell = {}
    /\ SpawnWithSand # {}
    /\ Falling \cup Sliding # {}    \* Represents the sand isn't at rest.  If
                                    \* it was, it'd mean blocked spawn cell.
    /\ SpawnCell' = SpawnWithSand
    /\ SpawnWithSand' = {}
    /\ SandCells' = SandCells \cup GetNewCell
    /\ Falling' = SandCells' \ SandCells
    /\ Sliding' = {}

MakeSandCellFall ==
(***************************************************************************)
(* Sand cell starts falling.                                               *)
(***************************************************************************)
    /\ Sliding # {}
    /\ Sliding' = {}
    /\ Falling' = Sliding
    /\ UNCHANGED << SpawnCell, SpawnWithSand, SandCells >>

MakeSandCellSlide ==
(***************************************************************************)
(* Sand cell starts sliding.                                               *)
(***************************************************************************)
    /\ Falling # {}
    /\ Falling' = {}
    /\ Sliding' = Falling
    /\ UNCHANGED << SpawnCell, SpawnWithSand, SandCells >>

MakeSandCellRest ==
(***************************************************************************)
(* Sand cell comes to rest.                                                *)
(***************************************************************************)
    /\ Falling \cup Sliding # {}
    /\ Falling' = {}
    /\ Sliding' = {}
    /\ UNCHANGED << SpawnCell, SpawnWithSand, SandCells >>

DestroySandCell ==
(***************************************************************************)
(* A sand cell falls off the bottomless cave.                              *)
(***************************************************************************)
    /\ Falling # {}
    /\ Falling' = {}
    /\ SandCells' = SandCells \ Falling
    /\ SpawnCell' = SpawnCell \cup SpawnWithSand
    /\ SpawnWithSand' = {}
    /\ UNCHANGED << Sliding >>

NoActiveCells ==
    /\ Falling = {}
    /\ Sliding = {}
    /\ UNCHANGED vars

CellComesToRest ==
(***************************************************************************)
(* Eventually an active sand cell should come to rest.                     *)
(***************************************************************************)
    /\ Falling \cup Sliding # {}
    /\ Falling' = {}
    /\ Sliding' = {}
-----------------------------------------------------------------------------
Init == /\ SpawnCell \in { x \in SUBSET(Cells): Cardinality(x) = 1 }
        /\ SpawnWithSand = {}
        /\ SandCells = {}
        /\ Falling = {}
        /\ Sliding = {}

Next == \/ SpawnSandCell
        \/ SeparateSandCellFromSpawn
        \/ MakeSandCellFall
        \/ MakeSandCellSlide
        \/ MakeSandCellRest
        \/ DestroySandCell
        \/ NoActiveCells

Spec == Init /\ [][Next]_vars /\ WF_vars(CellComesToRest)
=============================================================================
\* Modification History
\* Last modified Sun Aug 27 21:33:55 KST 2023 by hcs
\* Created Thu Aug 24 11:54:19 KST 2023 by hcs
