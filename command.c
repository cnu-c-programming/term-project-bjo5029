#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "file_io.h"

ShellResult handle_list(char *args, Student **head) {
    (void)args;
    if (*head == NULL) {
        printf("No students found.\n");
        return SHELL_OK;
    }
    printf("ID   Name       Score\n");
    Student *cur = *head;
    while (cur != NULL) {
        printf("%-5d%-10s%d\n", cur->id, cur->name, cur->score);
        cur = cur->next;
    }
    return SHELL_OK;
}

#ifdef ADMIN_MODE
Command commands[] = {
    {"list", handle_list, "list", "List all students"},
    {"exit", NULL,        "exit", "Exit program"}
};
#endif

#ifdef CLIENT_MODE
Command commands[] = {
    {"list", handle_list, "list", "List all students"},
    {"exit", NULL,        "exit", "Exit program"}
};
#endif

static int num_commands = sizeof(commands) / sizeof(commands[0]);

ShellResult dispatch_command(const char *input, Student **head, const char *csv_path) {
    (void)csv_path;

    char buf[256];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *cmd  = strtok(buf, " ");
    char *args = strtok(NULL, "");

    if (cmd == NULL)
        return SHELL_OK;

    if (strcmp(cmd, "exit") == 0) {
        printf("Goodbye.\n");
        return SHELL_EXIT;
    }

    for (int i = 0; i < num_commands; i++) {
        if (strcmp(cmd, commands[i].name) == 0) {
            return commands[i].handler(args, head);
        }
    }

    printf("Unknown command or permission denied.\n");
    return SHELL_ERR_UNKNOWN_COMMAND;
}
