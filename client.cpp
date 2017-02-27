#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

using namespace std;
// Global variables
pthread_t thread[5];
int i,j;
ifstream File;
string mat,line,s;
vector< vector< int > > matrixA,matrixB,matrixC;
vector<int> rowA,rowB,rowC;
//int matrixC[20][20];

int checkB_sent=0;
int rowCnt=0;

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

void *matrixInfo(void *args)
{
    int temp;
    vector<int> *mat_pointer = (vector<int> *)args;
    vector<int> mat_order = *(mat_pointer);

    temp = htonl(mat_order[0]);
    printf("Sending rowA size = %d\n",mat_order[0]);
    write(sockfd,&temp,sizeof(temp));
    sleep(1);

    temp = htonl(mat_order[1]);
    printf("Sending colA size = %d\n",mat_order[1]);
    write(sockfd,&temp,sizeof(temp));
    sleep(1);

    temp = htonl(mat_order[2]);
    printf("Sending rowB size = %d\n",mat_order[2]);
    write(sockfd,&temp,sizeof(temp));
    sleep(1);

    temp = htonl(mat_order[3]);
    printf("Sending colB size = %d\n",mat_order[3]);
    write(sockfd,&temp,sizeof(temp));
    sleep(1);

   // pthread_exit(NULL);
}

void *matrixValue(void *args)
{
    int temp,k;
    vector<int> *mat_pointer = (vector<int> *)args;
    vector<int> mat_order = *(mat_pointer);
    int ra = mat_order[0];
    int ca = mat_order[1];
    int rb = mat_order[2];
    int cb = mat_order[3];
    
  
      for(j=0;j<ca;j++)
      {
        
        temp = htonl(matrixA[rowCnt][j]);
        printf("Sending Data[%d][%d] = %d from row %d of MATRIX A\n",rowCnt,j,matrixA[rowCnt][j],rowCnt+1);
        write(sockfd,&temp,sizeof(temp));
        sleep(1);
      }
      rowCnt++;
      //if(checkB_sent==0)
     // {
        for(i=0;i<rb;i++)
          {
            for(j=0;j<cb;j++)
            {
              
              temp = htonl(matrixB[i][j]);
              printf("Sending Data[%d][%d] = %d from MATRIX B\n",i,j,matrixB[i][j]);
              write(sockfd,&temp,sizeof(temp));
              sleep(1);
            }
          }
}

void *getMatrix(void *args)
{
    int temp;

   // vector<int> *mat_pointer = (vector<int> *)args;
    //vector<int> mat_order = *(mat_pointer);
   // long ra = (long)args;
    int *x = (int *)args;
    int ra = *(x);
   // int cb = matrixB[0].size();
   // printf("rrrrrraa%ld\n",ra );

    char intBuffer[1024];
    memset(intBuffer, '\0', sizeof(intBuffer));

    int k = 0;
    while ( 1 )
    { 
      int nbytes = recv(sockfd, &intBuffer[k], sizeof(intBuffer[k]), 0); 
      //int nbytes = read(sockfd,&intBufferCoupReq[k],sizeof(intBufferCoupReq[k]));
      if ( nbytes == -1 ) { 
        printf("recv error\n"); 
        break; 
      }
      if ( nbytes ==  0 ) {
       printf("Received Row %d.\n",ra+1);
       break; 
     }
      k++;
    }
    int *myints = (int*) intBuffer;

    rowC.clear();

      for ( int i=0; i<(k/4); i++ ) {
        temp = ntohl(myints[i]);
        rowC.push_back(temp);
        //matrixC[ra][i] = temp;
      }
     matrixC.push_back(rowC);
      
      sleep(1);
    
}

