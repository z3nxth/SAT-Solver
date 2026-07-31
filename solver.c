// Created by Iftekhar Syed (iftekhar.rocks/blog)
// refer to the pdf(2: coding it) for rough workflow
// Born to pentest forced to cs

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// removed static definition so no BOF

typedef struct {
    int numVars;
    int numClauses;
    int **formula; // 2d array for the actual formula
    int *clauseSizes; // length of clayses 1d array
} CNFProblem;

// prototypes
CNFProblem parseDIMACS(const char *filename);
void freeCNFProblem(CNFProblem *cnf);

void evalClause(int clause[], int clauseSize, int assignments[], int result[]);
int unitPropagation(int **formula, int clauseSizes[], int numClauses, int assignments[], int changes[], int *changeCount);
int solve(int **formula, int clauseSizes[], int numClauses, int numVars, int assignments[]);

int main(int argc, char** argv)
{
    const char *filename;

    if (argc > 1) {
        filename = argv[1];
    } else {
        fprintf(stderr, "format: %s <input.cnf>", argv[0]);
        return 1;
    }

    printf("loading DIMACS filee: %s...\n", filename);
    CNFProblem cnf = parseDIMACS(filename);

    printf("Great. Formula loaded with %d variables and %d clauses.\n", cnf.numVars, cnf.numClauses);

    // prepare assignments array (2 for unassigned.)
    int *assignments = malloc((cnf.numVars + 1) * sizeof(int));
    for (int i = 0; i <= cnf.numVars; i++) {
        assignments[i] = 2;
    }

    printf("SOLVING...\n");
    int status = solve(cnf.formula, cnf.clauseSizes, cnf.numClauses, cnf.numVars, assignments);

    if (status == 1) {
        printf("\nResult: SAT with variable assignments: \n"); // yay
        for (int i = 1; i <= cnf.numVars; i++) {
            printf("  x%d = %d\n", i, assignments[i]);
        }
    } else {
        printf("\nResult: UNSAT.\n"); // aww
    }

    // clean stuff up
    free(assignments);
    assignments = NULL;
    freeCNFProblem(&cnf);

    return 0;
}

CNFProblem parseDIMACS(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("my condolences i failed in opening the file");
        exit(EXIT_FAILURE);
    }

    CNFProblem cnf;
    cnf.numVars = 0;
    cnf.numClauses = 0;

    char line[1024];

    // α) parse the headers (p ... or c ... for comment)
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'c') continue;
        if (line[0] == 'p') {
            sscanf(line, "p cnf %d %d", &cnf.numVars, &cnf.numClauses);
            break;
        }
    }

    // β) allocation and tracking
    cnf.formula = malloc(cnf.numClauses * sizeof(int *));
    cnf.clauseSizes = malloc(cnf.numClauses * sizeof(int));

    int *capacities = malloc(cnf.numClauses * sizeof(int));

    for (int i = 0; i < cnf.numClauses; i++) {
        capacities[i] = 8; // Default starting size
        cnf.formula[i] = malloc(capacities[i] * sizeof(int));
        cnf.clauseSizes[i] = 0;
    }

    // γ) read literals line by line to handle comments gracefully
    int currentClause = 0;

    while (currentClause < cnf.numClauses && fgets(line, sizeof(line), file)) {
        if (line[0] == 'c' || line[0] == '\n' || line[0] == '\r') continue;

        char *ptr = line;
        int literal;
        int charsRead;

        while (sscanf(ptr, "%d%n", &literal, &charsRead) == 1) {
            ptr += charsRead;

            if (literal == 0) {
                currentClause++; // Clause complete
                if (currentClause >= cnf.numClauses) break;
            } else {
                int size = cnf.clauseSizes[currentClause];

                // Expand buffer if current clause fills its capacity though this prlly wont be needed
                if (size >= capacities[currentClause]) {
                    capacities[currentClause] *= 2;
                    cnf.formula[currentClause] = realloc(cnf.formula[currentClause],
                                                         capacities[currentClause] * sizeof(int));
                }

                cnf.formula[currentClause][size] = literal;
                cnf.clauseSizes[currentClause]++;
            }
        }
    }

    free(capacities);
    fclose(file);
    return cnf;
}

void freeCNFProblem(CNFProblem *cnf) {
    for (int i = 0; i < cnf->numClauses; i++) {
        free(cnf->formula[i]);
    }
    free(cnf->formula);
    free(cnf->clauseSizes);
} // free my boy

void evalClause(int clause[], int clauseSize, int assignments[], int result[]) //result[0] is SAT/UNSAT/UNIT/UNDEC and [1] is the unit lit if there is one lol
{
    int unassigned = 0;
    int lastUnassigned = 0;

    for (int i = 0; i < clauseSize; i++)
    {
        int lit = clause[i];
        int varID = abs(lit);
        int varValue = assignments[varID];

        if ((lit > 0 && varValue == 1) || (lit < 0 && varValue == 0))
        {
            result[0] = 1; // SAT yay
            result[1] = 0; // theres no unit lit
            return;
        }
        else if (varValue == 2) {
            unassigned++;
            lastUnassigned = lit;
        }
    }

    if (unassigned == 0) {
        result[0] = 0; // conflict so UNSAT
        result[1] = 0;
    }
    else if (unassigned == 1) {
        result[0] = 3; // UNIT
        result[1] = lastUnassigned;
    }
    else {
        result[0] = 2; // Undeicded
        result[1] = 0;
    }
}

int unitPropagation(int **formula, int clauseSizes[], int numClauses, int assignments[], int changes[], int *changeCount)
{
    int changed = 1;

    while (changed)
    {
        changed = 0;

        for (int i = 0; i < numClauses; i++)
        {
            int result[2];
            evalClause(formula[i], clauseSizes[i], assignments, result);

            if (result[0] == 0) {
                return 0; // as in theres a conflict
            }
            else if (result[0] == 3) {
                int forcedLit = result[1];
                int varID = abs(forcedLit);

                assignments[varID] = (forcedLit < 0) ? 0 : 1;
                changes[(*changeCount)++] = varID;

                changed = 1;
                break;
            }
        }
    }

    return 1; // yay
}

int solve(int **formula, int clauseSizes[], int numClauses, int numVars, int assignments[])
{
    int *changes = malloc((numVars + 1) * sizeof(int));
    int changeCount = 0;

    int status = unitPropagation(formula, clauseSizes, numClauses, assignments, changes, &changeCount);
    if (status == 0) {
        for (int i = 0; i < changeCount; i++) {
            assignments[changes[i]] = 2;
        }
        free(changes);
        return 0; // this branch hsa conflicts [unsat :( ]
    }

    int varID = -1;
    for (int i = 1; i <= numVars; i++) {
        if (assignments[i] == 2) {
            varID = i;
            break;
        }
    }

    // Base Case: all variables assigned with no conflicts :))
    if (varID == -1) {
        free(changes);
        return 1;
    }

    // Left branch, affirm x_varID
    assignments[varID] = 1;
    if (solve(formula, clauseSizes, numClauses, numVars, assignments)) { // if it can be solved..
        free(changes);
        return 1;
    }

    // Right branch, negate x_varID
    assignments[varID] = 0;
    if (solve(formula, clauseSizes, numClauses, numVars, assignments)) { // same as above lol
        free(changes);
        return 1;
    }

    assignments[varID] = 2;
    for (int i = 0; i < changeCount; i++) {
        assignments[changes[i]] = 2;
    }

    free(changes);
    return 0;
}

// Genuinely wrote most of this at 3am
