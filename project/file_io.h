#ifndef FILE_IO_H
#define FILE_IO_H

#include "student.h"

int load_csv(const char *filename, Student **head);
int save_csv(const char *filename, Student *head);

#endif