int main() {


//File Reading from CSV
    int i,j;
    ifstream File;
    string mat,line,s;
    File.open("matrix.csv");
    
    int mat_check=1;
    i=0;
    
    while(!File.eof())
    {

        getline(File,s);
        if(s.empty())
        {
            mat_check=2;
            continue;
        }
        stringstream ss(s); // Turn the string into a stream.
        while(getline(ss,line,','))
        {
            if(mat_check==1)
            {
                //printf("%s",line.c_str());
                int num = atoi(line.c_str());
                rowA.push_back(num);

            }
            if(mat_check==2)
            {
                int num = atoi(line.c_str());
                rowB.push_back(num);
            }
        }
        if(mat_check==1)
        {

            matrixA.push_back(rowA);
            rowA.clear();
        }
        if(mat_check==2)
        {

            matrixB.push_back(rowB);
            rowB.clear();
        }
    }
    printf("MATRIX A\n");


    int rA = matrixA.size();
    int cA = matrixA[0].size();

    for( i=0; i<matrixA.size(); i++)
    {
      printf("| ");
        for(j=0; j<matrixA[i].size(); j++ )
          {
            printf("%d ",matrixA[i][j]);
          }

        printf(" |\n");

    }
   
     int rB = matrixB.size();
     int cB = matrixB[0].size();


    printf("MATRIX B\n");
    for( i=0; i<matrixB.size(); i++)
    {
      printf("| ");
        for(j=0; j<matrixB[i].size(); j++ )
        {
          
            printf("%d ",matrixB[i][j]);
        }

        printf(" |\n");

    }

    // file reading ends

//  Server Connection Starts
   
   portno = 13000;
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("There is error in opening socket");
      exit(1);
   }
   
   server = gethostbyname("127.0.0.1");
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   
    // vector for getting matrixInfo i.e order
    vector<int> info(4);
    info[0] = rA;
    info[1] = cA;
    info[2] = rB;
    info[3] = cB;

    // Here we do thread making 
    // first thread is to send order of matrices
    int t_create = pthread_create(&thread[0],NULL,matrixInfo,&info);

    if(t_create)
    {
      fprintf(stderr, "There is a error in making thread\n");
      exit(1);
    }

    printf("\nFirst Thread Created\n");

    t_create = pthread_join(thread[0],NULL);
    if(t_create)
    {
      fprintf(stderr, "There is a error in making thread\n");
      exit(1);
    }


    // second thread is to send row from first matrix and whole second matrix
    
    for(i=0;i<rA;i++)
    {
        
        t_create = pthread_create(&thread[1],NULL,matrixValue,&info);
        printf("\nThread Created\n");
        if(t_create)
        {
          fprintf(stderr, "There is a error in making thread\n");
          exit(1);
        }

        t_create = pthread_join(thread[1],NULL);
        if(t_create)
        {
          fprintf(stderr, "There is a error in making thread\n");
          exit(1);
        }
    }

    //Thread making for getting matrix from Server
    printf("\nReceiving Thread Created\n");
      for(i=0;i<rA;i++)
      {
       int t_create = pthread_create(&thread[2],NULL,getMatrix,&i);

        if(t_create)
        {
          fprintf(stderr, "There is a error in making thread\n");
          exit(1);
        }

        

        t_create = pthread_join(thread[2],NULL);
        if(t_create)
        {
          fprintf(stderr, "There is a error in making thread\n");
          exit(1);
        }

      }

 printf("MATRIX C\n");
 //printf("%d\n",matrixC.size());
 int matC[rA][cB];
  vector<vector<int> >::iterator row;
  vector<int>::iterator col;
  i=0;
  j=0;

  for( row=matrixC.begin(); row!=matrixC.end(); row++)
    {
      j=0;
        for(col=row->begin(); col!=row->end(); col++ )
        {
          
           matC[i][j++] = *col;
        }
        i++;

    }

   for( i=0; i<rA; i++)
    {
    
        for(j=0; j<cB; j++ )
        {
          
            printf("%d ",matC[i][j]);
        }

        printf("\n");

    }
    /*
  */
   return 0;
}
