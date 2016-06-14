/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"
#include "servo.h"
#include "fdserial.h"
#include "wavplayer.h"
#include "servodiffdrive.h"
#include "ping.h"                             // Include ping header

#define PIN_SOUND       26 // pin for the speaker, default=26
#define PIN_XBEE_RX      0
#define PIN_XBEE_TX      1
#define PIN_SERVO_LEFT  14
#define PIN_SERVO_RIGHT 15
#define PIN_SERVO_MID 19
#define PIN_PING 17

volatile int ch;
volatile int XeeChange=0;
volatile int cmDist=0;
volatile int turn=0;
volatile unsigned int lockR;            // Lock ID 
int DO = 22, CLK = 23, DI = 24, CS = 25;        // SD I/O pins
int * cog_ptr[8];
const char *WAVfile[] = {"jagger.wav","somebody.wav","callme.wav","rolling.wav","origin.wav","techloop.wav"};       // Set up techloop string
//int degree[] = {1080,1260,1440,1620,1800,
//                1620,1440,1260,1080,900,
//                720,540,360,180,0,
//                180,360,540,720,900};
int degreeindex=0;
int pinLeft, pinRight, rampLeft, rampRight;   // Variables shared by functions

const int mapsize=10;
int map[10][10]={0};
int state =0;//0 right 1 up 2 left 3 down 4 finish
int x;
int y;
const int wall=3;
const int space=0;
const int visit=1;
int lb=0,rb=0,fb=0;
int stop=0;

volatile fdserial *xbee;

void XBee();
void music(int ch);
//void robot();
void PING();
char char2lowercase (char c);
char Drive(int right, int front, int left);
void Turn_left();
void Turn_right();
void Straight();
void Turn_over();
void Backward();
void Record(int state);
void music(int ch);
int walk(int state, char move){
    if(state==0 && stop==0)
    {
        if(move=='s')
        {
            state=0;
            x=x+1;
            printf("fuck\n");
        }
        else if(move=='l')
        {
            state=1;
            printf("fuck\n");
        }
        else if(move=='r')
        {
            state=3;
        }
        if(map[y][x+1]==visit)
          stop=1;
    }
    else if(state==1 && stop==0)
    {
        if(move=='s')
        {
            state=1;
            y=y-1;
        }
        else if(move=='l')
        {
            state=2;
        }
        else if(move=='r')
        {
            state=0;
        }
        if(map[y-1][x]==visit)
          stop=1;
    }
    else if(state==2 && stop==0)
    {
        if(move=='s')
        {
            state=2;
            x=x-1;
        }
        else if(move=='l')
        {
            state=3;
        }
        else if(move=='r')
        {
            state=1;
        }
        if(map[y][x-1]==visit)
          stop=1;
    }

    else if(state==3 && stop==0)
    {
        if(move=='s')
        {
            state=3;
            y=y+1;
        }
        else if(move=='l')
        {
            state=0;
        }
        else if(move=='r')
        {
            state=2;
        }
        if(map[y+1][x]==visit)
          stop=1;
    }
    //if(stop==1)
      //state =4;
    return state;
}

