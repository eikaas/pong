#include <stdlib.h>
#include <assert.h>
#include "pong.h"

int init() {
    myPong = malloc(sizeof(struct Pong));

    myPong->programExited = FALSE;
    myPong->gameRunning = FALSE;

    myPong->bgColor.r = 100;
    myPong->bgColor.g = 100;
    myPong->bgColor.b = 100;

    if (myPong) {
        myPong->title = "Simple game of pong";
    } else {
        return FALSE;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to Initialize SDL\n");
        return FALSE;
    }

    if (!(myPong->screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE))) {
        printf("Failed to set video mode (%dx%d@%d)\n", WIDTH, HEIGHT, DEPTH);
        return FALSE;
    }

    if (TTF_Init() < 0) {
        printf("Failed to Initialized SDL_ttf\n");
        return FALSE;
    }

    SDL_WM_SetCaption(myPong->title, NULL);

    /* Initialize the players */
    if ((player0 = newPlayer("Player 1", PLAYER0)) == NULL)
        return FALSE;
    if ((player1 = newPlayer("Player 2", PLAYER1)) == NULL)
        return FALSE;


    /* Allocate memory for the Dot struct */
    if ((dot = malloc(sizeof(struct Dot))) != NULL) {

        /* Velocity should perhaps be random */
        dot->xVel = -1 * VELOCITY;
        dot->yVel = VELOCITY;

        /* Set the initial dot position and dimensions. */
        dot->box.x = WIDTH / 2;
        dot->box.y = HEIGHT / 2;
        dot->box.h = 10;
        dot->box.w = 10;

        /* Set the initial dot color */
        dot->color.r = 128;
        dot->color.g = 255;
        dot->color.b = 128;

    } else {
        return FALSE;
    }

    /* Everything initialized successfully */
    return TRUE;
}

void cleanup() {
    // Free Player
    free(player0->paddle);
    free(player1->paddle);

    free(player0->scoreStr);
    free(player1->scoreStr);

    free(player0);
    free(player1);

    // Free Dot
    free(dot);

    // Free menu
    SDL_FreeSurface(menu->headerSurface);
    SDL_FreeSurface(menu->option1Surface);
    SDL_FreeSurface(menu->option2Surface);

    free(menu->headerFont);
    free(menu->optionFont);

    free(menu);

    // Free Background
    SDL_FreeSurface(myPong->background);

    SDL_FreeSurface(myPong->screen);
    free(myPong);

    SDL_Quit();
}

void initMenu() {
    menu = malloc(sizeof(struct Menu));

    /* Menu options font color */
    menu->optionColor.r = 255;
    menu->optionColor.g = 0;
    menu->optionColor.b = 0;

    /* Menu / Splash font color */
    menu->headerColor.r = 255;
    menu->headerColor.g = 128;
    menu->headerColor.b = 128;

    if ((menu->optionFont= TTF_OpenFont(
                    "/usr/share/fonts/TTF/DejaVuSans.ttf", 24)) == NULL) {
        printf("Error opening font\n");
        exit(1);
    }

    if ((menu->headerFont = TTF_OpenFont(
                    "/usr/share/fonts/TTF/EnvyCodeR.ttf", 28)) == NULL) {
        printf("Error opening font\n");
        exit(1);
    }

    menu->headerSurface = TTF_RenderText_Solid(menu->headerFont,
            "Chiddy Pong", menu->headerColor);
    menu->option1Surface = TTF_RenderText_Solid(menu->optionFont,
            "Press 's' to start new game", menu->optionColor);
    menu->option2Surface = TTF_RenderText_Solid(menu->optionFont,
            "Press 'q' to quit", menu->optionColor);
}

int loadFiles() {
    if ((myPong->background = loadImage("gfx/bg.jpg", NULL)) == NULL)
        return FALSE;

    if((scoreFont = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 81)) == NULL)
        return FALSE;

    /* Everything loaded successfully */
    return TRUE;
}

Uint32 SDLColorToUint32(SDL_Color SDLcolor, SDL_Surface *screen) {
    Uint32 color = SDL_MapRGB(screen->format,
                                SDLcolor.r,
                                SDLcolor.g,
                                SDLcolor.b);
    return color;
}

Player newPlayer(char *name, int playerNo) {
    Player newPlayer;
    Paddle newPaddle;

    /* if memory allocation succeeds, initialize the Player */
    if ( ((newPlayer = malloc(sizeof(struct Player))) != NULL) &&
        (newPaddle = malloc(sizeof(struct Paddle))) != NULL) {

        newPlayer->name = name;

        newPlayer->scoreStr = malloc(sizeof(char) * SCORE_STR_SIZE);

        // Set value and paddle color
        if (playerNo == PLAYER0) {
            newPaddle->box.x = PADDLE_OFFSET;
            newPlayer->color.r = 255;
            newPlayer->color.g = 128;
            newPlayer->color.b = 128;
        } else {
            newPaddle->box.x = WIDTH - PADDLE_WIDTH - PADDLE_OFFSET;
            newPlayer->color.r = 128;
            newPlayer->color.g = 128;
            newPlayer->color.b = 255;
        }

        // Set default values
        newPlayer->score = 0;
        newPaddle->velocity = 0;
        newPaddle->box.y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
        newPaddle->box.w = PADDLE_WIDTH;
        newPaddle->box.h = PADDLE_HEIGHT;

        newPlayer->paddle = newPaddle;

        return newPlayer;
    } else {
        return NULL;
    }
}

