#ifndef MP3_READER_H
#define MP3_READER_H

#include <stdio.h>

// Structure used to store the MP3 file information.
typedef struct mp3_file
{
    char file_name[20];
    FILE *fptr;
} mp3;


// Different operations supported by the project.
typedef enum
{
    VIEW_TAGS,
    EDIT_TITLE,
    EDIT_ARTIST,
    EDIT_ALBUM,
    EDIT_YEAR,
    EDIT_GENRE,
    EDIT_COMMENT,
    HELP
} operation_type;


// Return status used to indicate success or failure.
typedef enum
{
    VIEW_SUCCESS,
    FAILURE,
    EDIT_SUCCESS
} status;


/* Operation handling */
operation_type check_oper(char *argv[]);

/* Validation */
status read_validate_argv(char *argv[], mp3 *mp3);

/* MP3 operations */
status view_tags(mp3 *mp3);
status edit_tags(char *argv[], mp3 *mp3);

/* Frame handling */
char *chck_frame(char buffer[]);

/* Utility */
void change_endian(void *ptr);
void print_help(const char *program_name);


/* Tag arrays */
extern char *tags[];
extern char *tag_names[];

#endif