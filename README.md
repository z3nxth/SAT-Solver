# DPLL SAT Solver
### Summary

---
(WRITEUP IN PROGRESS)\
Please see [these notes](https://iftekhar.rocks/satsolver.pdf) (or try [this link](https://files.catbox.moe/csvgoz.pdf))

---

A simple C [DPLL](https://en.wikipedia.org/wiki/DPLL_algorithm) implementation of a solver for the boolean satisfiability problem (SAT). The SAT problem questions whether there exists a *sat*isfiable interpretation of a boolean formula (this program uses DIMACS [CNF](https://en.wikipedia.org/wiki/Conjunctive_normal_form) format such as:

$$(x_1 \lor x_2) \land (\neg x_1 \lor x_3) \land (\neg x_1 \lor \neg x_3)$$

The boolean satisfiability problem was the first problem to be deemed [NP-complete](https://en.wikipedia.org/wiki/NP-completeness) by 1971 in the [Cook-Levin theorem](https://en.wikipedia.org/wiki/Cook–Levin_theorem). In essence, problems which are NP-complete are collectively a set of problems that are hard to solve with quickly verifiable solutions. For instance, another NP-complete problem is Sudoko.
<br>Moreover, Cook's theorem also states that all NP-complete problems can be 'reduced' to one another. This means that this SAT solver can solve Sudoko problems (in this case, after I remember to write a parser that converts them to CNF form.) 

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
