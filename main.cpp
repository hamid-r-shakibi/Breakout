#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <ctime>

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);



void setcursor(bool visible, DWORD size) // set bool visible = 0 - invisible, bool visible = 1 - visible
{
	if(size == 0)
	{
		size = 20;	// default cursor size Changing to numbers from 1 to 20, decreases cursor width
	}
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = size;
	SetConsoleCursorInfo(console,&lpCursor);
}


enum eDir { STOP = 0, LEFT = 1, UPLEFT = 2, DOWNLEFT = 3, RIGHT = 4, UPRIGHT = 5, DOWNRIGHT = 6};
//xy function start
void gotoxy(int x,int y)
{
    COORD coord1;
    coord1.X=x;
    coord1.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord1);
}
//xy function end

class timer
{
	clock_t startedAt;
	clock_t pausedAt;
	bool started;
	bool paused;
public:
	timer();
	void Pause();
	void Resume();
	void Stop();
	void Start();
	void Reset();
	clock_t GetTicks();
};

timer::timer()
{
	startedAt = 0;
	pausedAt = 0;
	paused = false;
	started = false;
}
void timer::Pause()
{
	if( paused || !started )
		return;
	paused = true;
	pausedAt = clock();
}
void timer::Resume()
{
	if( !paused )
		return;
	paused = false;
	startedAt += clock() - pausedAt;
}
void timer::Stop()
{
	started = false;
}
void timer::Start()
{
	if( started )
		return;
	started = true;
	paused = false;
	startedAt = clock();
}
void timer::Reset()
{
	paused = false;
	startedAt = clock();
}
clock_t timer::GetTicks()
{
	if( !started )
		return 0;
	if( paused )
		return pausedAt - startedAt;
	return clock() - startedAt;
}


class Ball
{
private:
    int x, y;
    int originalX, originalY;
    eDir direction;
public:
    Ball(int posX, int posY)
    {
        originalX = posX;
        originalY = posY;
        x = posX; y = posY;
        direction=STOP;
    }
    void Reset()
    {
        x = originalX; y = originalY;
        direction = STOP;
    }
    void changeDirection(eDir d)
    {
        direction = d;
    }
    void randomDirection()
    {
        direction = (eDir)((rand() % 6) + 1);
    }
    inline int getX() { return x; }
    inline int getY() { return y; }
    inline eDir getDirection() { return direction; }
    void Move()
    {
        switch (direction)
        {
        case STOP:
            break;
        case LEFT:
            {gotoxy(x,y);
            cout << "\b ";
            x--;
            gotoxy(x,y);
            cout << "\bO";
            break;}
        case RIGHT:
            {gotoxy(x,y);
            cout << "\b ";
            x++;
            gotoxy(x,y);
            cout << "\bO";
            break;}
        case UPLEFT:
            {gotoxy(x,y);
            cout << "\b ";
            x--; y--;
            gotoxy(x,y);
            cout << "\bO";
            break;}
        case DOWNLEFT:
            {gotoxy(x,y);
            cout << "\b ";
            x--; y++;
            gotoxy(x,y);
            cout << "\bO";
            break;}
        case UPRIGHT:
            {gotoxy(x,y);
            cout << "\b ";
            x++; y--;
            gotoxy(x,y);
            cout << "\bO";
            break;}
        case DOWNRIGHT:
            {gotoxy(x,y);
            cout << "\b ";
            x++;
            y++;
            gotoxy(x,y);
            cout << "\bO";
            break;}
        default:
            break;
        }
    }
    friend ostream & operator<<(ostream & o, Ball c)
    {
        o << "Ball [" << c.x << "," << c.y << "][" << c.direction << "]";
        return o;
    }
};

class Paddle
{
private:
    int x, y;
    int originalX, originalY;
public:
    Paddle()
    {
        x = y = 0;
    }
    Paddle(int posX, int posY) : Paddle()
    {
        originalX = posX;
        originalY = posY;
        x = posX;
        y = posY;
    }
    inline void Reset() { x = originalX; y = originalY; }
    inline int getX() { return x; }
    inline int getY() { return y; }
    inline void moveLeft()
    {
        gotoxy(x+4,y);
        cout << "\b\b\b\b    ";
        x--;
        gotoxy(x+4,y);
        cout << "\b\b\b\b";
        for(int i=0;i<4;i++)
            cout << "\xDB";
    }
    inline void moveRight()
    {
        gotoxy(x+4,y);
        cout << "\b\b\b\b    ";
        x++;
        gotoxy(x+4,y);
        cout << "\b\b\b\b";
        for(int i=0;i<4;i++)
            cout << "\xDB";
    }
    friend ostream & operator<<(ostream & o, Paddle c)
    {
        o << "Paddle [" << c.x << "," << c.y << "]";
        return o;
    }
};

class objects
{
private:
    int x , y;
    bool availbale;
public:
    objects()
    {
        x=(rand()%20)+10;
        y=(rand()%2)+5;
        availbale=true;
    }
    int getx(){return x;}
    int gety(){return y;}
    bool getstatus(){return availbale;}
    void changestatus(bool state)
    {
        availbale=state;
    }
};

