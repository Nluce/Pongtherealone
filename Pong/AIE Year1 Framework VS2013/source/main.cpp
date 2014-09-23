#include "AIE.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;

const int iScreenWidth = 800;// screen height and size
const int iScreenHeight = 600;
const char* pInvadersFont = "./fonts/invaders.fnt";//font

char pkScore1[10] = "00000";
char pkScore2[10] = "00000";

char pkMainmenu[] = "MAIN MENU";
char pkPong[] = "PONG";
char pkNewgame[] = "N - NEW GAME";
char pkQuit[] = "Q - QUIT";
char pkHighscoremenu[] = "H - HIGH SCORES";

char pkInsertCoins[] = "press enter to start!";
char pkCredits[] = "Credits";
char pkHiScore[] = "Hi Score";
char pkCredit[] = "Credit";

char pkBallAndPaddleSprite[] = "./images/ball_and_paddle.png";
char pkHighScoresTextFile[] = "PongHighScores.txt";
char pkGameOver[] = "GAME OVER";
char pkPlayerOneWins[] = "PLAYER  1  WINS";
char pkPlayerTwoWins[] = "PLAYER  2  WINS";
char pkReturnToMainMenu[] = "\"ESC\" TO GO TO THE MAIN MENU";
char pkHighscores[5][8];
void saveScore();
void loadScore();

enum GameStates
{
	eMAIN_MENU,
	eGAMEPLAY,
	eGAMEOVER,
	eHIGHSCORE
};


// keyboard codes
typedef enum{
	KK_ESC = 256,
	KK_ENTER = 257,
	KK_LEFT = 263,
	KK_RIGHT = 262,
	KK_UP = 265,
	KK_DOWN = 264,
};



class Player 
{
public:
	int score;
	float positionX;
	float positionY;
	int spriteID;
	int player;
	int upKey;
	int downKey;

	void init(int player);
	void move(float a_fDeltaT);
	void draw();
	bool hit();
	int hitAngle();
};

const int BALL_SIZE = 16;
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIGHT = 64;
const float PADDLE_SPEED = 600; // pixels per second
const float BALL_START_SPEED = 350; // pixels per second
// Other examples of constant styles
//const int kBallSize = 16;
//#define BALL_SIZE 16

class Ball
{
public:
	float positionX;
	float positionY;
	float velocityX;
	float velocityY;
	int spriteID;
	//int ballAge;


	void init()
	{
		spriteID = CreateSprite(pkBallAndPaddleSprite, BALL_SIZE, BALL_SIZE, true);
	}

	// draw the ball
	void draw();

	// Respawn the ball at the center of the screen
	void spawn();

	// moves the ball
	void move(float a_fDeltaT);

};


class GameState
{

public:
	GameState()
	{
		playerOneServes = true;
		delay = 0;
		currentState = eMAIN_MENU;
	}

	void score(int player);

	void init();
	void move(float a_fDeltaT);
	void draw();
	void newGame();

	bool playerOneServes;
	Ball ball;
	Player player1;
	Player player2;
	int delay;
	GameStates currentState;

};

void GameState_draw(GameState * gs);


GameState gameState;


// Respawn the ball at the center of the screen
void Ball::spawn()
{
	// Move the ball to the center of the screen
	positionX = iScreenWidth / 2;
	positionY = iScreenHeight / 2;
	// Player who lost last have the ball spawn in their direction
	velocityX = gameState.playerOneServes ? BALL_START_SPEED : -BALL_START_SPEED;
	// Pick a random vertical velocity
	velocityY = (rand() % 7 - 3) * 60;

	//ballAge = 0;
}


// moves the ball
void Ball::move(float a_fDeltaT)
{
	if (gameState.currentState == eGAMEOVER)
	{
		return;
	}
	if (gameState.delay > 0) 
	{
		gameState.delay--;
		if (gameState.delay <= 0)
		{
			spawn();
		}
		return;
	}

//	ballAge++;
//	if (ballAge > 18){
//		ballAge = 0;
//		if (velocityX > 0){
//			velocityX+= 0.1f;
//		}
//		else {
//			velocityX-= 0.1f;
//		}
//	}

	// Add the velocity to the position
	positionX += velocityX * a_fDeltaT;
	positionY += velocityY * a_fDeltaT;

	// ball needs to bounce off the top and bottom of the screen
	if (positionY < BALL_SIZE / 2){
		positionY = BALL_SIZE / 2;
		velocityY = abs(velocityY);
	}
	else if (positionY > iScreenHeight - BALL_SIZE / 2) {
		positionY = iScreenHeight - BALL_SIZE / 2;
		velocityY = -abs(velocityY);
	}

	//ball needs to bounce off of player paddles

	if (gameState.player1.hit())
	{
		velocityX = abs(velocityX);
		velocityY = gameState.player1.hitAngle() * 60;
	}
	
	if (gameState.player2.hit())
	{
		velocityX = -abs(velocityX);
		velocityY = gameState.player2.hitAngle() * 60;
	}


	//ball needs to score if it goes off the screen
	if (positionX < BALL_SIZE / 2){
		gameState.score(2);
	}

	if (positionX > iScreenWidth - BALL_SIZE / 2){
		gameState.score(1);
	}

}

