#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#define MAX 1000

void print_cwd(void);
char* remove_spaces(int, char*, char*);
void create_file(char*);
void count_wc(char*);
static volatile sig_atomic_t gotSigint = 0;
static void handler(int sig)
{
	if (sig == SIGINT) exit(EXIT_SUCCESS);
	if (sig == SIGKILL) exit(EXIT_SUCCESS);
}
int main (int argc, char *argv[]) {
int batchmode = 0;
char* batchfile;
char bm_command[MAX];
start:
//start an infinite loop
	while(!batchmode){ 	 
        	if(signal(SIGINT, handler) == SIG_ERR){
				printf("sigint error...");
				exit(EXIT_SUCCESS);
			}						
		    const char semi[2] = ";";
		    const char space[2] = " ";
		    pid_t child_pid;
        	char i[MAX]; //This will be our input.
        	print_cwd();        	        
        	
        	
        	if(fgets(i, MAX, stdin) == NULL){
        		printf("\n");
        		exit(EXIT_SUCCESS);
        	}        
	        	        	
         	int len = strlen(i);  
         	if(len > 512){
         		printf("You exceeded the 512 character maximum.\n\n");
         		break;
         	}
        	if(len > 0 && i[len-1] == '\n') i[len-1] = '\0'; // For the sake of it, I'll get rid of every \n in any given input.        	
        	// This is where the fun begins peepoSmile        	
          	char* sc_token = strtok(i, semi);
          	char* sc_token2 = sc_token;          	
        	while(sc_token != NULL) {        	
        		char result[MAX];  		
        		int skipflag = 0;
        		sc_token = remove_spaces(len, sc_token, result);
        		if(strcmp(sc_token, "")==0){
        			skipflag = 1;
        		}
        		if(strcmp(sc_token, "quit")==0){ //quit command.
        			kill(getpid(), SIGINT); // Sending a kill signal.
        		}         	
        		
        		else if((strcmp(sc_token, "ls")==0)){ // ls command.
        			
        			int stat_loc;
        			char clsl[] = "ls";
        	    	char* argLsL[] = {"ls", NULL};
        	    	int child_pid = fork();        	    	
        	    	if(child_pid == 0){
        	    		execvp(clsl, argLsL);
        	    		printf("This won't be printed if execvp is successful.");        	    	
        	    	} else if(child_pid < 0){
        	    		printf("Fork failed.");        	    	
        	    	} else {
        	    		waitpid(child_pid, &stat_loc, WUNTRACED);
        	    		printf("\n");
        	    	}	
        		}         		
        		else if(strcmp(sc_token, "ls-l")==0){ // ls -l command.
        			int stat_loc;
        			char clsl[] = "ls";
        	    	char* argLsL[] = {"ls", "-l", NULL};
        	    	int child_pid = fork();        	    	
        	    	if(child_pid == 0){
        	    		execvp(clsl, argLsL);
        	    		printf("This won't be printed if execvp is successful.");        	    	
        	    	} else if(child_pid < 0){
        	    		printf("Fork failed.");        	    	
        	    	} else {
        	    		waitpid(child_pid, &stat_loc, WUNTRACED);
        	    		printf("\n");
        	    	}	
        	  	}         	  	
        	  	// For reasons I really can't explain after trying to debug for a whole day and crying myself to sleep, I can't really seem to get 
        	  	// the touch command to work with any other commands simultaneously or using ;. Anything cast after touch will be ignored... at least the command works.
        	  	else if(strncmp(sc_token, "touch", strlen("touch"))==0){ 
        	  		printf("\n");
        	  		char* sp_token = strtok(sc_token2, space);
        	  		sp_token = strtok(NULL, space); // Advancing it once so that it never iterates at touch.        	  		
        	  		while(sp_token != NULL){
        	  			create_file(sp_token);
        	  			sp_token = strtok(NULL, space);
        	  		}
        	  		printf("\n");
        	  	}        	  	
        	  	else if(strncmp(sc_token, "cat", strlen("cat"))==0){ 
        	  		printf("\n");
        	  		int c;
        	  		char* sp_token = strtok(sc_token2, space);
        	  		sp_token = strtok(NULL, space); // Advancing it once so that it never iterates at cat.        	  		
        	  		// From my reading, cat is supposed to have the functions that if a file exists, it reads its contents,
        	  		// create files if the file does not exist, read multiple files.        	  		
        	  		while(sp_token != NULL){
        	  			FILE* fp = fopen(sp_token, "r+");
        	  			if(fp == NULL){
        	  				create_file(sp_token);
        	  				printf("Please enter the desired text to populate file %s.\n", sp_token);
        	  				char populate_file[MAX] = "Hello! I am dummy text.";
        	  				fgets(populate_file, MAX, stdin);
        	  				FILE* fp = fopen(sp_token, "w");
        	  				printf("Successfully wrote to the file.\n\n");
        	  				fputs(populate_file, fp);
        	  				fclose(fp);        	  				
        	  			} else {
        	  				printf("File \"%s\" reads as follows:\n\n", sp_token);
        	  				while(1){
        	  					c = fgetc(fp);
        	  					if(feof(fp)){
        	  						break;
        	  					}
        	  					printf("%c", c);
        	  				}
        	  				fclose(fp);
        	  					printf("\n\n");
        	  			}
        	  			sp_token = strtok(NULL, space);
        	  		}
        	  		printf("\n");
        	  	}        	  	
        	  	else if(strncmp(sc_token, "grep", strlen("grep"))==0){
        	  		printf("\n");
        	  		char temp[MAX];
        	  		char* sp_token = strtok(sc_token2, space);
        	  		char* string_to_search;
        	  		sp_token = strtok(NULL, space); // Advancing it once so it can tell me what the needed string is.
        	  		string_to_search = sp_token;
        	  		sp_token = strtok(NULL, space); //Advancing it again so it can find the needed file.
        	  		FILE* fp = fopen(sp_token, "r");
        	  		int lineno = 1;
        	  		int find_result = 0;
        	  		if((fp = fopen(sp_token, "r"))==NULL){
        	  			printf("This file does not exist...");
        	  			break;
        	  		}        	  		
        	  		printf("Searching for string \"%s\" in file \"%s\"...\n\n", string_to_search, sp_token);
        	  		while(fgets(temp, MAX, fp) != NULL){
        	  			if((strstr(temp, string_to_search)) != NULL) {
        	  				printf("A match was found on line: %d!\n", lineno);
        	  				printf("\n%s\n", temp);
        	  				find_result++;
        	  			}
        	  			lineno++;
        	  		}
        	  		
        	  		if(find_result == 0) printf("\nNo matches found.\n\n");
        	  		if(fp) fclose(fp);
        	  	}        	  	
        	  	else if(strncmp(sc_token, "wc", strlen("wc"))==0){ 
        	  		printf("\n");
        	  		FILE *fp;
        	  		int linecount = 0;
        	  		int charcount = 0;
        	  		int wordcount = 0;
        	  		int pastc = 0;
        	  		char* sp_token = strtok(sc_token2, space);
        	  		char* arg2;
        	  		sp_token = strtok(NULL, space); // Advancing it once so it can tell me what the needed string is.		
        	  		if(sp_token == NULL) break;
        	  		arg2 = sp_token;       	  		   	
        	  		sp_token = strtok(NULL, space); 
        	  		if((sp_token == NULL)){
	        	  		printf("You did not enter a valid path...\n");
    	    	  		break; // This means there was no path provided.
    	    	  	}
        	  		while(sp_token != NULL){
        	  			fp = fopen(sp_token, "r");
        	  			wordcount = 0;
        	  			charcount = 0;
        	  			linecount = 0;
        	  			if((sp_token == NULL) || (fp == NULL)){
        	  				printf("You did not enter a valid path...\n");
        	  				break;
        	  			} else if((strcmp(arg2, "-w"))==0) { // -w is just how many words there are.
        	  				while(1){
        	  					int c = fgetc(fp);
        	  					if(feof(fp)){
        	  						break;
        	  					}
        	  					charcount++;
        	  					if(pastc == ' ' && c != ' ') wordcount++;      	  						
        	  				}
        	  					fclose(fp);
        	  					if((charcount > 0) && (wordcount == 0)) wordcount++; //Checking in case there's only one word...
        	  					printf("In the file \"%s\", there is/are %d word(s).\n\n", sp_token, wordcount);
				       	  		sp_token = strtok(NULL, space);				        	  		
				    	} else if((strcmp(arg2, "-l"))==0) { // -l is how many lines there are...
        	  				while(1){
        	  					int c = fgetc(fp);
        	  					if(feof(fp)){
        	  						break;
        	  					}
        	  					if(c == '\n') linecount++;        	  						
        	  				}
        	  					fclose(fp);
        	  					if((charcount > 0) && (linecount == 0)) linecount++; //Checking in case there's only one word...
        	  					printf("In the file \"%s\", there is/are %d line(s).\n\n", sp_token, linecount);
				      	  		sp_token = strtok(NULL, space);				        	  		
        	  			} else if((strcmp(arg2, "-c"))==0) { // -c is how many bytes there are...
        	  				while(1){
        	  					if(feof(fp)){
        	  						break;
        	  					}
        	  					charcount++;        	  						
        	  				}
        	  					fclose(fp);
        	  					printf("In the file \"%s\", there is/are %d byte(s).\n\n", sp_token, charcount);
				       	  		sp_token = strtok(NULL, space);
        	  			} else if((strcmp(arg2, "-m"))==0) { // and -m is how many characters there are!
        	  				while(1){
        	  					if(feof(fp)){
        	  						break;
        	  					}
        	  					charcount++;        	  						
        	  				}
        	  					fclose(fp);
        	  					printf("In the file \"%s\", there is/are %d character(s).\n\n", sp_token, charcount);
				       	  		sp_token = strtok(NULL, space);
				       	  		
        	  			}    	  			
					}
        	  	} else if((strncmp(sc_token, "batch", strlen("batch")))==0){
        	  		char* sp_token = strtok(sc_token2, space);
        	  		sp_token = strtok(NULL, space);
        	  		if (sp_token == NULL){
        	  			printf("No file specified.\n");
        	  			break;
        	  		}
        	  		else {
        	  			batchfile = sp_token;
        	  			batchmode = 1;
        	  			break;
        	  		}
        	  		
        	  	}
        	  	else {       	  	
        	  		if(skipflag == 0) printf("\n'%s' is an unrecognized command.\n\n", sc_token);
        	  	 	else if(skipflag == 1)	skipflag = 0;
        		}        		
        		sc_token = strtok(NULL, semi);
        		sc_token2 = sc_token;        		
        	}
        }
	
	while(batchmode){
		FILE* fp;
		// printf("%s\n", batchfile);
		// fp = fopen(batchfile, "r");
		//Sorry, this doesn't work.
		/*
		while(1){
			printf("in while loop");
        	int c = fgetc(fp);
        	if(feof(fp)){
        		break;
        	}
        		bm_command[z] = c;
        		z++;
        		printf("point 2");
        	  	if(c == '\n'){
        	  		if(strncmp(bm_command, "ls", strlen("ls"))){
        	  			printf("peepoSmile");
        	  			goto ls;
        	  		}        	  						
        	}
        	  	fclose(fp);
        */
        batchmode = 0;
        goto start;
        break;
	}	
}

void print_cwd(void) { // I'm gonna thank EE3223 for teaching me how to do C++ classes.	
	char cwd[FILENAME_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s # ", cwd);
        } else {
            perror("getcwd() error...");
        }        
}
char* remove_spaces(int a, char* b, char* c) {	
	int j = 0;	
	int start = 0;	
	for(j = 0; j < a; j++){
    	if(b[j] == ' '){
        	continue;
        } else {
        	c[start] = b[j];
        	start++;
        }
    }    
    b = c;
    return b;
}
void create_file(char* a){
	FILE *fp;
	fp = fopen(a, "w");
    fclose(fp);
    printf("Successfully created file \"%s\".\n", a);
}
