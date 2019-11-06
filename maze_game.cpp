//
//  ps5.cpp
//  ps5
//
//  Created by Dan Izadi on 9/25/19.
//  Copyright © 2019 CMU. All rights reserved.
//
#define GL_SILENCE_DEPRECATION
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "fssimplewindow.h"

// program that takes the window, breaks into grid cells 25x25
// user can click on cells to create a maze
// user makes start and end points
// create a character
// have that lil guy navigate the maze using path finding algorithm
// user can navigate the maze and try to beat the lil guy
// if the user hits the wall they move backwards
// lil guys like flash wit it so user can put obstacles


const int nRows = 24; // ymax/ysize -- 600 pixels/25 pix per spot
const int nCols = 32; // xmax/xsize -- 800 pixels/25 pix per spot

void drawMap(int grid[nCols][nRows], int userState)
{
    int xsize = 25, ysize = 25,a=2; // pixels per square
    for(int i=0; i<nCols;i++)
    {
        for(int j=0;j<nRows;j++)
        {
            glClearColor(0,0.7,0.5,.3);
            if(grid[i][j] == 0) // 0 is free space
            {
                glColor3ub(255, 250, 250);
            }
            else if (grid[i][j]==-1) // - 1 is not free space
            {
                if (userState == 6)
                {
                    glColor3ub(rand()%100,rand()%100,rand()%100);
                }
                else
                {
                    glColor3ub(0,50,100);//dark blue
                }
                glBegin(GL_QUADS);
            }
            else if (grid[i][j]==2) // 2 is start
            {
                glColor3ub(0,220,0);
            }
            else if (grid[i][j]==3) // 3 is end goal
            {
                glColor3ub(255,50,0);
            }
            else if (grid[i][j]==1)
            {
                glColor3ub(200,200,0);
            }
            
            else if (grid[i][j] > 4)
            {
                //glBlendColor(0,0,0.5,.3);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
                //glBlendEquation(GL_ADD);
            }
            
            if(grid[i][j]!=1)
            {
                glBegin(GL_QUADS);
                glVertex2i(i*xsize+a,j*ysize+a);
                glVertex2i(i*xsize+xsize-a,j*ysize+a);
                glVertex2i(i*xsize+xsize-a, j*ysize+ysize-a);
                glVertex2i(i*xsize+a,j*ysize+ysize-a);
                glEnd();
            }
            else
            {
                glBegin(GL_TRIANGLE_FAN);
                glVertex2i(i*xsize+xsize/2, j*ysize);
                glVertex2i(i*xsize,j*ysize+ysize);
                glVertex2i(i*xsize+xsize-a, j*ysize+ysize);
                glEnd();
            }
            glDisable(GL_BLEND);
        }
    }
}

void drawPlayer(double x,double y, int player)
{
    // draw the user and opponent
    const double PI = 3.1415926;
    double r=4.0;
    if (player == 1)
    {
        glColor3ub(255,0,0);
        x = x*25+12.5;
        y=y*25+20;
    }
    else if (player == 0)
    {
        glColor3ub(0,0,255);
        x = x*25+12.5;
        y=y*25+5;
    }
    
    glBegin(GL_TRIANGLE_FAN);

    for(int i=0; i<360; i+=20)
    {
        double a=(double)i*PI/180.0;
        double s=sin(a);
        double c=cos(a);
        glVertex2d(x+c*r,y-s*r);
    }
    glEnd();
    
}

void drawTrail(double prevX, double prevY)
{
    glBegin(GL_LINE_STIPPLE);
    glColor3ub(0,0,10);
    
    double x = prevX * 25;
    double x_finish  = prevX*25+25;
    double y = prevY * 25;
    double y_finish  = prevY*25+25;

    glLineStipple(1,0*0101 );
    glVertex2f(x,y);
    glVertex2f(x_finish, y_finish);
    glEnd();
}

void Swap(double &a, double &b)
{
    double c=b;
    b=a;
    a=c;
}

void BubbleSort(int n, double values[], double x[], double y[])
{
    int i, j;
    for (i=0; i<n;i++)
    {
        for (j=i+1;j<n;j++)
        {
            if(values[i]>values[j])
            {
                Swap(values[i],values[j]);
                Swap(x[i],x[j]);
                Swap(y[i],y[j]);
            }
        }
    }
}