// draw the ball
void Ball::draw()
{
	if (gameState.delay > 0) {
		return;
	}
	MoveSprite(spriteID, positionX, positionY);
	DrawSprite(spriteID);

}
void addScore(int p1score, int p2score)
{
	char newScore[8];
	sprintf_s(newScore, 8, "%d-%d", p1score, p2score);
	strcpy_s(pkHighscores[4], 8, pkHighscores[3]);
	strcpy_s(pkHighscores[3], 8, pkHighscores[2]);
	strcpy_s(pkHighscores[2], 8, pkHighscores[1]);
	strcpy_s(pkHighscores[1], 8, pkHighscores[0]);
	strcpy_s(pkHighscores[0], 8, newScore);
	saveScore();
}

void saveScore()
{
	fstream file;

	file.open(pkHighScoresTextFile, ios_base::out);
	if (file.is_open())
	{
		for (int i = 0; i < 5; i++)
		{
			file << pkHighscores[i] << endl;
		}
		file.close();
	}
}

void loadScore()
{
	fstream file;

	file.open(pkHighScoresTextFile, ios_base::in);
	if (file.is_open())
	{
		for (int i = 0; i < 5; i++)
		{
			file >> pkHighscores[i];
		}
		file.close();
	}
}


void GameState::score(int player)
{
	switch (player){
	case 1:
			player1.score++;
			if (player1.score >= 11 )
			{
				currentState = eGAMEOVER;
				addScore(player1.score, player2.score);
			}
			playerOneServes = false;
			break;
		case 2:
			player2.score++;
			if (player2.score >= 11)
			{
				currentState = eGAMEOVER;
				addScore(player1.score, player2.score);
			}
			playerOneServes = true;
			break;
	}

	// no ball for 1.2 seconds frames
	delay = 1200;

}


void GameState::init()
{
	ball.init();
	player1.init(1);
	player2.init(2);
}
void GameState::newGame()
{
	currentState = eGAMEPLAY;
	ball.spawn();
	player1.score = 0;
	player2.score = 0;
}
void GameState::move(float a_fDeltaT)
{
	ball.move(a_fDeltaT);
	player1.move(a_fDeltaT);
	player2.move(a_fDeltaT);
}



void GameState::draw()
{
	ball.draw();
	player1.draw();
	player2.draw();
	if (currentState == eGAMEOVER)
	{
		SetFont(pInvadersFont);
		// Draw Game over word on the screen
		DrawString(pkGameOver, iScreenWidth * 0.4f, iScreenHeight * 0.95f);
		if (player1.score > player2.score)
		{
			DrawString(pkPlayerOneWins, iScreenWidth * 0.4f, iScreenHeight * 0.8f);
		}
		else
		{
			DrawString(pkPlayerTwoWins, iScreenWidth * 0.4f, iScreenHeight * 0.8f);
		}
		DrawString(pkReturnToMainMenu, iScreenWidth * 0.2f, iScreenHeight * 0.3f);

	}
}

void Player::init(int playerIn)
{
	player = playerIn;

	spriteID = CreateSprite(pkBallAndPaddleSprite, PADDLE_WIDTH, PADDLE_HEIGHT, true);
	switch (player){
	case 1:
		upKey = 'W';
		downKey = 'S';
		positionX = iScreenWidth * 0.1f;
		break;
	case 2:
		upKey = KK_UP;
		downKey = KK_DOWN;
		positionX = iScreenWidth * 0.9f;
		break;
	}

	positionY = iScreenHeight / 2;
}

void Player::move(float a_fDeltaT)
{
	if (IsKeyDown(upKey)){
		positionY += PADDLE_SPEED * a_fDeltaT;
	}

	if (IsKeyDown(downKey)){
		positionY -= PADDLE_SPEED * a_fDeltaT;
	}

	if (positionY < 0)
	{
		positionY = 0;
	}

	if (positionY > iScreenHeight)
	{
		positionY = iScreenHeight;
	}


}

