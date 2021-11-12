#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   int mtext[2000];
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   int toend;
   key_t key;

   if ((key = ftok("msgq.txt", 'B')) == -1) { //if exist (this is why msgqsend must start first, since it creates the textfile)
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS)) == -1) { /* connect to the queue */
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to receive messages.\n");

   for(;;) { /* normally receiving never ends but just to make conclusion */
             /* this program ends with string of end */
      if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) { //if error while recieving
         perror("msgrcv");
         exit(1);
      }
      printf("recvd: \"%d\"\n", buf.mtext);
      toend = strcmp(buf.mtext,"end");
      if (toend == 0)
      break;
   }
   printf("message queue: done receiving messages.\n");
   system("rm msgq.txt");
   return 0;
}
