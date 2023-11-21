#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILES 16
#define MAX_FILESIZE_BLOCKS 8
#define MAX_FILENAME (16 + 1)
#define BLOCK_SIZE 1024
#define INPUT_FILE "input.txt"

struct inode
{
    char name[MAX_FILENAME];                // file name
    int size;                               // file size ( in number of blocks )
    int blockPointers[MAX_FILESIZE_BLOCKS]; // direct block pointers
    int used;                               // 0 = > inode is free ; 1 = > in use
};

FILE *disk;

int create(char name[MAX_FILENAME], int size)
{
    // Create a file with this name and this size

    // Step 1: check to see if we have sufficient free space on disk by
    // reading in the free block list. To do this:
    // move the file pointer to the start of the disk file.
    rewind(disk);
    // Read the first 128 bytes (the free/in-use block information)
    char free_block_list[128];
    fread(free_block_list, sizeof(char), 128, disk);
    // Scan the list to make sure you have sufficient free blocks to
    // allocate a new file of this size
    int found = 0;
    for (int i = 0; i < 128; i++)
    {
        if (free_block_list[i] == 0)
            found++;
    }
    if (found < size)
    {
        fprintf(stderr, "Insufficient disk size\n");
        return -1;
    }

    // Step 2: we look for a free inode on disk
    struct inode node;
    int inode_index = 0;
    for (int i = 0; i < 16; i++)
    {
        // Read in a inode
        fread(&node, sizeof(struct inode), 1, disk);
        // check the "used" field to see if it is free
        if (node.used == 1)
        {
            // If not, repeat the above two steps until you find a free inode
            continue;
        }
        inode_index = i;
        // Set the "used" field to 1
        node.used = 1;
        // Copy the filename to the "name" field
        strcpy(node.name, name);
        // Copy the file size (in units of blocks) to the "size" field
        node.size = size;
        break;
    }

    // Step 3: Allocate data blocks to the file
    // Scan the block list that you read in Step 1 for a free block
    int count = 0;
    for (int i = 0; i < 128; i++)
    {
        if (free_block_list[i] == 0)
        {
            // Once you find a free block, mark it as in-use (Set it to 1)
            free_block_list[i] = 1;
            // Set the blockPointer[i] field in the inode to this block number.
            node.blockPointers[count] = i;
            count++;
            if (count == size)
                break;
        }
    } // end for

    // Step 4: Write out the inode and free block list to disk
    // Move the file pointer to the start of the file
    rewind(disk);
    // Write out the 128 byte free block list
    fwrite(free_block_list, sizeof(char), 128, disk);
    // Move the file pointer to the position on disk where this inode was stored
    fseek(disk, 128 + inode_index * sizeof(struct inode), SEEK_SET);
    // Write out the inode
    fwrite(&node, sizeof(struct inode), 1, disk);

    return inode_index;

} // End Create

int delete(char name[MAX_FILENAME])
{
    // Delete the file with this name

    // Step 1: Locate the inode for this file
    // Move the file pointer to the 1st inode (129th byte)
    fseek(disk, 128, SEEK_SET);

    struct inode node;
    int inode_index = -1;
    for (int i = 0; i < 16; i++)
    {
        // Read in a inode
        fread(&node, sizeof(struct inode), 1, disk);
        if (node.used == 0)
        {
            // If the inode is free, repeat above step.
            continue;
        }
        // If the inode is in use, check if the "name" field in the
        // inode matches the file we want to delete. IF not, read the next
        // inode and repeat
        if (strcmp(node.name, name) != 0)
        {
            continue;
        }
        inode_index = i;
        break;
    }
    if (inode_index == -1)
    {
        fprintf(stderr, "File not found\n");
        return -1;
    }

    // Step 2: free blocks of the file being deleted
    // Read in the 128 byte free block list (move file pointer to start of the disk and read in 128 bytes)
    rewind(disk);
    char free_block_list[128];
    fread(free_block_list, sizeof(char), 128, disk);
    // Free each block listed in the blockPointer fields as follows:
    for (int i = 0; i < node.size; i++)
        free_block_list[node.blockPointers[i]] = 0;

    // Step 3: mark inode as free
    // Set the "used" field to 0.
    node.used = 0;

    // Step 4: Write out the inode and free block list to disk
    // Move the file pointer to the start of the file
    rewind(disk);
    // Write out the 128 byte free block list
    fwrite(free_block_list, sizeof(char), 128, disk);
    // Move the file pointer to the position on disk where this inode was stored
    fseek(disk, 128 + inode_index * sizeof(struct inode), SEEK_SET);
    // Write out the inode
    fwrite(&node, sizeof(struct inode), 1, disk);

    return inode_index;
} // End Delete

int ls(void)
{
    // List names of all files on disk

    // Step 1: read in each inode and print!
    // Move file pointer to the position of the 1st inode (129th byte)
    fseek(disk, 128, SEEK_SET);

    struct inode node;
    int found = -1;
    for (int i = 0; i < 16; i++)
    {
        // Read in a inode
        fread(&node, sizeof(struct inode), 1, disk);
        // If the inode is in-use
        if (node.used == 1)
        {
            // print the "name" and "size" fields from the inode
            printf("inode[%d]: <name>=`%s`, <size>=%d\n", i, node.name, node.size);
            found = i;
        }

    } // end for
    if (found == -1)
    {
        fprintf(stderr, "File not found\n");
        return -1;
    }
    return 1;

} // End ls

