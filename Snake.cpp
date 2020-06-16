#include <iostream>
#include <windows.h>
#include <list>
#include <thread>
using namespace std;
int nScreenWidth = 120;
int nScreenHeight = 30;

struct SnakeSegment
{
	int x;
	int y;
};


int main()
{
	std::chrono::milliseconds xspeed = 80ms;
	std::chrono::milliseconds yspeed = 100ms;
	//Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) { screen[i] = L' '; }
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwByteWritten = 0;
	while (true)
	{
		std::list<SnakeSegment> snake = { {60, 15}, {61,15}, {62,15}, {63,15}, {64,15}, {65, 15}, {66,15}, {67, 15}, {68, 15}, {69, 15} };
		int nFoodx = 20;
		int nFoody = 10;
		int nScore = 0;
		int nSnakeDirection = 3;
		bool bDead = false;
		bool bLeftKey = false, bRightKey = false, bLeftKeyold = false, bRightKeyold = false, bUpKey = false, bUpKeyOld = false, bDownKey = false, bDownKeyOld = false;
		std::chrono::milliseconds xspeed = 80ms;
		std::chrono::milliseconds yspeed = 100ms;


		while (!bDead)
		{
			//Input & Timing
			auto t1 = chrono::system_clock::now();
				while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? xspeed : yspeed))
				{
					//Get Input
					bLeftKey = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
					bRightKey = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
					if (bLeftKey && !bLeftKeyold)
					{
						nSnakeDirection--;
						if (nSnakeDirection == -1) { nSnakeDirection = 3; }
					}

					if (bRightKey && !bRightKeyold)
					{
						nSnakeDirection++;
						if (nSnakeDirection == 4) { nSnakeDirection = 0; }
					}
					bRightKeyold = bRightKey;
					bLeftKeyold = bLeftKey;
				}

				//Logic
				//Update Snake Position
				switch (nSnakeDirection)
				{
				case 0:snake.push_front({ snake.front().x, snake.front().y - 1 }); break;//up
				case 1:snake.push_front({ snake.front().x + 1, snake.front().y }); break;//Right
				case 2:snake.push_front({ snake.front().x, snake.front().y + 1 }); break;//Down
				case 3:snake.push_front({ snake.front().x - 1, snake.front().y }); break;//Left
				}


				//Chop snake Tail
				snake.pop_back();

				//Collision Detection
				//Collision of Snake and Wall
				if (snake.front().x < 0 || snake.front().x >= nScreenWidth) { bDead = true; }
				if (snake.front().y < 3 || snake.front().y >= nScreenHeight) { bDead = true; }

				//Collision of Snake and Fruit
				if (snake.front().x == nFoodx && snake.front().y == nFoody)
				{
					nScore++;
					xspeed -= 3ms; yspeed -= 3ms; //Speed Up Snake as it eats up fruits
					while (screen[nFoody * nScreenWidth + nFoodx] != L' ')
					{
						nFoodx = rand() % nScreenWidth;
						nFoody = (rand() % (nScreenHeight - 3)) + 3;
					}
					for (int i = 0; i < 5; i++)
					{
						snake.push_back({ snake.back().x, snake.back().y });
					}
				}

				//Collision of Snake on Snake
				for (list<SnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
				{
					if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y) { bDead = true; }
				}




				//Output
				//Clear Screen
				for (int i = 0; i < nScreenWidth * nScreenHeight; i++) { screen[i] = L' '; }

				//Draw Boarder & Score
				for (int i = 0; i < nScreenWidth; i++)
				{
					screen[i] = L'═';
					screen[2 * nScreenWidth + i] = L'═';
				}
				wsprintf(&screen[nScreenWidth + 5], L"WELCOME TO THE SNAKE GAME...    CONTROLS : RIGHT: →    LEFT: ←     SCORE: %d", nScore);

				//Draw Snake Body
				for (auto s : snake) { screen[s.y * nScreenWidth + s.x] = bDead ? L'+' : L'■'; }

				//Draw Snake Head
				screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'X' : L'■';

				//Draw Food
				screen[nFoody * nScreenWidth + nFoodx] = L'ô';

				//Replay Message
				if (bDead) 
				{ 
					std::chrono::milliseconds xspeed = 80ms; // Speed Restarter
					std::chrono::milliseconds yspeed = 100ms;// Speed Restarter
					wsprintf(&screen[15 * nScreenWidth + 40], L"       PRESS 'SPACE' TO RESTART"); 
				}

				//Display (console output)
				WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwByteWritten);
		}
		//wait for Space
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);

	}

	return 0;
}
