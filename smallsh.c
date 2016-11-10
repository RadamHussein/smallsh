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
	
	//check if cd has a parameter
	if (strlen(command) > 3){
		printf("passing control back to main\n");
		return 0;		
	}
	else{
		int result = strcmp(command, cd_command);

		if (result == 0){
			//printf("!! cd command received !!\n");
			char cwd[100];
			//printf("Home: %s\n", getenv("HOME"));
			chdir(getenv("HOME"));
			getcwd(cwd, sizeof(cwd));
			//printf("%s\n", cwd);
			return 1; 
		}
		else{
			return 0; 
		}
	}
}

int check_status_command(char *command){
	char status_command[] = "status\n";
	int result = strcmp(command, status_command);

	if (result == 0){
		printf("EXIT CODE: %d\n", getenv("?"));
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
	
	if (result == 0){
		return 1;
	}
	else {
		return 0;
	}

}

void handle_cd(char *command){
	//printf("%d\n", strlen(command));
	char cd_command[] = "cd\n";
	//printf("!! cd command received !!\n");
	char cwd[100];
	//printf("Home: %s\n", getenv("HOME"));
	chdir(getenv("HOME"));
	getcwd(cwd, sizeof(cwd));
	//printf("%s\n", cwd);
}

void handle_status(char *command){

}

void execute_command(char *command){
	//printf("command was: %s\n", command);
	//printf(getenv(command));	
	system(command);
}

int main(){
	int x = 100;
	int result;
	char command[2048];
	char exit_command[] = "exit\n";
	char cd_command[] = "cd\n";
	char status_command[] = "status\n";

	while (x == 100){
		printf(": ");
		fgets(command, 20, stdin);
		fflush(stdout);
	
		result = strcmp(command, exit_command);

		if (result == 0){
			//user entered exit command
			break;
		}
		else{
			if (check_cd_command(command) == 0 && check_status_command(command) == 0){
				
				//check_status_command(command);			
			
				result = check_for_comment(command[0]);
				if (result == 0){
					//printf("%s", command);
					fflush(stdout);
					execute_command(command);				
				}
			}

			fflush(stdout);	
		}
	};

return 0;
}
