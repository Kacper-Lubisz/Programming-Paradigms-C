#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"gol.h"

enum ArgumentParserStates {
    AWAIT_NEXT,
    INPUT_FILE,
    OUTPUT_FILE,
    GENERATION_NUMBER,
};

/**
 * -i input_filename to specify that the initial generation of the universe should be read from a file. If this option is not specified, you should let the user type in the input. [4 marks]
 * -o output_filename to specify a file into which the final generation should be output. If this option is not specified, you should output the final generation on the screen. [4 marks]
 * -g number_of_generations to specify the number of new generations for which the game should be run. (Set to 5 if this option is not given.) [4 marks]
 * -s to print statistics after the final generation has been output. [4 marks]
 * -t to use the torus topology for the rule.  If this is not specified, use the rule for cells outside the universe being permanently dead. [4 marks]
 *
 * @param argc The number of command-line arguments
 * @param argv The array of command line arguments
 * @return the programs exit status
 */
int main(int argc, char **argv) {

    char *in_path = NULL;
    char *out_path = NULL;

    int number_of_generations = -1;
    int print_stats = 0;
    int use_torus_topology = 0;

    enum ArgumentParserStates parser_state = AWAIT_NEXT;
    for (int a = 1; a < argc; ++a) {
        if (parser_state == AWAIT_NEXT) {
            if (strcmp("-i", argv[a]) == 0) {
                parser_state = INPUT_FILE;
            } else if (strcmp("-o", argv[a]) == 0) {
                parser_state = OUTPUT_FILE;
            } else if (strcmp("-g", argv[a]) == 0) {
                parser_state = GENERATION_NUMBER;
            } else if (strcmp("-s", argv[a]) == 0) {
                print_stats = 1;
            } else if (strcmp("-t", argv[a]) == 0) {
                use_torus_topology = 1;
            } else if (strcmp("-help", argv[a]) == 0) {
                printf("Usage: gameoflife [options]\n"
                       "Options:"
                       "\n  -help\t\tDisplays the usage message"
                       "\n  -i <file>\tSpecify the input file, otherwise stdin"
                       "\n  -o <file>\tSpecify the output file, otherwise stdout"
                       "\n  -g <number>\tSpecify the number of generations to be ran"
                       "\n  -s\t\tPrint statistics to stdout after simulating"
                       "\n  -t\t\tUse torus topology, otherwise cells outside the universe are permanently dead"
                );
                exit(0);
            } else {
                fprintf(stderr, "Invalid command '%s', see 'gameoflife -help'", argv[a]);
                exit(1);
            }
        } else if (parser_state == INPUT_FILE) {

            if (in_path != NULL && strcmp(argv[a], in_path) != 0) {
                fprintf(stderr, "Invalid arguments, input path is passed twice and is contradicting");
                exit(1);
            }

            parser_state = AWAIT_NEXT;
            in_path = argv[a];

        } else if (parser_state == OUTPUT_FILE) {
            parser_state = AWAIT_NEXT;
            out_path = argv[a];


            if (out_path != NULL && strcmp(argv[a], out_path) != 0) {
                fprintf(stderr, "Invalid arguments, output path is passed twice and is contradicting");
                exit(1);
            }

        } else {//parser_state == GENERATION_NUMBER
            parser_state = AWAIT_NEXT;
            int new_number = atoi(argv[a]);

            if (new_number < 0) {
                fprintf(stderr, "Invalid number of generations, must be at least 0");
                exit(1);
            } else if (number_of_generations != -1 && number_of_generations != new_number) {
                fprintf(stderr, "Invalid arguments, number of generations is passed twice and is contradicting");
                exit(1);
            }

            number_of_generations = new_number;

        }
    }

    if (parser_state != AWAIT_NEXT) {
        fprintf(stderr, "Last argument flag is missing a value");
        exit(1);
    }

    // set the default values
    if (number_of_generations == -1) {
        number_of_generations = 5;
    }

    struct universe v;

    FILE *in_file;
    if (in_path != NULL) {
        in_file = fopen(in_path, "r");
        if (in_file == NULL) {
            fprintf(stderr, "Failed to open input file");
            exit(1);
        }
    } else {
        in_file = stdin;
    }

    read_in_file(in_file, &v);
    fclose(in_file);

    int (*rule)(struct universe *u, int column, int row);
    if (use_torus_topology) {
        rule = will_be_alive_torus;
    } else {
        rule = will_be_alive;
    }

    for (int i = 0; i < number_of_generations; ++i) {
        evolve(&v, rule);
    }

    if (print_stats) {
        print_statistics(&v);
    }

    FILE *out_file;
    if (out_path != NULL) {
        out_file = fopen(out_path, "w");
        if (out_file == NULL) {
            fprintf(stderr, "Failed to open output file");
            exit(1);
        }
    } else {
        out_file = stdout;
    }
    write_out_file(out_file, &v);
    fclose(out_file);

    return 0;

}
