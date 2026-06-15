#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

Student *add_student(Student **head, int id, const char *name, int score) {
    Student *new = (Student *)malloc(sizeof(Student));
    if (new == NULL) {
        printf("Error: memory allocation failed.\n");
        return NULL;
    }
    new->id = id;
    strncpy(new->name, name, 31);
    new->name[31] = '\0';
    new->score = score;
    new->next = NULL;

    if (*head == NULL) {
        *head = new;
        return new;
    }

    Student *cur = *head;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = new;
    return new;
}

Student *find_student(Student *head, int id) {
    Student *cur = head;
    while (cur != NULL) {
        if (cur->id == id)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

int delete_student(Student **head, int id) {
    if (*head == NULL)
        return -1;

    if ((*head)->id == id) {
        Student *tmp = *head;
        *head = (*head)->next;
        free(tmp);
        return 0;
    }

    Student *cur = *head;
    while (cur->next != NULL) {
        if (cur->next->id == id) {
            Student *tmp = cur->next;
            cur->next = tmp->next;
            free(tmp);
            return 0;
        }
        cur = cur->next;
    }
    return -1;
}

void free_list(Student **head) {
    Student *cur = *head;
    while (cur != NULL) {
        Student *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    *head = NULL;
}
