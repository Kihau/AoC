#include "all_days.h"

#define MAX_FILES_PER_DIR 32
#define MAX_SUBDIRS_PER_DIR 16
#define MAX_DIR_NAME 128
#define MAX_DELETE_SIZE 100000
#define TOTAL_DISK_SPACE 70000000
#define SPACE_NEEDED 30000000

#define DIR_UNLISTED -1
#define DIR_SIZE_UNKNOWN 0

#define FOLDER_ICON " "
// #define FOLDER_ICON ""

typedef struct Directory {
    // Size of all files in the current directory + size of content from all sub directories
    // When the directory was not listed yet, value is set to DIR_UNLISTED
    long content_size;

    // Only sizes of the files are stored.
    long file_sizes;
    // Compute hashcodes instead?
    char dir_name[MAX_DIR_NAME];

    struct Directory *subdirs[MAX_SUBDIRS_PER_DIR];
    int subdirs_len;

    struct Directory *parent_dir;
} Directory; 

static void print_subdirectory_tree(Directory *dir) {
    char indents[8][128] = {};
    int len = 0;

    Directory *p1 = dir->parent_dir;
    Directory *cur = dir;
    while (true) {
        cur = p1;
        p1 = p1->parent_dir;
        if (!p1) break;

        int p1_len = p1->subdirs_len;
        if (p1->subdirs[p1_len - 1] != cur)
            strcpy(indents[len++], "│  ");
        else strcpy(indents[len++], "   ");
    } 

    for (int i = len - 1; i >= 0; i--) {
        printf("%s", indents[i]);
    }

    // print without the newline character
    Directory *p = dir->parent_dir;
    if (p->subdirs[p->subdirs_len - 1] != dir) // and when filesize != 0 (or -1?)
        printf("├─ "FOLDER_ICON"%.*s", (int)strlen(dir->dir_name) - 1 , dir->dir_name);
    else printf("└─ "FOLDER_ICON"%.*s", (int)strlen(dir->dir_name) - 1, dir->dir_name);
    printf("    - %li, %li\n", dir->content_size, dir->file_sizes);

    for (int i = 0; i < dir->subdirs_len; i++) {
        print_subdirectory_tree(dir->subdirs[i]);
    }
}

static void print_directory_tree(Directory *dir) {
    printf(FOLDER_ICON"%.*s", (int)strlen(dir->dir_name) - 1 , dir->dir_name);
    printf("    - %li, %li\n", dir->content_size, dir->file_sizes);
    for (int i = 0; i < dir->subdirs_len; i++) {
        print_subdirectory_tree(dir->subdirs[i]);
    }
}

static Directory *find_subdirectory(Directory *dir, const char *dir_name) {
    for (int i = 0; i < dir->subdirs_len; i++) {
        if (!strcmp(dir->subdirs[i]->dir_name, dir_name)) {
            return dir->subdirs[i];
        }
    }
    return NULL;
}


static void change_dir(Directory **current, const char *dir_name) {
    if (dir_name[0] == '.') {
        // NOTE: Assuming that the input doesn't try to go up the root dir
        *current = (*current)->parent_dir;
    } else {
        Directory *sub_dir = find_subdirectory(*current, dir_name);
        assert(sub_dir && "Directory not found. Is the input correct?");
        *current = sub_dir;
    }
}

static Directory *create_dir(char *dir_name) {
    Directory *dir = malloc(sizeof(Directory));
    dir->content_size = DIR_SIZE_UNKNOWN;
    dir->file_sizes = 0;
    strcpy(dir->dir_name, dir_name);

    // dir->sub_dirs = {};
    dir->subdirs_len = 0;
    dir->parent_dir = NULL;
    
    return dir;
}

static void free_dir(Directory *dir) {
    for (int i = 0; i < dir->subdirs_len; i++) {
        free_dir(dir->subdirs[i]);
    }
    free(dir);
}

static void compute_listing(FILE *input, Directory *dir) {
    char buffer[128] = {};
    int last_pos = 0;

    dir->file_sizes = 0;

    // When no subdirs are detected I cant calculate the overall content size
    while (fgets(buffer, sizeof(buffer), input)) {
        if (buffer[0] == '$') {
            fseek(input, last_pos, SEEK_SET);
            return;
        } else if (buffer[0] == 'd') {
            // ptr + 01234........
            // ptr = dir something
            Directory *sub_dir = create_dir(buffer + 4);
            sub_dir->parent_dir = dir;

            dir->subdirs_len++;
            assert(dir->subdirs_len < MAX_SUBDIRS_PER_DIR && "Max subdirs limit exceded");
            dir->subdirs[dir->subdirs_len - 1] = sub_dir;
        } else {
            int i = -1;
            while (buffer[++i] != ' ');
            char filesize_buf[16] = {};
            strncpy(filesize_buf, buffer, i);
            dir->file_sizes += atoi(filesize_buf);
        }

        last_pos = ftell(input);
    }
}

static long find_part1_sum(Directory *dir) {
    long sum = 0;

    if (dir->content_size <= MAX_DELETE_SIZE)
        sum += dir->content_size;

    for (int i = 0; i < dir->subdirs_len; i++) {
        sum += find_part1_sum(dir->subdirs[i]);
    }

    return sum;
}

static long find_min_dirsize(Directory *dir, const long min_del, int curr_min) {
    if (dir->content_size < curr_min && dir->content_size > min_del)
        curr_min =  dir->content_size;

    for (int i = 0; i < dir->subdirs_len; i++) {
        curr_min = find_min_dirsize(dir->subdirs[i], min_del, curr_min);
    }

    return curr_min;
}

static long find_part2_dirsize(Directory *dir) {
    int space_left = TOTAL_DISK_SPACE - dir->content_size;
    return find_min_dirsize(dir, SPACE_NEEDED - space_left, dir->content_size);
}

// Find total sum size of directories marked for deletation
void day7(FILE *input) { 
    Directory *root = create_dir("/\n");
    Directory *current = root;

    char buffer[128] = {};
    // Ignore first line since it (presumably) is "$ cd /"
    fgets(buffer, sizeof(buffer), input);

    while (fgets(buffer, sizeof(buffer), input)) {
        if (buffer[2] == 'l' && current->content_size == DIR_SIZE_UNKNOWN) {
            compute_listing(input, current);
            // After the listing was completed, increase content size of all parent directories
            long file_sizes = current->file_sizes;
            Directory *temp = current;
            while (temp) {
                temp->content_size += file_sizes;
                temp = temp->parent_dir;
            }
        } else if (buffer[2] == 'c') {
            // ptr + "012345......."
            // ptr = "$ cd some_dir"
            change_dir(&current, buffer + 5);
        } 
        // else assert(false && "Incorrect command. Is the input correct?");
    }

    // This is pretty cool :)
    print_directory_tree(root);
    long total_size = find_part1_sum(root);
    long min_dir_size = find_part2_dirsize(root);

    printf("PART 1: Total (most) size = %li\n", total_size);
    printf("PART 2: Deleted directory size = %li\n", min_dir_size);

    free_dir(current);
}