class GameManger
{
private:
    timer timeloop1;
    int width, height;
    int score1;
    char left, rigth;
    bool quit;
    Ball *ball;
    Paddle *player1;
    objects *blocks;
public:
    GameManger(int w, int h)
    {
        srand(time(NULL));
        quit = false;
        left = 'a';
        rigth = 'd';
        score1 = 0;
        width = w; height = h;
        ball = new Ball(w / 2 - 2,18);
        player1 = new Paddle(w / 2 - 3,19);
        blocks = new objects[20];
    }
    ~GameManger()
    {
        delete ball;
        delete player1;
    }
    void ScoreUp(Paddle * player)
    {
        gotoxy(player1->getX()+4,player1->getY());
        cout << "\b\b\b\b    ";
        gotoxy(ball->getX(),ball->getY());
        cout << "\b ";
        score1--;
        ball->Reset();
        player1->Reset();
        gotoxy(ball->getX()+1,ball->getY());
        cout << "\bO";
        gotoxy(player1->getX()+4,player1->getY());
        cout << "\b\b\b\b";
        for(int i=0;i<4;i++)
            cout << "\xDB";
    }
    //drawing function starts
    void Draw()
    {
        for(int i=0;i<20;i++)
        {
            if(blocks[i].getstatus())
                {
                    gotoxy(blocks[i].getx(),blocks[i].gety());
                    cout << "\xB0";
                }
        }
    }
    //drawing function ends
    void Input()
    {
        int player1x = player1->getX();
        if (_kbhit())
        {

            if(ball->getDirection()==STOP)
            {
                gotoxy(ball->getX()+1,ball->getY());
                cout << "\b\b\b   ";
                ball->randomDirection();
            }
            char current = _getch();
            if (current == left)
                if (player1x > 1)
                    player1->moveLeft();
            if (current == rigth)
                if (player1x + 5 < width)
                    player1->moveRight();

            if (ball->getDirection() == STOP)
                ball->randomDirection();

            if (current == 'q')
                quit = true;
        }
    }
    void Logic()
    {
        int ballx = ball->getX();
        int bally = ball->getY();
        int player1x = player1->getX();
        int player1y = player1->getY();
        //paddle
        for (int i = 1; i < 5; i++)
            if (ballx == player1x + i)
                if (bally == player1y - 1)
                    if(ball->getDirection() == DOWNRIGHT)
                        ball->changeDirection(UPRIGHT);
                    else
                        ball->changeDirection(UPLEFT);
        //top wall
        if (bally == 1)
            if(ball->getDirection() == UPRIGHT)
                {
                    ball->changeDirection(DOWNRIGHT);
                }
            else
                ball->changeDirection(DOWNLEFT);
        //right wall
        if (ballx == width - 1)
            if(ball->getDirection() == DOWNRIGHT)
                ball->changeDirection(DOWNLEFT);
            else
                ball->changeDirection(UPLEFT);
        //left wall
        if (ballx == 2)
            if(ball->getDirection() == DOWNLEFT)
                ball->changeDirection(DOWNRIGHT);
            else
                ball->changeDirection(UPRIGHT);
            //bottom wall
            if(bally==height-1)
            {
                ScoreUp(player1);
            }
            b2bsmash();
    }

