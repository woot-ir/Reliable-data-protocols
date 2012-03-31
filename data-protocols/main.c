/* 
 * File:   main.c
 * Author: Abhishek-Srinath
 *
 * Created on March 8, 2012, 3:11 PM
 */

#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose
..
   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 0    /* change to 1 if you're doing extra credit */
                           /* and write a routine called B_output */

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
  char data[20];
  };

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
   int seqnum;
   int acknum;
   int checksum;
   char payload[20];
    };

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

    
    /*THIS IS AB*/
    
    int A_toLayer4Count = 0;
    int A_toLayer3Count = 0;
    int B_tolayer5Count = 0;
    int B_tolayer4Count = 0;
/**
 * This is the function which takes in the string and the length . calculates the 
 * checksum based on the adding each character .
 * @param addr
 * @param len
 * @return 
 */
int in_cksum(char *addr, int len)
{
    int nleft = len;
    char *w = addr;
    int answer;
    int sum = 0; 
    /*
     *  My algorithm is simple, using a 32 bit accumulator (sum),
     *  we add sequential 16 bit words to it, and at the end, fold
     *  back all the carry bits from the top 16 bits into the lower
     *  16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        //printf("char:-%c\tvalue:-%d\n",*w,sum);
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1)
        sum += *(char *)w;

    /*
     * add back carry outs from top 16 bits to low 16 bits
     */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}
    
    struct pkt sending_pkt;
    int sendingSeqNum=0;
    int sendingSeqNumCopy=0;
    
    /**
     * called from layer 5, passed the data to be sent to other side
     * @param message
     * @return 
     */
A_output(message)
  struct msg message;
{
    A_toLayer4Count++;
    int i;
    int sendingCheckSum=0;
    /*Create a packet and send it to layer 3*/
    sendingCheckSum = in_cksum(message.data,20);
    sending_pkt.checksum=sendingCheckSum + sendingSeqNum;
    sending_pkt.seqnum=sendingSeqNum;
    sendingSeqNumCopy=sendingSeqNum;
    sendingSeqNum++;
    sendingSeqNum = sendingSeqNum % 2;
    
    for (i=0; i<20; i++)  
                sending_pkt.payload[i] = message.data[i];
    
    tolayer3(0,sending_pkt);
    A_toLayer3Count++;
    printf("\nA_output:-Packet with sequence no %d sent to layer 3\n",sendingSeqNumCopy);
    starttimer(0,15.0);

}

B_output(message)  /* need be completed only for extra credit */
  struct msg message;
{

}

  /**
   * called from layer 3, when a packet arrives for layer 4 
   * @param packet
   * @return 
   */
A_input(packet)
  struct pkt packet;
{
    //printf("\n received an ack %d\n",packet.acknum);
    int tempCheckSum;
    tempCheckSum = sendingSeqNumCopy + 5;
    printf("A's input\ttempchkSum-%d\tpktChkSum-%d",tempCheckSum,packet.checksum);
    if(packet.checksum == tempCheckSum && packet.acknum == sendingSeqNumCopy)
    {
        printf("\nA's input:-Received ack for pkt %d\n",packet.acknum);
        stoptimer(0);
       
    }
    else
    {
        printf("Inside A's input and inside else");
        //include the code to see if the pkt is corrupt or if there is a drop
    }
}

  /**
   * called when A's timer goes off 
   * @return 
   */
