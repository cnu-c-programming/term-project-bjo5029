#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"

int load_csv(const char *filename, Student **head) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: cannot open file %s.\n", filename);
        return -1;
    }

    char line[256];

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }
    line[strcspn(line, "\r\n")] = '\0';

    if (strcmp(line, "id,name,score") != 0) {
        printf("Error: invalid CSV header.\n");
        fclose(fp);
        return -1;
    }

    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;

        char *id_str    = strtok(line, ",");
        char *name      = strtok(NULL, ",");
        char *score_str = strtok(NULL, ",");

        if (id_str == NULL || name == NULL || score_str == NULL)
            continue;

        int id    = atoi(id_str);
        int score = atoi(score_str);

        add_student(head, id, name, score);
        count++;
    }

    fclose(fp);
    return count;
}

int save_csv(const char *filename, Student *head) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: cannot open file %s.\n", filename);
        return -1;
    }

    fprintf(fp, "id,name,score\n");

    int count = 0;
    Student *cur = head;
    while (cur != NULL) {
        fprintf(fp, "%d,%s,%d\n", cur->id, cur->name, cur->score);
        count++;
        cur = cur->next;
    }

    fclose(fp);
    return count;
}
