#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT,DOWN_RIGHT} dir_t;

typedef struct
{
    int i;
    int j;
} Vector2_t;

int train_crash (const char *track, const char *a_train, int a_train_pos, const char *b_train, int b_train_pos, int limit);
Vector2_t** trainVectors(char** track, int cols, int rows, char* train, int pos, dir_t* dir, int zeroPos);
char nextMove(char** track, Vector2_t* pos, int cols, int rows, dir_t* dir);
dir_t getDirection(Vector2_t pos);
bool kamikaze(Vector2_t** trainA, Vector2_t** trainB);
bool TBone(Vector2_t** trainA, Vector2_t** trainB, int aLen, int bLen);
bool cabooser(Vector2_t** trainA, Vector2_t** trainB, int aLen, int bLen);
bool selfDestruct(Vector2_t** train, int len);
bool crashAtStart(Vector2_t** trainA, Vector2_t** trainB, int aLen, int bLen);
bool chickenRun(Vector2_t** trainA, Vector2_t** trainB);
void cleanUp(Vector2_t** trainA, int aLen, Vector2_t** trainB, int bLen, char** track, int rows);
void shiftVectors(Vector2_t** train, int len, Vector2_t headLastPos);
bool inBounds(char** track, Vector2_t* pos, int cols, int rows, dir_t* dir);
void calcMaxWidthAndHeight(char* str, int* width, int* height);
void displayTrack(char** track, int cols, int rows);
int findZeroPosition(char* track);
void printDirection(dir_t dir);

