#include<stdio.h>
#include<stdlib.h>

//#define mapsize 10
const int mapsize=10;
int map[10][10]={0};
const int wall=3;
const int space=0;
const int visit=1;
int state;//0 right 1 up 2 left 3 down
char move; //s l r
int x;
int y;

int walk(int state, char move){
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
int main(){
    int i,j;
    
    
    map[mapsize-1][0]=wall; // left down corner =wall
    map[mapsize-2][0]=wall;
    map[mapsize-1][1]=wall;
    map[mapsize-2][1]=visit;
    state =0;
    x=1;
    y=8;
    
    while(1)
    {
        printf("next move?\n");
        scanf(" %c",&move);
        state=walk(state,move);
        map[y][x]=visit;
        printf("%c %d %d %d\n",move,x,y,state);
       // if(move=='s')
           // printf("fuck\n");
        for(i=0;i<mapsize;i++){
            for(j=0;j<mapsize;j++){
                printf("%d ",map[i][j]);
            }
            printf("\n");
        }
    }
}
