#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp3_reader.h"


/* -------------------- ID3 TAG INFORMATION -------------------- */

char *tags[] =
{
    "TIT2",
    "TPE1",
    "TALB",
    "TYER",
    "TDRC",
    "TCON",
    "COMM",
    "TRCK",
    "TPOS",
    "TCOM",
    "TPE2",
    "TCOP",
    "TPUB",
    "TENC",
    "USLT",
    "APIC"
};

char *tag_names[] =
{
    "Title",
    "Artist",
    "Album",
    "Year",
    "Recording Time",
    "Genre",
    "Comment",
    "Track Number",
    "Disc Number",
    "Composer",
    "Album Artist",
    "Copyright",
    "Publisher",
    "Encoded By",
    "Lyrics",
    "Album Art"
};


/* -------------------- OPERATION CHECK -------------------- */

operation_type check_oper(char *argv[])
{
    // Check which operation is requested from the command line.
    if (strcmp(argv[1], "-v") == 0)
    {
        return VIEW_TAGS;
    }

    else if (strcmp(argv[1], "-e") == 0)
    {
        // Check the tag option selected for editing.
        if (strcmp(argv[2], "-t") == 0)
            return EDIT_TITLE;

        else if (strcmp(argv[2], "-a") == 0)
            return EDIT_ARTIST;

        else if (strcmp(argv[2], "-A") == 0)
            return EDIT_ALBUM;

        else if (strcmp(argv[2], "-Y") == 0)
            return EDIT_YEAR;

        else if (strcmp(argv[2], "-G") == 0)
            return EDIT_GENRE;

        else if (strcmp(argv[2], "-C") == 0)
            return EDIT_COMMENT;

        else
        {
            fprintf(stderr, "Error: Invalid Input!\n");
            return HELP;
        }
    }

    else if (strcmp(argv[1], "-h") == 0)
    {
        return HELP;
    }

    else
    {
        fprintf(stderr, "Error: Invalid Input!\n");
        return HELP;
    }
}


/* -------------------- ARGUMENT VALIDATION -------------------- */

status read_validate_argv(char *argv[], mp3 *mp3)
{
    // Check the operation before opening the MP3 file.
    operation_type operation = check_oper(argv);

    if (operation == HELP)
    {
        print_help(argv[0]);
        return FAILURE;
    }

    if (operation == VIEW_TAGS)
    {
        if (strstr(argv[2], ".mp") != NULL)
        {
            if ((mp3->fptr = fopen(argv[2], "rb")) == NULL)
            {
                fprintf(stderr, "Error: File not found.\n");
                return FAILURE;
            }

            return VIEW_SUCCESS;
        }
    }

    else
    {
        if (strstr(argv[3], ".mp") != NULL)
        {
            if ((mp3->fptr = fopen(argv[3], "rb")) == NULL)
            {
                fprintf(stderr, "Error: File not found.\n");
                return FAILURE;
            }

            return EDIT_SUCCESS;
        }
    }

    return FAILURE;
}


/* -------------------- FRAME CHECK -------------------- */

char *chck_frame(char buffer[])
{
    for (int i = 0; i < 16; i++)
    {
        if (strncmp(tags[i], buffer, 4) == 0)
        {
            return tag_names[i];
        }
    }

    return NULL;
}


/* -------------------- VIEW TAGS -------------------- */

