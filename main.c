/*
 * main.c  –  Mini Student Shell
 *
 * TODO: Implement admin_shell and client_shell.
 *
 * Build:
 *   make admin   →  admin_shell  (compiled with -DADMIN_MODE)
 *   make client  →  client_shell (compiled with -DCLIENT_MODE)
 *
 * Usage:
 *   ./admin_shell [students.csv]
 *   ./admin_shell -f commands.txt [students.csv]
 *   ./client_shell [students.csv]
 *   ./client_shell -f commands.txt [students.csv]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "student.h"
#include "file_io.h"
#include "command.h"

/* ---------------------------------------------------------------
 * TODO: Implement the interactive shell loop.
 *   - Print a prompt and read a line from stdin.
 *   - Parse the line into a command and arguments.
 *   - Dispatch to the appropriate handler function.
 *   - Loop until the user types "exit" or EOF.
 * --------------------------------------------------------------- */
void run_shell(Student **head, const char *csv_path) {
    char line[256];

    while (1) {
#ifdef ADMIN_MODE
        printf("admin> ");
#else
        printf("client> ");
#endif
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0')
            continue;

        ShellResult result = dispatch_command(line, head, csv_path);
        if (result == SHELL_EXIT)
            break;
    }
}

void run_command_file(const char *cmd_file, Student **head, const char *csv_path) {
    FILE *fp = fopen(cmd_file, "r");
    if (fp == NULL) {
        printf("Error: cannot open command file %s.\n", cmd_file);
        run_shell(head, csv_path);
        return;
    }

    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        line_num++;

        if (line[0] == '\0' || line[0] == '#')
            continue;

        printf("[command file:%d] %s\n", line_num, line);
        ShellResult result = dispatch_command(line, head, csv_path);

        if (result == SHELL_EXIT)
            break;
        if (result != SHELL_OK)
            printf("Skipped line %d.\n", line_num);
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    const char *csv_path  = "students.csv"; /* default CSV file */
    const char *cmd_file  = NULL;           /* -f <file> argument */

    if (argc < 2) {
#ifdef ADMIN_MODE
        printf("Usage: ./admin_shell <csv_file> [-f command_file]\n");
#else
        printf("Usage: ./client_shell <csv_file> [-f command_file]\n");
#endif
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            cmd_file = argv[++i];
        } else {
            csv_path = argv[i];
        }
    }

    Student *head = NULL;
    set_csv_path(csv_path);

#ifdef ADMIN_MODE
    printf("[Admin Program]\n");
    int loaded = load_csv(csv_path, &head);
    if (loaded >= 0)
        printf("Loaded %d students from %s.\n", loaded, csv_path);

    if (cmd_file) {
        run_command_file(cmd_file, &head, csv_path);
    } else {
        run_shell(&head, csv_path);
    }

    free_list(&head);

#elif defined(CLIENT_MODE)
    printf("[Client Program]\n");
    int loaded = load_csv(csv_path, &head);
    if (loaded >= 0)
        printf("Loaded %d students from %s.\n", loaded, csv_path);

    if (cmd_file) {
        run_command_file(cmd_file, &head, csv_path);
    } else {
        run_shell(&head, csv_path);
    }

    free_list(&head);

#else
#error "Define either -DADMIN_MODE or -DCLIENT_MODE when compiling."
#endif

    return 0;
}
