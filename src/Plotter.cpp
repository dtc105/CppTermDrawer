#include <array>
#include <vector>
#include <windows.h>
#include <math.h>
#include <algorithm>
#include <string.h>
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
    this->plot.resize(rows, std::vector<int>(cols, 2147483646));
}

void Plotter::addLine(std::array<std::array<float,3>, 2> endpoints) {
    this->lines.push_back(endpoints);
}

void Plotter::addLine(float x0, float y0, float z0, float x1, float y1, float z1) {
    std::array<std::array<float, 3>, 2> newLine = {{{x0, y0, z0}, {x1, y1, z1}}};
    this->lines.push_back(newLine);
}

void Plotter::resetPlot() {
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            plot[j][i] = INT_MAX;
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
void Plotter::drawLine(int x0, int y0, float z0, int x1, int y1, float z1) {
    if (abs(x1 - x0) > abs(y1 - y0)) {
        if (x0 > x1) {
            int tempX = x0;
            int tempY = y0;
            float tempZ = z0;
            x0 = x1;
            y0 = y1;
            z0 = z1;
            x1 = tempX;
            y1 = tempY;
            z1 = tempZ;
        }

        int dx = x1 - x0;
        int dy = y1 - y0;
        float dz = z1 - z0;

        int dirY = (dy < 0) ? -1 : 1;
        int dirZ = (dz < 0) ? -1 : 1;
        
        dy *= dirY;
        dz *= dirZ;

        if (dx != 0) {
            int y = y0;
            float z = z0;

            int py = 2 * dy - dx;
            float pz = 2 * dz - dx;

            for (int i = 0; i < dx + 1; i++) {
                this->putPixel(x0 + i, y, z);
                // std::cout << (x0 + i) << " " << y << " " << z << "\n";
                if (py >= 0) {
                    y += dirY;
                    py -= 2 * dx;
                }
                py += 2 * dy;

                if (pz >= 0) {
                    z += dirZ;
                    pz -= 2 * dx;
                }
                pz += 2 * dz;
            }
        }
    } else {
        if (y0 > y1) {
            int tempX = x0;
            int tempY = y0;
            float tempZ = z0;
            x0 = x1;
            y0 = y1;
            z0 = z1;
            x1 = tempX;
            y1 = tempY;
            z1 = tempZ;
        }

        int dx = x1 - x0;
        int dy = y1 - y0;
        float dz = z1 - z0;

        int dirX = (dx < 0) ? -1 : 1;
        int dirZ = (dz < 0) ? -1 : 1;

        dx *= dirX;
        dz *= dirZ;

        if (dy != 0) {
            int x = x0;
            float z = z0;

            int px = 2 * dx - dy;
            float pz = 2 * dz - dy;

            for (int i = 0; i < dy + 1; i++) {
                this->putPixel(x, y0 + i, z);
                // std::cout << x << " " << (y0 + i) << " " << z << "\n";
                if (px >= 0) {
                    x += dirX;
                    px -= 2 * dy;
                }
                px += 2 * dx;
                
                if (pz >= 0) {
                    z += dirZ;
                    pz -= 2 * dy;
                }
                pz += 2 * dz;
            }
        }
    }
}

void Plotter::putPixel(int x, int y, int z) {
    if (x > 0 && x < cols && y > 0 && y < rows) {
        if (z < this->plot[y][x]) {
            this->plot[y][x] = z;
        }
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
        float x1 = lines[i][1][0];
        float y1 = lines[i][1][1];
        float z1 = lines[i][1][2];

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
        float x1 = lines[i][1][0];
        float y1 = lines[i][1][1];
        float z1 = lines[i][1][2];

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
        float x1 = lines[i][1][0];
        float y1 = lines[i][1][1];
        float z1 = lines[i][1][2];

        lines[i][0] = {x0 * cosRes - y0 * sinRes, x0 * sinRes + y0 * cosRes, z0};
        lines[i][1] = {x1 * cosRes - y1 * sinRes, x1 * sinRes + y1 * cosRes, z1};
    }       
}

const float maxZoom = 50.0f;
void Plotter::changeZoomBy(float change) {
    if (this->zoom + change <= 0 || this->zoom + change > maxZoom) return;
    this->zoom += change;
}

const float maxDistance = 50.0f;
void Plotter::changeDistanceBy(float change) {
    if (this->distance + change <= 0 || this->distance + change > maxDistance) return;
    this->distance += change;
}

const std::string chars = "@%#*+=-:. "; 
const float z_min = -5.0;   
const float z_max = 5.0; 
char Plotter::getChar(float z) {
    if (z <= z_min) return '$';
    if (z >= z_max) return ' ';

    float z_norm = (z - z_min) / (z_max - z_min); 
    int index = static_cast<int>(z_norm * (chars.size() - 1));

    return chars[index];
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
        drawLine(coords[0][0], coords[0][1], line[0][2], coords[1][0], coords[1][1], line[1][2]);
    }

    std::string buffer;
    buffer.reserve(rows * cols);

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            buffer += this->getChar(this->plot[j][i]);
            // if (this->plot[j][i] == INT_MIN) continue;
            // std::cout << this->plot[j][i] << " ";
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
    std::cout << "\x1B[2J\x1B[H";
}