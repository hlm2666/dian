#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void help()
{
    printf("Usage: ./main [OPTION]...\n");
    printf("Options:\n");
    printf("  -h, --help\t\tDisplay this help message and exit\n");
}
int main(int argc,char *argv[])
{
    
    if(argc==1)
    {
        printf("No arguments given\n");
    }
    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0)
        {
            help();
            return 0;
        }
    }
    printf("Invalid argument\n");
    return 0;
}