#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <errno.h>


int check_status_command(char *command){
	//printf("check_status_command\n");
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
	//printf("check_for_comment\n");
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
	//printf("handle cd\n");
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
	//printf("calling process is %d\n", getpid());
	//printf("parent process is %d\n", getppid());
	system(command);
}

void handle_cd_relative(char *command){
	//printf("handle_cd_relative\n");
	char cwd[100];
	char changeDir[100];
	char result;
	char *directory;
	int x;

	//get the current working directory
	getcwd(cwd, sizeof(cwd));

	//parse the directory to move to out of the cd command
	directory = strtok(command, " ");
	directory = strtok(NULL, "\n");
	//printf("directory %s\n", directory);

	//change directory
	result = sprintf(changeDir, "%s/%s", cwd, directory);
	//result = sprintf(changeDir, "/%s", directory);
	printf("changing to %s\n", changeDir);
	//fflush(stdout);
	x = chdir(changeDir);
	printf("operation %d\n", x);
	printf("the outcome of chdir() was %s\n", strerror(errno));
	getcwd(cwd, sizeof(cwd));
	printf("changed to %s\n", cwd);
	fflush(stdin);
}

int check_cd_command(char *command){
	//printf("check_cd_command\n");
	char cd_command[] = "cd\n";
	char longer_command[] = "cd";
	char destCommand[100];
	int result;
	char *temp;

	//copy command to new string before messing with it
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
			//printf("directory changed. returning...\n");
			//fflush(stdout);
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

/*
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
		else if (command[i] == '#'){
			printf("comment found");
		}
	}
}
*/

int is_background(char *command){
	int i;
	for (i = 0; i < strlen(command); i++){
		if (command[i] == '&'){
			return 1;
		}
	}
	return 0;
}

//checks for $ in the command
int is_pid_command(char *command){
	int i;
	for (i = 0; i < strlen(command); i++){
		if (command[i] == '$'){
			return 1;
		}
	}
	return 0;
}

//this function takes the $$ at the end of a command 
//and replaces it with the pid
void append_pid(char *command){
	int parent_pid = getpid();
	char newCommand[100];
	//char *newCommand;
	char *tempString;
	tempString = strtok(command, "$");
	sprintf(newCommand, "%s%d", tempString, parent_pid);
	strcpy(command, newCommand);
}

void handle_input(char *command){
	pid_t spawnpid = -5;
	int result;
	int exitMethod;
	int child_pid;
	int parent_pid;
	pid_t background_pid = -5;
	char tempCommand[2048];
	char cd_command[] = "cd\n";
	char status_command[] = "status\n";

	
		//check for $$ in a command. If present replace $$ with pid
		if (is_pid_command(command) == 1){
			append_pid(command);
			//printf("returned %s\n", command);
		}

		//save a copy of the command
		strcpy(tempCommand, command);
		//printf("moving on..\n");

		if (check_cd_command(command) == 1){
			//cd command has been executed
			fflush(stdin);
			return;
		}
		//status command
		else if (check_status_command(command) == 1){
			handle_status(command);
			return;
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
					//check_command_type(command);
					spawnpid = fork();
					switch (spawnpid){
						case -1:
							perror("Hull Breach!");
							exit(1);
							return;
							//break;
						case 0:
							child_pid = getpid(); 

							//if this is a background process, save the pid
							if (is_background(command) == 1){
								background_pid = child_pid;
							}

							execlp(command, NULL);
							execute_command(command);
							exit(0);					

							perror("exec");
							//return(1);
							return;

							//break;
						default:
							parent_pid = getpid();
							pid_t exitpid = wait(&exitMethod);
							//printf("parent: child exited [%d]\n", exitMethod);
							//fflush(stdout);

							//exit(0);
							break;
					}
				}
			}
		}
		fflush(stdout);
}

int main(){
	int x = 100;
	int result;
	char command[2048];
	char tempCommand[2048];
	char exit_command[] = "exit\n";
	char cd_command[] = "cd\n";
	char status_command[] = "status\n";
	//pid_t spawnpid = -5;
	int ten = 10;
	int exitMethod;
	int child_pid;
	int parent_pid;
	pid_t background_pid = -5;


	while (x == 100){
		//printf("At the Top\n");
		fflush(stdout);
		//spawnpid = -5;
		//ten = 10;	//remove later
		printf(": ");
		fflush(stdout);
		fgets(command, 20, stdin);
		fflush(stdout);
		//fflush(stdin);

		//check for exit command	
		result = strcmp(command, exit_command);		

		//exit command
		if (result == 0){
			//user entered exit command
			break;
		}

		//do things with the input
		handle_input(command);
		
	};
	//kill(background_pid, SIGTERM);

/*
	if (background_pid == 0){
		kill(background_pid, SIGTERM);
	}
*/

return 0;
}