int read_block(char name[MAX_FILENAME], int blockNum, char buf[BLOCK_SIZE])
{
    // read this block from this file

    // Step 1: locate the inode for this file
    // Move file pointer to the position of the 1st inode (129th byte)
    fseek(disk, 128, SEEK_SET);

    struct inode node;
    int found = -1;
    for (int i = 0; i < 16; i++)
    {
        // Read in a inode
        fread(&node, sizeof(struct inode), 1, disk);
        // If the inode is in use, compare the "name" field with the above file
        if (node.used == 1 && strcmp(node.name, name) == 0)
        {
            // print the "name" and "size" fields from the inode
            found = i;
            break;
        }
        // IF the file names don't match, repeat
    }
    if (found == -1)
    {
        fprintf(stderr, "File not found\n");
        return -1;
    }

    // Step 2: Read in the specified block
    // Check that blockNum < inode.size, else flag an error
    if (blockNum >= node.size)
    {
        fprintf(stderr, "blockNum is out of range\n");
        return -1;
    }
    // Get the disk address of the specified block
    int addr = node.blockPointers[blockNum];
    // move the file pointer to the block location (i.e., to byte # addr*1024 in the file)
    fseek(disk, BLOCK_SIZE * addr, SEEK_SET);
    // Read in the block! => Read in 1024 bytes from this location into the buffer "buf"
    fread(buf, sizeof(char), BLOCK_SIZE, disk);

    return 1;
} // End read

int write_block(char name[MAX_FILENAME], int blockNum, char buf[BLOCK_SIZE])
{
    // write this block to this file

    // Step 1: locate the inode for this file
    // Move file pointer to the position of the 1st inode (129th byte)
    fseek(disk, 128, SEEK_SET);

    struct inode node;
    int found = -1;
    for (int i = 0; i < 16; i++)
    {
        // Read in a inode
        fread(&node, sizeof(struct inode), 1, disk);
        // If the inode is in use, compare the "name" field with the above file
        if (node.used == 1 && strcmp(node.name, name) == 0)
        {
            // print the "name" and "size" fields from the inode
            found = i;
            break;
        }
        // IF the file names don't match, repeat
    }
    if (found == -1)
    {
        fprintf(stderr, "File not found\n");
        return -1;
    }

    // Step 2: Write to the specified block
    // Check that blockNum < inode.size, else flag an error
    if (blockNum >= node.size)
    {
        fprintf(stderr, "blockNum is out of range\n");
        return -1;
    }
    // Get the disk address of the specified block
    int addr = node.blockPointers[blockNum];
    // move the file pointer to the block location (i.e., byte # addr*1024)
    fseek(disk, BLOCK_SIZE * addr, SEEK_SET);
    // Write the block! => Write 1024 bytes from the buffer "buff" to this location
    fwrite(buf, sizeof(char), BLOCK_SIZE, disk);

    return 1;
} // end write

int main(void)
{
    FILE *input = fopen(INPUT_FILE, "r");
    if (input == NULL)
    {
        perror("[ERROR] input file");
        return EXIT_FAILURE;
    }

    char disk_name[MAX_FILENAME] = {0};
    fscanf(input, "%16s\n", disk_name);
    disk = fopen(disk_name, "r+b");
    if (disk == NULL)
    {
        perror("[ERROR] disk file");
        return EXIT_FAILURE;
    }

    char command;
    while (fscanf(input, "%c\n", &command) != EOF)
    {
        if (command == 'C')
        {
            char file_name[MAX_FILENAME] = {0};
            int size;
            fscanf(input, "%16s %d\n", file_name, &size);

            int place = create(file_name, size);
            if (place != -1)
                printf("Created file %s at inode %d\n", file_name, place);
        }
        else if (command == 'D')
        {
            char file_name[MAX_FILENAME] = {0};
            fscanf(input, "%16s\n", file_name);

            int place = delete (file_name);
            if (place != -1)
                printf("Deleted file %s at inode %d\n", file_name, place);
        }
        else if (command == 'L')
        {
            printf("======ls=====\n");
            ls();
            printf("=============\n");
        }
        else if (command == 'R')
        {
            char file_name[MAX_FILENAME] = {0};
            int block_num;
            fscanf(input, "%16s %d\n", file_name, &block_num);

            char buf[BLOCK_SIZE];
            int res = read_block(file_name, block_num, buf);

            if (res == 1)
            {
                printf("Read block %d from file %s:\n", block_num, file_name);
                for (int i = 0; i < BLOCK_SIZE; i++)
                    printf("%d", buf[i]);
                printf("\n");
            }
        }
        else if (command == 'W')
        {
            char file_name[MAX_FILENAME] = {0};
            int block_num;
            fscanf(input, "%16s %d\n", file_name, &block_num);

            char buf[BLOCK_SIZE] = {0};
            // filling dummy buffer
            for (int i = 1; i <= BLOCK_SIZE; i++)
                if (i % (block_num + 1) == 0)
                    buf[i - 1] = 1;
            int res = write_block(file_name, block_num, buf);

            if (res == 1)
            {
                printf("Wrote to block %d to file %s:\n", block_num, file_name);
                for (int i = 0; i < BLOCK_SIZE; i++)
                    printf("%d", buf[i]);
                printf("\n");
            }
        }
        else
        {
            fprintf(stderr, "[ERROR] Unknown command: %c\n", command);
        }
    }

    return EXIT_SUCCESS;
}