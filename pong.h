#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"

#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32
#define FREQ 60

#define TRUE 1
#define FALSE 0

#define PLAYER0 0
#define PLAYER1 1

#define VELOCITY 5

#define PADDLE_OFFSET WIDTH / 30
#define PADDLE_WIDTH WIDTH / 50
#define PADDLE_HEIGHT HEIGHT / 6

#define PADDLE_MAX_VEL 4

#define SCORE_STR_SIZE 3

/*
 * Structures
 */
typedef struct Pong {
    char *title;
    SDL_Surface *screen;
    SDL_Surface *background;
    SDL_Color bgColor;
    SDL_Event event;
    int programExited;
    int gameRunning;
} *Pong;

typedef struct Menu {
    SDL_Surface *headerSurface;
    SDL_Surface *option1Surface;
    SDL_Surface *option2Surface;

    SDL_Color optionColor; /* SDL_Color menuColor = { 255, 0, 0 }; */
    SDL_Color headerColor; /* = { 255, 155, 55 }; */

    TTF_Font *headerFont;
    TTF_Font *optionFont;

} *Menu;

typedef struct Paddle {
    int velocity;
    SDL_Rect box;
} *Paddle;

typedef struct Player {
    char *name;
    int score;
    char *scoreStr;
    SDL_Color color;
    Paddle paddle;
} *Player;

typedef struct Dot {
    int xVel, yVel;
    SDL_Rect box;
    SDL_Color color;
} *Dot;

typedef struct Color {
    Uint8 red;
    Uint8 green;
    Uint8 blue;
} *Color;

/*
 * Globals
 */
Pong myPong;
Menu menu;
Player player0;
Player player1;
Dot dot;

/* Fonts 'n text surfaces */

TTF_Font *scoreFont;

int gameExited = FALSE;


/*
 * Prototypes
 */

/* Initialize game
   returns TRUE on success, FALSE if anything fails. */
int init();

/* Initializes menu surfaces */
void initMenu();

/* Loads game files */
int loadFiles();

void cleanup();

/* Converts and SDL_Color to an Uint32 */
Uint32 SDLColorToUint32(SDL_Color SDLcolor, SDL_Surface *screen);

/* Given a player name and a player number
   returns pointer to a new Player struct
   returns NULL on failure. */
Player newPlayer(char *name, int playerNo);

/* draws the background image to the screen */
void drawBackground();

/* Checks for collisions between two SDL_Rects
   returns TRUE on collision, FALSE otherwise. */
int checkCollision(SDL_Rect A, SDL_Rect B);

/* Returns TRUE if box is within sizeX*sizeY
   returns FALSE oterwise */
int isWithinScreen(SDL_Rect box, int sizeX, int sizeY);

/* Handling the dot movement */
void moveDot(Dot dot);

/* Updates the position of the dot */
void drawDot(Dot dot, SDL_Surface *screen);

/* Handles the Paddle movement */
void movePaddle(Paddle paddle);

/* Troll AI: Invlulnerable */
void trollCPU(Player troll);

/* draws the paddle */
void drawPaddle(Player player, SDL_Surface *screen);

/* Handles the players keystrokes */
void handlePlayerInput(Player player);

/* Applies a surface from source to destination
   clip must be set to NULL if not supplied */
void applySurface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip);

/* The main game loop */
void gameLoop();

/* Menu functions */
void menuEvents();
void menuLogic();
void menuRender();

/* Game functions */
void gameEvents();
void gameLogic();
void gameRender();

/* Loads an image, using IMG_Load(), optimizese it and 
   colorkeys it if Color is != NULL
   retunrs an SDL_Surface* on success, NULL on failure */
SDL_Surface *loadImage(char *filename, Color color);

