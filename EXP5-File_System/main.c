/*
 *	main.c
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#include "../mywlib/fs/kernel.h"
#include "../mywlib/fs/fat16.h"

void operate(){
	char command[20];
	strcpy(command,"ls");
	while(strcmp(command, "exit")){
		printf("mywfs>>~%s$ ",currentdir);
		scanf("%s", command);
		if(!strcmp(command, "ls")){
			ls();
		}else if(!strcmp(command, "format")){
			format();
		}else if(!strcmp(command, "mkdir")){
			char name[11];
			scanf("%s", name);
			mkdir(name);
		}else if(!strcmp(command, "cd")){
			char dirname[110];
			scanf("%s",dirname);
			cd(dirname);
		}else if(!strcmp(command, "rmdir")){
			char dirname[110];
			scanf("%s",dirname);
			rmdir(dirname);
		}else if(!strcmp(command, "create")){
			char filename[110];
			scanf("%s",filename);
			create(filename);
		}else if(!strcmp(command, "open")){
			char filename[110];
			scanf("%s",filename);
			open(filename);
		}else if(!strcmp(command, "close")){
			close(fileopenptr);
		}else if(!strcmp(command, "rmfile")){
			char filename[110];
			scanf("%s",filename);
			rmfile(filename);
		}else if(!strcmp(command, "write")){
			write(fileopenptr);
		}else if(!strcmp(command, "read")){
			read(fileopenptr);
		}else if(!strcmp(command, "h")){
			printf("myw file system\n");
			printf("version: %1.2f\n", VERSION);
			printf("Useage:[command] -[option]\n");
			printf("Options:\n");
			printf("    h       --help              give this help\n");
			printf("    ls      --list-files        list files and directory on current directory\n");
			printf("    format  --format            format this file system\n");
			printf("    cd      --change-directory	change current directory, example cd ./fs/include \n");
			printf("    mkdir   --make-directory    make directory in current path, example mkdir fs\n");
			printf("    create  --create            create a file\n");
			printf("    close   --close             close current opened file\n");
			printf("    open    --open              open a file at current directory\n");
			printf("    write   --write             write data to opened file\n");
			printf("    read    --read              read data from a opened file\n");
			printf("    rmdir  --delete directory  delete a directory at current directory example rmdir fs\n");
			printf("    rmfile --delete file       delete a file at current directory, example rmfile fs.txt\n");
			printf("    exit    --exit              exit this file system\n");
			printf("\n");
		} else if(!strcmp(command, "exit")){
			continue;
		}else {
			printf("no such command!\nget help by input h\n");
		}
	}
	
}

int main(){
	startsys();
	operate();
	existsys();
	return 0;
}
