/*
 * Author: Derek Calzadillas
 * Purpose: To demonstrate knowledge of process management
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//max length of a command
#define MAX_LINE 80
#define MAX_HIST 10

int flag = 0;
static int histFront = 0;
static int histRear = -1;
char *history[MAX_HIST];
int histFlag = 0;

int countTokens(char *args)
{
    int i, count;
    for (i = 0, count = 0; args[i]; i++)
        count += (args[i] == ' ');
    count++;
    return count;
}

char **tokenize(char *args, char **toks, int count)
{
    //save first token
    char delim[] = " ";
    char *ptr = strtok(args, delim);
    toks[0] = ptr;

    //save the other tokens
    int i = 1;
    while (ptr != NULL)
    {
        //strtok fills spaces with NULL, so next NULL to delim
        ptr = strtok(NULL, delim);
        toks[i] = ptr;
        i++;
    }
    //strtok is shit, so if only 1 token need to trunc last char
    if (count == 1)
    {
        toks[0][strlen(toks[0]) - 1] = '\0';
        //if 'exit' then exit
        if (strcmp(toks[0], "exit") == 0)
        {
            printf("User exiting...\n");
            exit(0);
        }
    }
    else
    {
        //if more than one token, need to trunc last char of last elem
        toks[count - 1][strlen(toks[count - 1]) - 1] = '\0';
    }

    //last token should be null for exevp
    if (strcmp(toks[count - 1], "&") == 0)
    {
        flag = 1;               //set flag to not wait
        toks[count - 1] = NULL; //overwrite with null
    }
    toks[count] = NULL; //last save as NULL
    return toks;
}

void historyFeature(char *args)
{
    //increment rear queue
    histRear = (histRear + 1) % MAX_HIST; //resets at after reaches 9, (9+1)%10 == 0

    //(histRear still = 0 ) 1 = 0 ... will fail until (histRear = 9 ) 0 = 0
    if ((histRear + 1) % MAX_HIST == histFront)
    {
        history[histRear] = strdup(args);
        //move front to point to
        histFront = (histFront + 1) % MAX_HIST; //move front up one (first time = 1)
    }
    else
    {
        history[histRear] = strdup(args);
    }
}

int getTrueIndex(int x)
{
    int arr[MAX_HIST];
    //when history makes one full circle, flag will be true
    if (!histFlag)
    {
        //first fill, rear is max moving down to zero
        for (int i = histRear; i >= 0; i--)
        {
            arr[i] = i;
        }
        if (arr[x] > x)
        {
            printf("Index history[%d] has not been entered into the system.\n", x);
            return -1;
        }
    }
    else
    {
        //count down to zero from rear
        int count = MAX_HIST - 1;
        for (int i = histRear; i >= 0; i--)
        {
            arr[count] = i;
            count--;
        }
        //edge case
        if (histRear == 8)
        {
            arr[count] = MAX_HIST - 1;
        }
        //go down to zero from MAX to front - 1
        if (count > 0)
        {
            for (int i = MAX_HIST - 1; i >= histFront - 1; i--)
            {
                arr[count] = i;
                count--;
            }
        }
    }
    return arr[x]; //value of true index in array
}

void displayHistory()
{
    if (!histFlag)
    {
        for (int i = histRear; i >= 0; i--)
        {
            printf("%d %s", i, history[i]);
            fflush(stdout);
        }
    }
    else
    {
        for (int i = MAX_HIST - 1; i >= 0; i--)
        {
            printf("%d %s", i, history[getTrueIndex(i)]);
            fflush(stdout);
        }
    }
    //trigger else statment (rear loops before front)
    if (histRear == 9)
        histFlag = 1;
}

int checkSpecialTokens(char *args)
{

    //first cmd, so just return -1
    if (histRear == -1)
    {
        return -1;
    }
    //check for token 0-9
    for (int i = 0; i < MAX_HIST; i++)
    {
        if (args[0] == '!' && (args[1] == i + '0'))
            return i;
    }
    //most recent cmd return 10
    if (args[0] == '!' && (args[1] == '!'))
        return MAX_HIST;
    //not special return -1
    if (strcmp(args, "history") == 0)
    {
        return MAX_HIST + 1; //11 just display hist
    }
    return -1;
}

int main(void)
{
    char *args = (char *)malloc(MAX_LINE / 2 + 1);
    if (args == NULL)
    {
        printf("No memory\n");
        return 1;
    }

    int should_run = 1; //will exit loop in tokenize function

    fflush(stdout);
    printf("Welcome to Unix Shell History Program.\n");
    printf("Type in a shell command or check and execute historial commands.\n");
    fflush(stdout);

    while (should_run)
    {
        printf("osh>");
        fflush(stdout);

        //get input
        fgets(args, MAX_LINE / 2 + 1, stdin);

        //size of char always one, but future proof in case something weird goes on
        if (strlen(args) + 1 * sizeof(char) > (MAX_LINE / 2 + 1 * sizeof(char)))
        {
            printf("Exiting...");
            exit(1);
        }
        else
        {
            //preserve args before tokenization
            char *cpy = (char *)malloc(MAX_LINE / 2 + 1);

            //check the tokenIDS and get numbered cmd
            int tokenID = checkSpecialTokens(args);

            //check all elements 0-10 if histFlag already triggered
            if (histFlag)
            {
                for (int i = 0; i < MAX_HIST; i++)
                {
                    if (tokenID == i)
                    {
                        strcpy(args, history[getTrueIndex(i)]);
                    }
                }
                if (tokenID == MAX_HIST)
                {
                    strcpy(args, history[getTrueIndex(histRear)]);
                }
                else if (tokenID == MAX_HIST + 1)
                { ///display hist
                    strcpy(args, "history");
                    break;
                }
            }
            else
            { //check only initialized elements for 0-10 (start at true rear)
                for (int i = histRear; i >= 0 && histRear != -1; i--)
                {
                    if (tokenID == i)
                    {
                        strcpy(args, history[getTrueIndex(i)]);
                    }
                }
                if (tokenID == MAX_HIST)
                { //print most recent
                    strcpy(args, history[getTrueIndex(histRear)]);
                }
                else if (tokenID == MAX_HIST + 1)
                { ///display hist
                    strcpy(args, "history");
                    break;
                }
            }

            //cpy args to save full length
            strcpy(cpy, args);

            //count tokens in args statement
            int count = countTokens(args);

            //tokenize args input (+1 b/c last token needs to be NULL)
            char *tokens[count + 1];
            tokenize(args, tokens, count);

            historyFeature(cpy);

            //check user input to execute command
            int rc;
            if ((rc = fork()) < 0)
            { //fork child here
                //fork failed; exit
                fprintf(stderr, "fork failed\n");
                exit(1);
            }
            else if (rc == 0 && !(strcmp(tokens[0], "history") == 0) && !flag)//if child and not history
            {   
                execvp(tokens[0], tokens); //run child proc

            }
            else if (rc == 0 && !(strcmp(tokens[0], "history") == 0) && flag)//if child and not history
            {   
                /* To run this process in the background need to redirect
                 * stderr and stdout to null device since it's not important
                 * to the parent shell. The parent is not going to wait and instead
                 * loop back around to the prompt
                 */
                int output, err, new; fflush(stdout); 
                output = dup(1); err = dup(2); //cpy original stdout and stderr
                new = open("/dev/null", O_WRONLY); //create new fd for null device
                dup2(new, 1); //stdout redirects to null dev
                dup2(new, 2); //stderr redirects to null dev
                close(new);
                execvp(tokens[0], tokens); //run child proc in background
                if (execvp(tokens[0], tokens) < 0) {    
                    printf("ERROR: Invalid command\n");
                    break;
                }
                fflush(stdout);
                dup2(output, 1); //restore original stdout to fd1
                dup2(err, 2); //restore original stderr to fd2
                close(output);

            }
            else if (rc == 0 && (strcmp(tokens[0], "history") == 0))//if child and history
            { 
                displayHistory(); //run hist proc
            }
            else
            { //parent
                if (!flag) //default flag 0 is wait
                {               
                    wait(NULL); //parent waits here for child to exec first
                }
                else
                { //flag is 1 so concurrent was specified
                    //parent will not wait
                    flag = 0; //reset flag
                }
            }
        } //end else

    } //end while
    return 0;
}