A_timerinterrupt()
{
    printf("\n Inside A's timerInterrupt\n ");
    tolayer3(0,sending_pkt);
    A_toLayer3Count++;
    starttimer(0,15.0);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
A_init()
{
    printf("\nInside A's init\n");
}


/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* */
int receivingSeqNum = 0;
struct pkt ackPkt;
int oncethrough=0;
int state=0;
//int tolayer5Count=0;

/**
 * called from layer 3, when a packet arrives for layer 4 at B
 * @param packet
 * @return 
 */
B_input(packet)
  struct pkt packet;
{
   int receivingCheckSum = 0;
   int ackCheckSum = 0;
   receivingCheckSum=in_cksum(packet.payload,20); 
   receivingCheckSum += packet.seqnum;
   B_tolayer4Count++;
   switch(state)
   {
       
       case 0 :
                if (packet.checksum == receivingCheckSum && packet.seqnum == receivingSeqNum)
                {
                        printf("\nState 0\n");
                        printf("\nB_input:-The packet received from A is not corrupted and has correct seq no %d\n",receivingSeqNum);
                        tolayer5(1,packet.payload);
                        B_tolayer5Count++;
			//tolayer5Count++;
                        ackPkt.acknum=receivingSeqNum;
                        ackCheckSum = ackPkt.acknum + 5;
                        ackPkt.checksum = ackCheckSum ;
                        printf("\nB_input:-Sending ack %d",receivingSeqNum);
                        tolayer3(1,ackPkt);
                        oncethrough=1;
                        state++;
                }
                else if (packet.checksum != receivingCheckSum || packet.seqnum == receivingSeqNum + 1)
                {
                        printf("\nState 0\n");
                        printf("\n B_input:-Wrong seqNo. so Sending the ack %d",receivingSeqNum + 1);
                        ackPkt.acknum = receivingSeqNum + 1;
                        ackCheckSum = ackPkt.acknum + 5;
                        ackPkt.checksum = ackCheckSum ;
                        if(oncethrough == 1)
                        {
                                tolayer3(1,ackPkt);
                        }
                }   
                break;
                
       case 1 :
                if (packet.checksum == receivingCheckSum && packet.seqnum == receivingSeqNum + 1)
                {
                        printf("\nState 1\n");
                        printf("\nB_input:-The packet received from A is not corrupted and has correct seq no %d\n",receivingSeqNum + 1);
                        tolayer5(1,packet.payload);
			B_tolayer5Count++;
                        ackPkt.acknum=receivingSeqNum + 1;
                        ackCheckSum = ackPkt.acknum + 5;
                        ackPkt.checksum = ackCheckSum ;
                        printf("\nB_input:-Sending ack %d",receivingSeqNum + 1);
                        tolayer3(1,ackPkt);
                        state = 0;
                }
                else if ( packet.checksum != receivingCheckSum || packet.seqnum == receivingSeqNum )
                {
                        printf("\nState 1\n");
                        printf("\n B_input:-Wrong seqNo. so Sending the ack %d",receivingSeqNum );
                        ackPkt.acknum = receivingSeqNum;
                        ackCheckSum = ackPkt.acknum + 5;
                        ackPkt.checksum = ackCheckSum;
                        tolayer3(1,ackPkt);
                }   
                break;
                
           
   }
    //printf("%d\n%d\n%s",packet.checksum,packet.seqnum,packet.payload);
}

/* called when B's timer goes off */
B_timerinterrupt()
{
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
B_init()
{
    printf("Inside B's init\n");
}


/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event {
   float evtime;           /* event time */
   int evtype;             /* event type code */
   int eventity;           /* entity where event occurs */
   struct pkt *pktptr;     /* ptr to packet (if any) assoc w/ this event */
   struct event *prev;
   struct event *next;
 };
struct event *evlist = NULL;   /* the event list */

/* possible events: */
#define  TIMER_INTERRUPT 0  
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define   A    0
#define   B    1



int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */ 
int nsimmax = 0;           /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;            /* probability that a packet is dropped  */
float corruptprob;         /* probability that one bit is packet is flipped */
float lambda;              /* arrival rate of messages from layer 5 */   
int   ntolayer3;           /* number sent into layer 3 */
int   nlost;               /* number lost in media */
int ncorrupt;              /* number corrupted by media*/

 int main()
{
   struct event *eventptr;
   struct msg  msg2give;
   struct pkt  pkt2give;
   
   int i,j;
   char c; 
  
   init();
   A_init();
   B_init();
   
   while (1) {
        eventptr = evlist;            /* get next event to simulate */
        if (eventptr==NULL)
           goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist!=NULL)
           evlist->prev=NULL;
        if (TRACE>=2) {
           printf("\nEVENT time: %f,",eventptr->evtime);
           printf("  type: %d",eventptr->evtype);
           if (eventptr->evtype==0)
	       printf(", timerinterrupt  ");
             else if (eventptr->evtype==1)
               printf(", fromlayer5 ");
             else
	     printf(", fromlayer3 ");
           printf(" entity: %d\n",eventptr->eventity);
           }
        time = eventptr->evtime;        /* update time to next event time */
        if (nsim==nsimmax)
	  break;                        /* all done with simulation */
        if (eventptr->evtype == FROM_LAYER5 ) {
            generate_next_arrival();   /* set up future arrival */
            /* fill in msg to give with string of same letter */    
            j = nsim % 26; 
            for (i=0; i<20; i++)  
               msg2give.data[i] = 97 + j;
            if (TRACE>2) {
               printf("          MAINLOOP: data given to student: ");
                 for (i=0; i<20; i++) 
                  printf("%c", msg2give.data[i]);
               printf("\n");
	     }
            nsim++;
            if (eventptr->eventity == A) 
               A_output(msg2give);  
             else
               B_output(msg2give);  
            }
          else if (eventptr->evtype ==  FROM_LAYER3) {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i=0; i<20; i++)  
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
	    if (eventptr->eventity ==A)      /* deliver packet by calling */
   	       A_input(pkt2give);            /* appropriate entity */
            else
   	       B_input(pkt2give);
	    free(eventptr->pktptr);          /* free the memory for packet */
            }
          else if (eventptr->evtype ==  TIMER_INTERRUPT) {
            if (eventptr->eventity == A) 
	       A_timerinterrupt();
             else
	       B_timerinterrupt();
             }
          else  {
	     printf("INTERNAL PANIC: unknown event type \n");
             }
        free(eventptr);
        }

terminate:
   printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n",time,nsim);
   printf("Protocol: [AB]\n");
   printf("%d of packets sent from the Application Layer of Sender A\n",A_toLayer4Count); 
   printf("%d of packets sent from the Transport Layer of Sender A\n",A_toLayer3Count);
   printf("%d packets received at the Transport layer of receiver B\n",B_tolayer4Count);
   printf("%d of packets received at the Application layer of receiver B\n", B_tolayer5Count);
   printf("Total time: %f time units\n",time);
  // printf("Total time: %0.6lf seconds\n",(tEndUpload - tStartUpload));
   printf("Throughput = %f packets/time units\n",(float)B_tolayer5Count/time);
return 0;
   /*****************************************************************************************/
   /* Add your results here!!! *********************/
   /*****************************************************************************************/
}