//char advance(dir_t* dir, char** track, int cols, int rows, Vector2_t* pos);
int train_crash (const char *track, const char *a_train, int a_train_pos,
                 const char *b_train, int b_train_pos, int limit)
{
  printf("%s\n", track);

    int rows = 0, cols = 0;
    calcMaxWidthAndHeight(track, &cols, &rows);

    char** rectangularTrack = calloc(rows, sizeof(char*));

    int zeroPosition = findZeroPosition(track);

    int i = 0;
    for(int i = 0; i < rows; i++)
        *(rectangularTrack + i) = calloc(cols, sizeof(char));

    for(int i = 0, k = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++, k++)
        {
            if(track[k] == '\n')
            {
                k++;
                break;
            }
            rectangularTrack[i][j] = track[k];
        }
    }

    //displayTrack(rectangularTrack, cols, rows);

    int aLen = strlen(a_train);
    int bLen = strlen(b_train);

    dir_t trainADir;
    dir_t* trainADirPtr = &trainADir;
    Vector2_t** trainA = trainVectors(rectangularTrack, cols, rows, a_train, a_train_pos, trainADirPtr, zeroPosition);
    bool aStation = false;

    dir_t trainBDir;
    dir_t* trainBDirPtr = &trainBDir;
    Vector2_t** trainB = trainVectors(rectangularTrack, cols, rows, b_train, b_train_pos, trainBDirPtr, zeroPosition);
    bool bStation = false;
  

    for(int i = 0; i < aLen; i++)
        printf("train A : %d %d\n", trainA[i]->i, trainA[i]->j);
    for(int i = 0; i < bLen; i++)
        printf("train B : %d %d\n", trainB[i]->i, trainB[i]->j);

    int aTimer = aLen - 1;
    int bTimer = bLen - 1;

    if(crashAtStart(trainA, trainB, aLen, bLen))
    {
      cleanUp(trainA, aLen, trainB, bLen, rectangularTrack, rows);
      return 0;
    }
  
    bool aisExpress = false;
    bool bisExpress = false;

    if(a_train[0] == 'X' || a_train[0] == 'x')
        aisExpress = true;
    if(b_train[0] == 'X' || b_train[0] == 'x')
        bisExpress = true;

    bool aS = false;
    bool bS = false;

    bool setatrue = false;
    bool setbtrue = false;
    int elapsedTime = 0;

    printf("train A initial direction: ");
    printDirection(trainADir);
    printf("train B initial direction: ");
    printDirection(trainBDir);
  
    for(;elapsedTime < limit; elapsedTime++)
    {

        if(kamikaze(trainA, trainB) && (aStation||bStation))
        {
          cleanUp(trainA, aLen, trainB, bLen, rectangularTrack, rows);
          printf("KAMIKAZE + 1");
          return elapsedTime+1;
        }

        if(kamikaze(trainA, trainB))
        {
          cleanUp(trainA, aLen, trainB, bLen, rectangularTrack, rows);
          printf("KAMIKAZE + 0");
          return elapsedTime;
        }
      
        if(cabooser(trainA, trainB, aLen, bLen) && (aStation == false && bStation == false))
        {
          cleanUp(trainA, aLen, trainB, bLen, rectangularTrack, rows);
          return elapsedTime;
        }

        if(cabooser(trainA, trainB, aLen, bLen) && (aStation || bStation))
        {
          cleanUp(trainA, aLen, trainB, bLen, rectangularTrack, rows);
          return elapsedTime-1;
        }

        if(TBone(trainA, trainB, aLen, bLen))
        {
            cleanUp(trainA, aLen, trainB, bLen, rectangularTrack, rows);
            return elapsedTime;
        }
      
        if(TBone(trainA, trainB, aLen, bLen) && (aStation || bStation))
        {
            cleanUp(trainA, aLen, trainB, bLen, rectangularTrack, rows);
            return elapsedTime;
        }
      

        if(aTimer <= 0 && aStation)
        {
            aTimer = aLen - 1;
            aStation = false;
        }

        if(bTimer <= 0 && bStation)
        {
            bTimer = bLen - 1;
            bStation = false;
        }

        if(aStation)
            aTimer--;

        if(bStation)
            bTimer--;

        if(aStation == false)
        { 
          Vector2_t headLastPos;
          headLastPos.i = trainA[0]->i;
          headLastPos.j = trainA[0]->j; 
          char c = nextMove(rectangularTrack, trainA[0], cols, rows, trainADirPtr);
          
          //printf("Head i j %d %d\n", trainA[0]->i, trainA[0]->j);
          //printf("C1 i j %d %d\n", trainA[1]->i, trainA[1]->j);
          shiftVectors(trainA, aLen, headLastPos);
          
          //printf("Head i j %d %d\n", trainA[0]->i, trainA[0]->j);
          //printf("C1 i j %d %d\n", trainA[1]->i, trainA[1]->j);
          
          if(selfDestruct(trainA, aLen))
              return elapsedTime+1;
          
          if(c == 'S' && !aisExpress)
                  aStation = true;
            
        }

        if(bStation == false)
        {
          Vector2_t headLastPos;
          headLastPos.i = trainB[0]->i;
          headLastPos.j = trainB[0]->j;

          char c = nextMove(rectangularTrack, trainB[0], cols, rows, trainBDirPtr);
          
          //printf("Head i j %d %d\n", trainB[0]->i, trainB[0]->j);
          //printf("C1 i j %d %d\n", trainB[1]->i, trainB[1]->j);
          shiftVectors(trainB, bLen, headLastPos);
          
          //printf("Head i j %d %d\n", trainB[0]->i, trainB[0]->j);
          //printf("C1 i j %d %d\n", trainB[1]->i, trainB[1]->j);
          
          if(selfDestruct(trainB, bLen))
            return elapsedTime+1;

          if(c == 'S' && !bisExpress)
            bStation = true;
        }
      
      
    }

    return -1;
}