    void b2bsmash()
    {
        int ballx=ball->getX();
        int bally=ball->getY();
        eDir balldir=ball->getDirection();

        for(int i=0;i<20;i++)
        {
            if(blocks[i].getstatus())
                {
                    //right smash
                    if(ballx==blocks[i].getx()+1 && ballx==blocks[i].gety())
                    {
                        if(balldir==DOWNLEFT)
                        {
                            ball->changeDirection(DOWNRIGHT);
                            blocks[i].changestatus(0);
                            gotoxy(blocks[i].getx(),blocks[i].gety());
                            cout << "\b ";
                        }
                        else if(balldir==UPLEFT)
                            {
                                ball->changeDirection(UPRIGHT);
                                blocks[i].changestatus(0);
                                gotoxy(blocks[i].getx(),blocks[i].gety());
                                cout << "\b ";
                            }
                        else if(balldir==LEFT)
                            {
                                ball->changeDirection((eDir)((rand()%2)+5));
                                blocks[i].changestatus(0);
                                gotoxy(blocks[i].getx(),blocks[i].gety());
                                cout << "\b ";
                            }
                    }
                //left smash
                else if(ballx==blocks[i].getx()-1 && ballx==blocks[i].gety())
                {
                    if(balldir==DOWNRIGHT)
                    {
                        ball->changeDirection(DOWNLEFT);
                        blocks[i].changestatus(0);
                        gotoxy(blocks[i].getx(),blocks[i].gety());
                        cout << "\b ";
                    }
                    else if(balldir==UPRIGHT)
                        {
                            ball->changeDirection(UPLEFT);
                            blocks[i].changestatus(0);
                            gotoxy(blocks[i].getx(),blocks[i].gety());
                            cout << "\b ";
                        }
                    else if(balldir==RIGHT)
                        {
                            ball->changeDirection((eDir)((rand()%2)+2));
                            blocks[i].changestatus(0);
                            gotoxy(blocks[i].getx(),blocks[i].gety());
                            cout << "\b ";
                        }
                }
                //up smash
                else if(ballx==blocks[i].getx() && ballx==blocks[i].gety()-1)
                {
                    if(balldir==DOWNRIGHT)
                    {
                        ball->changeDirection(UPRIGHT);
                        blocks[i].changestatus(0);
                        gotoxy(blocks[i].getx(),blocks[i].gety());
                        cout << "\b ";
                    }
                    else if(balldir==DOWNLEFT)
                        {
                            ball->changeDirection(UPLEFT);
                            blocks[i].changestatus(0);
                            gotoxy(blocks[i].getx(),blocks[i].gety());
                            cout << "\b ";
                        }
                }
                //down smash
                else if(ballx==blocks[i].getx() && ballx==blocks[i].gety()+1)
                {
                    if(balldir==UPRIGHT)
                    {
                        ball->changeDirection(DOWNRIGHT);
                        blocks[i].changestatus(0);
                        gotoxy(blocks[i].getx(),blocks[i].gety());
                        cout << "\b ";
                    }
                    else if(balldir==UPLEFT)
                        {
                            ball->changeDirection(DOWNLEFT);
                            blocks[i].changestatus(0);
                            gotoxy(blocks[i].getx(),blocks[i].gety());
                            cout << "\b ";
                        }
                }
                //corners
               if(ballx==blocks[i].getx()+1 && ballx==blocks[i].gety()+1)
                {
                    if(balldir==UPLEFT)
                    {
                        ball->changeDirection(DOWNRIGHT);
                        blocks[i].changestatus(0);
                        gotoxy(blocks[i].getx(),blocks[i].gety());
                        cout << "\b ";
                    }
                }
                else if(ballx==blocks[i].getx()+1 && ballx==blocks[i].gety()-1)
                {
                    if(balldir==DOWNLEFT)
                    {
                        ball->changeDirection(UPRIGHT);
                        blocks[i].changestatus(0);
                        gotoxy(blocks[i].getx(),blocks[i].gety());
                        cout << "\b ";
                    }
                }
                else if(ballx==blocks[i].getx()-1 && ballx==blocks[i].gety()+1)
                {
                    if(balldir==UPRIGHT)
                    {
                        ball->changeDirection(DOWNLEFT);
                        blocks[i].changestatus(0);
                        gotoxy(blocks[i].getx(),blocks[i].gety());
                        cout << "\b ";
                    }
                }
                else if(ballx==blocks[i].getx()-1 && ballx==blocks[i].gety()-1)
                {
                    if(balldir==DOWNRIGHT)
                    {
                        ball->changeDirection(UPLEFT);
                        blocks[i].changestatus(0);
                        gotoxy(blocks[i].getx(),blocks[i].gety());
                        cout << "\b ";
                    }
                }
            }
        }
    }
    void Run()
    {
        //start
        for (int i = 0; i < width ; i++)
            cout << "\xB2";
        cout << endl;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                int player1x = player1->getX();
                int player1y = player1->getY();

                if (j == 0)
                {
                    gotoxy(j,i);
                    cout << "\xB2";
                }
                if (i == 0)
                {
                    gotoxy(j,i);
                    cout << "\xB2";
                }
               if (ball->getX() == j && ball->getY() == i)
                    {
                        gotoxy(j,i);
                        cout << "O";
                    } //ball
                else if (player1x == j && player1y == i)
                {
                    gotoxy(j,i);
                    cout << "\xDB";//player1
                }
                else if (player1x +1 == j && player1y == i)
                {
                    gotoxy(j,i);
                    cout << "\xDB"; //player1
                }
                else if (player1x + 2 == j && player1y == i)
                {
                        gotoxy(j,i);
                    cout << "\xDB"; //player1
                }
                else if (player1x + 3 == j && player1y  == i)
                {
                    gotoxy(j,i);
                    cout << "\xDB"; //player1
                }
                else
                    cout << " ";

                if (j == width - 1)
                {
                    gotoxy(j,i);
                    cout << "\xB2";
                }

            }
            cout << endl;
        }
        gotoxy(0,height);
        for (int i = 0; i < width ; i++)
            cout << "\xB2";
        cout << endl;
        //end
        setcursor(0,0);
        Draw();
        while (!quit)
        {
            if(ball->getDirection()==STOP)
            {
                Input();
                continue;
            }
            ball->Move();
            Logic();
            timeloop1.Reset();
            timeloop1.Start();
            while(timeloop1.GetTicks()<150)
            {
                Input();

            }
        }
    }
};


int main()
{
    GameManger c(40, 20);
    c.Run();
    return 0;
}
