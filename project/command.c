#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "file_io.h"

static const char *g_csv_path = NULL;

void set_csv_path(const char *path) {
    g_csv_path = path;
}

static int parse_int(const char *str, int *out) {
    char *end;
    long val = strtol(str, &end, 10);
    if (*end != '\0')
        return -1;
    *out = (int)val;
    return 0;
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
    int id;
    if (parse_int(args, &id) < 0 || id <= 0) {
        printf("Error: invalid id.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
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

    int id, score;
    if (parse_int(id_str, &id) < 0 || id <= 0) {
        printf("Error: invalid id.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (parse_int(score_str, &score) < 0) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
    }
    if (name[0] == '\0' || strchr(name, ',') != NULL) {
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
    int id;
    if (parse_int(args, &id) < 0 || id <= 0) {
        printf("Error: invalid id.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (delete_student(head, id) < 0) {
        printf("Error: student not found.\n");
        return SHELL_ERR_STUDENT_NOT_FOUND;
    }
    printf("Student deleted.\n");
    return SHELL_OK;
}

ShellResult handle_stats(char *args, Student **head) {
    (void)args;
    if (*head == NULL) {
        printf("No student data available.\n");
        return SHELL_OK;
    }
    int count = 0, sum = 0, max = -1, min = 101;
    Student *cur = *head;
    while (cur != NULL) {
        sum += cur->score;
        if (cur->score > max) max = cur->score;
        if (cur->score < min) min = cur->score;
        count++;
        cur = cur->next;
    }
    printf("Count: %d\n", count);
    printf("Average: %.1f\n", (double)sum / count);
    printf("Max: %d\n", max);
    printf("Min: %d\n", min);
    return SHELL_OK;
}

ShellResult handle_help(char *args, Student **head) {
    (void)args;
    (void)head;
    printf("Commands:\n");
#ifdef ADMIN_MODE
    printf("  save                     Save students to CSV\n");
    printf("  add <id> <name> <score>  Add a student\n");
    printf("  delete <id>              Delete a student\n");
    printf("  update <id> <score>      Update student score\n");
    printf("  sort <name|score>        Sort students\n");
#endif
    printf("  reload                   Reload students from CSV\n");
    printf("  find <id>                Find student by ID\n");
    printf("  list                     List all students\n");
    printf("  stats                    Show statistics\n");
    printf("  clear                    Clear screen\n");
    printf("  exit                     Exit program\n");
    return SHELL_OK;
}

ShellResult handle_clear(char *args, Student **head) {
    (void)args;
    (void)head;
    printf("\033[2J\033[H");
    return SHELL_OK;
}

static void swap_students(Student *a, Student *b) {
    int tmp_id    = a->id;    a->id    = b->id;    b->id    = tmp_id;
    int tmp_score = a->score; a->score = b->score; b->score = tmp_score;
    char tmp_name[32];
    strcpy(tmp_name, a->name);
    strcpy(a->name,  b->name);
    strcpy(b->name,  tmp_name);
}

ShellResult handle_sort(char *args, Student **head) {
    if (args == NULL) {
        printf("Error: missing argument.\n");
        return SHELL_ERR_MISSING_ARGUMENT;
    }

    int by_name;
    if (strcmp(args, "name") == 0)
        by_name = 1;
    else if (strcmp(args, "score") == 0)
        by_name = 0;
    else {
        printf("Error: invalid sort key.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }

    int swapped;
    do {
        swapped = 0;
        Student *cur = *head;
        while (cur != NULL && cur->next != NULL) {
            int cmp = by_name ? strcmp(cur->name, cur->next->name)
                               : cur->score - cur->next->score;
            if (cmp > 0) {
                swap_students(cur, cur->next);
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);

    if (by_name)
        printf("Students sorted by name.\n");
    else
        printf("Students sorted by score.\n");
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

    int id, score;
    if (parse_int(id_str, &id) < 0 || id <= 0) {
        printf("Error: invalid id.\n");
        return SHELL_ERR_INVALID_ARGUMENT;
    }
    if (parse_int(score_str, &score) < 0) {
        printf("Error: invalid score.\n");
        return SHELL_ERR_INVALID_SCORE;
    }
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

ShellResult handle_save(char *args, Student **head) {
    (void)args;
    int count = save_csv(g_csv_path, *head);
    if (count >= 0)
        printf("Saved %d students to %s.\n", count, g_csv_path);
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
    {"save",   handle_save,   "save",                    "Save to CSV"},
    {"sort",   handle_sort,   "sort <name|score>",        "Sort students"},
    {"stats",  handle_stats,  "stats",                   "Show statistics"},
    {"help",   handle_help,   "help",                    "Show help"},
    {"clear",  handle_clear,  "clear",                   "Clear screen"},
};
#endif

#ifdef CLIENT_MODE
Command commands[] = {
    {"list",   handle_list,   "list",      "List all students"},
    {"find",   handle_find,   "find <id>", "Find student by ID"},
    {"reload", handle_reload, "reload",    "Reload from CSV"},
    {"stats",  handle_stats,  "stats",     "Show statistics"},
    {"help",   handle_help,   "help",      "Show help"},
    {"clear",  handle_clear,  "clear",     "Clear screen"},
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
