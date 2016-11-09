#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
	int x = 100;
	int result;
	char command[20];
	char exit_command[] = "exit\n";

	while (x == 100){
		printf(": ");
		fgets(command, 20, stdin);
	
		result = strcmp(command, exit_command);

		if (result == 0){
			printf("You want to exit?");
		}
		else{
			printf("%s", command);		
		}
	};

return 0;
}