/*
int main()
{
    const char* track =
    "                                /------------\\\n"
    "/-------------\\                /             |\n"
    "|             |               /              S\n"
    "|             |              /               |\n"
    "|        /----+--------------+------\\        |   \n"
    "\\       /     |              |      |        |     \n"
    " \\      |     \\              |      |        |                    \n"
    " |      |      \\-------------+------+--------+---\\\n"
    " |      |                    |      |        |   |\n"
    " \\------+--------------------+------/        /   |\n"
    "        |                    |              /    | \n"
    "        \\------S-------------+-------------/     |\n"
    "                             |                   |\n"
    "/-------------\\              |                   |\n"
    "|             |              |             /-----+----\\\n"
    "|             |              |             |     |     \\\n"
    "\\-------------+--------------+-----S-------+-----/      \\\n"
    "              |              |             |             \\\n"
    "              |              |             |             |\n"
    "              |              \\-------------+-------------/\n"
    "              |                            |               \n"
    "              \\----------------------------/ \n";

    //printf("Elapsed Time: %d\n", train_crash (track, "Aaaa", 147, "Bbbbbbbbbbb", 288, 1000));

    const char* track2 =
    "/-----------------\\\n"
    "|                 |\n"
    "|                 |\n"
    "|                 |\n"
    "|                 |\n"
    "\\---------S-------/\n";

        //printf("Elapsed Time: %d\n", train_crash (track2, "xX", 15, "Zzzzzzzzzzzzzz", 40, 100));
        //printf("Elapsed Time: %d\n", train_crash (track2, "aA", 10, "bbbbbB", 30, 200));

    const char* track3 =
    "/-------\\\n"
    "|       |\n"
    "|       |\n"
    "|       |\n"
    "\\-------+--------\\\n"
    "        |        |\n"
    "        |        |\n"
    "        |        |\n"
    "        \\--------/\n";




    //printf("Elapsed Time: %d\n", train_crash(track3, "aaaA", 0, "bbbbbbbbbbbbbB", 30, 100));


    const char* track4 =
    "/-------\\\n"
    "|       |\n"
    "|       |\n"
    "\\-------+-------------S-----------------------------------------------------\\\n"
    "        |                                                                   |\n"
    "        |                                                                   |\n"
    "        \\-------------------------------------------------------------------/\n";

    //printf("Elapsed Time: %d\n", train_crash(track4, "aA", 10, "oooooooooooooooooooooooooO", 70, 200));

    const char* track5 =
    "/----\\     /----\\\n"
    "|     \\   /     |\n"
    "|      \\ /      |\n"
    "|       S       |\n"
    "|      / \\      |\n"
    "|     /   \\     |\n"
    "\\----/     \\----/\n";

    //printf("Elapsed Time: %d\n", train_crash (track5, "Eeeeeeee", 32, "Xxxx", 23, 100));

//printf("elapsed time: %d\n", train_crash (track5, "Eee", 10, "aaA", 20, 100));


    const char* track6 =
    "/---\\\n"
    "|   |\n"
    "\\--\\|\n"
    "   ||\n"
    "   |\\------\\\n"
    "   |/----\\ |\n"
    "   ||    | |\n"
    "/--/|    | |\n"
    "|   |    | |\n"
    "\\---/    \\-/\n";




    printf("elapsed time: %d\n", train_crash (track6, "Aaaaaaaa", 12, "Xxx", 43, 100));

    const char* track7 =
"/-------\\\n"
"|       |\n"
"|       |\n"
"|       |\n"
"\\-------+-------------S-----------------------------------------------\\\n"
"        |                                                             |\n"
"        |                                                             |\n"
"        \\-----------------------------------S-------------------------+--------\\\n"
"                                                                      |        |\n"
"                                                                      |        |\n"
"                                                                      |        |\n"
"                                                                      \\--------/\n";

    //printf("elapsed time: %d\n", train_crash (track7, "ddddD", 134, "aaaaaaA", 127, 100));



    return 0;
}
*/

Vector2_t** trainVectors(char** track, int cols, int rows, char* train, int pos, dir_t* dir, int zeroPos)
{
    int len = strlen(train);

    Vector2_t** vectors = calloc(len, sizeof(Vector2_t*));

    for(int i = 0;i < len; i++)
        vectors[i] = calloc(1, sizeof(Vector2_t));

    char c;
    dir_t dirTrain = RIGHT;
    dir_t* dirPtr = &dirTrain;

    Vector2_t head;
    head.i = 0;
    head.j = zeroPos;

    Vector2_t* headPtr = &head;
    dir_t* lastDir;
    for(int i = 0; i < pos; i++)
    {
        c = nextMove(track, headPtr, cols, rows, dirPtr); // finding the position of the engine on the track.
        //printDirection(*dirPtr);
    }
  
    int rot = train[0] >= 'A' && train[0] <= 'Z' ? 1 : 0; // 1: anti-clockwise / 0: clockwise.
    //if(rot == 1)
      //*dirPtr = LEFT;
    //else
      //*dirPtr = RIGHT;
  
    vectors[0]->i = headPtr->i;
    vectors[0]->j = headPtr->j;
  
    Vector2_t locate;
    locate.i = 0;
    locate.j = zeroPos;
  
    Vector2_t* locatePtr = &locate;
  
  /*
    while(locatePtr->i != headPtr->i || locatePtr->j != headPtr->j)
    {
      c = nextMove(track, locatePtr, cols, rows, dirPtr);
    }
  */
    dir_t copy = *dirPtr;
    //printf("rot: %d\n", rot);
    
    if(rot == 0)
    {
    switch(*dirPtr)
    {
      case LEFT: *dirPtr = RIGHT; break;
      case RIGHT: *dirPtr = LEFT; break;
      case UP: *dirPtr = DOWN; break;
      case DOWN: *dirPtr = UP; break;
      case UP_RIGHT: *dirPtr = DOWN_LEFT; break;
      case UP_LEFT: *dirPtr = DOWN_RIGHT; break;
      case DOWN_LEFT: *dirPtr = UP_RIGHT; break;
      case DOWN_RIGHT: *dirPtr = UP_LEFT; break;
    }
    }
  
    printf("carriages go ");
    printDirection(*dirPtr);
    for(int i = 1; i < len; i++)
    {
        c = nextMove(track, headPtr, cols, rows, dirPtr);
        vectors[i]->i = headPtr->i;
        vectors[i]->j = headPtr->j;
    }
  
    if(rot == 1)
    {
      switch(copy)
    {
      case LEFT: copy = RIGHT; break;
      case RIGHT: copy = LEFT; break;
      case UP: copy = DOWN; break;
      case DOWN: copy = UP; break;
      case UP_RIGHT: copy = DOWN_LEFT; break;
      case UP_LEFT: copy = DOWN_RIGHT; break;
      case DOWN_LEFT: copy = UP_RIGHT; break;
      case DOWN_RIGHT: copy = UP_LEFT; break;
    }
    }
    *dir = copy;
    return vectors;
}

