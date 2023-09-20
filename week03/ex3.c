#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME 63ULL
#define MAX_DIRNAME 63ULL
#define MAX_PATH 2048ULL
#define MAX_DATA 1024ULL

struct Directory
{
    char name[MAX_DIRNAME];
    struct File **files;
    struct Directory **directories;
    unsigned char nf;
    unsigned char nd;
    char path[MAX_PATH];
};

struct File
{
    unsigned long long id;
    char name[MAX_FILENAME];
    unsigned long long size;
    char data[MAX_DATA];
    struct Directory *directory;
};

void overwrite_to_file(struct File *file, const char *str)
{
    if (!file)
    {
        printf("Error writing to file: file does not exist\n");
        return;
    }
    if (strlen(str) > MAX_DATA)
    {
        printf("Error writing to file: maximum size of data is %lld\n", MAX_DATA);
        return;
    }
    strcpy(file->data, str);
    file->size = strlen(str);
}

void append_to_file(struct File *file, const char *str)
{
    if (!file)
    {
        printf("Error appending to file: file does not exist\n");
        return;
    }
    if (file->size + strlen(str) > MAX_DATA)
    {
        printf("Error appending to file: maximum size of data is %lld\n", MAX_DATA);
        return;
    }
    strcat(file->data, str);
    file->size += strlen(str);
}

void printp_file(struct File *file)
{
    printf("%s/%s\n", file->directory->path, file->name);
}

void add_file(struct File *file, struct Directory *dir)
{
    if (!dir)
    {
        printf("Error adding file to directory: directory does not exist\n");
        return;
    }
    if (dir->nf == 255)
    {
        printf("Error adding file to directory: maximum number of files is 256\n");
        return;
    }
    dir->files[dir->nf] = file;
    dir->nf++;
    file->directory = dir;
}

// Prints the name of the File file
void show_file(struct File *file)
{
    if (file)
        printf("%s ", file->name);
}

// Displays the content of the Directory dir
void show_dir(struct Directory *dir)
{
    if (!dir)
        return;

    printf("\nDIRECTORY\n");
    printf(" path: %s\n", dir->path);
    printf(" files:\n");
    printf(" [ ");
    for (int i = 0; i < dir->nf; i++)
    {
        show_file(dir->files[i]);
    }

    printf("]\n");
    printf(" directories:\n");
    printf(" { ");
    for (int i = 0; i < dir->nd; i++)
    {
        show_dir(dir->directories[i]);
    }
    printf("}\n");
}

// Adds the subdirectory dir1 to the directory dir2
void add_dir(struct Directory *dir1, struct Directory *dir2)
{
    if (dir1 && dir2)
    {
        dir2->directories[dir2->nd] = dir1;
        dir2->nd++;
        char temp_path[MAX_PATH];
        if (strcmp(dir2->path, "/"))
        {
            strcpy(temp_path, dir2->path);
            strcat(temp_path, "/");
            strcat(temp_path, dir1->name);
            strcpy(dir1->path, temp_path);
        }
        else
        {
            strcpy(temp_path, "/");
            strcat(temp_path, dir1->name);
            strcpy(dir1->path, temp_path);
        }
    }
}

struct Directory *create_directory(char *name)
{
    if (strlen(name) >= MAX_DIRNAME)
    {
        printf("Error creating directory: maximum size of dirname is %lld\n", MAX_DIRNAME);
        return NULL;
    }
    struct Directory *dir = (struct Directory *)malloc(sizeof(struct Directory));
    strcpy(dir->name, name);
    dir->nf = 0;
    dir->files = (struct File **)malloc(sizeof(struct File *));
    dir->nd = 0;
    dir->directories = (struct Directory **)malloc(sizeof(struct Directory *));
    return dir;
}

struct File *create_file(unsigned long long id, char *name)
{
    if (strlen(name) >= MAX_FILENAME)
    {
        printf("Error creating file: maximum size of filename is %lld\n", MAX_FILENAME);
        return NULL;
    }
    struct File *file = (struct File *)malloc(sizeof(struct File));
    file->id = id;
    strcpy(file->name, name);
    return file;
}

int main(void)
{
    struct Directory *root = create_directory("");
    strcpy(root->path, "/");

    struct Directory *home = create_directory("home");
    add_dir(home, root);

    struct Directory *bin = create_directory("bin");
    add_dir(bin, root);

    struct File *bash = create_file(0, "bash");
    add_file(bash, bin);

    struct File *ex3_1 = create_file(1, "ex3_1.c");
    add_file(ex3_1, home);

    struct File *ex3_2 = create_file(2, "ex3_2.c");
    add_file(ex3_2, home);

    overwrite_to_file(ex3_1, "int printf(const char * format, ...);");

    overwrite_to_file(ex3_2, "//This is a comment in C language");

    overwrite_to_file(bash, "Bourne Again Shell!!");

    append_to_file(ex3_1, "int main(){printf(”Hello World!”)}");

    printp_file(bash);
    printp_file(ex3_1);
    printp_file(ex3_2);

    show_dir(root);

    printf("Content of bash: %s\n", bash->data);
    printf("Content of ex3_1.c: %s\n", ex3_1->data);
    printf("Content of ex3_2.c: %s\n", ex3_2->data);

    free(bash);
    free(ex3_1);
    free(ex3_2);
    free(home);
    free(bin);
    free(root);

    return 0;
}