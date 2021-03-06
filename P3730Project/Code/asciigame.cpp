//ASCII GAME
//CLAYTON SWEETEN

//Welcome to the code!  The comments are ment to guide readers, and keep things straight for organization (what little there may be) A large part of this is using curses, I will include a brief introducion to curses and describe some of the curses functions I used in the Latex file

//My code seems a bit complicated, so I tried to include step by step information where ever possible, so there is a lot of comments in here

//Note* in curses it is common to refer to the y position as "row" and the x as "col", additionaly curses functions take row and col arguments in the form (row,col) or (y,x). (Which can cause some confusion)

//This is veerrry important, includes "curses" what allows me to print to the terminal, and manipulate it.
#include <ncurses.h> 
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <math.h>
//These are for creating the map, helped keep things straight.
#define floor 0
#define wall 1
#define tree 2
#define water 3
#define blank 4
#define sword 5
#define key 6
#define door 7

//define all my functions
void init();
struct enemy  mvenemy(WINDOW *bigmap, WINDOW *messages, WINDOW *view, struct enemy mob, int row, int col);
void add(WINDOW *bigmap,int row, int col, char ch);
void map(WINDOW *bigmap);
struct pos center(WINDOW *bigmap,WINDOW *view,int row, int col);
void game_loop(WINDOW *messages,WINDOW *map,WINDOW *view, char main_char, int row, int col,int ch);