void shiftVectors(Vector2_t** train, int len, Vector2_t headLastPos)
{
    for(int i = 1; i < len; i++)
    {
        int x = train[i]->i;
        int y = train[i]->j;

        train[i]->i = headLastPos.i;
        train[i]->j = headLastPos.j;

        headLastPos.i = x;
        headLastPos.j = y;
    }
}

bool kamikaze(Vector2_t** trainA, Vector2_t** trainB)
{
    int ai = trainA[0]->i;
    int aj = trainA[0]->j;

    int bi = trainB[0]->i;
    int bj = trainB[0]->j;

    return ai == bi && aj == bj;
}

bool TBone(Vector2_t** trainA, Vector2_t** trainB, int aLen, int bLen)
{
    for(int i = 0; i < bLen; i++)
    {
        if(trainA[0]->i == trainB[i]->i && trainA[0]->j == trainB[i]->j)
            return true;
    }

    for(int i = 0; i < aLen; i++)
    {
        if(trainA[i]->i == trainB[0]->i && trainB[0]->j == trainA[i]->j)
            return true;
    }

    return false;
}

bool cabooser(Vector2_t** trainA, Vector2_t** trainB, int aLen, int bLen)
{
    int hai = trainA[0]->i; 
    int haj = trainA[0]->j; 

    int tai = trainA[aLen - 1]->i;
    int taj = trainA[aLen - 1]->j;

    int hbj = trainB[0]->j; 
    int hbi = trainB[0]->i; 

    int tbi = trainB[bLen - 1]->i;
    int tbj = trainB[bLen - 1]->j;

    return (hai == tbi && haj == tbj) || (hbi == tai && hbj == taj);
}

bool selfDestruct(Vector2_t** train, int len)
{
    
    //for(int i = 0; i < len; i++)
      //printf("HARM i j %d %d\n", train[i]->i, train[i]->j);
  
    Vector2_t* head = train[0];
    for(int i = 1; i < len; i++)
    {
        if(train[i]->i == head->i && train[i]->j == head->j)
        {
          printf("SELF HARM!\n");
          return true;
        }
    }
    return false;
}
bool crashAtStart(Vector2_t** trainA, Vector2_t** trainB, int aLen, int bLen)
{

    for(int i = 0; i < aLen; i++)
    {
        for(int j = 0; j < bLen; j++)
            if(trainA[i]->i == trainB[j]->i && trainA[i]->j == trainB[j]->j)
                return true;
    }
    return false;
}

bool chickenRun(Vector2_t** trainA, Vector2_t** trainB)
{
  return trainA[0]->i == trainB[0]->i && trainA[0]->j == trainB[0]->j;
}

void cleanUp(Vector2_t** trainA, int aLen, Vector2_t** trainB, int bLen, char** track, int rows)
{
  for(int i = 0; i < aLen; i++)
    free(trainA[i]);
  free(trainA);
  
  for(int i = 0; i < bLen; i++)
    free(trainB[i]);
  free(trainB);

  for(int i = 0; i < rows; i++)
    free(track[i]);
  free(track);
}

