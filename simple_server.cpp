/* run using ./server <port> */
#include "http_server.hh"
#include "http_server_api.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <pthread.h>
#define num_threads 5

int i=0;
pthread_t thread_id[num_threads];
void error(char *msg) {
  perror(msg);
  exit(1);
}
// get and put functions for queue
int buffer[num_threads];
int fill_ptr=0;
int use_ptr=0;
int count = 0;

void put(int value){
  buffer[fill_ptr]=value;
  fill_ptr =(fill_ptr+1)% num_threads;
  count++;
}

int get(){
  int tmp =buffer[use_ptr];
  use_ptr=(use_ptr+1)%num_threads;
  count--;
  return tmp;
}

//varables for lock and conditions
pthread_cond_t empty_1=PTHREAD_COND_INITIALIZER;
pthread_cond_t fill_1=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;


void * connection_handler(void* newsockfdptr){
    /* read message from client */
    while(1){
    int n;
    //int* newsockfdptr1;
    //newsockfdptr1=(int*)newsockfdptr;
    char handler_buffer[256];
    bzero(handler_buffer, 256);

    n=pthread_mutex_lock(&mutex);
    //if(n==0){cout<<"lock acquired by worker"<<endl;}
    while(count==0){
      pthread_cond_wait(&fill_1,&mutex);
      cout<<"Worker woke up and acquired lock again"<<endl;
      }
    int newsockfd_client= get();
    //cout<<"got new scoketfd"<<endl;
    pthread_cond_signal(&empty_1);
    pthread_mutex_unlock(&mutex);

    n = read(newsockfd_client,handler_buffer, 255);
    if (n < 0)
      error("ERROR reading from socket");
    printf("Here is the message: %s",handler_buffer);

    //HTTP_Request (string(handler_buffer));
    HTTP_Response *response=new HTTP_Response();
    response= handle_request(string(handler_buffer));
    //printf("\n%s/n",response->content_length);
    /* send reply to client */
    string output=response->get_string();
    n=output.length();
    char output_array[n + 1];
 
    // copying the contents of the
    // string to char array
    strcpy(output_array, output.c_str());
    //cout<<n<<endl;
    //cout << "\n output string is\n"<<output;
    //cout<<"output_array is \n"<<output_array;

    n = write(newsockfd_client, output_array, n+1);
    if (n < 0)
      error("ERROR writing to socket");
  }
}
int main(int argc, char *argv[]) {            //argv[argc] is a NULL pointer.
                                              //argv[0] holds the name of the program.
                                              //argv[1] points to the first command line argument and 
                                              //argv[n] points last argument.
  int sockfd, newsockfd, portno;	     
  socklen_t clilen;	        //<sys/socket.h> makes available a type, socklen_t, which is an unsigned opaque integral
                            //type of length of at least 32 bits
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr[num_threads];
  
  //creating thread pool
  for(int j=0;j<num_threads;j++){
  pthread_create(&thread_id[j],NULL,connection_handler,NULL);
  }


  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }
  
    /* create socket */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
      error("ERROR opening socket");

    /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
    */

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* bind socket to this port number on this machine */

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      error("ERROR on binding");

    /* listen for incoming connection requests */
  while(1){
    listen(sockfd, num_threads);
    clilen = sizeof(cli_addr[i]);
    cout<<"listening"<<endl;

    /* accept a new request, create a newsockfd */

    newsockfd= accept(sockfd, (struct sockaddr *)&cli_addr[i], &clilen);
    cout<<"when count is "<<count<<endl;
    if (newsockfd < 0)
      error("ERROR on accept");
    else{
        //void* newsockfdptr=&newsockfd[i];
        //pthread_create(&thread_id[i],NULL,connection_handler,newsockfdptr);
        //i++;  
        int q=pthread_mutex_lock(&mutex);
        if(q==0){cout<<"lock acquired by master"<<endl;}
        while(count==num_threads){
          pthread_cond_wait(&empty_1,&mutex);
          cout<<"master wake up and lock acquired again"<<endl;}
        put(newsockfd);
        q=pthread_cond_signal(&fill_1);
        if(q==0){
        cout<<"signal sent"<<endl;}
        q =pthread_mutex_unlock(&mutex);
        if(q==0){
        cout<<"lock released by master"<<endl;}
      }  
  }
    return 0;
}
