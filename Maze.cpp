#include "Maze.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// the number of the rows and cols can be changed to change the size the maze
// will generate

Stack::Stack(int Size) {// stack structure generates the stack
  size = Size;
  top = -1;
  solving = false;
  stack = (int **)malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++) {
    stack[i] = (int *)malloc(2 * sizeof(int));
  }
}
Stack::~Stack() {// stack destructure free all the memory once the stack is out of scope
  for (int i = 0; i < size; i++) {
    free(stack[i]);
  }
  free(stack);
}
void Stack::peek(int *x, int *y) {//looks at the top element of the stack
  *x = stack[top][0];
  *y = stack[top][1];
}
void Stack::push(int x, int y) {//pushes value on to stack
  top++;
  stack[top][0] = x; // updates the x value
  stack[top][1] = y; // updates the y value
}
void Stack::clear(bool mode) {//clears the stack 
  solving = mode;// set mode to pixel or graph prinitng the frontier
  top = -1;
}
void Stack::pop() { top--; }

Node::Node() { // creates default node;
  for (int i = 0; i < 4; i++)
    edge[i] = false;
  visited = false;
}

PixelMaze::PixelMaze(int w, int h) {
  width = w;
  height = h;
  maze = (int **)malloc(height * sizeof(int *));
  for (int i = 0; i < height; i++) {
    maze[i] = (int *)malloc(width * sizeof(int));
  }
}
PixelMaze::~PixelMaze() {
  for (int y = 0; y < height; y++) {
    free(maze[y]);
  }
  free(maze);
}

GraphMaze::GraphMaze(int r, int c) {
  rows = r;
  cols = c;
  maze = (Node ***)malloc(r * sizeof(Node **));
  for (int y = 0; y < r; y++) {
    maze[y] = (Node **)malloc(c * sizeof(void *)); // Node pointer
    for (int x = 0; x < c; x++) {
      maze[y][x] = new Node(); // Populate maze with dummy nodes.
    }
  }
}
GraphMaze::~GraphMaze() {
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      delete maze[y][x];
    }
    free(maze[y]);
  }
  free(maze);
}

Maze::Maze(int r, int c)
    : stack((2 * r + 1) * (2 * c + 1)), pixelMaze(2 * c + 1, 2 * r + 1),
      graphMaze(r, c) {
  scale = SCREEN / pixelMaze.height;
  screenHeight = pixelMaze.height * scale + 50;
  screenWidth = pixelMaze.width * scale;
}

bool GraphMaze::validPos(int x,
                         int y) { // input an cell location and will return
                                  // false if it has already been visited
  // switch case??? <- NO >:(
  if (x < 0 || y < 0) { // Cant have negative index
    return false;
  }
  if (x >= cols || y >= rows) { // Can't go past bounds
    return false;
  }
  if (maze[y][x]->visited) { // search the visited list to see if there is a
                            // match it will return true
    return false;
  }
  return true; // if the function doesn't find a match it return true;
}

void move(int *x, int *y,
          Direction dir) { // changes the x y values by one based on the direction
  if (dir == UP)
    *y -= 1; // up
  else if (dir == DOWN)
    *y += 1; // down
  else if (dir == LEFT)
    *x -= 1; // left
  else if (dir == RIGHT)
    *x += 1; // right
}

Direction GraphMaze::getDir(int x, int y) { // picks up down left or right.
  Direction validDirs[4];// array of all possible directions
  int size = 0;
  for (int i = 0; i < 4; i++) {
    int nx = x; // new temp x,y
    int ny = y;
    move(&nx, &ny, (Direction)i);      // Move temp node in direction
    if (validPos(nx, ny)) { // See if we havent explored in that direction
      validDirs[size++] = (Direction)i;
    }
  }
  if (size == 0)
    return INVALID; // No valid direction to move
  return validDirs[rand() % size];
}