status view_tags(mp3 *mp3)
{
    // Start reading the MP3 frames after the 10-byte ID3 header.
    FILE *fptr = mp3->fptr;

    fseek(fptr, 10, SEEK_SET);

    printf("\n+-----------------------------------------------------------------------------------------+\n");
    printf("|                                        \033[1;36mMP3 TAGS\033[0m                        \t\t  |\n");
    printf("+-----------------------------------------------------------------------------------------+\n");

    for (int i = 0; i < 6; i++)
    {
        char frame_id[4];
        char size_bytes[4];

        // Read the frame information one frame at a time.
        /* Read frame ID */
        fread(frame_id, 4, 1, fptr);

        char *tag_name = chck_frame(frame_id);

        if (tag_name == NULL)
        {
            return FAILURE;
        }

        /* Read frame size */
        fread(size_bytes, 4, 1, fptr);

        /*
         * ID3v2.4 uses syncsafe integers.
         * Each byte contains only 7 bits.
         */
        int frame_size =
            ((unsigned char)size_bytes[0] << 21) |
            ((unsigned char)size_bytes[1] << 14) |
            ((unsigned char)size_bytes[2] << 7)  |
            (unsigned char)size_bytes[3];

        /* Skip 2 flag bytes */
        fseek(fptr, 2, SEEK_CUR);

        /* Read the complete frame content, including its encoding byte. */
        char frame_content[frame_size + 1];

        if (fread(frame_content, 1, frame_size, fptr) != (size_t)frame_size)
        {
            return FAILURE;
        }

        frame_content[frame_size] = '\0';

        printf("| \033[1;33m%-36s\033[0m | %-48s |\n", tag_name, frame_content + 1);
    }

    printf("+--------------------------------------+--------------------------------------------------+\n");
    printf("\033[1;32mTags read successfull\033[0m\n\n");

    return VIEW_SUCCESS;
}


/* -------------------- EDIT TAGS -------------------- */

