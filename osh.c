#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80 /* The maximum length command */
#define HISTORY_NUMBER 11 /* The maximum histories user can see & use */
#define PARALLEL_PID 1 /* parallel the child process */
#define SERIAL_PID 0 /* serial the child process */

int main(void){
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    FILE *file;
    char *filename = "osh_history.txt";
    char history[HISTORY_NUMBER][MAX_LINE];
    int noc = 0; // number of commands

    while (should_run)
    {
        usleep(10000);
        
        printf("osh>");
        noc++;
        file = fopen(filename,"a+");
        char c;
        int j = 1; // the number of words
        int f = 0; // serial or parallel
        
        // input command
        fprintf(file, "%d ", noc);

        fgets(history[noc%HISTORY_NUMBER], MAX_LINE, stdin);//stdin means input by keybord
        fputs(history[noc%HISTORY_NUMBER], file);
        // the last word end by '\n'
        history[noc%HISTORY_NUMBER][strlen(history[noc%HISTORY_NUMBER])-1] = 0;

        fclose(file);

        fflush(stdout);

        if (history[noc%HISTORY_NUMBER][0] == '!'){
            if (history[noc%HISTORY_NUMBER][1] == '!'){
                if (noc == 1){
                    printf("No commands in history.\n");
                    noc--;continue;
                }
                else
                    strcpy(history[noc%HISTORY_NUMBER],history[(noc-1)%HISTORY_NUMBER]);
            }else{
                int index = history[noc%HISTORY_NUMBER][1] - '0';
                if (noc < index){
                    printf("No such command in history.\n");
                    noc--;continue;
                }
                else
                    strcpy(history[noc%HISTORY_NUMBER],history[(index)%HISTORY_NUMBER]);
            }
        }

        /* get the first part */
        char *s = {" "};
        char temp[MAX_LINE];
        strcpy(temp, history[noc%HISTORY_NUMBER]);
        args[0] = strtok(temp, s);

        for (int i = 1; args[i-1] != NULL; i++){
            // printf( "%d %s\n", j, args[i-1]);
            j++;
            args[i] = strtok(NULL, s);
        }
        j--;
        
        /* the last line should be NULL */
        if(args[j-1][0] == '&'){
            f = 1;
            args[j-1] = NULL;
        }else{
            f = 0;
            args[j] = NULL;
        }
        /* thanks to https://blog.csdn.net/joker0910/article/details/6828319 */

        if (args[0][0] == 0)
            continue; // nothing then continue
        
        if (strcmp(history[noc%HISTORY_NUMBER],"exit") == 0)
            break; // exit to break
        
        if (strcmp(args[0],"history") == 0){
            int i = noc%HISTORY_NUMBER;
            if (noc < HISTORY_NUMBER){
                for(int k = i-1; k >= 1; k--)
                    printf("%d %s\n", k, history[k]);
            }else{
                for(int k = HISTORY_NUMBER-1; k >= 1; k--){
                    printf("%d %s\n", k, history[i%HISTORY_NUMBER]);
                    if(--i < 0) i += HISTORY_NUMBER;
                }
            }
            continue;
        }

        pid_t pid;
        /* fork a child process */
        pid = fork();
        if (pid < 0) { /* error occurred */ 
            printf("Fork Failed\n"); 
            return 1;
        }else if (pid == 0) { /* child process */
            // sleep(5);
            execvp(args[0], args);

            // without exit, the illegal input can't be solved 
            exit(EXIT_SUCCESS);

        }else { /* parent process */
            if (f){
                /* parent will not wait for the child to complete */
                // Orphans
                // wait(NULL);
            }else{
                /* parent will wait for the child to complete */
                //printf("Child pid %d\n",pid);
                wait(NULL);
                //printf("Child Complete\n");
            }
        }
    }

    return 0;
}
    /**
     * After reading user input, the steps are:
     * (1) fork a child process using fork()
     * (2) the child process will invoke execvp()
     * (3) if command include &. parent will invoke wait()
     */
    // char *kill[] = {"kill","-9",pid_char};
    //             printf("kill child process %d successed\n",pid);
    //             execvp(kill[0], kill);