void Maze::print(int steps) {
  // After profiling, this is by far the slowest part of the program
  // Using textures here would really speed this up
  ClearBackground(BLACK);
  int x = 0;
  int y = 50;
  BeginDrawing();
  for (int i = 0; i < pixelMaze.height; i++) // y
  {
    x = 0;
    for (int j = 0; j < pixelMaze.width; j++) // x
    {
      switch (pixelMaze.maze[i][j]) {
      case START:
        DrawRectangle(x, y, scale, scale, LIME);
        break;
      case END:
        DrawRectangle(x, y, scale, scale, RED);
        break;
      case VISITED:
        DrawRectangle(x, y, scale, scale, SKYBLUE);
        break;
      case NOTIT:
        DrawRectangle(x, y, scale, scale, DARKBLUE);
        break;
      case PATH:
        DrawRectangle(x, y, scale, scale, WHITE);
        break;
      default: // WALL
        // No need to draw over bg colour
        break;
      }
      x += scale;
    }
    y += scale;
  }
  stack.peek(&x, &y);
  if (!stack.solving) {// if the stack is in graph maze mode converts the cords into pixel maze to solve
    x = x * 2 + 1;
    y = y * 2 + 1;
  }
  DrawRectangle(scale * x, 50 + scale * y, scale, scale, PURPLE);
  if (steps > -1) { // Steps it took to solve pass -1 to disable
    DrawText(TextFormat("Maze solved in %i steps!", steps), 30, 10, 40,
             WHITE);
  }
  WaitTime(5.0 / (graphMaze.rows * graphMaze.cols));
  EndDrawing();
  //----------------------------------------------------------------------------------
}

void PixelMaze::render(
    GraphMaze *gMaze) { // Convert first array into one to be printed
  // making the boarder of the array to be printed 0, so theres a wall around it
  // for the horozontal
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      maze[y][x] = WALL;
    }
  }

  for (int y = 0; y < gMaze->rows; y++) { // traverses the whole maze
    for (int x = 0; x < gMaze->cols; x++) {
      int nx = 2 * x + 1;
      int ny = 2 * y + 1;
      if (gMaze->maze[y][x]->edge[UP]) {
        maze[ny - 1][nx] = PATH;
      }
      if (gMaze->maze[y][x]->edge[DOWN]) {
        maze[ny + 1][nx] = PATH;
      }
      if (gMaze->maze[y][x]->edge[RIGHT]) {
        maze[ny][nx + 1] = PATH;
      }
      if (gMaze->maze[y][x]->edge[LEFT]) {
        maze[ny][nx - 1] = PATH;
      }

      if (gMaze->maze[(ny - 1) / 2][(nx - 1) / 2]->visited) {
        maze[ny][nx] = PATH;
        if (x == 0 && y == 0)
          maze[ny][nx] = START;
        if (y == gMaze->rows - 1 && x == gMaze->cols - 1)
          maze[ny][nx] = END;
      }
    }
  }
}
void Maze::generate() {
  srand(time(0)); // returns the time in seconds of the system clock this is
                  // used in the random direction algo
  Node *start = graphMaze.maze[0][0];
  Node *end = graphMaze.maze[graphMaze.rows - 1][graphMaze.cols - 1];
  end->visited = true; // starting path;
  stack.clear(false);
  stack.push(graphMaze.cols - 1,
             graphMaze.rows - 1);   // puts the starting node on the stack
  while (stack.top > -1) { // all nodes are not visted
    pixelMaze.render(&graphMaze);;
    print(); // prints everystep it makes
    int x, y;
    stack.peek(&x, &y);
    Direction dir = graphMaze.getDir(x, y); // get random direction
    if (dir == INVALID ||
        (x == 0 && y == 0)) { // no valid direction or hits the end node
      stack.pop(); // pop untill there is a path to an unexplored node
      continue;    // kicks back to the top of while loop
    }
    int nx = x; // Position of movement
    int ny = y;
    move(&nx, &ny, dir); // move the current location to the next
    graphMaze.maze[y][x]->edge[dir] = true; //
    graphMaze.maze[ny][nx]->visited = true;  // mark new maze node as visted
    stack.push(nx, ny);                     // put new node on the visted list
  }
  save();
}

void Maze::save() { // Save the final array
  FILE *fp;
  fp = fopen("Maze.txt", "w");
  pixelMaze.render(&graphMaze);
  fprintf(fp, "%i %i", graphMaze.rows,
          graphMaze.cols); // Print size so it can be read
  for (int i = 0; i < pixelMaze.height; i++) {
    fprintf(fp, "\n");
    for (int j = 0; j < pixelMaze.width; j++) {
      fprintf(fp, "%i ", pixelMaze.maze[i][j]);
    }
  }
  fclose(fp);
}