dir_t getDirection(Vector2_t pos)
{
  int i = pos.i;
  int j = pos.j;
  
  if(i == 1 && j == 0)
    return DOWN;
  if(i == 0 && j == 1)
    return RIGHT;
  if(i == 1 && j == 1)
    return DOWN_RIGHT;
  if(i == 0 && j == -1)
    return LEFT;
  if(i == -1 && j == 0)
    return UP;
  if(i == -1 && j == 1)
    return UP_RIGHT;
  if(i == -1 && j == -1)
    return UP_LEFT;
  if(i == 1 && j == -1)
    return DOWN_LEFT;
}


dir_t di[8] = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT,DOWN_RIGHT};
char nextMove(char** track, Vector2_t* pos, int cols, int rows, dir_t* dir)
{
    int i = pos->i;
    int j = pos->j;
    char rail = track[i][j];

    if(rail != '/' && rail != '\\')
    {
        switch(*dir)
        {
            case RIGHT: return track[pos->i][pos->j++];
            case DOWN: return track[pos->i++][pos->j];
            case DOWN_RIGHT: return track[pos->i++][pos->j++];
            case DOWN_LEFT: return track[pos->i++][pos->j--];
            case LEFT: return track[pos->i][pos->j--];
            case UP: return track[pos->i--][pos->j];
            case UP_RIGHT: return track[pos->i--][pos->j++];
            case UP_LEFT: return track[pos->i--][pos->j--];
        }
    }

    // dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '/' && *dir == RIGHT)
    {
        if(inBounds(track, pos, cols, rows, di + 3) && (track[i][j+1] == '-' || track[i][j+1] == '+' || track[i][j+1] == 'S'))
        {
            *dir = RIGHT;
            return track[pos->i][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 5) && (track[i-1][j+1] == '/' || track[i-1][j+1] == 'S' || track[i-1][j+1] == 'X' || track[i-1][j+1] == '+'))
        {
            *dir = UP_RIGHT;
            return track[pos->i--][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 0) && track[i-1][j] == '|')
        {
            *dir = UP;
            return track[pos->i--][pos->j];
        }
    }
// dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '/' && *dir == UP)
    {
        if(inBounds(track, pos, cols, rows, di) && (track[i-1][j] == '|' || track[i-1][j] == '+' || track[i-1][j] == 'S'))
        {
            *dir = UP;
            return track[pos->i--][pos->j];
        }
        if(inBounds(track, pos, cols, rows, di + 3) && (track[i][j+1] == '-' || track[i][j+1] == '+' || track[i][j+1] == 'S'))
        {
            *dir = RIGHT;
            return track[pos->i][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 5) && (track[i-1][j+1] == '/' || track[i-1][j+1] == 'S' || track[i-1][j+1] == 'X' || track[i-1][j+1] == '+'))
        {
            *dir = UP_RIGHT;
            return track[pos->i--][pos->j++];
        }
    }

// dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '/' && *dir == DOWN)
    {

        if(inBounds(track, pos, cols, rows, di + 1) && (track[i+1][j] == '|' || track[i+1][j] == '+' || track[i+1][j] == 'S'))
        {
            *dir = DOWN;
            return track[pos->i++][pos->j];
        }
        if(inBounds(track, pos, cols, rows, di + 2) && (track[i][j-1] == '-' || track[i][j-1] == '+' || track[i][j-1] == 'S'))
        {
            *dir = LEFT;
            return track[pos->i][pos->j--];
        }
        if(inBounds(track, pos, cols, rows, di + 6) && track[i+1][j-1] == '/')
        {
            *dir = DOWN_LEFT;
            return track[pos->i++][pos->j--];
        }
    }

    // dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '/' && *dir == LEFT)
    {
        if(inBounds(track, pos, cols, rows, di + 1) && (track[i+1][j] == '|' || track[i+1][j] == '+' || track[i+1][j] == 'S'))
        {
            *dir = DOWN;
            return track[pos->i++][pos->j];
        }
        if(inBounds(track, pos, cols, rows, di + 6) && (track[i+1][j-1] == '/' || track[i+1][j-1] == 'X' || track[i+1][j-1] == 'S'))
        {
            *dir = DOWN_LEFT;
            return track[pos->i++][pos->j--];
        }
    }

    //////////////////////////////////////////////
// dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};


    if(rail == '/' && *dir == UP_RIGHT)
    {
        if(inBounds(track, pos, cols, rows, di + 3) && track[i][j+1] == '-')
        {
            *dir = RIGHT;
            return track[pos->i][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 5) && (track[i-1][j+1] == '/' || track[i-1][j+1] == 'S' || track[i-1][j+1] == 'X'))
        {
            *dir = UP_RIGHT;
            return track[pos->i--][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di) && track[i-1][j] == '|')
        {
            *dir = UP;
            return track[pos->i--][pos->j];
        }
    }

    // dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};


    if(rail == '/' && *dir == DOWN_LEFT)
    {

        if(inBounds(track, pos, cols, rows, di + 6) && (track[i+1][j-1] == '/' || track[i+1][j-1] == 'S' || track[i+1][j-1] == 'X') )
        {
            *dir = DOWN_LEFT;
            return track[pos->i++][pos->j--];
        }
        if(inBounds(track, pos, cols, rows, di + 1) && (track[i+1][j] == '|' || track[i+1][j] == '+' || track[i+1][j] == 'S'))
        {
            *dir = DOWN;
            return track[pos->i++][pos->j];
        }

        if(inBounds(track, pos, cols, rows, di + 2) && (track[i][j-1] == '-' || track[i][j-1] == '+' || track[i][j-1] == 'S'))
        {
            *dir = LEFT;
            return track[pos->i][pos->j--];
        }
    }

/////////////////////////////   ' / ' DONE   ///// // / / // /

// dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '\\' && *dir == RIGHT)
    {
        if(inBounds(track, pos, cols, rows, di + 1) && (track[i+1][j] == '|' || track[i+1][j] == '+' || track[i+1][j] == 'S') )
        {
            *dir = DOWN;
            return track[pos->i++][pos->j];
        }
      
        if(inBounds(track, pos, cols, rows, di + 3) && (track[i][j+1] == '-' || track[i][j+1] == '+' || track[i][j+1] == 'S') )
        {
            *dir = RIGHT;
            return track[pos->i][pos->j++];
        }

        if(inBounds(track, pos, cols, rows, di + 7) && (track[i+1][j+1] == '\\' || track[i+1][j+1] == 'S' || track[i+1][j+1] == 'X' ))
        {
            *dir = DOWN_RIGHT;
            return track[pos->i++][pos->j++];
        }
    }

    // dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '\\' && *dir == LEFT)
    {
        if(inBounds(track, pos, cols, rows, di + 4) && (track[i-1][j-1] == '\\' || track[i-1][j-1] == 'S' || track[i-1][j-1] == 'X'))
        {
            *dir = UP_LEFT;
            return track[pos->i--][pos->j--];
        }
        if(inBounds(track, pos, cols, rows, di + 0) && (track[i-1][j] == '|' || track[i-1][j] == 'S' || track[i-1][j] == '+'))
        {
            *dir = UP;
            return track[pos->i--][pos->j];
        }
              if(inBounds(track, pos, cols, rows, di + 2) && (track[i][j-1] == '-' || track[i][j-1] == 'S' || track[i][j-1] == '+'))
        {
            *dir = LEFT;
            return track[pos->i][pos->j--];
        }
    }

// dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '\\' && *dir == UP)
    {
        if(inBounds(track, pos, cols, rows, di ) && (track[i-1][j] == '|' || track[i-1][j] == '+' || track[i-1][j] == 'S'))
        {
            *dir = UP;
            return track[pos->i--][pos->j];
        }
        if(inBounds(track, pos, cols, rows, di + 2) && (track[i][j-1] == '-' || track[i][j-1] == '+' || track[i][j-1] == 'S'))
        {
            *dir = LEFT;
            return track[pos->i][pos->j--];
        }
        if(inBounds(track, pos, cols, rows, di + 4) && (track[i-1][j-1] == '\\' || track[i-1][j-1] == 'S' || track[i-1][j-1] == 'X'))
        {
            *dir = UP_LEFT;
            return track[pos->i--][pos->j--];
        }
    }

    if(rail == '\\' && *dir == DOWN)
    {
        if(inBounds(track, pos, cols, rows, di + 7) && (track[i+1][j+1] == '\\' || track[i+1][j+1] == 'S' || track[i+1][j+1] == 'X'))
        {
            *dir = DOWN_RIGHT;
            return track[pos->i++][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 3) && track[i][j+1] == '-')
        {
            *dir = RIGHT;
            return track[pos->i][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 1) && (track[i+1][j] == '|' || track[i+1][j] == 'S' || track[i+1][j] == '+'))
        {
            *dir = DOWN;
            return track[pos->i++][pos->j];
        }
    }

    //////////////////////////////////////////////

// dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '\\' && *dir == DOWN_RIGHT)
    {
        if(inBounds(track, pos, cols, rows, di + 3) && track[i][j+1] == '-')
        {
            *dir = RIGHT;
            return track[pos->i][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 7) && (track[i+1][j+1] == '\\' || track[i+1][j+1] == 'S' || track[i+1][j+1] == 'X'))
        {
            *dir = DOWN_RIGHT;
            return track[pos->i++][pos->j++];
        }
        if(inBounds(track, pos, cols, rows, di + 1) && track[i+1][j] == '|')
        {
            *dir = DOWN;
            return track[pos->i++][pos->j];
        }
    }

    // dir_t di[8] = {UP 0, DOWN 1, LEFT 2, RIGHT 3 , UP_LEFT 4 , UP_RIGHT 5 , DOWN_LEFT 6 ,DOWN_RIGHT 7};

    if(rail == '\\' && *dir == UP_LEFT)
    {
        if(inBounds(track, pos, cols, rows, di + 2) && track[i][j-1] == '-')
        {
            *dir = LEFT;
            return track[pos->i][pos->j--];
        }
        if(inBounds(track, pos, cols, rows, di + 4) && (track[i-1][j-1] == '\\'  || track[i-1][j-1] == 'S' || track[i-1][j-1] == 'X'))
        {
            *dir = UP_LEFT;
            return track[pos->i--][pos->j--];
        }

        if(inBounds(track, pos, cols, rows, di + 0) && track[i-1][j] == '|')
        {
            *dir = UP;
            return track[pos->i--][pos->j];
        }

    }
}