void drawBackground() {
    applySurface(0, 0, myPong->background, myPong->screen, NULL);
}

void newBackground() {
    SDL_Rect bg;
    bg.x = 0;
    bg.y = 0;
    bg.h = HEIGHT;
    bg.w = WIDTH;
    SDL_FillRect(myPong->screen, &bg, 
            SDLColorToUint32(myPong->bgColor, myPong->screen));
}

int checkCollision(SDL_Rect A, SDL_Rect B) {
    int leftSideA, leftSideB;
    int rightSideA, rightSideB;
    int topSideA, topSideB;
    int bottomSideA, bottomSideB;

    leftSideA = A.x;
    leftSideB = B.x;

    rightSideA = A.x + A.w;
    rightSideB = B.x + B.w;

    topSideA = A.y;
    topSideB = B.y;

    bottomSideA = A.y + A.h;
    bottomSideB = B.y + B.h;

    if (rightSideA <= leftSideB)
        return TRUE;

    if (topSideA >= bottomSideB)
        return TRUE;

    if (leftSideA >= rightSideB)
        return TRUE;

    if (bottomSideA <= topSideB)
        return TRUE;

    return FALSE;
}

int isWithinScreen(SDL_Rect box, int sizeX, int sizeY) {
    if ((box.x + box.w) > WIDTH -1 || box.x == 0) {
        return FALSE;
    }
    if ((box.y + box.h) >= HEIGHT -1 || box.y == 0) {
        return FALSE;
    }
    return TRUE;
}

void moveDot(Dot dot) {
    /* flip the dot velocity if position
       is at the edge of the screen */

    /* Screen edge collision detection 
       Using the VELOCITY here to compensate for the dot moving VELOCITY number
       of pixels per frame */
    if ((dot->box.x + dot->box.w) > WIDTH - VELOCITY || dot->box.x == 0) {
        /* if the dot is at either edge, give points accordingly */
        if (dot->box.x == 0 && player1->score < 1000) {
            player1->score++;
        } else if ((dot->box.x + dot->box.w) == WIDTH && player0->score < 1000) {
            player0->score++;
        }
        dot->xVel *= -1;
    }
    if ((dot->box.y + dot->box.h) >= HEIGHT - VELOCITY || dot->box.y == 0) {
        dot->yVel *= -1;
    }

    if (checkCollision(player0->paddle->box, dot->box)) {
        dot->xVel *= -1;
    }
    if (checkCollision(player1->paddle->box, dot->box)) {
        dot->xVel *= -1;
    }

    /* move the dot */
    dot->box.x += dot->xVel;
    dot->box.y += dot->yVel;
}

void drawDot(Dot dot, SDL_Surface *screen) {
    SDL_FillRect(screen, &dot->box, 
            SDLColorToUint32(dot->color, myPong->screen));
}

void movePaddle(Paddle paddle) {
    /* if the paddle is moving down, and the edge of the screen is 
       not yet reached - allow the paddle to move downwards*/
    if ((paddle->velocity > 0) && paddle->box.y + paddle->box.h < HEIGHT) {
        paddle->box.y += paddle->velocity;
    }
    /* else if the paddle velocity is upwards, and the top is
       not yet reached - allow the paddle to move upwards */
    else if ((paddle->velocity < 0) && paddle->box.y > 0) {
        paddle->box.y += paddle->velocity;
    }
}

void drawPaddle(Player player, SDL_Surface *screen) {
    SDL_FillRect(screen, &player->paddle->box, 
            SDLColorToUint32(player->color, myPong->screen));
}

/* Todo: Figure out a way to make the keybindings customizable */
void handlePlayerInput(Player player) {
    if (myPong->event.type == SDL_KEYDOWN) {
        switch(myPong->event.key.keysym.sym) {
            case SDLK_UP: player->paddle->velocity = -1 * PADDLE_MAX_VEL; break;
            case SDLK_DOWN: player->paddle->velocity = PADDLE_MAX_VEL; break;
            case SDLK_LEFT: printf("Hurr\n"); break;
            default: break;
        }
    } else if (myPong->event.type == SDL_KEYUP) {
        switch(myPong->event.key.keysym.sym) {
            case SDLK_UP: player->paddle->velocity = 0; break;
            case SDLK_DOWN: player->paddle->velocity = 0; break;
            default: break;
        }
    }
}