void Player::draw()
{
	MoveSprite(spriteID, positionX, positionY);
	DrawSprite(spriteID);
}



bool Player::hit()
{
	int deltaX = abs(gameState.ball.positionX - positionX);

	if (deltaX < PADDLE_WIDTH){
		int deltaY = abs(gameState.ball.positionY - positionY);

		if (deltaY < PADDLE_HEIGHT / 2){
			return true;
		}
	}

	return false;
}

int Player::hitAngle()
{
	int deltaY = gameState.ball.positionY - positionY;

	return deltaY / 4;
}



void UpdateMainMenu();
void UpdateHighScore();
void UpdateGameState(float a_fDeltaT);
//Player global variables


int main(int argc, char* argv[])
{
	//\ Lets initialise the AIE Framework and give the window it creates an appropriate title
	Initialise(iScreenWidth, iScreenHeight, false, "Space Invaders");
	SetBackgroundColour(SColour(0x00, 0x00, 0x00, 0xFF));
	AddFont(pInvadersFont);
	srand((unsigned)time(NULL));

	gameState.init();

	loadScore();

	bool quit = false;
	do
	{
		// fDeltaTime is the elapsed time since the last frame.
		float fDeltaT = GetDeltaTime();
		ClearScreen();
		switch (gameState.currentState)
		{
		case eMAIN_MENU:
			UpdateMainMenu();
			if (IsKeyDown('N'))
			{
				gameState.newGame();
			}
			else if (IsKeyDown('Q'))
			{
				quit = true;
			}
			else if (IsKeyDown('H'))
			{
				gameState.currentState = eHIGHSCORE;
			}
			break;
		case eHIGHSCORE:
			UpdateHighScore();
			if (IsKeyDown(KK_ESC))
			{
				gameState.currentState = eMAIN_MENU;
			}

			break;
		case eGAMEPLAY:
		case eGAMEOVER:
			UpdateGameState(fDeltaT);
			if (IsKeyDown(KK_ESC))
			{
				gameState.currentState = eMAIN_MENU;
			}
			break;
		default:
			break;
		}
	} while (FrameworkUpdate() == false && quit != true);

	//DestroySprite(player.iSpriteID);
	//DestroySprite(iArcadeMarquee);
	Shutdown();

	return 0;
}

void UpdateMainMenu()
{
	SetFont(pInvadersFont);

	DrawString(pkPong, iScreenWidth * 0.28f, iScreenHeight * 0.95f);
	DrawString(pkMainmenu, iScreenWidth * 0.28f, iScreenHeight * 0.75f);
	DrawString(pkNewgame, iScreenWidth * 0.28f, iScreenHeight * 0.4f);
	DrawString(pkQuit, iScreenWidth * 0.28f, iScreenHeight * 0.3f);
	DrawString(pkHighscoremenu, iScreenWidth * 0.28f, iScreenHeight * 0.5f);
}

void UpdateHighScore()
{
	SetFont(pInvadersFont);

	DrawString(pkGameOver, iScreenWidth * 0.28f, iScreenHeight * 0.95f);

	for (int i = 0; i < 5; i++)
	{
		DrawString(pkHighscores[i], iScreenWidth * 0.28f, iScreenHeight * (0.85f - i * 0.1f));
	}

	DrawString(pkReturnToMainMenu, iScreenWidth * 0.28f, iScreenHeight * 0.3f);
}

clock_t lastMove = 0;

void UpdateGameState(float a_fDeltaT)
{
	char score[10];

	SetFont(pInvadersFont);

	sprintf_s(score, sizeof score, "%d", gameState.player1.score);
	DrawString(score, iScreenWidth * 0.25f, iScreenHeight * 0.95f);

	sprintf_s(score, sizeof score, "%d", gameState.player2.score);
	DrawString(score, iScreenWidth * 0.75f, iScreenHeight * 0.95f);

	

	gameState.move(a_fDeltaT);
	gameState.draw();
}


/*	SetFont(pInvadersFont);
	DrawString(pkScore1, iScreenWidth * 0.025f, iScreenHeight - 2);
	DrawString(pkHiScore, iScreenWidth * 0.425f, iScreenHeight - 2);
	DrawString(pkScore2, iScreenWidth * 0.8f, iScreenHeight - 2);
	DrawString(pkCredit, iScreenWidth * 0.7f, 38);
	*/


	//for (int i = 256; i < 512; i++){
	//	if (IsKeyDown(i)){
	//		char buffer[20];
	//		sprintf(buffer, "%d", i);
	//		DrawString(buffer, iScreenWidth * 0.7f, 99);
	//	}
	//}
