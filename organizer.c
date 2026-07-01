

#include <stdio.h>     
#include <string.h>    
#include <dirent.h>     
#include <sys/stat.h>   

#ifdef _WIN32
    #include <direct.h>   /
    #include <windows.h>  
    #define MAKE_DIR(path) mkdir(path)
    #define SLEEP_SECONDS(s) Sleep((s) * 1000)  
#else
    #include <unistd.h>   
    #define MAKE_DIR(path) mkdir(path, 0755)
    #define SLEEP_SECONDS(s) sleep(s)
#endif


#define TWO_HOURS 7200

const char *get_category(const char *filename) {

  
    const char *dot = strrchr(filename, '.');

   
    if (dot == NULL) {
        return "Others";
    }

    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0 ||
        strcmp(dot, ".png") == 0 || strcmp(dot, ".gif") == 0) {
        return "Images";
    }

    if (strcmp(dot, ".pdf") == 0 || strcmp(dot, ".doc") == 0 ||
        strcmp(dot, ".docx") == 0 || strcmp(dot, ".txt") == 0) {
        return "Documents";
    }

    if (strcmp(dot, ".mp4") == 0 || strcmp(dot, ".mkv") == 0 ||
        strcmp(dot, ".avi") == 0) {
        return "Videos";
    }

    if (strcmp(dot, ".mp3") == 0 || strcmp(dot, ".wav") == 0) {
        return "Music";
    }

    if (strcmp(dot, ".zip") == 0 || strcmp(dot, ".rar") == 0) {
        return "Archives";
    }

    if (strcmp(dot, ".c") == 0 || strcmp(dot, ".py") == 0 ||
        strcmp(dot, ".java") == 0 || strcmp(dot, ".html") == 0) {
        return "Code";
    }

    return "Others";
}


void make_folder_if_needed(const char *path) {
    struct stat st;

    if (stat(path, &st) == -1) {
        MAKE_DIR(path);
        printf("Created folder: %s\n", path);
    }
}


void organize_folder(const char *folder_path) {

    DIR *dir = opendir(folder_path);
    if (dir == NULL) {
        printf("ERROR: Could not open folder %s\n", folder_path);
        return;
    }

    int files_moved = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        char *name = entry->d_name;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
            continue;
        }

        
        if (name[0] == '.') {
            continue;
        }

        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", folder_path, name);

       
        struct stat st;
        stat(full_path, &st);

        if (S_ISDIR(st.st_mode)) {
            
            continue;
        }

       
        const char *category = get_category(name);

        
        char category_path[1024];
        snprintf(category_path, sizeof(category_path), "%s/%s", folder_path, category);
        make_folder_if_needed(category_path);

       
        char destination[1024];
        snprintf(destination, sizeof(destination), "%s/%s", category_path, name);

        
        if (rename(full_path, destination) == 0) {
            printf("Moved: %s -> %s/\n", name, category);
            files_moved++;
        } else {
            printf("ERROR: Could not move %s\n", name);
        }
    }

    closedir(dir);
    printf("Done. %d file(s) organized.\n\n", files_moved);
}


int main(int argc, char *argv[]) {

    setvbuf(stdout, NULL, _IONBF, 0);


    const char *target_folder = ".";
    if (argc > 1) {
        target_folder = argv[1];
    }

    printf("Smart File Organizer started.\n");
    printf("Watching folder: %s\n", target_folder);
    printf("It will organize files now, then again every 2 hours.\n\n");

   
    while (1) {
        organize_folder(target_folder);
        SLEEP_SECONDS(TWO_HOURS);
    }

    return 0;
}