void pathFind(int endX,int endY, double &compX, double &compY, int grid[nCols][nRows], int prevPosX, int prevPosY)
{
    
    // real time path finding algorithm for computer player
    
    double step = 1.0;
    double xmoves[4]={compX+step,compX-step,compX,compX};
    double ymoves[4]={compY,compY,compY-step,compY+step};
    //  [0] - east, [1]- west, [2]- north, [3]-south
    double costs[4] = {INFINITY, INFINITY, INFINITY, INFINITY};// initialize
    int numPossMoves = 4;
    
    double openX[nRows*nCols], openY[nRows*nCols];
    double goldX[nRows*nCols], goldY[nRows*nCols];

    int open_count=0;
    int gold_count=0;
    
    for (int j=0;j<nRows;j++)
    {
        for (int c=0;c<nCols;c++)
        {
            if (c!=compX && j != compY)
            {
                if (grid[c][j]==0)
                {
                    openX[open_count] = c;
                    openY[open_count] = j;
                    open_count++;
                    // number and coords of unused spots on board
                }
                else if (grid[c][j]==1)
                {
                    goldX[gold_count] = c;
                    goldY[gold_count] = j;
                    gold_count++;
                    // number and coords of gold on board
                }
            }
        }
    }
    
    // closest gold is current goal
    double gold_dist[nRows*nCols];
    int goalx, goaly;
    

    for(int k=0;k<gold_count;k++)
    {
        gold_dist[k]= sqrt((goldX[k]-compX)*(goldX[k]-compX)+(goldY[k]-compY)*(goldY[k]-compY));
    }
    BubbleSort(gold_count, gold_dist, goldX, goldY);
    goalx = goldX[0];
    goaly = goldY[0];
    
    if (gold_count==0)
    {
        goalx = endX;
        goaly=endY;
    }
    
    // get the distances to every unused space on the board
    // if stuck go to nearest unused space
    
    double open_dist[nRows*nCols];
    for (int h=0; h<open_count;h++)
    {
        open_dist[h]= sqrt((openX[h]-compX)*(openX[h]-compX)+(openY[h]-compY)*(openY[h]-compY));
    }
    BubbleSort(open_count, open_dist, openX, openY);
    double failX = openX[0];
    double failY = openY[0];

    // count number of open and used spots around the current cell
    int numSpots=0, numUsed=0;
    int gridValue[4];
    for (int m=0;m<4;m++)
    {
        gridValue[m] = grid[(int)xmoves[m]][(int)ymoves[m]];
        if (gridValue[m]>=0 && gridValue[m]<5)
        {
            numSpots++;
        }
        if (grid[(int)xmoves[m]][(int)ymoves[m]]==5)
        {
            numUsed ++;
        }
    }
    
    for (int i=0;i<4 ; i++)
    {
        gridValue[i] = grid[(int)xmoves[i]][(int)ymoves[i]];
        if (gridValue[i]>=0 && gridValue[i]<5)
        {
            costs[i] = sqrt((xmoves[i]-goalx)*(xmoves[i]-goalx)+(ymoves[i]-goaly)*(ymoves[i]-goaly));//+open_count;
        }
    }
    
    for (int g=0;g<numPossMoves;g++)
    {
        if (numSpots==0 && numUsed != 0)
        {
            if (gridValue[g]>0)
            {
                costs[g] =sqrt((xmoves[g]-goalx)*(xmoves[g]-goalx)+(ymoves[g]-goaly)*(ymoves[g]-goaly));//+open_count;
            }
        }
        else if (numSpots==0 && numUsed == 0)
        {
            if (gridValue[g]==6 && gridValue[g]==7)
            {
                costs[g] =sqrt((xmoves[g]-goalx)*(xmoves[g]-goalx)+(ymoves[g]-goaly)*(ymoves[g]-goaly));//+open_count;
            }
            else if (gridValue[g]>7)
            {
                xmoves[0]=prevPosX;
                ymoves[0] = prevPosY;
            }
        }
        
    }
    
    BubbleSort(numPossMoves, costs, xmoves, ymoves);
    compX = xmoves[0];
    compY= ymoves[0];
    
    int indicator=0; // if stuck
    for (int c = 0;c<4;c++)
    {
        if (costs[c]!=INFINITY)
        {
            indicator++;
        }
    }
    
    if (indicator == 0)
    {
        xmoves[0]=failX;
        ymoves[0]=failY;
    }
    
    compX = xmoves[0];
    compY= ymoves[0];
    
    
}

int countGold(int grid[nCols][nRows])
{
    int gold_count=0;
    for (int i=0;i<nCols;i++)
    {
        for (int j=0;j<nRows;j++)
        {
            if (grid[i][j] == 1)
                gold_count++;
        }
    }
    return gold_count;
}

