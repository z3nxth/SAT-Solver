# DPLL SAT Solver

[rough notes](https://iftekhar.rocks/satsolver.pdf) (or try [this link](https://files.catbox.moe/csvgoz.pdf))

### What is a SAT solver?
In essence, a SAT (boolean satisfiability problem) solver attempts to find solutions to a boolean formula. For instance, an example formula could be *<u>A</u> **and** **not** <u>B</u>*, for instance, where the formula is true when A is true **and** B is false.<br>
In order to SAT solve, input of a formula has to be provided in a format called [DIMACS CNF (Conjunctive Normal Form)](https://en.wikipedia.org/wiki/Conjunctive_normal_form). All boolean formulas may be represented in this format. An example formula in CNF could be:

$$\Phi = (x_1 \lor x_2) \land (\neg x_1 \lor x_3) \land (\neg x_1 \lor \neg x_3)$$

Notice that the formula $\Phi$ is represented by three clauses, that must **all** be true (represented by $\land$) in order for the formula to be true. In turn, each clause is made true when at minimum one literal (such as $\neg x_1$) is true. In essence, at least one literal from every clause is necessarily true if formula $\Phi$ is to be true.


### Usage
Compile:
```bash
gcc solver.c -o solver
```
Run:
```bash
./solver.c <input.cnf>
```
Example DIMACS cnf (this formula is equivalent to the one above in the summary):
```plain
c Example cnf. An example solution is 1F 2T 3A where A is ambiguous.
p cnf 3 3
1 2 0
-1 3 0
-1 -3 0
```
Result with solver:
```bash
$ ./solver formula.cnf                                       ─╯

loading dimacs/cnf filee formula.cnf...
Great, formula loaded with 3 variables and 3 clauses.
SOLVING...

Result: SAT with variable assignments:
  x1 = 0
  x2 = 1
  x3 = 1
```


### Why are SAT solvers interesting?
SAT solvers, as previously mentioned, solve the boolean satisfiability problem. This problem was the first to be proven to be NP-Complete (**NP-c**) in the [Cook-Levin theorem](https://en.wikipedia.org/wiki/Cook–Levin_theorem). This is special because it means that all problems in the set [NP](https://en.wikipedia.org/wiki/NP_(complexity)) (all decision problems solvable or verifiable in polynomial time) are **at most as difficult to solve as the SAT problem.** <br>
### How SAT solvers can practically solve sudoku, Rubik's Cubes and more
**Further, this means that all problems in the set NP-c can be "reduced", or converted to the SAT problem and solved!** Examples include (with links to references):
* [Sudoku](https://fse.studenttheses.ub.rug.nl/22745/1/bMATH_2020_HoexumES.pdf.pdf)
* [Minesweeper](https://doi.org/10.1007/BF03025367)
* An $n \times n \times n$ [Rubik's Cube](https://arxiv.org/pdf/1706.06708)
* [Battleships](https://www.mountainvistasoft.com/docs/BattleshipsAsDecidabilityProblem.pdf)
* [Kakuro](https://mediatum.ub.tum.de/doc/1287480/document.pdf)
* [Tetris](https://erikdemaine.org/papers/Tetris_TR2002/paper.pdf)

> [!NOTE]
> I'm currently attempting to write programs that reduce Sudoku and the $n \times n \times n$ Rubik's Cube to CNF after implementing CDCL (more on that below).

### About the algorithm
This is a [DPLL](https://en.wikipedia.org/wiki/DPLL_algorithm) implementation of a solver written in C. The Davis–Putnam–Logemann–Loveland (DPLL) algorithm is described as a "complete, complete, backtracking-based search algorithm for deciding the satisfiability of propositional logic formulae in conjunctive normal form, i.e. for solving the CNF-SAT problem." by [Wikipedia](https://en.wikipedia.org/wiki/DPLL_algorithm). If there were a formula $\Phi$, DPLL would attempt to solve it by:
* First scanning $\Phi$ for any _unit clauses_. A clause is unit where there is **exactly one** unassigned literal (namely that has not been set to TRUE or FALSE by the algorithm.)
* Unit literals necessarily would be set to TRUE in order to satisfy the formula (as if one lit is true, the clause is necessarily true.)
* Then, $\Phi$ would be scanned for _pure literals_. These are literals that **have a constant polarity** (they are either always affirmed or always negated in the formula.) For instance, in the formula $\Phi = (\neg x_1 \lor x_2) \land (\neg x_1 \lor x_3) \land (\neg x_1 \lor \neg x_3)$ mentioned in the beginning, the literal $\neg x_1$ is pure.
* Pure literals would then be set to TRUE because there would not be any conflict anywhere (for instance, setting $\neg x_1$ TRUE would make all clauses it is in true because (evidently) it is true for all clauses it is in.
* Next, $\Phi$ would be checked to see if it was empty (no remaining clauses). If it was, the problem is SAT (solved) and finished. Else if empty clauses were there, it would be UNSAT (not satisfied currently).
* In the final stage, a literal $L$ would be chosen. There are two possibilities; either $L$ is affirmed, or it is negated. DPLL will now recursively affirm or negate $L$ such that the formula would be TRUE. Results would be OR-ed of both branches and the problem would therefore be SAT.

### CDCL (Conflict-Driven Clause Learning)
Whilst DPLL is a modest algorithm, CDCL is its better in many ways. There are a few problems that DPLL fails to address. For instance, *nothing is learnt* from conflicts other than the current assignment (so only one permutation is known to be UNSAT by DPLL). In addition, backtracking is circumscribed to a single level such that the algorithm may continue to explore a space that is inevitably UNSAT. <br>
CDCL offers many improvements.

(writing cdcl)

### References
[University of Washington | CSE 442: Conflict-Driven Clause Learning](https://cse442-17f.github.io/Conflict-Driven-Clause-Learning/) was very useful trying to get my head around CNFSAT and DPLL/CDCL.




