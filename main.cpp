#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#define str(x) dynamic_cast<std::ostringstream &>((std::ostringstream() << std::dec << x)).str()

using namespace sf;

const int   SIMWIDTH  = 220;
const int   SIMHEIGHT = 220;
const float SIMSCALE  = 3;
const std::string SIMNAME = "Game of life!";

bool runSimulation = true;

struct Cell {
    bool alive = false;
    bool used  = false;
    int  age   = 0;

    void activate() {
        alive = true;
        used  = true;
        age   = 0;
    }

    void kill() {
        alive = false;
    }

    void update() {
        if(alive) age++;
    }
};

int main()
{
    Cell    mainGrid[SIMWIDTH][SIMHEIGHT];
    Texture gridTexture;
    Sprite  gridSprite;
    Image   gridImage;
    int     numberAlive = 0;

    RenderWindow window(VideoMode(SIMWIDTH*SIMSCALE, SIMHEIGHT*SIMSCALE), SIMNAME, Style::Close);

    while (window.isOpen())
    {
        Event event;
        while(window.pollEvent(event)) {
            if(event.type == Event::Closed) window.close();
            // Freeze the simulation while we are adding/removing cells.
            if(event.type == Event::MouseButtonPressed)  runSimulation = false;
            if(event.type == Event::MouseButtonReleased) runSimulation = true;
        }

        // Find which cell we are hovering over (if we are at all)
        Vector2i mouseLocation = Vector2i(Mouse::getPosition(window).x/SIMSCALE, Mouse::getPosition(window).y/SIMSCALE);
        if(mouseLocation.x < SIMWIDTH && mouseLocation.y < SIMHEIGHT
           && mouseLocation.x >= 0 && mouseLocation.y >= 0
        ) {
            if(Mouse::isButtonPressed(Mouse::Left)) {
                numberAlive += (!mainGrid[mouseLocation.x][mouseLocation.y].alive ? 1 : 0);
                mainGrid[mouseLocation.x][mouseLocation.y].activate();
            } else if(Mouse::isButtonPressed(Mouse::Right)) {
                numberAlive -= (mainGrid[mouseLocation.x][mouseLocation.y].alive ? 1 : 0);
                mainGrid[mouseLocation.x][mouseLocation.y].kill();
            }
        }

        // Process
        if(runSimulation) {
            Cell updatedGrid[SIMWIDTH][SIMHEIGHT];
            numberAlive = 0;

            for(int x = 0; x < SIMWIDTH; x++) {
            for(int y = 0; y < SIMHEIGHT; y++) {
                // Count neighbours
                int neighbours = 0;
                for(int j = -1; j <= 1; j++) {
                for(int k = -1; k <= 1; k++) {
                    // Check for it not being itself and being inside the simulation
                    if((j != 0 || k != 0)
                       && x+j < SIMWIDTH && y+k < SIMHEIGHT
                       && x+j >= 0 && y+k >= 0
                       && mainGrid[x+j][y+k].alive
                    )
                        neighbours++;
                }}

                // Increment numberAlive if current cell is alive
                numberAlive += (mainGrid[x][y].alive ? 1 : 0);

                // Copy cell to the new grid
                updatedGrid[x][y] = mainGrid[x][y];

                if(neighbours < 2 || neighbours > 3) { // Over/Undercrowded? Kill!
                    updatedGrid[x][y].kill();
                } else if(neighbours == 3) { // === 3, bring them alive
                    updatedGrid[x][y].activate();
                } else { // Update their age.
                    updatedGrid[x][y].update();
                }
            }}

            // Copy back to main grid
            for(int x = 0; x < SIMWIDTH; x++) {
            for(int y = 0; y < SIMHEIGHT; y++) {
                mainGrid[x][y] = updatedGrid[x][y];
            }}
        }

        // Draw
        gridImage.create(SIMWIDTH, SIMHEIGHT, Color::White);

        for(int x = 0; x < SIMWIDTH; x++) {
        for(int y = 0; y < SIMHEIGHT; y++) {
            if(mainGrid[x][y].used)
                gridImage.setPixel(x, y, Color(220, 252, 252));

            if(mainGrid[x][y].alive)
                gridImage.setPixel(x, y, Color(245, 132, 61));
        }}

        gridTexture.loadFromImage(gridImage);
        gridSprite.setTexture(gridTexture);
        gridSprite.setScale(SIMSCALE, SIMSCALE);

        // Cell count
        window.setTitle(SIMNAME + " - " + str(numberAlive) + " cells.");

        window.clear();
        window.draw(gridSprite);
        window.display();
    }

    return 0;
}
