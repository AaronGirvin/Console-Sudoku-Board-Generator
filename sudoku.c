/*
Sudoku game - Aaron Girvin 29/03/2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX 9
#define SUBSIZE 3

int board[MAX][MAX];

const int SET[MAX] = {1,2,3,4,5,6,7,8,9};

int indices[MAX * MAX];

int sol;

void printBoard() {
    printf("=====================\n");
    for (size_t i = 0; i < MAX; i++)
    {
        printf("| ");
        for (size_t j = 0; j < MAX; j++)
        {
            if (board[i][j]) {
                printf("%d ", board[i][j]);
            }
            else {
                printf("  ");
            }
            
        }
        printf("|");
        printf("\n");
    }
    printf("=====================\n");
    printf("\n");
}

int checkMove(int x, int y, int n) {
    int i,j;
    
    //check row
    for (j = 0; j < MAX; j++)
    {
        if (j != y) {
            if (board[x][j] == n) {
                return 0;
            }
        }
    }

    //check column
    for (i = 0; i < MAX; i++)
    {
        if (i != x) {
            if (board[i][y] == n) {
                return 0;
            }
        }
    }

    // Check if number already exists in the 3x3 subgrid
    int subgrid_row = x - (x % SUBSIZE);
    int subgrid_col = y - (y % SUBSIZE);
    for (i = subgrid_row; i < subgrid_row + SUBSIZE; i++) {
        for (j = subgrid_col; j < subgrid_col + SUBSIZE; j++) {
            if (!(i == x && j == y)) {
                if (board[i][j] == n) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void shuffle(int * array, int size) {
    int r = 0;
    for (size_t i = 0; i < size; i++)
    {
        do { r = (rand() % size); } while (i == r);
        int temp = array[i];
        array[i] = array[r];
        array[r] = temp;
    }
}

int * findNextSpace(int * output) {
    output = malloc(sizeof(int) * 2);
    for (size_t i = 0; i < MAX; i++)
    {
        for (size_t j = 0; j < MAX; j++)
        {
            if (board[i][j] == 0) {
                output[0] = i;
                output[1] = j;
                return output;
            }
        }
    }
    //there are no more spaces
    free(output);
    return NULL;
}

int solveBoard() {
    //function will solve board for the one unique solution
    //board array will be modified so the result is the completed board
    int * coord = findNextSpace(coord);

    if (coord == NULL) {
        //we have filled the board
        return 1;
    }

    int x = coord[0];
    int y = coord[1];

    free(coord);

    for (size_t i = 0; i < MAX; i++)
    {
        //try each digit
        board[x][y] = SET[i];
        if (checkMove(x, y, SET[i])) {
            if(solveBoard()) {
                //this subgraph works
                return 1;
            }
        }
    }
    board[x][y] = 0;
    return 0;
}

int tryBoard() {
    //will attempt to solve board to see if there is atleast one solution
    //once founnd will backtrack and revert board to previous state

    int * coord = findNextSpace(coord);

    if (coord == NULL) {
        //we have filled the board
        return 1;
    }

    int x = coord[0];
    int y = coord[1];

    free(coord);

    for (size_t i = 0; i < MAX; i++)
    {
        
        //try each digit
        board[x][y] = SET[i];
        if (checkMove(x, y, SET[i])) {
            if(tryBoard()) {
                //this subgraph works
                board[x][y] = 0;
                return 1;
            }
        }
    }
    board[x][y] = 0;
    return 0;
}

void solveAllBoard() {
    //will solve board by full search finding the total number of possible solutions
    //will backtrack and remove changes

    int * coord = findNextSpace(coord);

    if (coord == NULL) {
        //we have filled the board
        sol++;
        return;
    }

    int x = coord[0];
    int y = coord[1];

    free(coord);

    for (size_t i = 0; i < MAX; i++)
    {
        //try each digit
        board[x][y] = SET[i];
        if (checkMove(x, y, SET[i])) {
            solveAllBoard();
        }
    }
    
    //this allows us to reset the board on the backtrack
    board[x][y] = 0;
    return;
}

void threaded_solveAllBoard() {
    //threaded implementation will use threads for the 9 trees at the start of every full search
    int * coord = findNextSpace(coord);

    if (coord == NULL) {
        //we have filled the board
        sol++;
        return;
    }

    int x = coord[0];
    int y = coord[1];

    free(coord);

    #pragma omp parallel for
    for (size_t i = 0; i < MAX; i++)
    {
        //try each digit
        board[x][y] = SET[i];
        if (checkMove(x, y, SET[i])) {
            solveAllBoard();
        }
    }
    
    //this allows us to reset the board on the backtrack
    board[x][y] = 0;
    return;
}

void genPuzzleBoard(int total) {
    //take indices array and convert to coord
    //remove the number at this location
    //try to solve for a unique solution
    //repeat

    for (size_t i = 0; i < total; i++)
    {
        int index = indices[i];
        int x = index / MAX;
        int y = index % MAX;

        int temp = board[x][y];
        board[x][y] = 0;
        sol = 0;
        solveAllBoard();
        //threaded_solveAllBoard();
        if (!(sol == 1)) {
            //no solution or no unique solution
            board[x][y] = temp;
        }
    }
    printBoard();
}

void init_board() {
    int i, j;
    for (i = 0; i < MAX; i++) {
        for (j = 0; j < MAX; j++) {
            //set default board values
            board[i][j] = 0;
            //fill the indices array
            indices[(i * MAX) + j] = (i * MAX) + j;
        }
    }
    shuffle(indices, MAX * MAX);
}

void genFullBoard() {
    //generate MAX * MAX board full of numbers

    int set[MAX] = {1,2,3,4,5,6,7,8,9};
    
    init_board();
    

    for (size_t i = 0; i < (MAX * MAX); i++)
    {
        shuffle(set, MAX);

        int index = indices[i];
        int x = index / MAX;
        int y = index % MAX;

        for (size_t j = 0; j < MAX; j++)
        {
            board[x][y] = set[j];
            if (checkMove(x, y, set[j])) {
                if (tryBoard()) {
                    break;
                }
            }
        }
    }
    
    printBoard();
}

void main() {
    srand(time(NULL));
    genFullBoard();
    genPuzzleBoard(81);
    solveBoard();
    printBoard();
}
    