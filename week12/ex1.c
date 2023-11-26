#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

#define KBD_EVENT_FILE "/dev/input/by-path/platform-i8042-serio-0-event-kbd"

int main(void)
{
    // open the file to capture keyboard events
    FILE *file = fopen(KBD_EVENT_FILE, "rb");
    if (file == NULL)
    {
        perror("open file");
        return EXIT_FAILURE;
    }
    // open the output file
    FILE *output = fopen("ex1.txt", "w");
    if (output == NULL)
    {
        perror("output");
        return EXIT_FAILURE;
    }

    // print available shortcuts (duplicating to output file)
    printf("AVAILABLE SHORTCUTS:\n");
    fprintf(output, "AVAILABLE SHORTCUTS:\n");
    printf("    * e+x: exit\n");
    fprintf(output, "    * e+x: exit\n");
    printf("    * p+e: \"I passed the Exam!\"\n");
    fprintf(output, "    * p+e: \"I passed the Exam!\"\n");
    printf("    * c+a+p: \"Get some cappuccino!\"\n");
    fprintf(output, "    * c+a+p: \"Get some cappuccino!\"\n");
    printf("    * a+b+c: The alphabet!\n");
    fprintf(output, "    * a+b+c: The alphabet!\n");

    // we will store key press events in an array
    // array index:   0     1 2 3 4     5
    // time       : oldest            newest
    int codes[6] = {0};
    struct input_event event;
    while (1)
    {
        // read an event
        fread(&event, sizeof(struct input_event), 1, file);
        // skip it if not a key press
        if (event.type != EV_KEY)
            continue;

        // if this event is a key press
        if (event.value == 1)
        {
            // shift all event codes left by one
            for (int i = 0; i < 5; i++)
                codes[i] = codes[i + 1];
            // and put the new code at the end (newest)
            codes[5] = event.code;

            // output info both to stdout and file
            printf("PRESSED ");
            fprintf(output, "PRESSED ");
        }
        // if this event is a key repeat
        else if (event.value == 2)
        {
            // output info both to stdout and file
            printf("REPEATED ");
            fprintf(output, "REPEATED ");
        }
        // if this event is a key release
        else if (event.value == 0)
        {
            // remove the code from event codes array
            // as this key is not pressed anymore
            for (int i = 0; i < 6; i++)
                if (codes[i] == event.code)
                    codes[i] = 0;

            // output info both to stdout and file
            printf("RELEASED ");
            fprintf(output, "RELEASED ");
        }
        // output info both to stdout and file
        printf("0x%x (%d)\n", event.code, event.code);
        fprintf(output, "0x%x (%d)\n", event.code, event.code);

        // if keys are pressed in order `e + x` and no other keys are pressed => exit the program
        if (codes[0] == 0 && codes[1] == 0 && codes[2] == 0 && codes[3] == 0 && codes[4] == KEY_E && codes[5] == KEY_X)
        {
            printf("[SHORTCUT] Exiting...\n");
            fprintf(output, "[SHORTCUT] Exiting...\n");
            break;
        }
        // if keys are pressed in order `p + e` and no other keys are pressed => print `I passed the Exam!`
        else if (codes[0] == 0 && codes[1] == 0 && codes[2] == 0 && codes[3] == 0 && codes[4] == KEY_P && codes[5] == KEY_E)
        {
            printf("[SHORTCUT] I passed the Exam!\n");
            fprintf(output, "[SHORTCUT] I passed the Exam!\n");
        }
        // if keys are pressed in order `c + a + p` and no other keys are pressed => print `Get some cappuccino!`
        else if (codes[0] == 0 && codes[1] == 0 && codes[2] == 0 && codes[3] == KEY_C && codes[4] == KEY_A && codes[5] == KEY_P)
        {
            printf("[SHORTCUT] Get some cappuccino!\n");
            fprintf(output, "[SHORTCUT] Get some cappuccino!\n");
        }
        // if keys are pressed in order `a + b + c` and no other keys are pressed => print the alphabet
        else if (codes[0] == 0 && codes[1] == 0 && codes[2] == 0 && codes[3] == KEY_A && codes[4] == KEY_B && codes[5] == KEY_C)
        {
            printf("[SHORTCUT] ");
            fprintf(output, "[SHORTCUT] ");
            for (int i = 0; i < 26; i++)
            {
                printf("%c", (char)(65 + i));
                fprintf(output, "%c", (char)(65 + i));
            }
            printf("\n");
            fprintf(output, "\n");
        }
    }

    // close the files
    fclose(file);
    fclose(output);

    return EXIT_SUCCESS;
}