void displayTrack(char** track, int cols, int rows)
{
    printf("\n");
    for(int i = 0; i < rows; i++)
    {
        printf("[");
        for(int j = 0; j < cols; j++)
            printf("%c", track[i][j]);
        printf("]\n");
    }
    printf("\n");
}

void calcMaxWidthAndHeight(char* str, int* width, int* height)
{
    int maxWidth = 0;
    int currWidth = 0;
    int height_ = 0;

    while(*str)
    {
        currWidth++;
        if(*str == '\n')
        {
            ++height_;
            maxWidth = currWidth > maxWidth ? currWidth : maxWidth;
            currWidth = 0;
        }
        str++;
    }
    *width = maxWidth;
    *height = height_;
}

int findZeroPosition(char* track)
{
    int zeroPos = 0;
    while(*track == ' ')
    {
        zeroPos++;
        track++;
    }
    return zeroPos;
}

bool inBounds(char** track, Vector2_t* pos, int cols, int rows, dir_t* dir)
{
    switch(*dir)
    {
        case RIGHT: return (pos->j + 1) < cols;
        case DOWN:  return (pos->i + 1) < rows;
        case DOWN_RIGHT: return (pos->i + 1) < rows && (pos->j + 1) < cols ;
        case DOWN_LEFT: return (pos->i + 1) < rows && (pos->j - 1) >= 0;
        case LEFT: return (pos->j - 1) >= 0;
        case UP: return (pos->i - 1) >= 0;
        case UP_RIGHT: return (pos->i - 1) >= 0 && (pos->j + 1) < cols;
        case UP_LEFT: return (pos->i - 1) >= 0 && (pos->j - 1) >= 0;
    }
}

void printDirection(dir_t dir)
{
  switch(dir)
  {
      case UP: printf("UP\n"); return;
      case DOWN: printf("DOWN\n"); return;
      case LEFT: printf("LEFT\n"); return;
      case RIGHT: printf("RIGHT\n"); return;
      case UP_LEFT: printf("UP_LEFT\n"); return;
      case UP_RIGHT: printf("UP_RIGHT\n"); return;
      case DOWN_LEFT: printf("DOWN_LEFT\n"); return;
      case DOWN_RIGHT: printf("DOWN_RIGHT\n"); return;
      default: printf("UNDEFINED\n"); return;
  }
}