init()                         /* initialize the simulator */
{
  int i;
  float sum, avg;
  float jimsrand();
  
  
   printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
   printf("Enter the number of messages to simulate: ");
   scanf("%d",&nsimmax);
   printf("Enter  packet loss probability [enter 0.0 for no loss]:");
   scanf("%f",&lossprob);
   printf("Enter packet corruption probability [0.0 for no corruption]:");
   scanf("%f",&corruptprob);
   printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
   scanf("%f",&lambda);
   printf("Enter TRACE:");
   scanf("%d",&TRACE);

   srand(9999);              /* init random number generator */
   sum = 0.0;                /* test random number generator for students */
   for (i=0; i<1000; i++)
      sum=sum+jimsrand();    /* jimsrand() should be uniform in [0,1] */
   avg = sum/1000.0;
   if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n" ); 
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit(0);
    }

   ntolayer3 = 0;
   nlost = 0;
   ncorrupt = 0;

   time=0.0;                    /* initialize time to 0.0 */
   generate_next_arrival();     /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand() 
{
  double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
  float x;                   /* individual students may need to change mmm */ 
  x = rand()/mmm;            /* x should be uniform in [0,1] */
  return(x);
}  

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/
 
generate_next_arrival()
{
   double x,log(),ceil();
   struct event *evptr;
    char *malloc();
   float ttime;
   int tempint;

   if (TRACE>2)
       printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");
 
   x = lambda*jimsrand()*2;  /* x is uniform on [0,2*lambda] */
                             /* having mean of lambda        */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + x;
   evptr->evtype =  FROM_LAYER5;
   if (BIDIRECTIONAL && (jimsrand()>0.5) )
      evptr->eventity = B;
    else
      evptr->eventity = A;
   insertevent(evptr);
} 


insertevent(p)
   struct event *p;
{
   struct event *q,*qold;

   if (TRACE>2) {
      printf("            INSERTEVENT: time is %lf\n",time);
      printf("            INSERTEVENT: future time will be %lf\n",p->evtime); 
      }
   q = evlist;     /* q points to header of list in which p struct inserted */
   if (q==NULL) {   /* list is empty */
        evlist=p;
        p->next=NULL;
        p->prev=NULL;
        }
     else {
        for (qold = q; q !=NULL && p->evtime > q->evtime; q=q->next)
              qold=q; 
        if (q==NULL) {   /* end of list */
             qold->next = p;
             p->prev = qold;
             p->next = NULL;
             }
           else if (q==evlist) { /* front of list */
             p->next=evlist;
             p->prev=NULL;
             p->next->prev=p;
             evlist = p;
             }
           else {     /* middle of list */
             p->next=q;
             p->prev=q->prev;
             q->prev->next=p;
             q->prev=p;
             }
         }
}

