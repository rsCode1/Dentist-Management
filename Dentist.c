#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include<semaphore.h>
#include <stdlib.h>
#define N 10
int clientsCount=0;
//semaphores for clients
sem_t outClinic,clientPay,clientTreatement,sofa,clientPayed,mutex;
//semaphores for dentists     
sem_t dentistTreatement,dentistPayed;


//a node struct for a linked list
typedef struct node
{
    int num;
    struct node* next;
}node;
//head of the clients on sofa list
node* sofaList;
//head of standing clients list
node* standList;


//function that adds to end of linked list
node* addNode(node* head , int num){
    node* newClient = (node*)malloc(sizeof(struct node));
    if(!newClient) 
        exit(1);
    //saving the new client details
    //and pointing to null
    newClient->num = num;
    newClient->next = NULL;

    //if the list is empty just make it the head
    if(head == NULL)
    {
        head = newClient;
    }
    //else save it to end of list
    else{
        node* temp=head;
        while(temp->next!=NULL)
            temp=temp->next;
        temp->next = newClient;
    }
    return head;
}

//removes head node from list
node* removeNode(node* head ,int* clientNum){
    if (head==NULL)
        return head;
    node* temp=head;
    *clientNum=head->num;
    head=head->next;
    free(temp);
    return head;
}
//free memory of both lists
void freelists()
{
   node* temp;
    //free memory of sofa list
   while (sofaList != NULL)
    {
       temp = sofaList;
       sofaList = sofaList->next;
       free(temp);
    }
    //free memory of stand list
    while (standList != NULL)
    {
       temp = standList;
       standList = standList->next;
       free(temp);
    }
    
}

void* client(void* num){
    int* ClientNumPointer=(int*) num;
    while(1){
        sem_wait(&mutex); //mutex to lock
        if(clientsCount<N){//we will do this only if there is room for clients
            sem_post(&mutex);//mutex to unlock
            sem_wait(&mutex);//mutex to lock
            standList=addNode(standList,*ClientNumPointer); //adding to standing room list
            sem_post(&mutex);//mutex to unlock
            sem_wait(&mutex);//mutex to lock
            clientsCount++; //incresing client count in clinic
            sem_post(&mutex);//mutex to unlock
            printf("\nI'm Patient #%d, I got into clinic",*ClientNumPointer); 
            sleep(1);
            sem_wait(&sofa); //if possible,sit on sofa
            sem_wait(&mutex);//mutex to unlock
            standList=removeNode(standList,ClientNumPointer);//removing node from standing list
            sem_post(&mutex);//mutex to unlock
            printf("\nI'm Patient #%d, I'm sitting on the sofa",*ClientNumPointer);
            sofaList=addNode(sofaList,*ClientNumPointer);//adding to sofa list
            sem_wait(&clientTreatement); //if possible ,get treatment
            sleep(1);
            sem_wait(&mutex);//mutex to lock
            sofaList=removeNode(sofaList,ClientNumPointer);
            sem_post(&mutex);//mutex to unlock
            printf("\nI'm Patient #%d, I'm getting treatment",*ClientNumPointer);
            sleep(1);
            sem_post(&sofa);//if client gets treatment,we get space in sofa
            sem_post(&dentistTreatement);//after treatment is over,we can have more clients
            sem_wait(&clientPay);//let client attemp to pay
            printf("\nI'm Patient #%d, I'm paying now",*ClientNumPointer);
            
            sem_post(&dentistPayed);//dentist received payment
            sem_wait(&clientPayed);//client payed
            sleep(1);
        }else{
            printf("\nI'm Patient #%d, I'm out of clinic",*ClientNumPointer);
            sem_post(&mutex); //if clientsCount>N we cant enter clinic
            sleep(1);
            sem_wait(&outClinic);

            


        }
        
        
    }

}
void* dentist(void* num){
    int DentistNum= *(int*) num;
    while (1){
        sem_wait(&dentistTreatement);//wait for client to get treatment
        printf("\nI'm Dental Hygienist #%d, I'm working now",DentistNum);
        sem_post(&clientTreatement);//treatment os over 
        sem_post(&clientPay);// let client attempt to pay
        sleep(1);
         
        sem_wait(&dentistPayed); //receive payment
        printf("\nI'm Dental Hygienist #%d, I'm getting a payment",DentistNum);
        sem_post(&clientPayed); //client has payed
        sem_wait(&mutex); //lock mutex
        clientsCount--; //decrese number of clients in clinic
        sem_post(&mutex); //unlock mutex
        sem_post(&outClinic); 
        sleep(1);
    }
}
int main(int argc,char* argv[]){
    pthread_t clients[N+2] ;//clients thread array
     pthread_t dentists[3];//dentists thread array
    int clientSerial[N+2]; //client serial number
    int dentistSerial[3];//dentists serial number
    int i;

    // initialize semaphores in chronological order
    //1 client is out of clinic
    //2 client enter clinic ,he can stand or sit on sofa
    //3 client wants to receive his tretmented
    //4 dentists gives treatment
    //5 clients wants to pay
    //6 dentist can receive payment
    //7 clients has payed
    sem_init(&outClinic,0,0); 
    sem_init(&sofa,0,4);
    sem_init(&clientTreatement,0,3);
    sem_init(&dentistTreatement,0,0);
    sem_init(&clientPay , 0 ,0);
    sem_init(&clientPayed , 0 ,1);
    sem_init(&dentistPayed,0,0);
    sem_init(&mutex,0,1);

    //setting serial number for clients
    for(i=0 ; i<N+2 ; i++){
        clientSerial[i] = i+1; //i+1 to avoid 0
    }   
    
    //setting serial number for dentists
    for(i=0 ; i<3 ; i++){
        dentistSerial[i] = i+1; //i+1 to avoid 0
    }
    //creating thread for clients
    for(i=0 ; i<N+2 ; i++){
        if(pthread_create(&clients[i] , NULL , client , (void*)(&clientSerial[i]))!=0){
            freelists();//free memory if thread failed
            printf("Failed to create or join thread,freed lists!!!\n");
            exit(1);
        }
    }

    //creating thread for dentists
    for(i=0 ; i<3 ; i++){
        if(pthread_create(&dentists[i] , NULL , dentist , (void*)(&dentistSerial[i]))!=0){
            freelists();//free memory if thread failed
            printf("Failed to create or join thread,freed lists!!!\n");
            exit(1);
        }
    }

    //join threads for clients
    for(i=0 ; i<N+2 ; i++)
        if(pthread_join(clients[i] , NULL)!=0){
            freelists();//free memory if thread failed
            printf("Failed to create or join thread,freed lists!!!\n");
            exit(1);
        }
    

    //join threads for dentists
    for(i=0 ; i<3 ; i++)
        if(pthread_join(dentists[i] , NULL)!=0){
            freelists();//free memory if thread failed
            printf("Failed to create or join thread,freed lists!!!\n");
            exit(1);
        }
    freelists();//free memory 
    
    return 0;
}

