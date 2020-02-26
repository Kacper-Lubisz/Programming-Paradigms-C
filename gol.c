#include"gol.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct LinkedList {
    char *data;
    struct LinkedList *next;
};

/**
 * This function checks if the universe pointer is controlled by this library, in other words, was read by read_in_file.
 * It does so by asserting the correctness of the magic number stored in the universe.
 * This has a 2^-32 probability of failure assuming that memory values are uniformly distributed
 */
void assert_valid_magic_number(struct universe *u) {
    if (u->magic_number != MAGIC_NUMBER) {
        fprintf(stderr, "Invalid universe, magic number doesn't match");
        exit(1);
    }
}

void read_in_file(FILE *infile, struct universe *u) {

    if (infile == NULL) {
        fprintf(stderr, "Error, file pointer is null");
        exit(1);
    } else if (u == NULL) {
        fprintf(stderr, "Error, null universe pointer");
        exit(1);
    }

    u->magic_number = MAGIC_NUMBER;

    int buffer_size = 256;
    // this was made flexible before I realised there was an upper limit to the number of columns
    // in retrospect this loading function could be simplified if I assumed an inflexible number of columns

    char temp[buffer_size];

    struct LinkedList *current_buffer = NULL;
    struct LinkedList *start_buffer = NULL;

    while (1) { // this loads the file into a linked list of buffers

        if (!fgets(temp, buffer_size, infile) || ferror(infile)) {
            break;
        }

        if (current_buffer == NULL) {
            current_buffer = malloc(sizeof(struct LinkedList));
            if (current_buffer == NULL) {
                fprintf(stderr, "Failed to allocate memory");
                exit(1);
            }
            current_buffer->data = malloc(buffer_size);
            current_buffer->next = NULL;
            if (current_buffer->data == NULL) {
                fprintf(stderr, "Failed to allocate memory");
                exit(1);
            }
            start_buffer = current_buffer;
        } else {
            struct LinkedList *next_buffer = malloc(sizeof(struct LinkedList));
            next_buffer->data = malloc(buffer_size);
            next_buffer->next = NULL;
            if (next_buffer->data == NULL) {
                fprintf(stderr, "Failed to allocate memory");
                exit(1);
            }
            current_buffer->next = next_buffer;
            current_buffer = next_buffer;
        }
        memcpy(current_buffer->data, temp, buffer_size);

    }

    u->n_columns = 0;

    current_buffer = start_buffer;
    while (current_buffer != NULL) { // this finds n_columns
        for (int i = 0; i < buffer_size; ++i) {
            char current = (current_buffer->data)[i];
            if (current == 0) {
                break;
            } else if (current == '\n' || current == EOF) {
                current_buffer = NULL;
                break;
            } else if (current == '.' || current == '*') {
                u->n_columns++;
            } else {
                fprintf(stderr, "Failed to load, unexpected character '%c'", current);
                exit(1);
            }
        }
        if (current_buffer != NULL) {
            current_buffer = current_buffer->next;
        }
    }

    if (u->n_columns > 512) {
        fprintf(stderr, "Failed to load, the number of columns must be at most 512");
        exit(1);
    }

    struct LinkedList *start_row = malloc(sizeof(struct LinkedList));
    struct LinkedList *current_row = start_row;
    start_row = current_row;
    int current_row_cursor = 0;

    if (current_row == NULL) {
        fprintf(stderr, "Failed to allocate memory");
        exit(1);
    }
    current_row->data = malloc(sizeof(u->n_columns));
    current_row->next = NULL;
    if (current_row->data == NULL) {
        fprintf(stderr, "Failed to allocate memory");
        exit(1);
    }

    u->currently_alive = 0;
    u->iterations_lived = 0;

    u->n_rows = 1;

    current_buffer = start_buffer;
    // copy all of the data from the buffer linked list to a linked list of rows
    while (current_buffer != NULL) {
        for (int i = 0; i < buffer_size; ++i) {

            char current = current_buffer->data[i];
            if (current == 0) {
                break;
            } else if (current == '\n' || current == EOF) {
                if (current_row_cursor != u->n_columns) {
                    fprintf(
                            stderr,
                            "Failed to load, irregular number of cells in row (this can be caused by whitespace before or after the board data)"
                    );
                    exit(1);
                } else {
                    current_row_cursor = 0;
                    u->n_rows++;

                    struct LinkedList *next_row = malloc(sizeof(struct LinkedList));
                    if (next_row == NULL) {
                        fprintf(stderr, "Failed to allocate memory");
                        exit(1);
                    }
                    next_row->data = malloc(u->n_columns);
                    next_row->next = NULL;
                    if (next_row->data == NULL) {
                        fprintf(stderr, "Failed to allocate memory");
                        exit(1);
                    }
                    current_row->next = next_row;
                    current_row = next_row;

                }
            } else if (current == '.' || current == '*') {
                if (current_row_cursor == u->n_columns) {
                    fprintf(
                            stderr,
                            "Failed to load, irregular number of cells in row (this can be caused by whitespace before or after the board data)"
                    );
                    exit(1);
                }
                if (current == '*') {
                    u->currently_alive++;
                }

                current_row->data[current_row_cursor] = current;
                current_row_cursor++;
            } else {
                fprintf(stderr, "Failed to load, unexpected character %c", current);
                exit(1);
            }
            if (current == EOF) {
                current_buffer = NULL;
                break;
            } else if (current == '\n') {
                break;
            }
        }
        if (current_buffer != NULL) {
            current_buffer = current_buffer->next;
        }
    }

    u->total_lived = u->currently_alive;

    if (current_row_cursor == 0) {
        u->n_rows--;

    } else if (current_row_cursor != u->n_columns) {
        fprintf(
                stderr,
                "Failed to load, irregular number of cells in row (this can be caused by whitespace before or after the board data)"
        );
        exit(1);
    }

    current_buffer = start_buffer; // free up buffers;
    while (current_buffer != NULL) {
        struct LinkedList *next = current_buffer->next;

        free(current_buffer->data);
        free(current_buffer);
        current_buffer = next;
    }

    u->grid = (char **) malloc(sizeof(char *) * u->n_rows);

    current_row = start_row;
    for (int r = 0; r < u->n_rows; ++r) {
        u->grid[r] = current_row->data;

        struct LinkedList *next_row = current_row->next;
        free(current_row);
        current_row = next_row;
    }
}

