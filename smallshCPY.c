#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>


int check_status_command(char *command){
	char status_command[] = "status\n";
	int result = strcmp(command, status_command);

	if (result == 0){
		return 1;
	}
	else{
		return 0; 
	}
}

int check_for_comment(char characterToCheck){
	char comment[] = "#";
	char *ch;
	ch = &characterToCheck;
	int result = strcmp(ch, comment);
	//printf("result: %d\n", result);
	
	if (result == 0){
		return 1;
	}
	else {
		return 0;
	}

}

void handle_cd(char *command){
	char cd_command[] = "cd\n";
	char cwd[100];
	chdir(getenv("HOME"));
	getcwd(cwd, sizeof(cwd));
	//printf("%s\n", cwd);
}

void handle_status(char *command){
	printf("EXIT CODE: %d\n", getenv("?"));
}

void execute_command(char *command){
	//printf("command was: %s\n", command);
	//printf("length of command was %d\n", strlen(command));
	system(command);
}

void handle_cd_relative(char *command){
	char cwd[100];
	char changeDir[100];
	char result;
	char *directory;

	//get the current working directory
	getcwd(cwd, sizeof(cwd));

	//parse the directory to move to out of the cd command
	directory = strtok(command, " ");
	directory = strtok(NULL, " ");

	//change directory
	result = sprintf(changeDir, "%s/%s", cwd, directory);
	//printf("changing to %s\n", changeDir);
	chdir(changeDir);
}

int check_cd_command(char *command){
	//printf("lenght of cd: %d\n", strlen(command));
	char cd_command[] = "cd\n";
	char longer_command[] = "cd";
	char destCommand[100];
	int result;
	char *temp;

	//copy command to new string before messing with it
	memset(destCommand, '\0', sizeof(destCommand));
	strcpy(destCommand, command);

	//get the first 2 characters from command and compare
	if (strlen(command) > 3){
		temp = strtok(command, " ");
		//printf("temp contains: %s\n", temp);
		//printf("length of temp: %d\n", strlen(temp));

		result = strcmp(temp, longer_command);
		//printf("cd compare result: %d\n", result);

		if (result == 0){
			//printf("command contains %s\n", destCommand);
			//execute cd command
			handle_cd_relative(destCommand);
			return 1;
		}
		else{
			return 0;
		}
	}
	//command contains only cd. Compare to cd_command
	else{
		result = strcmp(command, cd_command);

		if (result == 0){
			//execute the cd command
			handle_cd(command);
			return 1; 
		}
		else{
			return 0; 
		}
	}
}

void check_command_type(char *command){
	int i; 
	for (i = 0; i < strlen(command); i++){
		if (command[i] == '>'){
			printf("> found\n");
		}
		else if (command[i] == '<'){
			printf("< found\n");
		}
		else if (command[i] == '&'){
			printf("& found\n");
		}
	}
}

int main(){
	int x = 100;
	int result;
	char command[2048];
	char tempCommand[2048];
	char exit_command[] = "exit\n";
	char cd_command[] = "cd\n";
	char status_command[] = "status\n";
	pid_t spawnpid = -5;
	int ten = 10;
	int exitMethod;

	while (x == 100){
		Top:
		spawnpid = -5;
		ten = 10;	//remove later
		printf(": ");
		fgets(command, 20, stdin);
		fflush(stdout);

		//check for exit command	
		result = strcmp(command, exit_command);

		//save a copy of the command
		strcpy(tempCommand, command);		

		//exit command
		if (result == 0){
			//user entered exit command
			break;
		}
		//cd command
		else if (check_cd_command(command) == 1){
			//skip to bottom of loop. cd command has been executed
			goto Top;
		}
		//status command
		else if (check_status_command(command) == 1){
			handle_status(command);
		}
		//other command
		else{
			//restore command after getting messed up in check_cd_command()	
			strcpy(command, tempCommand);

			//first make sure that more than 1 character was entered
			if (strlen(command) >= 2){
				//next check if the first character was an #
				result = check_for_comment(command[0]);		
				if (result == 0){
					//fflush(stdout);
					check_command_type(command);
					spawnpid = fork();
					switch (spawnpid){
						case -1:
							perror("Hull Breach!");
							exit(1);
							break;
						case 0:
							ten = ten + 1; 
							//printf("I am the child! ten = %d\n", ten);
							printf("execing %s...\n", command);
							fflush(stdout);
							execlp(command, NULL);
							execute_command(command);
							exit(0);					

							perror("exec");
							return(1);
							
							break;
						default:
							ten = ten - 1;
							//printf("I am the parent! ten = %d\n", ten);
							pid_t exitpid = wait(&exitMethod);
							printf("parent: child exited [%d]\n", exitMethod);
							fflush(stdout);

							//exit(0);
							break;
					}
					//printf("This will be executed by both of us\n");
				}
			}
		}
		fflush(stdout);
	};

return 0;
}
