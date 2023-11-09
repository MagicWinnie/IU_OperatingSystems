#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define MAX_PATH_LENGTH (1024)
#define MAX_FILE_AMOUNT (65535)

int fd;
int wd[MAX_FILE_AMOUNT];
char base_path[MAX_PATH_LENGTH];

void add_dir_subdirs(const char *fn, uint32_t mask)
{
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH_LENGTH];
    struct stat info;

    // printf("%s\n", fn);
    int wd_ = inotify_add_watch(fd, fn, mask);
    if (wd_ == -1)
    {
        perror("[ERROR] inotify add watch");
        fprintf(stderr, "[ERROR] Path: %s\n", fn);
        exit(EXIT_FAILURE);
    }
    wd[0] = wd_;
    int i = 1;

    if ((dir = opendir(fn)) == NULL)
    {
        perror("[ERROR] opendir() error");
        fprintf(stderr, "[ERROR] path: %s\n", fn);
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.')
        {
            strcpy(path, fn);
            strcat(path, "/");
            strcat(path, entry->d_name);
            if (stat(path, &info) != 0)
            {
                fprintf(stderr, "[ERROR] stat() error on %s\n", path);
                exit(EXIT_FAILURE);
            }
            if (S_ISDIR(info.st_mode))
            {
                // printf("%s\n", path);
                wd_ = inotify_add_watch(fd, path, mask);
                if (wd_ == -1)
                {
                    perror("[ERROR] inotify add watch");
                    fprintf(stderr, "[ERROR] Path: %s\n", path);
                    exit(EXIT_FAILURE);
                }
                wd[i] = wd_;
                i++;
                if (i == MAX_FILE_AMOUNT)
                {
                    fprintf(stderr, "[ERROR] Maximum amount of files/dirs achieved\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    wd[i] = -1; // end of watch descriptors
    closedir(dir);
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

void print_stats(const char *fn)
{
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH_LENGTH];

    if ((dir = opendir(fn)) == NULL)
        return;

    printf("=====================================\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.')
        {
            strcpy(path, fn);
            strcat(path, "/");
            strcat(path, entry->d_name);
            printf("-------------------------------------\n");
            printf("Path: %s\n", path);
            print_stat(path);
            printf("-------------------------------------\n");
        }
    }
    closedir(dir);
    printf("=====================================\n");
}

void remove_watches()
{
    for (int i = 0; i < MAX_FILE_AMOUNT; i++)
    {
        if (wd[i] == -1)
            break;
        else
            inotify_rm_watch(fd, wd[i]);
    }
}

void terminate(int code)
{
    print_stats(base_path);
    remove_watches();
    close(fd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "[ERROR] Usage: %s <PATH TO MONITORED DIRECTORY>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, terminate);

    strcpy(base_path, argv[1]);
    if (strlen(base_path) > 0 && base_path[strlen(base_path) - 1] == '/')
        base_path[strlen(base_path) - 1] = '\0';
    print_stats(base_path);

    fd = inotify_init();
    if (fd < 0)
    {
        perror("[ERROR] inotify init");
        exit(EXIT_FAILURE);
    }

    add_dir_subdirs(base_path, IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN | IN_ATTRIB);

    char buffer[EVENT_BUF_LEN] = {0};
    while (1)
    {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0)
        {
            perror("[ERROR] reading changes");
            exit(EXIT_FAILURE);
        }

        int i = 0;
        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len)
            {
                char absolute_path[MAX_PATH_LENGTH] = {0};
                strcat(absolute_path, base_path);
                strcat(absolute_path, "/");
                strcat(absolute_path, event->name);

                printf("-------------------------------------\n");
                printf("%s ", event->name);
                if (event->mask & IN_ACCESS)
                    printf("is accessed\n");
                else if (event->mask & IN_CREATE)
                    printf("is created\n");
                else if (event->mask & IN_DELETE)
                    printf("is deleted\n");
                else if (event->mask & IN_MODIFY)
                    printf("is modified\n");
                else if (event->mask & IN_OPEN)
                    printf("is opened\n");
                else if (event->mask & IN_ATTRIB)
                    printf("has metadata changed\n");
                print_stat(absolute_path);
                printf("-------------------------------------\n");
            }
            i += EVENT_SIZE + event->len;
        }
    }

    return EXIT_SUCCESS;
}