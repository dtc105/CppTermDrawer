#include <array>
#include <vector>
#include <windows.h>
#include <math.h>
#include <iostream>
#include "Plotter.hpp"

// @brief Will tries its best to draw all points given in the terminal
// @param distance the distance the camera is from (0,0,0)
// @param zoom the factor all points will be scaled by
// @param points the initial points to plot
Plotter::Plotter(float distance, float zoom) {
    // Setting given initial values
    this->distance = distance;
    this->zoom = zoom;

    // Get terminal size
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    this->cols = csbi.srWindow.Right - csbi.srWindow.Left;
    this->rows = csbi.srWindow.Bottom - csbi.srWindow.Top;
    // End get terminal size

    // Set the array to be 'empty'
    this->plot.resize(rows, std::vector<char>(cols, ' '));
}

void Plotter::addLine(std::array<std::array<float,3>, 2> endpoints) {
    this->lines.push_back(endpoints);
}

void Plotter::addLine(float x0, float y0, float z0, float x1, float y1, float z1) {
    this->lines.push_back({x0, y0, z0, x1, y1, z1});
}

void Plotter::resetPlot() {
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            plot[j][i] = ' ';
        }
    }
}

std::array<std::array<int,2>,2> Plotter::getCoords(std::array<std::array<float,3>, 2> endpoints) {
    float x0 = endpoints[0][0];
    float y0 = endpoints[0][1];
    float z0 = endpoints[0][2];
    float x1 = endpoints[1][0];
    float y1 = endpoints[1][1];
    float z1 = endpoints[1][2];

    float scaler0 = this->distance * this->zoom / (this->distance + z0);
    float scaler1 = this->distance * this->zoom / (this->distance + z1);

    int xIndex0 = x0 * scaler0 + (cols - 1) / 2;
    int yIndex0 = y0 * scaler0 + (rows - 1) / 2;
    int xIndex1 = x1 * scaler1 + (cols - 1) / 2;
    int yIndex1 = y1 * scaler1 + (rows - 1) / 2;

    return {{
        {xIndex0, yIndex0}, 
        {xIndex1, yIndex1}
    }};
}

// @brief Draws a line between two end points
// @param x0 the x coordinates of one of the endpoints
// @param y0 the y coordinates of one of the endpoints
// @param x1 the x coordinates of the other endpoint
// @param x1 the y coordinates of the other endpoint
void Plotter::drawLine(int x0, int y0, int x1, int y1) {
    if (abs(x1 - x0) > abs(y1 - y0)) {
        if (x0 > x1) {
            int tempX = x0;
            int tempY = y0;
            x0 = x1;
            y0 = y1;
            x1 = tempX;
            y1 = tempY;
        }

        int dx = x1 - x0;
        int dy = y1 - y0;

        int dir = (dy < 0) ? -1 : 1;
        dy *= dir;

        if (dx != 0) {
            int y = y0;
            int p = 2 * dy - dx;
            for (int i = 0; i < dx + 1; i++) {
                this->putPixel(x0 + i, y);
                if (p >= 0) {
                    y += dir;
                    p -= 2 * dx;
                }
                p += 2 * dy;
            }
        }
    } else {
        if (y0 > y1) {
            int tempX = x0;
            int tempY = y0;
            x0 = x1;
            y0 = y1;
            x1 = tempX;
            y1 = tempY;
        }

        int dx = x1 - x0;
        int dy = y1 - y0;

        int dir = (dx < 0) ? -1 : 1;
        dx *= dir;

        if (dy != 0) {
            int x = x0;
            int p = 2 * dx - dy;
            for (int i = 0; i < dy + 1; i++) {
                this->putPixel(x, y0 + i);
                if (p >= 0) {
                    x += dir;
                    p -= 2 * dy;
                }
                p += 2 * dx;
            }
        }
    }
}

void Plotter::putPixel(int x, int y) {
    if (x > 0 && x < cols && y > 0 && y < rows) {
        this->plot[y][x] = (char)219u;
    }
}
        
// @brief Rotates all points along the x axis
// @param theta the angle in radians of which to rotate the points
void Plotter::rotateX(float theta) { 
    // Calculates the cos and sin of the angle to avoid redundant calculations
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    for (int i = 0; i < (int)this->lines.size(); i++) {
        float x0 = lines[i][0][0];
        float y0 = lines[i][0][1];
        float z0 = lines[i][0][2];
        float x1 = lines[i][1][3];
        float y1 = lines[i][1][4];
        float z1 = lines[i][1][5];

        lines[i][0] = {x0, y0 * cosRes - z0 * sinRes, y0 * sinRes + z0 * cosRes};
        lines[i][1] = {x1, y1 * cosRes - z1 * sinRes, y1 * sinRes + z1 * cosRes};
    }     
}

// @brief Rotates all points along the y axis
// @param theta the angle in radians of which to rotate the points
void Plotter::rotateY(float theta) { 
    // Calculates the cos and sin of the angle to avoid redundant calculations
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    for (int i = 0; i < (int)this->lines.size(); i++) {
        float x0 = lines[i][0][0];
        float y0 = lines[i][0][1];
        float z0 = lines[i][0][2];
        float x1 = lines[i][1][3];
        float y1 = lines[i][1][4];
        float z1 = lines[i][1][5];

        lines[i][0] = {x0 * cosRes + z0 * sinRes, y0, -x0 * sinRes + z0 * cosRes};
        lines[i][1] = {x1 * cosRes + z1 * sinRes, y1, -x1 * sinRes + z1 * cosRes};
    }         
}

// @brief Rotates all points along the Z axis
// @param theta the angle in radians of which to rotate the points
void Plotter::rotateZ(float theta) { 
    // Calculates the cos and sin of the angle to avoid redundant calculations
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    for (int i = 0; i < (int)this->lines.size(); i++) {
        float x0 = lines[i][0][0];
        float y0 = lines[i][0][1];
        float z0 = lines[i][0][2];
        float x1 = lines[i][1][3];
        float y1 = lines[i][1][4];
        float z1 = lines[i][1][5];

        lines[i][0] = {x0 * cosRes - y0 * sinRes, x0 * sinRes + y0 * cosRes, z0};
        lines[i][1] = {x1 * cosRes - y1 * sinRes, x1 * sinRes + y1 * cosRes, z1};
    }       
}

// @brief Calculates what should be draw for each cell then draws the points
void Plotter::draw() {
    // Move cursor to the starting position
    std::cout << "\033[H";

    // Hide cursor
    std::cout << "\033[?25l";

    this->resetPlot();

    for (const auto& line : this->lines) {
        std::array<std::array<int,2>,2> coords = this->getCoords(line);
        drawLine(coords[0][0], coords[0][1], coords[1][0], coords[1][1]);
    }

    std::string buffer;
    buffer.reserve(rows * cols);

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            buffer += this->plot[j][i];
        }
        // Add newline except after the last row
        if (j != rows - 1) {
            buffer += '\n';
        }
    }

    std::cout << buffer;
    std::cout.flush();  // Ensure output is immediately displayed
}


void Plotter::end() {
    std::cout << "\033[?25h";
}