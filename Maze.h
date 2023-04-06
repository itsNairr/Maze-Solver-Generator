#pragma once
#define SCREEN 1500 // Screen height
#define WALL 1
#define PATH 0
#define START 2
#define WALL 1
#define PATH 0
#define START 2
#define END 3
#define VISITED -1
#define NOTIT -2

typedef enum {
	UP=0,DOWN=1,LEFT=2,RIGHT=3,INVALID=-1
} Direction;
void move(int *x, int *y, Direction dir);

class Node {
	public:
	bool edge[4];
	bool visited;
	Node();
};

class Stack {
private:
	int size;
	int **stack;
public:
	bool solving; // Toggle generate / solve
	int top;
	Stack(int size);
	~Stack();
	void peek(int *x, int *y);
	void push(int x,int y);
	void clear(bool mode); // Set solving or not
	void pop();
};

class GraphMaze;
class PixelMaze {
	public:
	int width;
	int height;
  int **maze;
	PixelMaze(int w, int h);
	~PixelMaze();
	bool validPos(int x, int y); // Pixel
	Direction getDir(int *x, int *y); // Pixel
	bool isSolved(Stack *stack); // Pixel
	void render(GraphMaze *maze); // Both
};
class GraphMaze {
	public:
	int rows;
	int cols;
	Node ***maze;
	GraphMaze(int r, int c);
	~GraphMaze();
	Direction getDir(int x, int y); // Graph
	bool validPos(int x, int y); // Graph
};

class Maze {
	private:
	int scale;
	public:
	int screenWidth;
	int screenHeight;
		PixelMaze pixelMaze;
		GraphMaze graphMaze;
		Stack stack;
	Maze(int r, int c);
	void generate(); // Graph
	void save();
	void print(int steps=-1);
	void load();
	int solve(); // Pixel
};
