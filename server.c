#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define FILEMAX 500 //max lines in file


typedef struct record{
char id[9]; // max 8 chars with /0
int odometer;
float gallons;
}record;
//*

record arr_records[FILEMAX];
char output1[300]; // char array to write back to pipe


void work(char usrin[]){
    float first= 9999999;
    float last= -1;
    float totalgas=0;
    float mpg=0.000;
    char tempid[9];
    char tempcmd[5];
    int positions[FILEMAX];
    for(int i = 0; i < FILEMAX; i++){
        positions[i] = -1;
    }
    for(int i = 0; i < 4; i++){
        tempcmd[i]= usrin[i];
    }

    if(strcmp(tempcmd, "list") == 0){
        for(int i = 5; i < 14; i++){
        tempid[i-5]= usrin[i];
        if(tempid[i-5] == '\n'){
            tempid[i-5] = '\0';
        }
        }
        for(int i =0; i < (FILEMAX) ; i++){
            if (strcmp(tempid , arr_records[i].id) == 0){
                positions[i]= i;
            }
        }
        for(int i = 0; i < FILEMAX ; i++){          //sorts position order of odometer least to greatest

        if(positions[i] != -1){
            for(int j = 0; j < FILEMAX ; j++){
                if(positions[j] != -1){
                    if(arr_records[positions[i]].odometer < arr_records[positions[j]].odometer){
                        int temp = positions[i];
                        positions[i]=positions[j];
                        positions[j]=temp;
                    }
                }
            }
        }
    }
        for(int i = 0; i < (FILEMAX) ; i++){        // puts all entries into char array
        if(positions[i] != -1){
            strcat(output1, arr_records[positions[i]].id);
            strcat(output1, " odometer: ");
            char odo[15];
            sprintf(odo, "%d", arr_records[positions[i]].odometer);
            strcat(output1, odo);
            strcat(output1, " gallons: ");
            char gas[9];
            gcvt(arr_records[positions[i]].gallons, 8, gas);
            strcat(output1, gas);
            strcat(output1, "\n");
        }
    }
        strcat(output1, "\0");
    }
    else{
        for(int i = 4; i < 13; i++){
            tempid[i-4]= usrin[i];
        }
        for(int i = 4; i < 14; i++){
            tempid[i-4]= usrin[i];
            if(tempid[i-4] == '\n'){
                tempid[i-4] = '\0';
            }
        }
        for(int i =0; i < (FILEMAX) ; i++){
            if (strcmp(tempid , arr_records[i].id) == 0){
                positions[i]= i;
            }
        }
        for(int i = 0; i < FILEMAX ; i++){          //sorts position order of odometer least to greatest
            if(positions[i] != -1){
                for(int j = 0; j < FILEMAX ; j++){
                    if(positions[j] != -1){
                        if(arr_records[positions[i]].odometer < arr_records[positions[j]].odometer){
                            int temp = positions[i];
                            positions[i]=positions[j];
                            positions[j]=temp;
                        }
                    }
                }
            }
        }

        for(int i = 0; i < (FILEMAX) ; i++){        // does mpg
            if(positions[i] !=-1){
                if(first>arr_records[positions[i]].odometer){
                    first = arr_records[positions[i]].odometer;
                }
                if(last<arr_records[positions[i]].odometer){
                    last = arr_records[positions[i]].odometer;
                }
                totalgas = totalgas + arr_records[positions[i]].gallons;
            }
        }
        int miles = last - first;
        mpg = miles/totalgas;
        char gas[9];
        gcvt(mpg, 8, gas);

        strcat(output1, "Average MPG = ");


        strcat(output1, gas);
        strcat(output1, "\n");
    }


    memset(tempid, 0, sizeof(tempid));
    memset(tempcmd, 0, sizeof(tempcmd));
}



int main(int argc, char** argv)
{
    FILE *ftpr;
    char chr[100]; // char array for reading in lines for file
    ftpr = fopen("gasdata.txt", "r");
    if(ftpr == NULL)
    {
        printf("Error!");
        exit(1);
    }
    fclose(ftpr);
    fopen("gasdata.txt", "r");

    int lastelem=0;

    int flag=0;
    while (fgets(chr, 100, ftpr) != NULL) // loop for reading line and extracting needs from line
    {
        if(chr[0] == '\n'){
            break;
        }
        lastelem++;
        char tempchr[9]; // temp array for id
        char tempodo[13]; // temp array for odometer
        char tempgas[13]; // temp array for gallons
        int delim=0;
        int elem=0;
        for(int i = 0; i<sizeof(chr);i++)
        {
            if(chr[i]==' ')
            {
                delim++;
                i++;
                elem = i;
            }
            switch(delim)
            {
                case 0:
                    tempchr[i]=chr[i];
                    break;
                case 1:
                    tempodo[i-elem]=chr[i];
                    break;
                case 2:
                    tempgas[i-elem]=chr[i];
                    break;
            }
        }

        tempchr[9] = '\0';


        strcpy(arr_records[flag].id, tempchr);
        arr_records[flag].odometer = atoi(tempodo);
        arr_records[flag].gallons = atof(tempgas);

        memset(tempchr, 0, sizeof(tempchr));
        memset(tempodo, 0, sizeof(tempodo));
        memset(tempgas, 0, sizeof(tempgas));
        flag++;
    }
    fclose(ftpr);
    for(int i = 0; i < lastelem;i++)
    {
        printf("element = %d:  id = %s,  odometer = %d,  gallons = %f \n", i, arr_records[i].id, arr_records[i].odometer, arr_records[i].gallons);
    }
    int boolean = 1; // will be 0 when exit is in pipe
    char check[50] = "exit\n";
    int fd[2];
    int fd2[2];
    sscanf(argv[0],"%d",&fd[0]); // assigns the file decriptors
    sscanf(argv[1],"%d",&fd2[1]);


    while(boolean == 1){
        char usr[50];       //input from interface
        char server[300];    // output from server


        if(read(fd[0], usr, 50) == -1)
        {
            printf("error reading pipe \n");
            return 2;
        }

        if(strcmp(usr, check) == 0){ // server is exited here
            boolean = 0;
            sprintf(server, "%d", getpid());
            write(fd2[1], server, 300 );
            exit(0);
        }
        else{
            work(usr);
            strcpy(server, output1);
        }

        if(write(fd2[1], server, 300 ) == -1)
        {
            printf("error writing to pipe \n");
            return 3;
        }

        memset(output1, 0, sizeof(output1));
        memset(server, 0, sizeof(server));
        memset(usr, 0, sizeof(usr));
    }

    return 0;
}
//*/
