#include <stdio.h>
#include <string.h>
#include "common/list.h"

typedef struct _student {
    int age;
    double tall;
    char name[10];
} student;

int main(int argc, char *argv[])
{
    list *ls = NULL;

    student stu;
    stu.age = 12;
    stu.tall = 172.2;
    strcpy(stu.name, "hello");
    
    list_append(ls, &stu);


    return 0;
}