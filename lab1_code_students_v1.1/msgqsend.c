#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h> // use time.h header file to use time  
#include <unistd.h>

time_t t1; // declare time variable 

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   int mtext;
};

int main(void) {
   srand(time(NULL));
   struct my_msgbuf buf;
   int msqid;
   int len;
   key_t key;
   system("touch msgq.txt");

   if ((key = ftok("msgq.txt", 'B')) == -1) { //if file exists
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) { //if textfile works as a middleman
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to send messages.\n");
   printf("Enter lines of text, ^D to quit:\n");
   buf.mtype = 1; /* we don't really care in this case */

   char keykey[5];
   
   printf("ready to start! press enter to start sending\n");
   while(strcmp(fgets(keykey, 5, stdin), "\n") == 1);
   for(int i = 0; i < 50; ++i){
      //fgets(buf.mtext, sizeof buf.mtext, stdin);
      buf.mtext = (int)rand();
      printf("newly randomiced number nr %d: %d\n", i, buf.mtext);
      
      len = sizeof(buf.mtext);
      /* remove newline at end, if it exists */
      //if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0'; //checks if users pressed enter
      //printf("pringting (trying) all in buf: %d\n", *buf.mtext);
      if (msgsnd(msqid, &buf, len, 0) == -1) /* +1 for '\0' */    //tries to send
         perror("msgsnd");
      
   } //captures the users input
   //strcpy(buf.mtext, "end"); //writes 'end' at the end of the sending text to notify receiver
   buf.mtext = -1;
   len = sizeof(buf.mtext);
   if (msgsnd(msqid, &buf, len, 0) == -1) /* +1 for '\0' */ //send final message
      perror("msgsnd");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) { //releases control of textfile
      perror("msgctl");
      exit(1);
   }
   printf("message queue: done sending messages.\n");
   return 0;
}
