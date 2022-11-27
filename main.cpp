#include "SFML/Graphics.hpp"
#include <iostream>
#include <time.h>
using namespace sf;

const int rowLength = 20;
const int coloumnLength = 10;

int frameBlocks[rowLength][coloumnLength] = { 0 };

struct Cubes
{
    int x, y; // x and y components of the arrays a and b
} a[4], b[4]; // two global variables of struct Cubes named a and b of size 4

int tetriminoes[7][4] = // array of predefined tetriminoes shapes
{
    {3, 5, 4, 6}, // S
    {2, 3, 5, 7}, // L
    {1, 3, 5, 7}, // I
    {3, 5, 4, 7}, // T
    {2, 3, 4, 5}, // O
    {3, 5, 7, 6}, // J
    {2, 4, 5, 7}, // Z
}; 

bool check();
void Run(Clock &clock, RenderWindow &window);
void Move();
void Rotate();
void Tick();
void CheckLines();
void Draw(RenderWindow& window, Sprite& s, Sprite& background, Sprite& frame, Text &text);


int movementOnXAxis = 0;
bool rotate = 0;
int colorNumber = 1;
float timer = 0, delayTimeToDescend = 1;
int score = 0;


int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(320, 480), "Tetris");

    Texture textureTile, textureBackground, textureFrame;
    textureTile.loadFromFile("images/tiles.png");
    textureBackground.loadFromFile("images/background.png");
    textureFrame.loadFromFile("images/frame.png");

    Sprite tile(textureTile), background(textureBackground), frame(textureFrame);

    Font font;

    font.loadFromFile("fonts/gomarice_no_continue.ttf");

    Text text;

    // select the font
    text.setFont(font); // font is a sf::Font

    // set the string to display
    text.setString("GAME OVER");

    // set the character size
    text.setCharacterSize(54);

    // set the color
    text.setFillColor(Color::Black);

    // set the text style
    text.setStyle(Text::Regular);

    // set the absolute position of the entity
    text.setPosition(52.5f, 185.f);


    Clock clock;

    while (window.isOpen())
    {
        Run(clock, window);

        Move();

        Rotate();

        Tick();

        CheckLines();      

        Draw(window, tile, background, frame, text);
    }

    return 0;
}


bool check()
{
    for (int i = 0; i < 4; i++)
    {
        // not letting the tile escape the frame area at the side and at the bottom
        if (a[i].x < 0 || a[i].x >= coloumnLength || a[i].y >= rowLength)
        {
            return 0;
        }
        // not letting the tile to overlap or pass through another tile placed at a point
        else if (frameBlocks[a[i].y][a[i].x])
        {
            return 0;
        }
    }

    return 1;
}

void Run(Clock &clock, RenderWindow &window)
{
    float time = clock.getElapsedTime().asSeconds();
    clock.restart();
    timer += time;

    Event ev;
    while (window.pollEvent(ev))
    {
        if (ev.type == Event::Closed)
        {
            window.close();
        }

        if (ev.type == Event::KeyPressed)
        {
            if (ev.key.code == Keyboard::Up)
            {
                rotate = true;

            }
            else if (ev.key.code == Keyboard::Left)
            {
                movementOnXAxis = -1;
            }
            else if (ev.key.code == Keyboard::Right)
            {
                movementOnXAxis = 1;
            }
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Down))
    {
        delayTimeToDescend = 0.05;
    }
}

void Move()
{
    //// <- Move -> ///
    for (int i = 0; i < 4; i++)
    {
        b[i] = a[i];
        a[i].x += movementOnXAxis;
    }
    if (!check())
    {
        for (int i = 0; i < 4; i++)
            a[i] = b[i];
    }
}

void Rotate()
{
    //////Rotate//////
    if (rotate)
    {
        Cubes centerOfRotation = a[1]; // center of rotation
        for (int i = 0; i < 4; i++)
        {
            int x = a[i].y - centerOfRotation.y;
            int y = a[i].x - centerOfRotation.x;
            a[i].x = centerOfRotation.x - x;
            a[i].y = centerOfRotation.y + y;
        }
        if (!check())
        {
            for (int i = 0; i < 4; i++)
            {
                a[i] = b[i];
            }
        }
    }
}

void Tick()
{
    ///////Tick//////
    if (timer > delayTimeToDescend)
    {
        for (int i = 0; i < 4; i++)
        {
            b[i] = a[i];
            a[i].y += 1;

        }

        if (!check())
        {
            for (int i = 0; i < 4; i++)
            {
                frameBlocks[b[i].y][b[i].x] = colorNumber;

            }

            colorNumber = 1 + rand() % 7;
            int n = rand() % 7;

            for (int i = 0; i < 4; i++)
            {
                a[i].x = tetriminoes[n][i] % 2;
                a[i].y = tetriminoes[n][i] / 2;
            }
        }

        timer = 0;
    }
}

void CheckLines()
{
    ///////check lines//////////
    int k = rowLength - 1;

    for (int i = rowLength - 1; i > 0; i--)
    {
        int count = 0;

        for (int j = 0; j < coloumnLength; j++)
        {
            if (frameBlocks[i][j])
            {
                count++;
            }

            frameBlocks[k][j] = frameBlocks[i][j];
        }
        if (count < coloumnLength)
        {
            k--;
        }
    }
            
    movementOnXAxis = 0;
    rotate = 0;
    delayTimeToDescend = 0.3;
}

void Draw(RenderWindow &window, Sprite &tile, Sprite &background, Sprite &frame, Text &text)
{
    /////////draw//////////
    window.clear(Color::White);
    window.draw(background);

    // displaying the blocks at bottom
    for (int i = 0; i < rowLength; i++)
    {
        for (int j = 0; j < coloumnLength; j++)
        {
            if (frameBlocks[i][j] == 0)
            {
                continue;
            }
            
            tile.setTextureRect(IntRect(frameBlocks[i][j] * 18, 0, 18, 18));
            tile.setPosition(j * 18, i * 18);
            tile.move(28, 31); // offset
            window.draw(tile);


            // Game Over Functionality
            if (tile.getPosition().y <= 100)
            {                
                window.draw(text);
            }
        }
       
    }

    // spawning new blocks
    for (int i = 0; i < 4; i++)
    {
        tile.setTextureRect(IntRect(colorNumber * 18, 0, 18, 18));
        tile.setPosition(a[i].x * 18, a[i].y * 18);
        tile.move(28, 31); // offset
        window.draw(tile);
    }

    window.draw(frame);
    window.display();
}