void trollCPU(Player troll) {
    if (dot->box.y < troll->paddle->box.y) {
    troll->paddle->velocity = -1 * PADDLE_MAX_VEL;
    } else {
        troll->paddle->velocity = 1 * PADDLE_MAX_VEL;
    }
}

void applySurface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip) {
    SDL_Rect offset;

    assert(source != NULL);
    assert(destination != NULL);

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, clip, destination, &offset);
}

void startNewGame() {
    /* Reset Dot Position */
    dot->xVel = -1 * VELOCITY;
    dot->yVel = VELOCITY;
    dot->box.x = WIDTH / 2;
    dot->box.y = HEIGHT / 2;

    /* Reset player position */
    player0->paddle->box.x = PADDLE_OFFSET;

    player1->paddle->box.x = WIDTH - PADDLE_WIDTH - PADDLE_OFFSET;

    player0->paddle->velocity = 0;
    player1->paddle->velocity = 0;

    player0->paddle->box.y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    player1->paddle->box.y = HEIGHT / 2 - PADDLE_HEIGHT / 2;

    /* reset player score */
    player0->score = 0;
    player1->score = 0;

    /* Start the game */
    myPong->gameRunning = TRUE;
}

void menuEvents() {
    while (SDL_PollEvent( &myPong->event )) {

        if (myPong->event.type == SDL_KEYDOWN) {
            switch(myPong->event.key.keysym.sym) {
                case SDLK_q: myPong->programExited = TRUE; break;
                case SDLK_s: startNewGame(); break;
                default: break;
            }
        }
    }
}
void menuLogic() {

}
void menuRender() {

    drawBackground();

    /* Set the header */
    /* how to this: WIDTH / 2 - (widthOf(headerText) / 2) */
    applySurface(20, 20, menu->headerSurface, myPong->screen, NULL);

    /* Set the menu text */
    applySurface(WIDTH / 4, HEIGHT / 2, menu->option1Surface, myPong->screen, NULL);
    applySurface(WIDTH / 4, HEIGHT / 2 + 40, menu->option2Surface, myPong->screen, NULL);


    SDL_Delay(1000 / FREQ);
    SDL_Flip(myPong->screen);

}

void gameLoop() {

    /* Put these somewhere else */
    initMenu();

    while (myPong->programExited == FALSE) {
        menuEvents();
        menuLogic();
        menuRender();

        while (myPong->gameRunning == TRUE) {
            gameEvents();
            gameLogic();
            gameRender();
        }
    }
}

void gameEvents() {
    while (SDL_PollEvent( &myPong->event )) {

        handlePlayerInput(player0);

        if (myPong->event.type == SDL_KEYDOWN) {
            switch(myPong->event.key.keysym.sym) {
                case SDLK_q: myPong->gameRunning = FALSE; break;
                default: break;
            }
        }
    }
}

void gameLogic() {
    moveDot(dot);
    trollCPU(player1);

    movePaddle(player1->paddle);
    movePaddle(player0->paddle);
}

void showScore() {
    SDL_Surface *p0Score = NULL;
    SDL_Surface *p1Score = NULL;

    sprintf(player0->scoreStr, "%d", player0->score);
    sprintf(player1->scoreStr, "%d", player1->score);

    p0Score = TTF_RenderText_Solid(scoreFont, player0->scoreStr, player0->color);
    p1Score = TTF_RenderText_Solid(scoreFont, player1->scoreStr, player1->color);

    applySurface(WIDTH / 8, HEIGHT / 2 - 40, p0Score, myPong->screen, NULL);
    applySurface((WIDTH / 8) * 6, HEIGHT / 2 - 40, p1Score, myPong->screen, NULL);

    /* This one fixed the memory leak */
    SDL_FreeSurface(p0Score);
    SDL_FreeSurface(p1Score);
}

void gameRender() {
    newBackground();
    // drawBackground();
    drawDot(dot, myPong->screen);
    drawPaddle(player0, myPong->screen);
    drawPaddle(player1, myPong->screen);

    showScore();

    SDL_Delay(1000 / FREQ);
    SDL_Flip(myPong->screen);
}

SDL_Surface *loadImage(char *filename, Color color) {
    SDL_Surface *loadedImage = NULL;
    SDL_Surface *optimizedImage = NULL;

    loadedImage = IMG_Load(filename);

    if (loadedImage != NULL) {
        optimizedImage = SDL_DisplayFormat( loadedImage );
        SDL_FreeSurface( loadedImage );
        if (optimizedImage != NULL && color != NULL) {
            Uint32 colorKey = SDL_MapRGB( optimizedImage->format, color->red, color->green, color->blue);
            SDL_SetColorKey(optimizedImage,  SDL_SRCCOLORKEY, colorKey);
        }

        return optimizedImage;

    } else {
        return NULL;
    }
}