//==========================================================================
// Initialize the ncurses library
//==========================================================================
//This just starts up curses, so that we can use windows, key presses, print stuff to the terminal etc...
void init() {
  initscr();
  clear();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);
}
//=========================================================================
//Enemy structure
//=========================================================================
//Defined by a character, a row position, and a col position.
struct enemy{
  char echar;
  int erow;
  int ecol;
};
//=========================================================================
//Enemy movement
//=========================================================================
//Moves an enemy towards the player if they get within a defined distance
//this function returns an enemy struct, most importantly a row and col position, that the enemy will move to in the game loop.
struct enemy  mvenemy(WINDOW *bigmap, WINDOW *messages, WINDOW *view, struct enemy mob, int row, int col){
  //cacluate the absolute distance to the player
  double dist=sqrt(pow(mob.erow-row,2)+pow(mob.ecol-col,2));
  //specific x and y distances
  double xdistance=mob.ecol-col;
  double ydistance=mob.erow-row;
  //if the player is within a 4 character area
  if(dist<=4){
    //delete the enemy's current position
    wattron(bigmap,COLOR_PAIR(4));
    mvwaddch(bigmap,mob.erow,mob.ecol,'-');
    //Now these next ifs are to determine what direction in the x and y direction the enemy should move
    //if the y distance is less then 4 and positive, move up
    if(ydistance<=4 & ydistance>0){
      mob.erow=mob.erow-1;
      //checks what the next character is
      char target=mvwinch(bigmap,mob.erow,mob.ecol);
      //These are for collision, so the enemy cant go through objects
      if(target=='@' | target=='A' | target=='~' | target=='#' | target=='X' | target=='k' | target=='t'){
	mob.erow=mob.erow+1;
      }
    }
    //if the y distnace is greater then -4 and less then zero, move down
    if(ydistance>=-4 & ydistance<0){
      mob.erow=mob.erow+1;
      char target=mvwinch(bigmap,mob.erow,mob.ecol);
      if(target=='@' | target=='A' | target=='~' | target=='#' | target=='X' | target=='k' | target=='t'){
	mob.erow=mob.erow-1;
      }
    }
    //if the x distance is less then 4 and positive, move left.
    if(xdistance<=4 & xdistance>0){
      mob.ecol=mob.ecol-1;
      char target=mvwinch(bigmap,mob.erow,mob.ecol);
      if(target=='@' | target=='A' | target=='~' | target=='#' | target=='X' | target=='k' | target=='t'){
	mob.ecol=mob.ecol+1;
      }
    }
    //if the x distance is greater then -4 and negative, move right
    if(xdistance>=-4 & xdistance<0){
      mob.ecol=mob.ecol+1;  
      char target=mvwinch(bigmap,mob.erow,mob.ecol);
      if(target=='@' | target=='A' | target=='~' | target=='#' | target=='X' | target=='k' | target=='t'){
	mob.ecol=mob.ecol-1;
      }
    }
  }
  //return the struct, with the new values of x and y, or "col" and "row"
  return mob;
}
//=========================================================================
//Add a character and change color
//=========================================================================
//adds a chosen character at a chosen spot on the map
void add(WINDOW *bigmap,int row, int col, char ch){
  //change the color
  wattron(bigmap,COLOR_PAIR(1));
  //add the character at the given place
  mvwaddch(bigmap,row,col,ch);
} 
//=========================================================================
//Battle function
//=========================================================================
//the function used to battle with an enemy character
//Includes the dice rolls, if the multiplier is >1 then increase the dice roll. Has the win/loss screen as well
struct enemy battle(WINDOW *messages, WINDOW *bigmap,WINDOW *view,char ch, struct enemy mob, int multiplier, int boss){
  //start the message window
  wrefresh(messages);
  //seed for the random number generator
  srand(time(NULL));
  //define the player and monster "dice"
  int playerroll;
  int monsterroll;
  //if the multiplier is greater then zero, then that means they picked up a sword
  if(multiplier>0){
    //so this should give a player dice roll from 5-6
    playerroll=(rand()%2)+5;
    //and a monster dice roll from 1-5
    monsterroll=(rand()%5)+1;
  }
  else{
    // if they havent picked up the sword, their dice roll chance is decreased to 1-6
    playerroll=(rand()%6)+1;
    monsterroll=(rand()%5)+1;
  }
  //If they encounter the boss, set the boss's roll to be 4-6
  if(boss==1){
    monsterroll=(rand()%2)+4;
  }
  //If the player encounters an enemy, either a normal 'X' or the boss 'B'
  if(ch=='X' | 'B'){
    //Print the rolling message to the terminal
    mvwprintw(messages,0,0,"Enemy! , You roll %d, He rolls %d", playerroll,monsterroll);
    wrefresh(messages);
    // if they win the dice roll
    if(playerroll>monsterroll){
      //delete the monster (technically I just moved him to a place of the map....)
      wattron(bigmap,COLOR_PAIR(4));
      mvwaddch(bigmap,mob.erow,mob.ecol,'-');
      mvwprintw(messages,0,0,"You win!");
      wrefresh(view);
      wrefresh(messages);
      //This function waits for user input
      getch();
      //This is where the monster is moved when he is defeated (somewhere the player cant go/see)
      mob.erow=500;
      mob.ecol=500;
    }
    // If they tie, say so
    if(playerroll==monsterroll){
      mvwprintw(messages,0,0,"Tie!    ");
      wrefresh(messages);
      getch();
    }
    // If the monster wins the roll, print a lose message and quit the game :(
    if(playerroll<monsterroll){
      mvwprintw(messages,0,0,"You lose!");
      wrefresh(messages);
      getch();
      //wait for a key press
      //then clear all the map/message screen
      wclear(bigmap);
      wclear(messages);
      wrefresh(bigmap);
      wrefresh(messages);
      //print the loss message
      mvwprintw(messages,0,0,"You lost :(");
      wrefresh(messages);
      getch();
      //this function is important when exiting curses, otherwise the terminal malfunctions
      endwin();
      //exits the program
      exit(0);
    }
  }
  //cleans up the message screen after each battle
  wclear(messages);
  wrefresh(messages);
  //return the mobs' info
  return mob;
}
//=========================================================================
//Map function
//=========================================================================
//The map is created by defining a massive array, and printing to the terminal with the "for" loop below
void map(WINDOW *bigmap){
  //some defined colors
  init_pair(1,COLOR_RED,COLOR_BLACK);
  init_pair(2,COLOR_GREEN,COLOR_BLACK);
  init_pair(3,COLOR_BLUE,COLOR_BLACK);
  init_pair(4, COLOR_WHITE,COLOR_BLACK);
  init_pair(5,COLOR_BLACK,COLOR_BLACK);
  init_pair(6,COLOR_YELLOW,COLOR_BLACK);
  init_pair(7,COLOR_MAGENTA,COLOR_BLACK);
  
  //This represents the entire game map, I defined a massive array of numbers, each number represents a character (remember those define statements at the top?)
 int map1[24][99]={
   {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
   {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,2,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,2},
   {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,2,0,0,0,0,0,2,2,0,0,2,2,2,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,2},
   {2,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,0,0,0,0,0,2,2,0,0,0,5,0,0,2},
   {2,0,2,2,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,0,0,0,0,2,2,0,0,0,0,0,0,2},
   {2,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,3,3,0,0,0,2,2,2,0,0,0,0,0,2},
   {2,0,0,2,2,0,0,0,2,2,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,2},
   {2,0,0,2,2,2,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,2},
   {2,0,0,0,0,2,0,0,0,0,0,0,0,2,2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,2,2,0,0,0,0,1,0,0,0,0,0,1,0,2,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,2,0,0,0,0,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
   {2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
   {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,1,1,0,0,1,1,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,1,1,0,0,0,1,1,0,2,2,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
   {4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,1,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,4,1,0,1,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,0,1,4,4,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,7,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,4,4,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,4,4,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,0,1,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,1,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4},
   {4,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4},
   {4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
   {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
 };
 
 //For loop that iterates through the y/x values and prints to the terminal depending on what number it encounters
 for (int y=0; y<24; y++){
   move(y,0);
   for(int x=0; x<99; x++){
     switch(map1[y][x]){
     case floor:// So if the number is defined as a floor (0) 
       wattron(bigmap,COLOR_PAIR(4));//change the color
       mvwaddch(bigmap, y, x,'-');//and add the chosen character at that position on "bigmap"
       break;
     case wall:
       wattron(bigmap,COLOR_PAIR(4));
       mvwaddch(bigmap, y, x, '#');
       break;
     case tree:
       wattron(bigmap,COLOR_PAIR(2));
       mvwaddch(bigmap,y,x, 'A');
       break;
     case water:
       wattron(bigmap,COLOR_PAIR(3));
       mvwaddch(bigmap,y,x,'~');
       break;
     case blank:
       wattron(bigmap,COLOR_PAIR(5));
       mvwaddch(bigmap,y,x,' ');
       break;
     case sword:
       wattron(bigmap,COLOR_PAIR(7));
       mvwaddch(bigmap,y,x,'t');
       break;
     case  key:
       wattron(bigmap,COLOR_PAIR(6));
       mvwaddch(bigmap,y,x,'k');
       break;
     case door:
       wattron(bigmap,COLOR_PAIR(6));
       mvwaddch(bigmap,y,x,'/');
       break;
      }
    }
  }
}
//=========================================================================
//This class was created so that I could return two numbers with the following function, the class basically contains an x and y coordinate of the window
//=========================================================================
struct pos{
  int windowrow;
  int windowcol;
};
//=========================================================================
//Center Function
//=========================================================================\
//This is arguable the most complicated function besides the game loop itself, the idea is it keeps a small window(aka a viewport) centered around the character, and moves when they do.  Most importantly the viewport stops if it reaches the edge of the map
struct pos center(WINDOW *bigmap,WINDOW *view,int row, int col){
  
  struct pos vpos;
  int y, x;
  //get the dimensions of the bigmap
  getmaxyx(bigmap,y,x);
  int vy, vx;
  //get the dimensions of the viewport
  getmaxyx(view,vy,vx);
  //so curses defines a window's position by its top left corner, these two variables give me the center of the window
  int xwin=col-vx/2;
  int ywin=row-vy/2;
  int newx, newy;
  //The window is moved some distance call it "delta" if that distance goes outside of the bigmap, move the small window back the same distance "delta" this effectively keeps the viewport inside the bigmap.
  if(xwin + vx >= x){
    int delta = x - (xwin + vx);
    newx=xwin+delta;
  }
  else{
    //If it doesnt encounter an edge, then move it with the player like normal
    newx=xwin;
  }
  //check the y direction
  if(ywin + vy >= y){
    int delta = y - (ywin + vy);
    newy=ywin+delta;
  }
  else{
    newy=ywin;
  }
  //these are to see if it goes the the far left or to the top, which are defined as (0,0) in curses
  // I dont need the center in these cases because I can set the windos top left corner to be 0, which keeps it in the bigmap
  if(ywin<0){
    newy=0;
  }
  if(xwin<0){
    newx=0;
  }
  vpos.windowrow=newy;
  vpos.windowcol=newx;

  //return the new place to move the viewport to
  return vpos;
}
//=========================================================================
// Main loop of the game
//==========================================================================
void game_loop(WINDOW *messages,WINDOW *map,WINDOW *view, char main_char, int row, int col,int ch) {
  //This is where everything in the game is, movement, collision, centering, etc....
  //Its quite long and kinda complicated.  I will try to explain it step by step, for one key press direction (the other directions are all pretty much the same)
  
  // First check if the user wants to quit
  if(ch == 'q' || ch =='Q') return;

  //Define and create all of the monsters (AKA "mobs")
  enemy mob1;
  enemy mob2;
  enemy mob3;
  enemy mob4;
  enemy mob5;
  enemy mob6;
  
  //define their starting positions and character
  mob1.erow=5;
  mob1.ecol=33;
  mob1.echar='X';
  mob2.erow=8;
  mob2.ecol=70;
  mob2.echar='X';
  mob3.erow=13;
  mob3.ecol=12;
  mob3.echar='X';
  mob4.erow=20;
  mob4.ecol=8;
  mob4.echar='X';
  mob5.erow=21;
  mob5.ecol=9;
  mob5.echar='X';
  //this one is the 'boss'
  mob6.erow=16;
  mob6.ecol=91;
  mob6.echar='B';
  
  //add them all to the map
  add(map,mob1.erow,mob1.ecol,mob1.echar);
  add(map,mob2.erow,mob2.ecol,mob2.echar);
  add(map,mob3.erow,mob3.ecol,mob3.echar);
  add(map,mob4.erow,mob4.ecol,mob4.echar);
  add(map,mob5.erow,mob5.ecol,mob5.echar);
  add(map,mob6.erow,mob6.ecol,mob6.echar);
  
  //add the main character, position, color, etc..
  struct pos viewpos; 
  wattron(map, COLOR_PAIR(1));
  mvwaddch(map,row,col,'@');
  //center the viewport on the player
  viewpos=center(map,view,row,col);
  mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  //This is how you refresh a window if its a small one inside a bigger one
  touchwin(map);
  wrefresh(view);
  //Define the "multiplier" that is used by the battle function to determine the dice rolls
  int multiplier=0;
  //Start the number of keys at 0
  int keys=0;
  //Next the movement loop, this is where things got disorganized :(
  while(1) {
    //This variables is used to determine if the player has encountered the boss yet, for use in the battle function
    int boss=0;
    //Detect a key press
    ch = getch();
    //If they press left
    if(ch == KEY_LEFT) {
      //Erase their previous position
      wattron(map,COLOR_PAIR(4));
      mvwaddch(map,row, col,'-');
      //move them to the left
      col = col - 1;
      wattron(map, COLOR_PAIR(1));
      char target=mvwinch(map,row,col);
      //If their next position is a 'B', the boss.
      if(target=='B'){
	//set the boss variable to 1, letting the battle function know to use the boss's dice
	boss=1;
	col=col+1;
  	mvwaddch(map,row,col,main_char);
	mob6=battle(messages,map,view,target,mob6,multiplier,boss);
	//if his new position is 500, then that means he was defeated, (remember in the battle function I moved the enemy to (500,500) if they were defeatetd)
	if(mob6.erow==500){
	  //clear all the maps, print a you win message and quit the game
	  wclear(messages);
	  wclear(map);
	  wrefresh(messages);
	  wrefresh(map);
	  mvwprintw(messages,0,0,"You win! :)");
	  wrefresh(messages);
	  getch();
	  endwin();
	  exit(0);
	}

      }
      //A key
      if(target=='k'){
	//move the player over the key
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	//let them know they picked it up
	mvwprintw(messages,0,0,"You picked up a key");
	wrefresh(messages);
	//wait for them to press something
	getch();
	wclear(messages);
	wrefresh(messages);
	//then add the number of keys, (I only have 1 in the game)
	keys=keys+1;
      }
      // A sword
      if(target=='t'){
	//again let them pick it up
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	//Let them know they got a sword
	mvwprintw(messages,0,0,"You picked up a sword!");
	wrefresh(messages);
	getch();
	wclear(messages);
	wrefresh(messages);
	//and add the variable that tells the battle function if they have a sword
	multiplier=multiplier+1;
      }
      //Next if is used for collision, if the next character is any one of these, dont move
      if(target=='#' | target=='A' | target=='~' | target=='X' | target=='B'){
  	col=col+1;
  	mvwaddch(map,row,col,main_char);
	//battle functions for all of the mobs individually, I couldnt figure out a better way
	if(col-1==mob1.ecol){
	  mob1=battle(messages,map,view,target,mob1,multiplier,boss);
	  add(map,mob1.erow,mob1.ecol,mob1.echar);
	}
	if(col-1==mob2.ecol){
	  mob2=battle(messages,map,view,target,mob2,multiplier,boss);
	  add(map,mob2.erow,mob2.ecol,mob2.echar);
	}
	if(col-1==mob3.ecol){
	  mob3=battle(messages,map,view,target,mob3,multiplier,boss);
	  add(map,mob3.erow,mob3.ecol,mob3.echar);
	}
	if(col-1==mob4.ecol){
	  mob4=battle(messages,map,view,target,mob4,multiplier,boss);
	  add(map,mob4.erow,mob4.ecol,mob4.echar);
	}
	if(col-1==mob5.ecol){
	  mob5=battle(messages,map,view,target,mob5,multiplier,boss);
	  add(map,mob5.erow,mob5.ecol,mob5.echar);
	}
      }
      else{
	//If there wasnt anything blocking the player then procede
  	mvwaddch(map,row,col,main_char);

	//calls to the enemy movement function for each enemy (again best I could figure out)
	mob1=mvenemy(map,messages,view,mob1,row,col);
	add(map,mob1.erow,mob1.ecol,mob2.echar);
	mob2=mvenemy(map,messages,view,mob2,row,col);
	add(map,mob2.erow,mob2.ecol,mob2.echar);
	mob3=mvenemy(map,messages,view,mob3,row,col);
	add(map,mob3.erow,mob3.ecol,mob3.echar);
	mob4=mvenemy(map,messages,view,mob4,row,col);
	add(map,mob4.erow,mob4.ecol,mob4.echar);
	mob5=mvenemy(map,messages,view,mob5,row,col);
	add(map,mob5.erow,mob5.ecol,mob5.echar);
	mob6=mvenemy(map,messages,view,mob6,row,col);
	add(map,mob6.erow,mob6.ecol,mob6.echar);

	//Keep the viewport centered on the player
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
      }
    }
    //Same thing but for the other key directions, mostly the same, some col/row stuff is changed
    //I didnt comment anything on the other directions, so you can scroll all the way down to the main function.
    else if(ch == KEY_RIGHT) {
      wattron(map,COLOR_PAIR(4));
      mvwaddch(map,row, col,'-');
      col = col + 1;
      wattron(map,COLOR_PAIR(1));
      char target=mvwinch(map,row,col);
      if(target=='B'){
	boss=1;
	col=col-1;
  	mvwaddch(map,row,col,main_char);
	mob6=battle(messages,map,view,target,mob6,multiplier,boss);
	if(mob6.erow==500){
	  wclear(messages);
	  wclear(map);
	  wrefresh(messages);
	  wrefresh(map);
	  mvwprintw(messages,0,0,"You win! :)");
	  wrefresh(messages);
	  getch();
	  endwin();
	  exit(0);
	}
	
      }
      if(target=='k'){
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	mvwprintw(messages,0,0,"You picked up a key");
	wrefresh(messages);
	getch();
	wclear(messages);
	wrefresh(messages);
	keys=keys+1;
	}
      if(target=='t'){
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	mvwprintw(messages,0,0,"You picked up a sword!");
	wrefresh(messages);
	getch();
	wclear(messages);
	wrefresh(messages);
	multiplier=multiplier+1;
      }
      if(target=='#' | target=='A'| target=='~' | target=='X' | target=='B'){
  	col=col-1;
	mvwaddch(map,row,col,main_char);
	
	if(col+1==mob1.ecol & target=='X'){
	  mob1=battle(messages,map,view,target,mob1,multiplier,boss);
	  add(map,mob1.erow,mob1.ecol,mob1.echar);
	}
	if(col+1==mob2.ecol & target=='X'){
	  mob2=battle(messages,map,view,target,mob2,multiplier,boss);
	  add(map,mob2.erow,mob2.ecol,mob2.echar);
	}
	if(col+1==mob3.ecol & target=='X'){
	  mob3=battle(messages,map,view,target,mob3,multiplier,boss);
	  add(map,mob3.erow,mob3.ecol,mob3.echar);
	}
	if(col+1==mob4.ecol & target=='X'){
	  mob4=battle(messages,map,view,target,mob4,multiplier,boss);
	  add(map,mob4.erow,mob4.ecol,mob4.echar);
	}
	if(col+1==mob5.ecol & target=='X'){
	  mob5=battle(messages,map,view,target,mob5,multiplier,boss);
	  add(map,mob5.erow,mob5.ecol,mob5.echar);
	}
	
      }
      else{
	mvwaddch(map,row,col,main_char);

	mob1=mvenemy(map,messages,view,mob1,row,col);
	add(map,mob1.erow,mob1.ecol,mob2.echar);
	mob2=mvenemy(map,messages,view,mob2,row,col);
	add(map,mob2.erow,mob2.ecol,mob2.echar);
	mob3=mvenemy(map,messages,view,mob3,row,col);
	add(map,mob3.erow,mob3.ecol,mob3.echar);
	mob4=mvenemy(map,messages,view,mob4,row,col);
	add(map,mob4.erow,mob4.ecol,mob4.echar);
	mob5=mvenemy(map,messages,view,mob5,row,col);
	add(map,mob5.erow,mob5.ecol,mob5.echar);
	mob6=mvenemy(map,messages,view,mob6,row,col);
	add(map,mob6.erow,mob6.ecol,mob6.echar);

	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
      }
    }
    else if(ch == KEY_UP) {
      wattron(map,COLOR_PAIR(4));
      mvwaddch(map,row, col,'-');
      row = row - 1;
      wattron(map,COLOR_PAIR(1));
      char target=mvwinch(map,row,col);
      if(target=='B'){
	boss=1;
	row=row+1;
  	mvwaddch(map,row,col,main_char);
	mob6=battle(messages,map,view,target,mob6,multiplier,boss);
	if(mob6.erow==500){
	  wclear(messages);
	  wclear(map);
	  wrefresh(messages);
	  wrefresh(map);
	  mvwprintw(messages,0,0,"You win! :)");
	  wrefresh(messages);
	  getch();
	  endwin();
	  exit(0);
	}
	
      }
      if(target=='k'){
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	mvwprintw(messages,0,0,"You picked up a key");
	wrefresh(messages);
	getch();
	wclear(messages);
	wrefresh(messages);
	keys=keys+1;
	}
      if(target=='/'){
	if(keys==0){
	  row=row+1;
	  mvwaddch(map,row,col,main_char);
	  mvwprintw(messages,0,0,"You need a key!");
	  wrefresh(messages);
	  getch();
	  wclear(messages);
	  wrefresh(messages);
	}
	  if(keys>0){
	    wattron(map,COLOR_PAIR(4));
	    mvwaddch(map,row-1,col,'-');
	    wattron(map,COLOR_PAIR(1));
	    mvwaddch(map,row,col,main_char);
	    viewpos=center(map,view,row,col);
	    mvderwin(view,viewpos.windowrow,viewpos.windowcol);
	    wrefresh(view);
	    mvwprintw(messages,0,0,"You unlocked the door!");
	    wrefresh(messages);
	    getch();
	    wclear(messages);
	    wrefresh(messages);
	  }
      }
      if(target=='t'){
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	mvwprintw(messages,0,0,"You picked up a sword!");
	wrefresh(messages);
	getch();
	wclear(messages);
	wrefresh(messages);
	multiplier=multiplier+1;
      }
      if(target=='#' | target=='A' | target=='~' | target== 'X' | target=='B'){
	row=row+1;
  	mvwaddch(map,row,col,main_char);

	if(row-1==mob1.erow & target=='X'){
	  mob1=battle(messages,map,view,target,mob1,multiplier,boss);
	  add(map,mob1.erow,mob1.ecol,mob1.echar);
	}
	if(row-1==mob2.erow & target=='X'){
	  mob2=battle(messages,map,view,target,mob2,multiplier,boss);
	  add(map,mob2.erow,mob2.ecol,mob2.echar);
	}
	if(row-1==mob3.erow & target=='X'){
	  mob3=battle(messages,map,view,target,mob3,multiplier,boss);
	  add(map,mob3.erow,mob3.ecol,mob3.echar);
	}
	if(row-1==mob4.erow & target=='X'){
	  mob4=battle(messages,map,view,target,mob4,multiplier,boss);
	  add(map,mob4.erow,mob4.ecol,mob4.echar);
	}
	if(row-1==mob5.erow & target=='X'){
	  mob5=battle(messages,map,view,target,mob5,multiplier,boss);
	  add(map,mob5.erow,mob5.ecol,mob5.echar);
	}
      }
      else{
	mvwaddch(map,row,col,main_char);

	mob1=mvenemy(map,messages,view,mob1,row,col);
	add(map,mob1.erow,mob1.ecol,mob2.echar);
	mob2=mvenemy(map,messages,view,mob2,row,col);
	add(map,mob2.erow,mob2.ecol,mob2.echar);
	mob3=mvenemy(map,messages,view,mob3,row,col);
	add(map,mob3.erow,mob3.ecol,mob3.echar);
	mob4=mvenemy(map,messages,view,mob4,row,col);
	add(map,mob4.erow,mob4.ecol,mob4.echar);
	mob5=mvenemy(map,messages,view,mob5,row,col);
	add(map,mob5.erow,mob5.ecol,mob5.echar);
	mob6=mvenemy(map,messages,view,mob6,row,col);
	add(map,mob6.erow,mob6.ecol,mob6.echar);

	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
	wrefresh(view);
      }
    }
    else if(ch == KEY_DOWN) {
      wattron(map,COLOR_PAIR(4));
      mvwaddch(map,row, col,'-');
      row = row + 1;
      wattron(map,COLOR_PAIR(1));
      char target=mvwinch(map,row,col);
      if(target=='B'){
	boss=1;
	row=row-1;
  	mvwaddch(map,row,col,main_char);
	mob6=battle(messages,map,view,target,mob6,multiplier,boss);
	if(mob6.erow==500){
	  wclear(messages);
	  wclear(map);
	  wrefresh(messages);
	  wrefresh(map);
	  mvwprintw(messages,0,0,"You win! :)");
	  wrefresh(messages);
	  getch();
	  endwin();
	  exit(0);
	}

      }
        if(target=='k'){
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	mvwprintw(messages,0,0,"You picked up a key");
	wrefresh(messages);
	getch();
	wclear(messages);
	wrefresh(messages);
	keys=keys+1;
	}
     if(target=='t'){
	mvwaddch(map,row,col,main_char);
	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
	mvwprintw(messages,0,0,"You picked up a sword!");
	wrefresh(messages);
	getch();
	wclear(messages);
	wrefresh(messages);
	multiplier=multiplier+1;
      }
      if(target=='#' | target=='A' | target=='~' | target=='X' | target=='B'){
	row=row-1;
  	mvwaddch(map,row,col,main_char);

	if(row+1==mob1.erow & target=='X'){
	  mob1=battle(messages,map,view,target,mob1,multiplier,boss);
	  add(map,mob1.erow,mob1.ecol,mob1.echar);
	}
	if(row+1==mob2.erow & target=='X'){
	  mob2=battle(messages,map,view,target,mob2,multiplier,boss);
	  add(map,mob2.erow,mob2.ecol,mob2.echar);
	}
	if(row+1==mob3.erow & target=='X'){
	  mob3=battle(messages,map,view,target,mob3,multiplier,boss);
	  add(map,mob3.erow,mob3.ecol,mob3.echar);
	}
	if(row+1==mob4.erow & target=='X'){
	  mob4=battle(messages,map,view,target,mob4,multiplier,boss);
	  add(map,mob4.erow,mob4.ecol,mob4.echar);
	}
	if(row+1==mob5.erow & target=='X'){
	  mob5=battle(messages,map,view,target,mob5,multiplier,boss);
	  add(map,mob5.erow,mob5.ecol,mob5.echar);
	}
      }
      else{
	mvwaddch(map,row,col,main_char);

	mob1=mvenemy(map,messages,view,mob1,row,col);
	add(map,mob1.erow,mob1.ecol,mob2.echar);
	mob2=mvenemy(map,messages,view,mob2,row,col);
	add(map,mob2.erow,mob2.ecol,mob2.echar);
	mob3=mvenemy(map,messages,view,mob3,row,col);
	add(map,mob3.erow,mob3.ecol,mob3.echar);
	mob4=mvenemy(map,messages,view,mob4,row,col);
	add(map,mob4.erow,mob4.ecol,mob4.echar);
	mob5=mvenemy(map,messages,view,mob5,row,col);
	add(map,mob5.erow,mob5.ecol,mob5.echar);
	mob6=mvenemy(map,messages,view,mob6,row,col);
	add(map,mob6.erow,mob6.ecol,mob6.echar);

	viewpos=center(map,view,row,col);
	mvderwin(view,viewpos.windowrow,viewpos.windowcol);
  	wrefresh(view);
      }
      
    }
    //If they ever press q, quit
    else if(ch == 'q' || ch == 'Q') {
      break;
    }
  }
}
//=============================================================================
// Main Function!
//=============================================================================
//here we are finally, in the main function!

int main() {
  // Define main character symbol and initial position
  int row = 2, col = 2;
  char main_char = '@';
  // Start ncurses
  init();
  //Define the big window that will hold the entire map
  WINDOW *bigmap=newwin(24,114,1,0);
  //Define the small window that will exist inside the map window, and move with the player
  WINDOW *view=derwin(bigmap,12,38,0,0);
  //Define a small window at the top that will display messages
  WINDOW *messages=newwin(1,60,0,0);
  //check if the user's terminal supports colors (The labs computers do)
  if(has_colors()==FALSE){
    endwin();
    printf("nooo colors :(");
    exit(1);
  }
  //start color...
  start_color();
  // Print a welcome message on screen
  mvprintw(1,0,"Welcome! Press any key to play\nUse the arrow keys to move\nNavigate through the forest and dungeon and defeat the boss!\nLook for hidden items if you are having trouble!\nIf you want to quit press q!");
  // Wait until the user press a key
  int ch = getch();
  clear();
  refresh();
  // Draw the map
  map(bigmap);
  //start the game loop
  game_loop(messages,bigmap,view,main_char,row, col,ch);
  // Clear ncurses data structures when they quit.
  endwin(); 
  //thats all folks
  return 0;
}