printevlist()
{
  struct event *q;
  int i;
  printf("--------------\nEvent List Follows:\n");
  for(q = evlist; q!=NULL; q=q->next) {
    printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
    }
  printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
stoptimer(AorB)
int AorB;  /* A or B is trying to stop timer */
{
 struct event *q,*qold;

 if (TRACE>2)
    printf("          STOP TIMER: stopping timer at %f\n",time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
       /* remove this event */
       if (q->next==NULL && q->prev==NULL)
             evlist=NULL;         /* remove first and only event on list */
          else if (q->next==NULL) /* end of list - there is one in front */
             q->prev->next = NULL;
          else if (q==evlist) { /* front of list - there must be event after */
             q->next->prev=NULL;
             evlist = q->next;
             }
           else {     /* middle of list */
             q->next->prev = q->prev;
             q->prev->next =  q->next;
             }
       free(q);
       return;
     }
  printf("Warning: unable to cancel your timer. It wasn't running.\n");
}


starttimer(AorB,increment)
int AorB;  /* A or B is trying to stop timer */
float increment;
{

 struct event *q;
 struct event *evptr;
 char *malloc();

 if (TRACE>2)
    printf("          START TIMER: starting timer at %f\n",time);
 /* be nice: check to see if timer is already started, if so, then  warn */
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
   for (q=evlist; q!=NULL ; q = q->next)  
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
      printf("Warning: attempt to start a timer that is already started\n");
      return;
      }
 
/* create future event for when timer goes off */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + increment;
   evptr->evtype =  TIMER_INTERRUPT;
   evptr->eventity = AorB;
   insertevent(evptr);
} 


/************************** TOLAYER3 ***************/
tolayer3(AorB,packet)
int AorB;  /* A or B is trying to stop timer */
struct pkt packet;
{
 struct pkt *mypktptr;
 struct event *evptr,*q;
 char *malloc();
 float lastime, x, jimsrand();
 int i;


 ntolayer3++;

 /* simulate losses: */
 if (jimsrand() < lossprob)  {
      nlost++;
      if (TRACE>0)    
	printf("          TOLAYER3: packet being lost\n");
      return;
    }  

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */ 
 mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
 mypktptr->seqnum = packet.seqnum;
 mypktptr->acknum = packet.acknum;
 mypktptr->checksum = packet.checksum;
 for (i=0; i<20; i++)
    mypktptr->payload[i] = packet.payload[i];
 if (TRACE>2)  {
   printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
	  mypktptr->acknum,  mypktptr->checksum);
    for (i=0; i<20; i++)
        printf("%c",mypktptr->payload[i]);
    printf("\n");
   }

/* create future event for arrival of packet at the other side */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtype =  FROM_LAYER3;   /* packet will pop out from layer3 */
  evptr->eventity = (AorB+1) % 2; /* event occurs at other entity */
  evptr->pktptr = mypktptr;       /* save ptr to my copy of packet */
/* finally, compute the arrival time of packet at the other end.
   medium can not reorder, so make sure packet arrives between 1 and 10
   time units after the latest arrival time of packets
   currently in the medium on their way to the destination */
 lastime = time;
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==FROM_LAYER3  && q->eventity==evptr->eventity) ) 
      lastime = q->evtime;
 evptr->evtime =  lastime + 1 + 9*jimsrand();
 


 /* simulate corruption: */
 if (jimsrand() < corruptprob)  {
    ncorrupt++;
    if ( (x = jimsrand()) < .75)
       mypktptr->payload[0]='Z';   /* corrupt payload */
      else if (x < .875)
       mypktptr->seqnum = 999999;
      else
       mypktptr->acknum = 999999;
    if (TRACE>0)    
	printf("          TOLAYER3: packet being corrupted\n");
    }  

  if (TRACE>2)  
     printf("          TOLAYER3: scheduling arrival on other side\n");
  insertevent(evptr);
} 

tolayer5(AorB,datasent)
  int AorB;
  char datasent[20];
{
  int i;  
  if (TRACE>2) {
     printf("          TOLAYER5: data received: ");
     for (i=0; i<20; i++)  
        printf("%c",datasent[i]);
     printf("\n");
   }
  
}
