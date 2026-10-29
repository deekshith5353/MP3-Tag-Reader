/*
 * Project Name : MP3 Tag Reader
 * Name         : Deekshith
 * Batch        : 26012_A
 * Date         : 09-09-2026
 *
 * Description:
 * This project is used to read and edit the ID3 tag information
 * stored inside an MP3 file.
 *
 * Main Operations:
 * - View MP3 tag information
 * - Edit selected tag information
 * - Display help and usage details
 *
 * The program works through command line arguments.
 */

#include "mp3_reader.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // Declare variables used to store the MP3 file and operation status.
    mp3 mp3;
    status result;

    // Check for no arguments or help option.
    /* No arguments or help */
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-h") == 0))
    {
        print_help(argv[0]);
        return 0;
    }

    // Validate the given arguments and open the required MP3 file.
    /* Validate arguments and open the file */
    result = read_validate_argv(argv, &mp3);

    // View the tags when the user selects the view operation.
    if (result == VIEW_SUCCESS)
    {
        if (view_tags(&mp3) == FAILURE)
        {
            fprintf(stderr, "Error: Failed to read MP3 tags.\n");
            fclose(mp3.fptr);
            return 1;
        }

        // Close the file after displaying the tag information.
        fclose(mp3.fptr);
        return 0;
    }

    // Edit the tags when the user selects the edit operation.
    if (result == EDIT_SUCCESS)
    {
        if (edit_tags(argv, &mp3) == FAILURE)
        {
            fprintf(stderr, "Error: Failed to edit MP3 tags.\n");
            fclose(mp3.fptr);
            return 1;
        }

        // Close the file after editing is completed.
        fclose(mp3.fptr);
        return 0;
    }

    // Return failure when the operation is not successful.
    return 1;
}