// Solve

void Maze::load() {
  FILE *fp = fopen("Maze.txt", "r");
  int r, c;
  fscanf(fp, "%i %i", &r, &c);
  printf("%i x %i", r, c);
  for (int y = 0; y < pixelMaze.height; y++) {
    for (int x = 0; x < pixelMaze.width; x++) {
      fscanf(fp, "%i", &(pixelMaze.maze[y][x]));
    }
  }
  fclose(fp);
}

bool PixelMaze::isSolved(Stack *stack) { // checks if the finish node is in the
                                         // path stack meaning it has
                                         // solved the maze
  int x, y;
  stack->peek(&x, &y);
  if (x == width - 2 && y == height - 2) // Backwards is right
    return true;
  return false;
}
bool PixelMaze::validPos(int x,
                         int y) { // input an cell location and will return
                                  // false if it has already been visited
  if (x < 0 || y < 0) {           // Cant have negative index
    return false;
  }
  if (x >= width || y >= height) { // Can't go past bounds
    return false;
  }
  if (maze[y][x] == VISITED || maze[y][x] == NOTIT ||
      maze[y][x] == WALL) { // search the visited list to see if there is a
                            // match it will return true
    return false;
  }
  return true; // if the function doesn't find a match it return true;
}
Direction PixelMaze::getDir(int *x, int *y) { // pick a direction to travel
  // since we know we start in the top left corner we set a order of directions
  // to check so it will be quicker then randomly searching maybe add branch and
  // bound algo????
  Direction dirPriority[4];
  if (width - *x > width - *y) {
    dirPriority[0] = RIGHT;
    dirPriority[1] = DOWN;
    dirPriority[2] = LEFT;
    dirPriority[3] = UP;
  } else {
    dirPriority[0] = DOWN;
    dirPriority[1] = RIGHT;
    dirPriority[2] = UP;
    dirPriority[3] = LEFT;
  }
  for (int i = 0; i < 4; i++) {
    int nx = *x;
    int ny = *y;
    move(&nx, &ny, dirPriority[i]);
    if (validPos(nx, ny)) { // Found a valid direction
      move(x, y, dirPriority[i]);
      return dirPriority[i];
    }
  }
  return INVALID;
}

int Maze::solve() {
  load();
  int steps = 1;
  stack.clear(true);
  stack.push(1, 1);
  while (!pixelMaze.isSolved(&stack)) { // runs till it finds the end
    print();
    steps++;
    int x, y;
    stack.peek(&x, &y);
    if (pixelMaze.maze[y][x] != START)
      pixelMaze.maze[y][x] = VISITED;
    if (pixelMaze.getDir(&x, &y) == INVALID) {
      pixelMaze.maze[y][x] = NOTIT;
      stack.pop();
      continue;
    }
    stack.push(x, y);
  }
  return steps;
}

int main() {
  printf("\nEnter the width of the maze:");
  int width, height;
  scanf("%i", &width);
  printf("Enter the height of the maze:");
  scanf("%i", &height);
  float aspectRatio = ((float)width)/height;
  InitWindow(900, 950, "Maze");
  InitAudioDevice();
  Sound wii = LoadSound("wii.mp3");
  PlaySound(wii);

  // Solve

  while (!WindowShouldClose()) {
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) break;
    ClearBackground(RAYWHITE);
    BeginDrawing();
     DrawText("Welcome to the best", 200, 60, 50, BLACK);
     DrawText("Maze Generator and Solver!", 90, 120, 50, BLACK);
     DrawText("Press Anywhere to Begin...", 190, 400, 40, BLACK);
     DrawText("By Hari, Justin, Nox, Koa", 30, 890, 40, BLACK);
     DrawText("MREN 178", 30, 830, 40, BLACK);
    EndDrawing();
  }
  CloseWindow();
  int size = height;
  while (true) {
    // Clear the screen and draw the image
    Maze maze = Maze(size, aspectRatio*size);
    InitWindow(maze.screenWidth, maze.screenHeight, "Maze");
    SetTargetFPS(60000);
    size *= 2;
    maze.generate(); // create maze
    maze.print(maze.solve());
    WaitTime(5);
    CloseWindow();
  }
  StopSound(wii);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}

// you are awesome!