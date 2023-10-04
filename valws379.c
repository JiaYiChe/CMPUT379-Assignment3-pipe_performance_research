#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <stdbool.h>
#include <getopt.h>

#define MAX_SIZE 1024
#define area 2
unsigned long long skipize=0;
unsigned long long pageSize;
unsigned long long windowsSize;

int readListIndex=0;//keep track on which position should readList be at(with some calculation)
unsigned long long countCurrent=0;//keep track on how many lines have read
unsigned long long uniquetPages=0;//keep track on how many unique pages 
int multiOfArraySize=1;//everytime the hash array size need realloc this number will be the increasement of the size (one times of the windowsSize, two times of the windowsSize,etc)

char readAddress[MAX_SIZE]; //to store the address of current line
char readSize[MAX_SIZE];//to store the data size of current line

int winArraySize=0;//keep track how many value had store into the hash table
unsigned long long currentWinSize;//current max size of hash table
//this is the hash table struct
struct windows{
    unsigned long long pageNum;
    int count;
};
struct windows *winList;
//this function will look over the hash table to determine whether there is certain page number exist
int overView(unsigned long long pageNumber){
    for(int i =0;i<currentWinSize;i++){
        if(winList[i].pageNum!=0 && winList[i].pageNum == pageNumber){
            return i;
        }
    }
    return -1;
}
//this function will realloc the hash table malloc when its full
void reallocateMem(){
    multiOfArraySize+=1;
    winList = (struct windows*)realloc(winList,multiOfArraySize*windowsSize*sizeof(struct windows));
    currentWinSize = windowsSize*multiOfArraySize;
}
//this function will calculate which spot the page number should be in hash table
int calcSpot(unsigned long long pageNumber){
    int spot = pageNumber%windowsSize;
    return spot;
}
//this function will transform the Hex address into a base 10 number
unsigned long long transHex(){
    int base=16;
    unsigned long long result;
    result = strtoull(readAddress,NULL,base);
    return result;
}
//this function is doing the job of insert the page number and if this is new page to insert will make the count to 1, else will add the count +=1
void insertPlus(unsigned long long pageNumber,int spot){
    int check=1;
    int count =0;
    int result = overView(pageNumber);
    //check if page number exist and add 1 to the count
    if(result>=0){
        check=2;
        winList[result].count+=1;
    }else if(result==-1){
        int nspot = spot;
        //if page number not exist will go to the spot should be in hash table, if spot is taken then go to the nearest spot
        while(check==1){
            count+=1;
            if(winList[nspot].pageNum==0 && winList[nspot].count==0){
                uniquetPages+=1;
                check=2;
                winList[nspot].pageNum=pageNumber;
                winList[nspot].count=1;
                
                break;
            }else{
                nspot=nspot+1;
                if(nspot>=currentWinSize){
                    nspot=0;
                }
            }
            //if hash table full realloc it
            if(count==currentWinSize){
                reallocateMem();
             
            }
        }
    }
}
//this function is to insert the current page to readlist and the hash table 
void insert(int ifTwo, unsigned long long readList[windowsSize][area]){
    unsigned long long address = transHex();
    unsigned long long pageNumber = address/pageSize;
    int spot = calcSpot(pageNumber);
    //enter new member to the readList 
    readList[readListIndex][0] = pageNumber;
    readList[readListIndex][1] = ifTwo;
    readListIndex+=1;
    if(readListIndex==windowsSize){
        readListIndex=0;
    }
    //check if there is adjacent page and call the insertPlus function to enter every page current have
    if(ifTwo==1){
        insertPlus(pageNumber,spot);
    }
    if(ifTwo==2){
        insertPlus(pageNumber,spot);
        if(uniquetPages>=currentWinSize){
            reallocateMem();
        }
        spot = calcSpot(pageNumber+1);
        insertPlus(pageNumber+1,spot);
    }

}
//this function is to search for the page number location in hash table
int search(unsigned long long pageN){
    int spot = calcSpot(pageN);
    int check=1;

    while(check==1){
        if(winList[spot].pageNum==pageN){
            check=2;
            break;
        }
        else{
            spot=spot+1;
            if(spot>=currentWinSize){
                spot=0;
            }
        }
    }

    return spot;
}
//this function is deleting the page number that doesn't need anymore and clean their spot
void deletePlus(unsigned long long pageN){
    int spot = search(pageN);
    if(spot>=0){
        if(winList[spot].count==1){
            
            uniquetPages-=1;
            winList[spot].pageNum=0;
            winList[spot].count=0;
            
        }
        else if(winList[spot].count>1){
            winList[spot].count-=1; 
        }
    }
}
//this function is for remove no adjacent page line 
void delete(unsigned long long readList[windowsSize][area]){

    unsigned long long pageN = readList[readListIndex][0];
    readList[readListIndex][0] = 0;
    readList[readListIndex][1] = 0;
    deletePlus(pageN);
}
//this function is for remove have adjacent page line
void deleteTwo(unsigned long long readList[windowsSize][area]){

    unsigned long long pageN = readList[readListIndex][0];
    readList[readListIndex][0] = 0;
    readList[readListIndex][1] = 0;

    deletePlus(pageN);
    deletePlus(pageN+1);
}
//this function is to split the line just got into address and data size
void split(char read[MAX_SIZE]){
    char delims[] = ",";
    char *result=NULL;
    result = strtok(read,delims);
    int i =1;
    while(result!=NULL){
        if(i==1){
            strcpy(readAddress,result);
            i+=1;
        }else{
            strcpy(readSize,result);
        }
        result = strtok(NULL,delims);
    }

}
//this function is checking if current line we have have adjacent page
int checkAdjacent(){
    unsigned long long address=transHex();
    int dataSize = atoi(readSize);
    unsigned long long start = address/pageSize;
    unsigned long long end = (address+dataSize-1)/pageSize;
    if(end>start){
        return 2;
    }
    return 1;
}
//this function is to continue read the line and decide what to do
void reading(){

    char read[MAX_SIZE];
    unsigned long long count=0;
    int isValue = 1;
    unsigned long long result;
    winList=(struct windows*)malloc(windowsSize*sizeof(struct windows));//setting up the hash table

    unsigned long long readList[windowsSize][area];//setting up the readList that store all the line we read help to determine when should which line be remove from hash table
    currentWinSize = windowsSize;
    unsigned long long skipnum=0;
    FILE *fp;
    //loop that read all the lines
    while(scanf("%s",read)!=EOF){
        
        if(isValue==2){
            if(strlen(read)<9){
                isValue=1;
            }else{
                //if there is skipize exist then skip ingore the line we have until the skip num is over
                if(skipnum<skipize){
                    isValue=1;
                    skipnum+=1;
                }
                else{
                    split(read);
                    //store into hash table without anyother action
                    if(countCurrent<windowsSize-1){
                        result = checkAdjacent();
                        insert(result,readList);
                    }else{//when finish read in the first windows size of line start the swap and print job
                        result = checkAdjacent();
                        insert(result,readList);
                        printf("%lld\n",uniquetPages);
                        
                        fp=fopen("workSet","a+");                   
                        fprintf(fp,"%lld %lld\n",count,uniquetPages);
                        fclose(fp);
                        
                        if(readList[readListIndex][1]==1){
                            
                            delete(readList);
                        }
                        if(readList[readListIndex][1]==2){
                            
                            deleteTwo(readList);
                        }
                        
                        count+=1;
                    }
                    //check if realloc need
                    if(uniquetPages>=currentWinSize){
                        reallocateMem();
                    }

                    isValue=1;
                    countCurrent+=1;
                }
                
            }
            
        }
        //check if next line is what we need
        else if(read[0]=='L'||read[0]=='I'||read[0]=='M'||read[0]=='S'){
            
            isValue = 2;
        }

    }
    free(winList);
}


int main(int argc, char *argv[]){

    if (argc>5||argc<3){
        printf("Invalid input arguments");
        exit(0);
    }

    if (argc==3){
        pageSize = atoi(argv[1]);
        windowsSize = atoi(argv[2]);
    }else{
        char state;
        if((state = getopt(argc,argv,"s:"))!=-1){
            switch(state){
                case 's':
                    skipize = atoi(optarg);
                    pageSize = atoi(argv[optind]);
                    windowsSize = atoi(argv[optind+1]);
            }
        }else{
            printf("Invalid input arguments");
            exit(0);
        }
    }

    reading();

}


