#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>



typedef struct entry {
  char *name;
  int is_dir;
  struct entry *next;
} entry_t;

int walk(const char* directory, const char* prefix) {
  entry_t *head = NULL, *current = NULL, *iter;
  size_t size = 0, index;
  struct stat stats;

  struct dirent *file_dirent;
  DIR *dir_handle;

  char *full_path, *segment, *pointer, *next_prefix;

  dir_handle = opendir(directory);
  if (!dir_handle) {
    fprintf(stderr, "Cannot open directory \"%s\"\n", directory);
    return -1;
  }

  while ((file_dirent = readdir(dir_handle)) != NULL) {
    if (file_dirent->d_name[0] == '.') {
      continue;
    }
    current = malloc(sizeof(entry_t));
    current->name = strcpy(malloc(strlen(file_dirent->d_name)), file_dirent->d_name);
    current->is_dir = file_dirent->d_type == DT_DIR;
    current->next = NULL;

    if (head == NULL) {
      head = current;
    } else{
      current->next = head;
      head = current;
    }

    size++;
  }

  closedir(dir_handle);
  if (head == NULL) {
    return 0;
  }

  for (index = 0; index < size; index++) {
    if (index == size - 1) {
      pointer = "└──";
      segment = "   ";
    } else {
      pointer = "├──";
      segment = "│  ";
    }
    
    char *path = malloc(strlen(directory) + strlen(head->name) + 2);
    sprintf(path, "%s/%s", directory, head->name);
    lstat(path, &stats);
    free(path);
    printf("%s%s[", prefix, pointer);
    switch (stats.st_mode & S_IFMT) {  
        case S_IFREG:  printf("-"); break;
        case S_IFDIR:  printf("d"); break;
        case S_IFCHR:  printf("c"); break;
        case S_IFBLK:  printf("b"); break;
        case S_IFLNK:  printf("l"); break;
        case S_IFIFO:  printf("p"); break;
        case S_IFSOCK: printf("s"); break;
        default:       printf("?"); break; 
        }
    printf( (stats.st_mode & S_IRUSR) ? "r" : "-");
    printf( (stats.st_mode & S_IWUSR) ? "w" : "-");
    printf( (stats.st_mode & S_IXUSR) ? "x" : "-");
    printf( (stats.st_mode & S_IRGRP) ? "r" : "-");
    printf( (stats.st_mode & S_IWGRP) ? "w" : "-");
    printf( (stats.st_mode & S_IXGRP) ? "x" : "-");
    printf( (stats.st_mode & S_IROTH) ? "r" : "-");
    printf( (stats.st_mode & S_IWOTH) ? "w" : "-");
    printf( (stats.st_mode & S_IXOTH) ? "x" : "-");
    printf("\t%s", getpwuid(stats.st_uid)->pw_name);
    printf("\t%s", getgrgid(stats.st_gid)->gr_name);
    printf("\t%ld", (long) stats.st_size);
    printf("\t%s]\n", head->name);

    if (head->is_dir) {
    	full_path = malloc(strlen(directory) + strlen(head->name) + 2);
    	sprintf(full_path, "%s/%s", directory, head->name);
    	next_prefix = malloc(strlen(prefix) + strlen(segment) + 1);
    	sprintf(next_prefix, "%s%s", prefix, segment);

    	walk(full_path, next_prefix);
    	free(full_path);
    	free(next_prefix);
    } 

    current = head;
    head = head->next;

    free(current->name);
    free(current);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  char* directory = argc > 1 ? argv[1] : ".";
  printf("%s\n", directory);

  walk(directory, "");

  return 0;
}
