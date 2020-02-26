#ifndef gol_h
#define gol_h

#include <stdbool.h>
#include <stdio.h>

#define MAGIC_NUMBER 2193512351

struct universe {
    int n_rows;
    int n_columns;

    int total_lived;
    int currently_alive;
    int iterations_lived;

    char **grid;

    unsigned long int magic_number;
};

/*Do not modify the next seven lines*/

/** todo fixme remove the hints
 * Reads in the file from the file pointer infile and stores the universe in the structure pointed to by u.
 * You must use dynamic memory allocation to set aside memory for storing the universe.
 * (Hint: how will you work out how many cells are in each row? You may want to fscanf() with %c.
 * Recall that fscanf() returns EOF when the file ends. Press Ctrl+D to send EOF if typing input in.)
 *
 * @param infile The file to be read from
 * @param u Pointer to the universe to read into
 */
void read_in_file(FILE *infile, struct universe *u);

/**
 * Writes the content of the universe pointed to by u into the file from the file pointer outfile.
 *
 * @param outfile The file to write the universe to
 * @param u Pointer to the universe to be written
 */
void write_out_file(FILE *outfile, struct universe *u);

/**
 * Returns if the specified cells is alive
 *
 * @param u The universe to specify the cell
 * @param column used to specify the cell in the universe
 * @param row used to specify the cell in the universe
 * @return 1 if the cell alive and 0 otherwise
 */
int is_alive(struct universe *u, int column, int row);

/**
 * Returns if the specified cell will be alive in the next generation, given the assumption that the cells adjacent to
 * the edges of the universe are always dead.
 *
 * @param u The universe to specify the cell
 * @param column used to specify the cell in the universe
 * @param row used to specify the cell in the universe
 * @return if the cell will be alive in the next iteration
 */
int will_be_alive(struct universe *u, int column, int row);

/**
 * Returns if the specified cell will be alive in the next generation, given the universe has a toroidal topology
 * (The column left of the left most column is the right most column etc.)
 *
 * @param u The universe to specify the cell
 * @param column used to specify the cell in the universe
 * @param row used to specify the cell in the universe
 * @return if the cell will be alive in the next iteration
 */
int will_be_alive_torus(struct universe *u, int column, int row);

/**
 * Changes the universe from the current generation to the next generation.
 * It uses the function pointed to by the function pointer rule to decide whether a cell will be alive or dead,
 * e.g. will_be_alive or will_be_alive_torus
 *
 * @param u The universe to evolve
 * @param rule The function which returns if a cell is alive in the next generation
 */
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row));

/**
 * Calculates the percentage of cells that are alive in the current generation and the average percentage that have been
 * alive in all of the generations so far (including the original generation).
 * Percentages are given to three decimal places, and printed to the screen in the following format:
 *  32.000% of cells currently alive
 *  10.667% of cells alive on average
 *
 * @param u The universe to print statistics of
 */
void print_statistics(struct universe *u);

/*You can modify after this line again*/

#endif