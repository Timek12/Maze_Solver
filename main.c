#include <stdio.h>
#include <stdlib.h>

int width, height;

void free_maze(char **maze) {
    // checks if maze is not NULL and frees each row
    if (maze != NULL) {
        for (int i = 0; *(maze + i) != NULL; i++) {
            free(*(maze + i));
        }
    }
    // frees the pointer to the array of rows
    free(maze);
}

int check_maze(const char **maze)
{
    int i, j;
    int count_a = 0, count_b = 0;
    // a - starting point   b - ending point
    // loops through each row and each character in the row to check if the maze has only one 'a' and one 'b', and that each row has the correct width
    for(i = 0; *(maze + i) != NULL; i++) {
        for(j = 0; *(*(maze + i) + j) != '\0'; j++) {
            if (*(*(maze + i) + j) == 'a')
                count_a++;
            if (*(*(maze + i) + j) == 'b')
                count_b++;
        }
        if (j != width - 1) {
            return -1;
        }
    }
    // returns -1 if maze has more or less than one 'a' or one 'b'
    if (count_a != 1 || count_b != 1)
        return -1;
    // returns -1 if maze has more or less rows than the expected height
    if (i != height) {
        return -1;
    }
    // returns 0 if the maze passes all checks
    return 0;
}

int load_maze(const char *filename, char ***labirynth)
{
    // Check input arguments are valid
    if(filename == NULL || labirynth == NULL){
        return 1;
    }

    // Open the file for reading
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        return 2;
    }

    // Determine the size of the file
    fseek(fp, 0, SEEK_END);
    int size = (int) ftell(fp);
    if(size == 0){
        fclose(fp);
        return 3;
    }
    rewind(fp);

    // Parse the file to determine the dimensions of the maze
    height = 0, width = 0;
    int row_width = 0;
    char c;
    while (fscanf(fp, "%c", &c) == 1)
    {
        if(c == '\n' || c == '\r'){
            height++;
            if(width == 0){
                width = row_width;
            }
            else if(width != row_width){
                if(row_width == 0)
                    break;
                else {
                    fclose(fp);
                    return 3;
                }
            }
            row_width = 0;
        }
        else{
            row_width++;
        }
    }
    if(row_width > 0){
        height++;
    }
    width++;

    // Make sure the maze is valid (i.e., has exactly one 'a' and one 'b')
    int a_count = 0, b_count = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            fscanf(fp, "%c", &c);
            if(c != '\n'){
                if(c == 'a')
                    a_count++;
                if(c == 'b')
                    b_count++;
            }
        }
    }
    if(a_count != 1 || b_count != 1)
        return 3;

    // Allocate memory for the maze
    *labirynth = malloc(sizeof(char *) * (height + 1));
    if(*labirynth == NULL){
        fclose(fp);
        return 4;
    }
    for(int i=0; i<height; i++){
        *(*labirynth + i) = malloc(sizeof(char) * (width));
        if(*(*labirynth + i) == NULL){
            free_maze(*labirynth);
            fclose(fp);
            return 4;
        }
    }

    // Read in the maze from the file and store it in memory
    rewind(fp);
    int j;
    for(int i=0; i<height; i++){
        for(j=0; j<width; j++){
            fscanf(fp, "%c", &c);
            if(c != '\n'){
                *(*(*labirynth + i) + j) = c;
            }
        }
        *(*(*labirynth + i) + j-1) = '\0';
    }

    *(*labirynth + height) = NULL;
    fclose(fp);
    return 0;
}

int solve_maze(char **maze, int x, int y) {
    // Check if maze is invalid
    if(check_maze((const char **) maze)){
        return -1;
    }

    // Check if exit is found
    if (*(*(maze + y) + x) == 'b') {
        return 1;
    }

    // Mark current position with '*' if it's not the starting position
    if(*(*(maze + y) + x) != 'a')
        *(*(maze + y) + x) = '*';

    // Recursively try to solve maze by exploring neighboring cells
    if (solve_maze(maze, x-1, y) == 1 || solve_maze(maze, x+1, y) == 1 || solve_maze(maze, x, y-1) == 1 || solve_maze(maze, x, y+1) == 1) {
        return 1;
    }

    // Unmark current position with '*' if it's not the starting position
    if(*(*(maze + y) + x) != 'a')
        *(*(maze + y) + x) = ' ';

    // Return 0 if exit not found
    return 0;
}

int main() {
    char *filename = malloc(sizeof(char) * 31);
    if(filename == NULL){
        printf("Failed to allocate memory");
        return 8;
    }

    printf("Enter filename:");
    if(scanf("%s", filename) != 1){
        printf("Incorrect input");
        free(filename);
        return 1;
    }


    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Couldn't open file");
        free(filename);
        return 4;
    }
    fclose(fp);
    char **labirynth;
    int result = load_maze(filename, &labirynth);
    switch (result) {
        case 2:
            printf("Couldn' open file");
            free(filename);
            free_maze(labirynth);
            return 4;

        case 3:
            printf("File corrupted");
            free(filename);
            return 6;

        case 4:
            printf("Failed to allocate memory");
            free(filename);
            return 8;

        default:
            break;
    }

    if(check_maze((const char **) labirynth)){
        printf("File corrupted");
        free_maze(labirynth);
        free(filename);
        return 6;
    }

    int x, y;
    for(int i=0; *(labirynth + i) != NULL; i++){
        for(int j=0; *(*(labirynth + i) + j) != '\0'; j++){
            if(*(*(labirynth + i) + j) == 'a'){
                x = j;
                y = i;
                break;
            }
        }
    }

    result = solve_maze(labirynth, x, y);
    if(result != 1){
        printf("Couldn't find path");
        free_maze(labirynth);
        free(filename);
        return 0;
    }

    for(int i=0; *(labirynth + i) != NULL; i++){
        for(int j=0; *(*(labirynth + i) + j) != '\0'; j++){
            printf("%c", *(*(labirynth + i) + j));
        }
        printf("\n");
    }

    free_maze(labirynth);
    free(filename);
    return 0;
}