int main (void) {
  xbee = fdserial_open(PIN_XBEE_RX, PIN_XBEE_TX, 0, 9600);
  sd_mount(DO, CLK, DI, CS);                      // Mount SD card
  drive_pins(PIN_SERVO_LEFT, PIN_SERVO_RIGHT);    // Set the Left and Right servos
  int dist0=0,dist90=0,dist180=0;                  //Declare distance for 0, 45, 90
  char move;
  const int ping_pause=800;
  lockR = locknew();                              // Check out a lock
  
  map[mapsize-1][0]=wall; // left down corner =wall
  map[mapsize-2][0]=wall;
  map[mapsize-1][1]=wall;
  map[mapsize-2][1]=visit;
  state =0;
  x=1;
  y=8;
  int i,j;

  fdserial_txChar(xbee,'g');
  fdserial_txFlush(xbee);
  while(1){
    //Sense distance around
    //cog_ptr[7] = cog_run(&PING,16);                 // Ask a cog to run the function PING()
    servo_angle(PIN_SERVO_MID, 100);                // Turn the standard servo to 90 degrees
    pause(ping_pause*2.5);                                  // Close those Cogs which control the servo
    dist0 = ping_cm(17);
       
    servo_angle(PIN_SERVO_MID, 1000);
    pause(ping_pause);
    dist90 = ping_cm(17);
    
    servo_angle(PIN_SERVO_MID, 1900);
    pause(ping_pause);
    dist180 = ping_cm(17);
    
    printf("%d %d %d\n",dist0,dist90,dist180);
    //pause(1000);
    //servo_speed
    move = Drive(dist0,dist90,dist180);
    Record(state);
    //send back data
    char message[10];
    char message2[10];
    char lbm[10];
    char fbm[10];
    char rbm[10];
    char package[10];
    //move = 'q';
    //state = 6;
    sprintf(message,"%d",state);
    sprintf(message2,"%c",move);
    sprintf(lbm,"%d",lb);
    sprintf(fbm,"%d",fb);
    sprintf(rbm,"%d",rb);
    package[0] = message[0];
    package[1] = message2[0];
    package[2] = lbm[0];
    package[3] = fbm[0];
    package[4] = rbm[0];
    
    printf("ch = %s %c\n",message,move);
    
    fdserial_txChar(xbee,package[0]);
    fdserial_txFlush(xbee);
    fdserial_txChar(xbee,package[1]);
    fdserial_txFlush(xbee);
    fdserial_txChar(xbee, lbm[0]);
    fdserial_txFlush(xbee);
    fdserial_txChar(xbee,fbm[0]);
    fdserial_txFlush(xbee);
    fdserial_txChar(xbee,rbm[0]);
    fdserial_txFlush(xbee);
    
    state = walk(state,move);
    //Turn_left();
    if(stop==1)
      state=4;
    map[y][x]=visit;
    printf("%c %d %d %d %d\n",move,x,y,state,stop);
    // if(move=='s')
    // printf("fuck\n");
    if(state!=4)
    {
      for(i=0;i<mapsize;i++){
        for(j=0;j<mapsize;j++){
            printf("%d ",map[i][j]);
        }
        printf("\n");
      }
    }
    else
    {
      printf("Finish scanning = =\n");
      //music(4);
      break;
    }
  }            
  return 0;
}
//0 right 1 up 2 left 3 down
void Record(int state)
{
   if(state==0)
   {
     if(rb==1)
      map[y+1][x]=wall;
     if(fb==1)
      map[y][x+1]=wall;
     if(lb==1)
      map[y-1][x]=wall;
   }
   else if(state==1)
   {
     if(rb==1)
      map[y][x+1]=wall;
     if(fb==1)
      map[y-1][x]=wall;
     if(lb==1)
      map[y][x-1]=wall;
   }
   else if(state==2)
   {
     if(rb==1)
      map[y-1][x]=wall;
     if(fb==1)
      map[y][x-1]=wall;
     if(lb==1)
      map[y+1][x]=wall;
   }
   else if(state==3)
   {
     if(rb==1)
      map[y][x-1]=wall;
     if(fb==1)
      map[y+1][x]=wall;
     if(lb==1)
      map[y][x+1]=wall;
   }            
}

char Drive(int right, int front, int left)
{
  const int limit =20;
  char move;
  if(right<limit)
  {
    if(front>limit)
    {
      Straight();
      move = 's';
    }      
    else if(front<limit && left< limit)
    {
      Turn_over();
      move = 'o';
    }      
    else if(left>limit && front<limit)
    {
      Turn_left();
      move = 'l';
    }
  }   
  else
  {
    Turn_right();
    move = 'r';
  }
  if(front<limit)
    fb=1;
  else
    fb=0;
  if(right<limit)
    rb=1;
  else
    rb=0;
  if(left<limit)
    lb=1;
  else
    lb=0;
  return move;        
      
}


void Turn_left(){
  //750
  const int duration=800;
  servo_speed(14,-50);
  servo_speed(15,-50);
  pause(duration);
  servo_speed(14,0);
  servo_speed(15,0);
}
void Turn_right(){
  const int duration =745;
  servo_speed(14,50);
  servo_speed(15,50);
  pause(duration);
  servo_speed(14,0);
  servo_speed(15,0);
}

void Straight(){
  const int duration =1900;
  servo_speed(14,50);
  servo_speed(15,-50);
  pause(duration);
  servo_speed(14,0);
  servo_speed(15,0);
}
void Turn_over(){
  const int duration=1490;
  servo_speed(14,-50);
  servo_speed(15,-50);
  pause(duration);
  servo_speed(14,0);
  servo_speed(15,0);
}
void Backward(){
  const int duration=1800;
  servo_speed(14,-50);
  servo_speed(15,50);
  pause(duration);
  servo_speed(14,0);
  servo_speed(15,0);
}


void PING(){                //Use Ping))) to measure distance
  while(lockset(lockR));   //Set the lock 
    cmDist = ping_cm(17);   //get the distance
    lockclr(lockR);        // Clear the lock
    cog_end(cog_ptr[7]);    //close itself
}  


char char2lowercase (char c) {
	return ('A'<=c && c<='Z') ? c+32 : c;
}
/*
void music(int ch){
  wav_stop();
  wav_play(WAVfile[ch-(int)'1']);                 // Pass the wavfile name to wav player (cost 2 core)
  wav_volume(6);                                  // Adjust volume
}  
*/