status edit_tags(char *argv[], mp3 *mp3)
{
    // Create a temporary file to store the updated MP3 content.
    rewind(mp3->fptr);

    FILE *temp = fopen("temp.mp3", "wb");

    if (temp == NULL)
    {
        fprintf(stderr, "Error: couldn't open temp file!\n");
        return FAILURE;
    }


    /* Copy ID3 header */
    char header[10];

    fread(header, 10, 1, mp3->fptr);
    fwrite(header, 10, 1, temp);


    // Select the frame according to the option given by the user.
    /* Find the frame that needs to be edited */
    char frame_tag[5];
    const char *tag_label;

    switch (check_oper(argv))
    {
        case EDIT_TITLE:
            strcpy(frame_tag, "TIT2");
            tag_label = "Title";
            break;

        case EDIT_ARTIST:
            strcpy(frame_tag, "TPE1");
            tag_label = "Artist";
            break;

        case EDIT_ALBUM:
            strcpy(frame_tag, "TALB");
            tag_label = "Album";
            break;

        case EDIT_YEAR:
            strcpy(frame_tag, "TYER");
            tag_label = "Year";
            break;

        case EDIT_GENRE:
            strcpy(frame_tag, "TCON");
            tag_label = "Genre";
            break;

        case EDIT_COMMENT:
            strcpy(frame_tag, "COMM");
            tag_label = "Comment";
            break;

        default:
            fprintf(stderr, "Error: Invalid operation\n");
            fclose(temp);
            return FAILURE;
    }

    char new_value[1024];

    printf("Enter new %s: ", tag_label);
    if (fgets(new_value, sizeof(new_value), stdin) == NULL)
    {
        fprintf(stderr, "Error: No value entered.\n");
        fclose(temp);
        return FAILURE;
    }

    new_value[strcspn(new_value, "\r\n")] = '\0';

    if (new_value[0] == '\0')
    {
        fprintf(stderr, "Error: The new tag value cannot be empty.\n");
        fclose(temp);
        return FAILURE;
    }


    // Process each MP3 frame and replace only the selected frame.
    /* Process frames */
    for (int i = 0; i < 6; i++)
    {
        char frame_id[4];
        char size_bytes[4];
        char flags[2];

        /* Read frame header */
        fread(frame_id, 4, 1, mp3->fptr);
        fread(size_bytes, 4, 1, mp3->fptr);
        fread(flags, 2, 1, mp3->fptr);


        /* Convert syncsafe size */
        int size =
            ((unsigned char)size_bytes[0] << 21) |
            ((unsigned char)size_bytes[1] << 14) |
            ((unsigned char)size_bytes[2] << 7)  |
            (unsigned char)size_bytes[3];


        /* Check whether this is the frame to edit */
        if (strncmp(frame_id, frame_tag, 4) == 0)
        {
            /* Write frame ID */
            fwrite(frame_id, 4, 1, temp);


            /* New text size + null byte */
            int new_size = strlen(new_value) + 1;

            char new_size_bytes[4];

            new_size_bytes[0] = (new_size >> 21) & 0x7F;
            new_size_bytes[1] = (new_size >> 14) & 0x7F;
            new_size_bytes[2] = (new_size >> 7)  & 0x7F;
            new_size_bytes[3] = new_size & 0x7F;

            /* Write new size */
            fwrite(new_size_bytes, 4, 1, temp);

            /* Write original flags */
            fwrite(flags, 2, 1, temp);

            /* Write null byte */
            char null= 0;
            fwrite(&null, 1, 1, temp);

            // Store the new tag value entered through the command line.
            /* Write new text */
            fwrite(new_value, strlen(new_value), 1, temp);

            /* Skip old frame content */
            fseek(mp3->fptr, size, SEEK_CUR);
        }

        else
        {
            /* Copy unchanged frame */
            fwrite(frame_id, 4, 1, temp);
            fwrite(size_bytes, 4, 1, temp);
            fwrite(flags, 2, 1, temp);


            /* Copy frame content */
            char *frame_content = malloc(size);

            if (frame_content == NULL)
            {
                fprintf(stderr, "Error: Memory allocation failed!\n");
                fclose(temp);
                return FAILURE;
            }

            fread(frame_content, 1, size, mp3->fptr);
            fwrite(frame_content, 1, size, temp);

            free(frame_content);
        }
    }


    // Copy the remaining audio data without changing it.
    /* Copy remaining MP3 audio data */

    long current_pos = ftell(mp3->fptr);

    fseek(mp3->fptr, 0, SEEK_END);

    long end_pos = ftell(mp3->fptr);

    long rest_size = end_pos - current_pos;

    fseek(mp3->fptr, current_pos, SEEK_SET);


    char *audio_buffer = malloc(rest_size);

    if (audio_buffer == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        fclose(temp);
        return FAILURE;
    }

    fread(audio_buffer, 1, rest_size, mp3->fptr);
    fwrite(audio_buffer, 1, rest_size, temp);

    free(audio_buffer);


    // Copy the completed temporary file to the requested path.
    fclose(temp);

    FILE *updated = fopen(argv[3], "wb");
    FILE *source = fopen("temp.mp3", "rb");
    unsigned char copy_buffer[8192];

    if (updated == NULL || source == NULL)
    {
        fprintf(stderr, "Error: Could not replace original file!\n");
        if (updated != NULL)
            fclose(updated);
        if (source != NULL)
            fclose(source);
        return FAILURE;
    }

    size_t bytes_read;
    while ((bytes_read = fread(copy_buffer, 1, sizeof(copy_buffer), source)) > 0)
    {
        if (fwrite(copy_buffer, 1, bytes_read, updated) != bytes_read)
        {
            fprintf(stderr, "Error: Could not write updated file!\n");
            fclose(source);
            fclose(updated);
            return FAILURE;
        }
    }

    fclose(source);
    fclose(updated);
    remove("temp.mp3");

    return EDIT_SUCCESS;
}


/* -------------------- HELP -------------------- */

void print_help(const char *program_name)
{
    // Display the command format and available options.
    printf("Usage:\n");

    printf(" %s -h //to view usage\n", program_name);

    printf(" %s -v <file.mp3> //to View MP3 tag information\n",
           program_name);

        printf(" %s -e -t <file.mp3> //to edit Title\n",
           program_name);

        printf(" %s -e -a <file.mp3> //to edit artist\n",
           program_name);

        printf(" %s -e -A <file.mp3> //to edit album\n",
           program_name);

        printf(" %s -e -Y <file.mp3> //to edit year\n",
           program_name);

        printf(" %s -e -G <file.mp3> //to edit genre\n",
           program_name);

        printf(" %s -e -C <file.mp3> //to edit comment\n",
           program_name);
}

