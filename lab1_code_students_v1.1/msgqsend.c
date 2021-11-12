#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h> // use time.h header file to use time  

time_t t1; // declare time variable 
srand ( (unsigned) time (&t1)); // pass the srand() parameter   

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   int mtext[200];
};

int main(void) {
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
   buf.mtype = 4; /* we don't really care in this case */
   for(;;){
      //fgets(buf.mtext, sizeof buf.mtext, stdin);
      for(int i = 0; i < 2; ++i){
         *buf.mtext += (int)rand() % 10;
      }
      len = strlen(buf.mtext);
      /* remove newline at end, if it exists */
      if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0'; //checks if users pressed enter
      if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */    //tries to send
         perror("msgsnd");
      sleep(0.1);
   } //captures the users input
   strcpy(buf.mtext, "end"); //writes 'end' at the end of the sending text to notify receiver
   len = strlen(buf.mtext);
   if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */ //send final message
      perror("msgsnd");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) { //releases control of textfile
      perror("msgctl");
      exit(1);
   }
   printf("message queue: done sending messages.\n");
   return 0;
}
