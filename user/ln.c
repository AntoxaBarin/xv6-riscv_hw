#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc == 4) {
    if (strcmp(argv[1], "-s") == 0) {
      int symlink_res = symlink(argv[3], argv[2]);
      if (symlink_res < 0) {
        fprintf(2, "link %s %s: failed\n", argv[2], argv[3]);
        exit(1);    
      } 
      exit(0);
    }
  }
  else if (argc != 3) {
    fprintf(2, "Usage: hard link: ln old new\nsymbolic link: ln -s path target\n");
    exit(1);
  }
  
  if(link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
