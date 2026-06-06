#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "file_io.h"

static const char *g_csv_path = NULL;

void set_csv_path(const char *path) {
    g_csv_path = path;
}

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

ShellResult handle_find(char *args, Student **head) {
    if (args == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id = atoi(args);
    Student *s = find_student(*head, id);
    if (s == NULL) {
        printf("Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    printf("ID: %d\nName: %s\nScore: %d\n", s->id, s->name, s->score);
    return SHELL_OK;
}

ShellResult handle_add(char *args, Student **head) {
    if (args == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char *id_str    = strtok(args, " ");
    char *name      = strtok(NULL, " ");
    char *score_str = strtok(NULL, " ");

    if (id_str == NULL || name == NULL || score_str == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    int id    = atoi(id_str);
    int score = atoi(score_str);

    if (id <= 0) {
        printf("Error: invalid id.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (name[0] == '\0') {
        printf("Error: invalid name.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (score < 0 || score > 100) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
    }
    if (find_student(*head, id) != NULL) {
        printf("Error: duplicate ID.\n");
        return SHELL_ERR_DUPLICATE_STUDENT;
    }

    add_student(head, id, name, score);
    printf("Student added.\n");
    return SHELL_OK;
}

ShellResult handle_delete(char *args, Student **head) {
    if (args == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    int id = atoi(args);
    if (delete_student(head, id) < 0) {
        printf("Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    printf("Student deleted.\n");
    return SHELL_OK;
}

ShellResult handle_update(char *args, Student **head) {
    if (args == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }
    char *id_str    = strtok(args, " ");
    char *score_str = strtok(NULL, " ");

    if (id_str == NULL || score_str == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    int id    = atoi(id_str);
    int score = atoi(score_str);

    if (score < 0 || score > 100) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
    }
    Student *s = find_student(*head, id);
    if (s == NULL) {
        printf("Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    s->score = score;
    printf("Student updated.\n");
    return SHELL_OK;
}

ShellResult handle_reload(char *args, Student **head) {
    (void)args;
    free_list(head);
    int count = load_csv(g_csv_path, head);
    if (count >= 0)
        printf("Reloaded %d students from %s.\n", count, g_csv_path);
    return SHELL_OK;
}

#ifdef ADMIN_MODE
Command commands[] = {
    {"list",   handle_list,   "list",                    "List all students"},
    {"find",   handle_find,   "find <id>",               "Find student by ID"},
    {"add",    handle_add,    "add <id> <name> <score>", "Add a student"},
    {"delete", handle_delete, "delete <id>",             "Delete a student"},
    {"update", handle_update, "update <id> <score>",     "Update student score"},
    {"reload", handle_reload, "reload",                  "Reload from CSV"},
};
#endif

#ifdef CLIENT_MODE
Command commands[] = {
    {"list",   handle_list,   "list",      "List all students"},
    {"find",   handle_find,   "find <id>", "Find student by ID"},
    {"reload", handle_reload, "reload",    "Reload from CSV"},
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
