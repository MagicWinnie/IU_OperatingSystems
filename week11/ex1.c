#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void ls(const char *path)
{
    DIR *mydir;
    struct dirent *myfile;

    char buf[512];
    mydir = opendir(path);
    while ((myfile = readdir(mydir)) != NULL)
    {
        sprintf(buf, "%s/%s", path, myfile->d_name);
        printf("%s\n", myfile->d_name);
    }
    closedir(mydir);
}

int main(void)
{
    ls("/");

    return EXIT_SUCCESS;
}