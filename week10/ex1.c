#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_PATH_LENGTH (1024)

char base_path[MAX_PATH_LENGTH];

void find_all_hlinks(const char *source)
{
}

void unlink_all(const char *source)
{
    struct stat source_stat;
    if (lstat(source, &source_stat) == -1)
    {
        perror("[ERROR] lstat");
        exit(EXIT_FAILURE);
    }

    DIR *dir = opendir(".");
    if (!dir)
    {
        perror("[ERROR] opendir");
        exit(EXIT_FAILURE);
    }

    char *last_link_path = NULL;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        struct stat entry_stat;
        if (lstat(entry->d_name, &entry_stat) == -1)
        {
            perror("lstat");
            continue;
        }

        if (entry_stat.st_ino == source_stat.st_ino)
        {
            if (last_link_path)
            {
                if (unlink(last_link_path) == -1)
                {
                    perror("unlink");
                    exit(EXIT_FAILURE);
                }
            }
            last_link_path = realpath(entry->d_name, NULL);
        }
    }

    if (last_link_path)
    {
        printf("The last hard link path: %s\n", last_link_path);
        free(last_link_path);
    }

    closedir(dir);
}

void create_sym_link(const char *source, const char *dst)
{
    char absolute_path[MAX_PATH_LENGTH];
    strcpy(absolute_path, base_path);
    strcat(absolute_path, "/");
    strcat(absolute_path, dst);

    if (symlink(source, absolute_path) == -1)
    {
        perror("[ERROR] symlink creation");
        fprintf(stderr, "[ERROR] path: %s\n", absolute_path);
        exit(EXIT_FAILURE);
    }
}

void create_hard_link(const char *source, const char *dst)
{
    char absolute_path[MAX_PATH_LENGTH];
    strcpy(absolute_path, base_path);
    strcat(absolute_path, "/");
    strcat(absolute_path, dst);

    if (link(source, absolute_path) == -1)
    {
        perror("[ERROR] hardlink creation");
        fprintf(stderr, "[ERROR] path: %s\n", absolute_path);
        exit(EXIT_FAILURE);
    }
}

void print_stat(const char *fn)
{
    // FROM https://linux.die.net/man/2/stat

    struct stat sb;

    if (stat(fn, &sb) != -1)
    {
        printf("File type:                ");

        switch (sb.st_mode & __S_IFMT)
        {
        case __S_IFBLK:
            printf("block device\n");
            break;
        case __S_IFCHR:
            printf("character device\n");
            break;
        case __S_IFDIR:
            printf("directory\n");
            break;
        case __S_IFIFO:
            printf("FIFO/pipe\n");
            break;
        case __S_IFLNK:
            printf("symlink\n");
            break;
        case __S_IFREG:
            printf("regular file\n");
            break;
        case __S_IFSOCK:
            printf("socket\n");
            break;
        default:
            printf("unknown?\n");
            break;
        }

        printf("I-node number:            %ld\n", (long)sb.st_ino);

        printf("Mode:                     %lo (octal)\n", (unsigned long)sb.st_mode);

        printf("Link count:               %ld\n", (long)sb.st_nlink);
        printf("Ownership:                UID=%ld   GID=%ld\n", (long)sb.st_uid, (long)sb.st_gid);

        printf("Preferred I/O block size: %ld bytes\n", (long)sb.st_blksize);
        printf("File size:                %lld bytes\n", (long long)sb.st_size);
        printf("Blocks allocated:         %lld\n", (long long)sb.st_blocks);

        printf("Last status change:       %s", ctime(&sb.st_ctime));
        printf("Last file access:         %s", ctime(&sb.st_atime));
        printf("Last file modification:   %s", ctime(&sb.st_mtime));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "[ERROR] Usage: %s <PATH TO MONITORED DIRECTORY>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(base_path, argv[1]);

    // create myfile1.txt
    FILE *file = fopen("myfile1.txt", "w");
    fprintf(file, "Hello world.");
    fclose(file);

    // create hardlinks myfile11.txt, myfile12.txt
    create_hard_link("myfile1.txt", "myfile11.txt");
    create_hard_link("myfile1.txt", "myfile12.txt");

    // find all hard links to myfile1.txt
    find_all_hlinks("myfile1.txt");

    // move the file myfile1.txt to another folder /tmp/myfile1.txt
    rename("myfile1.txt", "/tmp/myfile1.txt");

    // modify the file myfile11.txt (its content)
    //

    // create a symbolic link myfile13.txt in the watched directory to /tmp/myfile1.txt
    create_sym_link("/tmp/myfile1.txt", "myfile13.txt");

    // modify the file /tmp/myfile1.txt (its content)
    //

    // remove all duplicates of hard links to myfile11.txt
    unlink_all("myfile11.txt");

    return EXIT_SUCCESS;
}