void getUserKeystroke(int &posX, int &posY)
{
    int step = 1;
    if(0!=FsGetKeyState(FSKEY_RIGHT))
        {
            posX +=step;
        }
        if(0!=FsGetKeyState(FSKEY_LEFT))
        {
            posX -=step;
        }
        if(0!=FsGetKeyState(FSKEY_UP))
        {
            posY-=step;
        }
        if(0!=FsGetKeyState(FSKEY_DOWN))
        {
            posY+=step;
        }
}

int main(void)
{
    FsOpenWindow(0,0,800,600,1);
    srand((int)time(NULL));
    int xsize = 25, ysize = 25; // pixels per squares
    int grid[nCols][nRows];
    for (int i=0; i<nCols; i++) // initialize the map
    {
        for (int j=0; j<nRows; ++j)
        {
            grid[i][j]=-1;
        }
    }
    
    int userState = 0, gold_count=0;// 0 is build state with interactive map
    
    for (;;)
    {
        int posX, posY,begX,begY,endX, endY, gridX, gridY, goldX, goldY, blockX, blockY;
        double compX,compY;
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        FsPollDevice();
        auto key = FsInkey();
        
        int lb,mb,rb,mx,my;
        FsGetMouseEvent(lb,mb,rb,mx,my);
        int refresh_time = 20;
        
        if(lb==1) // on left click
        {
            if (userState == 0)
            {
                gridX = mx/xsize; // get x and y coords of the mouse
                gridY = my/ysize;
                grid[gridX][gridY] = 0; // make the gridspace white
            }
            if (userState == 1)
            {
                begX = mx/xsize; // get x and y coords of the mouse
                begY = my/ysize;
                grid[begX][begY] = 2;
                // make the gridspace black for start
            }
            if (userState == 2)
            {
                endX = mx/xsize; // get x and y coords of the mouse
                endY = my/ysize;
                grid[endX][endY] = 3;
                userState++; // auto change state after one cell is picked for start and end
            }
            if (userState == 4)
            {
                goldX = mx/xsize;
                goldY = my/ysize;
                grid[goldX][goldY]=1;
            }
            if (userState == 5)
            {
                blockX = mx/xsize;
                blockY = my/ysize;
                grid[blockX][blockY] = -1;
            }
        }
        
        
        drawMap(grid, userState);
        
        int player1 = 1; // user
        int player2 = 0;
        if (userState == 4) // 3 is ready-set-go type of deal
        {
            compX = begX;
            compY=begY;
            posY=begY;
            posX=begX;
            drawPlayer(posX, posY, player1);
            drawPlayer(compX, compY, player2);
        }
        
        if(FSKEY_SPACE==key && userState<5)
        {
            userState+=1;
        }
        
        int prevPosX =posX, prevPosY = posY;
        int prevCompX = compX, prevCompY = compY;
        
        if (userState ==5)
        {
            prevCompX=compX;
            prevCompY=compY;
    
            pathFind(endX,endY, compX, compY, grid, prevPosX, prevPosY);
            
            // user input
            getUserKeystroke(posX,posY);
            
            // Cant go outside map
            if(grid[posX][posY]==-1)
            {
                posX = prevPosX;
                posY = prevPosY;
            }
            else if (grid[posX][posY]==3 && gold_count == 0)
            {
                printf("WINNER\n");
                userState = 6;
            }
            else if (grid[posX][posY]==1)
            {
                grid[posX][posY]=0;
            }
            
            // mark the used gridspots
            if(grid[(int)compX][(int)compY]==0)
            {
                grid[(int)compX][(int)compY]=5;
            }
            else if (grid[(int)compX][(int)compY]>4)
            {
                grid[(int)compX][(int)compY]++;
            }
            else if (grid[(int)compX][(int)compY]==1)
            {
                grid[(int)compX][(int)compY]=5;
            }
            else if (grid[(int)compX][(int)compY]==3 && gold_count == 0)
            {
                printf("LOSER\n");
                userState = 6;
            }


            drawPlayer(posX,posY, player1);
            drawPlayer(compX,compY,player2);
            drawTrail(prevCompX, prevCompY);
            
            gold_count = countGold(grid);
            refresh_time = 200;
        }
        
        if (userState ==6)
        {
            refresh_time=200;
            if (key == FSKEY_ESC)
            {
                break;
            }
        }
        
        FsSwapBuffers();
        FsSleep(refresh_time);
    }
    
    
    return 0;
}
