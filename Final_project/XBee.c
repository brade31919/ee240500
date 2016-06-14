#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/select.h>

#define BUFLEN (255)
#define ZDEBUG (1)

const int mapsize=10;
int map[10][10]={0};
int state =0;//0 right 1 up 2 left 3 down
int x;
int y;
const int wall=3;
const int space=0;
const int visit=1;
int lb=0,rb=0,fb=0;
int a[4] ={0};

int walk(int state, char move);
void Record(int state);
int* Decode(char * package);

void clean_buffer () {
	fflush(stdin);
	while (getchar() != '\n');
}

int openserial (char *sdevfile) {
	int _serial_d = open(sdevfile, O_RDWR | O_NOCTTY);
	if (_serial_d == -1) perror("Unable to open device\n");
	return _serial_d;
}

int main (int argc, char *argv[]) {
	int serial_d;
	speed_t speed;
	struct termios soptions, soptions_org;
	char command;
	unsigned char send_buf[BUFLEN];
	unsigned char recv_buf[BUFLEN];
	int sent_c, recv_c;

	char move;

	//no port specified
	if (argc == 1) {
		printf("Parameter: /dev/ttyUSB0 [command]\n");
		printf("\"/dev/ttyUSB0\" is a USB serial device to support serial control.\n");
		return 1;
	}

	serial_d = openserial(argv[1]);
	if (serial_d == -1) return 1;

	// ----- Begin of setup serial ports
	tcgetattr(serial_d, &soptions_org);
	tcgetattr(serial_d, &soptions);

	speed = B9600; // Speed options: B19200, B38400, B57600, B115200
	cfsetispeed(&soptions, speed);
	cfsetospeed(&soptions, speed);

	// Enable the reciver and set local mode...
	soptions.c_cflag |= ( CLOCAL | CREAD );
	// Setting Parity Checking (8N1)
	soptions.c_cflag &= ~PARENB;
	soptions.c_cflag &= ~CSTOPB;
	soptions.c_cflag &= ~CSIZE;
	soptions.c_cflag |= CS8;

	// Local setting
	//soptions.c_lflag = (ICANON | ECHO | ECHOE); //canonical
	soptions.c_lflag =  ~(ICANON | ECHO | ECHOE | ISIG); //noncanonical

	// Input setting
	//soptions.c_iflag |= (IXON | IXOFF | IXANY); //software flow control
	soptions.c_iflag |= (INPCK | ISTRIP);
	soptions.c_iflag = IGNPAR;

	// Output setting
	soptions.c_oflag = 0;
	soptions.c_oflag &= ~OPOST;

	// Read options
	soptions.c_cc[VTIME] = 0;
	soptions.c_cc[VMIN] = 1; //transfer at least 1 character or block

	// Apply setting
	tcsetattr(serial_d, TCSANOW, &soptions);
	// ----- End of setup serial ports
	map[mapsize-1][0]=wall; // left down corner =wall
	map[mapsize-2][0]=wall;
    map[mapsize-1][1]=wall;
    map[mapsize-2][1]=visit;
    state =0;
    x=1;
    y=8;
    int i,j,k;
	int* package;
	char p[10];
	int start =0;
	//system("./plotmap.pyc &");
	
	
		while (1) {
        	printf("Receiving Data\n");
       	 	/*
			printf("Enter command ('q' to exit): ");
			command = getchar();
			clean_buffer();
			if (command == 'q') {
				printf ("Bye!\n");
				break;
			}
			printf("Sending command '%c'...\n", command);
			sent_c = write(serial_d, &command, 1); // Send command
			tcdrain(serial_d);
        	*/
			
			usleep(1000000); // Wait for response
			if(start ==0)
			{
				memset(recv_buf, '\0', BUFLEN);
				recv_c = read(serial_d, recv_buf, BUFLEN); // Get response message
				tcdrain(serial_d);
				if(recv_buf[0] == 'g')
					start =1;
				else
					start=0;
			}
			else{
			for(k=0; k<5;k++){
				memset(recv_buf, '\0', BUFLEN);
				recv_c = read(serial_d, recv_buf, BUFLEN); // Get response message
				tcdrain(serial_d);
				printf("%c\n", recv_buf[0]);
				p[k] = recv_buf[0];	
			}
			}
			package =Decode(p);
			move = p[1];
			state = package[0];
			lb = package[1];
			fb = package[2];
			rb = package[3];
			printf("parameter %d %c %d %d %d\n",p[0],p[1],p[2],p[3],p[4]);
			printf("parameter %d %c %d %d %d\n",state,move,lb,fb,rb);
			//Draw
			if(state==4)
			{
				printf("Finish scanning = =\n");
				break;
			}
			else
			{
				Record(state);
				state = walk(state,move);
    			map[y][x]=visit;
    			//printf("%c %d %d %d\n",move,x,y,state);
				FILE *f = fopen("map.txt","w");
    			for(i=0;i<mapsize;i++){
       				for(j=0;j<mapsize;j++){
           				printf("%d ",map[i][j]);
						fprintf(f,"%d ",map[i][j]);
					}
       				printf("\n");
					fprintf(f,"\n");
    			}
				fclose(f);
				//system("./plotmap.pyc &");
			}
		
	}
		// restore setting and close
		tcsetattr(serial_d, TCSANOW, &soptions_org);
		close(serial_d);
		return 0;
}
int walk(int state, char move)
{
    if(state==0)
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
    }
    else if(state==1)
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
    }
    else if(state==2)
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
    }

    else if(state==3)
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
    }
    return state;
}
int* Decode(char* package)
{
    //int a[4];// state, lb, fb, rb
	if(package[0] == '0')
	{
		a[0] = 0;
	}
	else if(package[0] == '1')
	{
		a[0] = 1;
	}
	else if(package[0] == '2')
	{
		a[0] = 2;
	}
	else if(package[0] == '3')
	{
		a[0] = 3;
	}
	if(package[2] == '0')
	{
		a[1] =0;
	}
	else if(package[2] == '1')
	{
		a[1] = 1;
	}
	if(package[3] == '0')
	{
		a[2] =0;
	}
	else if(package[3] == '1')
	{
		a[2] = 1;
	}
	if(package[4] == '0')
	{
		a[3] =0;
	}
	else if(package[4] == '1')
	{
		a[3] = 1;
	}
	return a;
}
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
	
	

