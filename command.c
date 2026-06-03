#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "file_io.h"

ShellResult dispatch_command(const char *input, Student **head, const char *csv_path) {
    (void)head;
    (void)csv_path;

    char buf[256];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *cmd = strtok(buf, " ");
    if (cmd == NULL)
        return SHELL_OK;

    if (strcmp(cmd, "exit") == 0) {
        printf("Goodbye.\n");
        return SHELL_EXIT;
    }

    printf("Unknown command or permission denied.\n");
    return SHELL_ERR_UNKNOWN_COMMAND;
}