void write_out_file(FILE *outfile, struct universe *u) {

    assert_valid_magic_number(u);

    if (outfile == NULL) {
        fprintf(stderr, "Error, file pointer is null");
        exit(1);
    } else if (u == NULL) {
        fprintf(stderr, "Error, null universe pointer");
        exit(1);
    }

    for (int r = 0; r < u->n_rows; ++r) {
        fputs(u->grid[r], outfile);
        fputs("\n", outfile);
    }

}

int is_alive(struct universe *u, int column, int row) {

    assert_valid_magic_number(u);

    if (u == NULL) {
        fprintf(stderr, "Error, null universe pointer");
        exit(1);
    } else if (column >= u->n_columns || column < 0) {
        fprintf(stderr, "Column index is out of bounds");
        exit(1);
    } else if (row >= u->n_rows || row < 0) {
        fprintf(stderr, "Row index is out of bounds");
        exit(1);
    }

    //(~0 ^ 1) == 0b11111110, ignore the least significant bit
    int mask = (~0 ^ 1);

    return (u->grid[row][column] & mask) == '*';
}

int will_be_alive(struct universe *u, int column, int row) {

    assert_valid_magic_number(u);

    if (u == NULL) {
        fprintf(stderr, "Error, null universe pointer");
        exit(1);
    } else if (column >= u->n_columns || column < 0) {
        fprintf(stderr, "Column index is out of bounds");
        exit(1);
    } else if (row >= u->n_rows || row < 0) {
        fprintf(stderr, "Row index is out of bounds");
        exit(1);
    }

    int alive_neighbours = 0;

    for (int dr = -1; dr < 2; ++dr) {
        for (int dc = -1; dc < 2; ++dc) {
            if (dr == dc && dc == 0) {
                continue;
            }

            if (column + dc == -1 || row + dr == -1 || column + dc == u->n_columns || row + dr == u->n_rows) {
                // dead cells outside the universe
                continue;
            }

            if (is_alive(u, column + dc, row + dr)) {
                alive_neighbours++;
            }
        }
    }
    return alive_neighbours == 3 || (alive_neighbours == 2 && is_alive(u, column, row));
}

/**
 * An implementation of the floored division modulus
 * @param a The dividend
 * @param b The divisor
 * @return The remainder
 */
int mod(int a, int b) {
    return ((a % b) + b) % b;
}

int will_be_alive_torus(struct universe *u, int column, int row) {

    assert_valid_magic_number(u);

    if (u == NULL) {
        fprintf(stderr, "Error, null universe pointer");
        exit(1);
    } else if (column >= u->n_columns || column < 0) {
        fprintf(stderr, "Column index is out of bounds");
        exit(1);
    } else if (row >= u->n_rows || row < 0) {
        fprintf(stderr, "Row index is out of bounds");
        exit(1);
    }

    int alive_neighbours = 0;

    for (int dr = -1; dr < 2; ++dr) {
        for (int dc = -1; dc < 2; ++dc) {
            if (dr == dc && dc == 0) {
                continue;
            }

            if (is_alive(u, mod(column + dc, u->n_columns), mod(row + dr, u->n_rows))) {
                alive_neighbours++;
            }
        }
    }

    return alive_neighbours == 3 || (alive_neighbours == 2 && is_alive(u, column, row));
}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)) {

    assert_valid_magic_number(u);

    if (u == NULL) {
        fprintf(stderr, "Error, null universe pointer");
        exit(1);
    } else if (rule == NULL) {
        fprintf(stderr, "Error, null rule pointer");
        exit(1);
    }

    u->currently_alive = 0;

    for (int r = 0; r < u->n_rows; ++r) {
        for (int c = 0; c < u->n_columns; ++c) {
            int will_be_alive = (*rule)(u, c, r);
            u->grid[r][c] |= will_be_alive; // future state is stored in the significant bit
            if (will_be_alive) {
                u->currently_alive++;
            }
        }
    }

    u->total_lived += u->currently_alive;
    u->iterations_lived++;

    for (int r = 0; r < u->n_rows; ++r) {
        for (int c = 0; c < u->n_columns; ++c) {
            if (u->grid[r][c] & 1) {
                u->grid[r][c] = '*';
            } else {
                u->grid[r][c] = '.';
            }
        }
    }
}

void print_statistics(struct universe *u) {

    assert_valid_magic_number(u);

    if (u == NULL) {
        fprintf(stderr, "Error, null universe pointer");
        exit(1);
    }

    float currently_alive = 100.0 * u->currently_alive / u->n_columns / u->n_rows;
    float average_alive = 100.0 * u->total_lived / u->n_columns / u->n_rows / (u->iterations_lived + 1);

    printf("%.3f%% of  cells currently alive\n", currently_alive);
    printf("%.3f%% of  cells alive on average\n", average_alive);
}