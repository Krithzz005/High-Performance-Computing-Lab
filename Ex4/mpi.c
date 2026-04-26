[23bcs074@mepcolinux ex5]$cat e5.c
III-B1@gbl:~/hpc/ex5$ cat student.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[20];
    int roll;
    int marks;
    char grade;
} Student;
char calculateGrade(int marks) {
    if (marks >= 90) return 'A';
    else if (marks >= 80) return 'B';
    else if (marks >= 70) return 'C';
    else if (marks >= 60) return 'D';
    else return 'F';
}

int main(void) {
    int rank, size, n;
    Student *students = NULL;
    Student *local_students = NULL;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Datatype MPI_Student;
    int blocklengths[4] = {20, 1, 1, 1};
    MPI_Datatype types[4] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_CHAR};
    MPI_Aint displacements[4];
    Student temp;
    MPI_Aint base_addr;
    MPI_Get_address(&temp, &base_addr);
    MPI_Get_address(&temp.name, &displacements[0]);
    MPI_Get_address(&temp.roll, &displacements[1]);
    MPI_Get_address(&temp.marks, &displacements[2]);
    MPI_Get_address(&temp.grade, &displacements[3]);
    for (int i = 0; i < 4; i++)
        displacements[i] -= base_addr;
    MPI_Type_create_struct(4, blocklengths, displacements, types, &MPI_Student);
    MPI_Type_commit(&MPI_Student);
    if (rank == 0) {
        FILE *fp = fopen("students.txt", "r");
        if (!fp) {
            printf("Unable to open input file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fscanf(fp, "%d", &n);
        if (n % size != 0) {
            printf("Error: number of students (%d) not divisible by processes (%d)\n", n, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        students = (Student *)malloc(n * sizeof(Student));
        for (int i = 0; i < n; i++) {
            fscanf(fp, "%s %d %d",
                   students[i].name,
                   &students[i].roll,
                   &students[i].marks);
            students[i].grade = '-';
        }
        fclose(fp);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int local_n = n / size;
    local_students = (Student *)malloc(local_n * sizeof(Student));
    MPI_Scatter(students, local_n, MPI_Student,
                local_students, local_n, MPI_Student,
                0, MPI_COMM_WORLD);
    for (int i = 0; i < local_n; i++) {
        local_students[i].grade = calculateGrade(local_students[i].marks);
    }
    MPI_Gather(local_students, local_n, MPI_Student,
               students, local_n, MPI_Student,
               0, MPI_COMM_WORLD);
    if (rank == 0) {
        FILE *out = fopen("grades.txt", "w");
        for (int i = 0; i < n; i++) {
            fprintf(out, "%s %d %d %c\n",
                    students[i].name,
                    students[i].roll,
                    students[i].marks,
                    students[i].grade);
        }
        fclose(out);
        printf("Grades written to grades.txt\n");
    }
    free(local_students);
    if (rank == 0) free(students);

    MPI_Type_free(&MPI_Student);
    MPI_Finalize();
    return 0;
}

III-B1@gbl:~/hpc/ex5$ cat students.txt
6
amyra           501             97
stella          502             65
alya            503             85
raji            504             56
rith            505             99
aju             506             77
III-B1@gbl:~/hpc/ex5$ cat grades.txt
amyra 501 97 A
stella 502 65 D
alya 503 85 B
raji 504 56 F
rith 505 99 A
aju 506 77 C

[23bcs074@mepcolinux ex5]$exit
exit
