#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

int check_cd_command(char *command){
	//printf("%d\n", strlen(command));
	char cd_command[] = "cd\n";
	int result = strcmp(command, cd_command);

	if (result == 0){
		return 1; 
	}
	else{
		return 0; 
	}
}

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

//void spawn_child(){}

int main(){
	int x = 100;
	int result;
	char command[2048];
	char exit_command[] = "exit\n";
	char cd_command[] = "cd\n";
	char status_command[] = "status\n";
	pid_t spawnpid = -5;
	int ten = 10;
	int exitMethod;

	while (x == 100){
		spawnpid = -5;
		ten = 10;	//remove later
		printf(": ");
		fgets(command, 20, stdin);
		fflush(stdout);
	
		result = strcmp(command, exit_command);

		//exit command
		if (result == 0){
			//user entered exit command
			break;
		}
		//cd command
		else if (check_cd_command(command) == 1){
			handle_cd(command);
		}
		//status command
		else if (check_status_command(command) == 1){
			handle_status(command);
		}
		//other command
		else{	
			//first make sure that more than 1 character was entered
			if (strlen(command) >= 2){
				//next check if the first character was an #
				result = check_for_comment(command[0]);		
				if (result == 0){
					//printf("%s", command);
					//fflush(stdout);
					spawnpid = fork();
					switch (spawnpid){
						case -1:
							perror("Hull Breach!");
							exit(1);
							break;
						case 0:
							ten = ten + 1; 
							printf("I am the child! ten = %d\n", ten);
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
					printf("This will be executed by both of us\n");
					//probably delete this:
					//execute_command(command);				
				}
			}
		}
		fflush(stdout);	
	};

return 0;
}
