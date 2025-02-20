#include <array>
#include <vector>
#define NOMINMAX
#include <windows.h>
#include <math.h>
#include <algorithm>
#include <string>
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
    this->renderStyle = false;

    // Get terminal size
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    this->cols = csbi.srWindow.Right - csbi.srWindow.Left;
    this->rows = csbi.srWindow.Bottom - csbi.srWindow.Top;
    // End get terminal size

    // Set the array to be 'empty'
    this->plot.resize(rows, std::vector<int>(cols, 2147483646));
}

void Plotter::addLine(std::array<fPoint3, 2> endpoints) {
    this->lines.push_back(endpoints);
}

void Plotter::addLine(float x0, float y0, float z0, float x1, float y1, float z1) {
    std::array<fPoint3, 2> newLine = {{{x0, y0, z0}, {x1, y1, z1}}};
    this->lines.push_back(newLine);
}

void Plotter::addTriangle(std::array<fPoint3, 3> vertices) {
    this->triangles.push_back(vertices);
}

void Plotter::addTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2) {
    std::array<fPoint3, 3> newTriangle = {{{x0, y0, z0}, {x1, y1, z1}, {x2, y2, z2}}};
    this->triangles.push_back(newTriangle);
}

void Plotter::addDoubleTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2) {
    this->addTriangle(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    this->addTriangle(x0, y0, z0, x2, y2, z2, x1, y1, z1);
}

void Plotter::addPolygon(std::vector<fPoint3> vertices) {
    if (vertices.size() < 3) return;

    for (int i = 1; i < vertices.size() - 1; i++) {
        this->addTriangle({{vertices[0], vertices[i], vertices[i + 1]}});
    }
}

void Plotter::resetPlot() {
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            plot[j][i] = INT_MAX;
        }
    }
}

std::array<iPoint2,2> Plotter::getCoords(std::array<fPoint3, 2> endpoints) {
    float x0 = endpoints[0].x;
    float y0 = endpoints[0].y;
    float z0 = endpoints[0].z;
    float x1 = endpoints[1].x;
    float y1 = endpoints[1].y;
    float z1 = endpoints[1].z;

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

std::array<iPoint2,3> Plotter::getCoords(std::array<fPoint3, 3> endpoints) {
    float x0 = endpoints[0].x;
    float y0 = endpoints[0].y;
    float z0 = endpoints[0].z;
    float x1 = endpoints[1].x;
    float y1 = endpoints[1].y;
    float z1 = endpoints[1].z;
    float x2 = endpoints[2].x;
    float y2 = endpoints[2].y;
    float z2 = endpoints[2].z;

    float scaler0 = this->distance * this->zoom / (this->distance + z0);
    float scaler1 = this->distance * this->zoom / (this->distance + z1);
    float scaler2 = this->distance * this->zoom / (this->distance + z2);

    int xIndex0 = x0 * scaler0 + (cols - 1) / 2;
    int yIndex0 = y0 * scaler0 + (rows - 1) / 2;
    int xIndex1 = x1 * scaler1 + (cols - 1) / 2;
    int yIndex1 = y1 * scaler1 + (rows - 1) / 2;
    int xIndex2 = x2 * scaler2 + (cols - 1) / 2;
    int yIndex2 = y2 * scaler2 + (rows - 1) / 2;

    return {{
        {xIndex0, yIndex0}, 
        {xIndex1, yIndex1},
        {xIndex2, yIndex2}
    }};
}

// @brief Draws a line between two end points
// @param x0 the x coordinates of one of the endpoints
// @param y0 the y coordinates of one of the endpoints
// @param x1 the x coordinates of the other endpoint
// @param x1 the y coordinates of the other endpoint
std::vector<std::vector<int>> Plotter::drawLine(iPoint2 p0, float z0, iPoint2 p1, float z1) {
    int yMin = std::min(p0.y, p1.y);
    int yMax = std::max(p0.y, p1.y);

    std::vector<std::vector<int>> xValues(yMax - yMin + 1); 

    if (abs(p1.x - p0.x) > abs(p1.y - p0.y)) {
        if (p0.x > p1.x) {
            int tempX = p0.x;
            int tempY = p0.y;
            float tempZ = z0;
            p0.x = p1.x;
            p0.y = p1.y;
            z0 = z1;
            p1.x = tempX;
            p1.y = tempY;
            z1 = tempZ;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        float dz = z1 - z0;

        int dirY = (dy < 0) ? -1 : 1;
        int dirZ = (dz < 0) ? -1 : 1;
        
        dy *= dirY;
        dz *= dirZ;

        if (dx != 0) {
            int y = p0.y;
            float z = z0;

            int py = 2 * dy - dx;
            float pz = 2 * dz - dx;

            for (int i = 0; i < dx + 1; i++) {
                this->putPixel(p0.x + i, y, z);
                xValues[y - yMin].push_back(p0.x + i);

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
        if (p0.y > p1.y) {
            int tempX = p0.x;
            int tempY = p0.y;
            float tempZ = z0;
            p0.x = p1.x;
            p0.y = p1.y;
            z0 = z1;
            p1.x = tempX;
            p1.y = tempY;
            z1 = tempZ;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        float dz = z1 - z0;

        int dirX = (dx < 0) ? -1 : 1;
        int dirZ = (dz < 0) ? -1 : 1;

        dx *= dirX;
        dz *= dirZ;

        if (dy != 0) {
            int x = p0.x;
            float z = z0;

            int px = 2 * dx - dy;
            float pz = 2 * dz - dy;

            for (int i = 0; i < dy + 1; i++) {
                this->putPixel(x, p0.y + i, z);
                xValues[p0.y + i - yMin].push_back(x);

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
    return xValues;
}

int edgeFunction (iPoint2 a, iPoint2 b, iPoint2 c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void Plotter::drawTriangle(iPoint2 p0, float z0, iPoint2 p1, float z1, iPoint2 p2, float z2) {
    iPoint2 p = {0, 0};

    int ABC = edgeFunction(p0, p1, p2);

    if (ABC <= 0) return; 

    int xMin = std::min(p0.x, p1.x);
    xMin = std::min(xMin, p2.x);
    int xMax = std::max(p0.x, p1.x);
    xMax = std::max(xMax, p2.x);
    int yMin = std::min(p0.y, p1.y);
    yMin = std::min(yMin, p2.y);
    int yMax = std::max(p0.y, p1.y);
    yMax = std::max(yMax, p2.y);

    for (p.y = yMin; p.y < yMax; p.y++) {
        for (p.x = xMin; p.x < xMax; p.x++) {
            int ABP = edgeFunction(p0, p1, p);
            int BCP = edgeFunction(p1, p2, p);
            int CAP = edgeFunction(p2, p0, p);

            
            if (ABP >= 0 && BCP >= 0 && CAP >= 0) {
                float weightA = BCP / (float)ABC;
                float weightB = CAP / (float)ABC;
                float weightC = ABP / (float)ABC;

                int z = z0 * weightA + z1 * weightB + z2 * weightC;

                this->putPixel(p.x, p.y, z);
            }
            if (ABP < 0 && BCP < 0 && CAP < 0) {
                float weightA = BCP / (float)ABC;
                float weightB = CAP / (float)ABC;
                float weightC = ABP / (float)ABC;

                int z = z0 * weightA + z1 * weightB + z2 * weightC;

                this->putPixel(p.x, p.y, z);
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
        float x0 = this->lines[i][0].x;
        float y0 = this->lines[i][0].y;
        float z0 = this->lines[i][0].z;
        float x1 = this->lines[i][1].x;
        float y1 = this->lines[i][1].y;
        float z1 = this->lines[i][1].z;

        this->lines[i][0] = {x0, y0 * cosRes - z0 * sinRes, y0 * sinRes + z0 * cosRes};
        this->lines[i][1] = {x1, y1 * cosRes - z1 * sinRes, y1 * sinRes + z1 * cosRes};
    }
    
    for (int i = 0; i < (int)this->triangles.size(); i++) {
        float x0 = this->triangles[i][0].x;
        float y0 = this->triangles[i][0].y;
        float z0 = this->triangles[i][0].z;
        float x1 = this->triangles[i][1].x;
        float y1 = this->triangles[i][1].y;
        float z1 = this->triangles[i][1].z;
        float x2 = this->triangles[i][2].x;
        float y2 = this->triangles[i][2].y;
        float z2 = this->triangles[i][2].z;

        this->triangles[i][0] = {x0, y0 * cosRes - z0 * sinRes, y0 * sinRes + z0 * cosRes};
        this->triangles[i][1] = {x1, y1 * cosRes - z1 * sinRes, y1 * sinRes + z1 * cosRes};
        this->triangles[i][2] = {x2, y2 * cosRes - z2 * sinRes, y2 * sinRes + z2 * cosRes};
    }
}

// @brief Rotates all points along the y axis
// @param theta the angle in radians of which to rotate the points
void Plotter::rotateY(float theta) { 
    // Calculates the cos and sin of the angle to avoid redundant calculations
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    for (int i = 0; i < (int)this->lines.size(); i++) {
        float x0 = lines[i][0].x;
        float y0 = lines[i][0].y;
        float z0 = lines[i][0].z;
        float x1 = lines[i][1].x;
        float y1 = lines[i][1].y;
        float z1 = lines[i][1].z;

        lines[i][0] = {x0 * cosRes + z0 * sinRes, y0, -x0 * sinRes + z0 * cosRes};
        lines[i][1] = {x1 * cosRes + z1 * sinRes, y1, -x1 * sinRes + z1 * cosRes};
    }      
    
    for (int i = 0; i < (int)this->triangles.size(); i++) {
        float x0 = this->triangles[i][0].x;
        float y0 = this->triangles[i][0].y;
        float z0 = this->triangles[i][0].z;
        float x1 = this->triangles[i][1].x;
        float y1 = this->triangles[i][1].y;
        float z1 = this->triangles[i][1].z;
        float x2 = this->triangles[i][2].x;
        float y2 = this->triangles[i][2].y;
        float z2 = this->triangles[i][2].z;

        this->triangles[i][0] = {x0 * cosRes + z0 * sinRes, y0, -x0 * sinRes + z0 * cosRes};
        this->triangles[i][1] = {x1 * cosRes + z1 * sinRes, y1, -x1 * sinRes + z1 * cosRes};
        this->triangles[i][2] = {x2 * cosRes + z2 * sinRes, y2, -x2 * sinRes + z2 * cosRes};
    }
}

// @brief Rotates all points along the Z axis
// @param theta the angle in radians of which to rotate the points
void Plotter::rotateZ(float theta) { 
    // Calculates the cos and sin of the angle to avoid redundant calculations
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    for (int i = 0; i < (int)this->lines.size(); i++) {
        float x0 = lines[i][0].x;
        float y0 = lines[i][0].y;
        float z0 = lines[i][0].z;
        float x1 = lines[i][1].x;
        float y1 = lines[i][1].y;
        float z1 = lines[i][1].z;

        lines[i][0] = {x0 * cosRes - y0 * sinRes, x0 * sinRes + y0 * cosRes, z0};
        lines[i][1] = {x1 * cosRes - y1 * sinRes, x1 * sinRes + y1 * cosRes, z1};
    }     
    
    for (int i = 0; i < (int)this->triangles.size(); i++) {
        float x0 = this->triangles[i][0].x;
        float y0 = this->triangles[i][0].y;
        float z0 = this->triangles[i][0].z;
        float x1 = this->triangles[i][1].x;
        float y1 = this->triangles[i][1].y;
        float z1 = this->triangles[i][1].z;
        float x2 = this->triangles[i][2].x;
        float y2 = this->triangles[i][2].y;
        float z2 = this->triangles[i][2].z;

        this->triangles[i][0] = {x0 * cosRes - y0 * sinRes, x0 * sinRes + y0 * cosRes, z0};
        this->triangles[i][1] = {x1 * cosRes - y1 * sinRes, x1 * sinRes + y1 * cosRes, z1};
        this->triangles[i][2] = {x2 * cosRes - y2 * sinRes, x2 * sinRes + y2 * cosRes, z2};
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

void Plotter::toggleRender() {
    this->renderStyle = !this->renderStyle;
}

const std::string chars = "@%#*+=-:. "; 
const float z_min = -5.0f;   
const float z_max = 5.0f; 
char getChar(int z) {
    if (z <= z_min || z >= z_max) return ' ';

    return chars[static_cast<int>((z - z_min) / (z_max - z_min) * (chars.size() - 1))];
}

std::string getAnsi(int z) {
    if (z <= z_min || z >= z_max) return " ";

    int value = 255 - static_cast<int>((z - z_min) / (z_max - z_min) * 255);
    std::string strValue = std::to_string(value);

    return "\033[38;2;" + strValue + ";" + strValue + ";" + strValue + "m" + (char)219u + "\033[0m";

    // return std::format("\033[38;2;{};{};{}m{}\033[0m", value, value, value, (char)219u);

}
// @brief Calculates what should be draw for each cell then draws the points
void Plotter::draw() {
    // Move cursor to the starting position
    std::cout << "\033[H";

    // Hide cursor
    std::cout << "\033[?25l";

    this->resetPlot();

    for (const auto& line : this->lines) {
        std::array<iPoint2,2> coords = this->getCoords(line);
        this->drawLine(coords[0], line[0].z, coords[1], line[1].z);
    }

    for (const auto& triangle : this->triangles) {
        std::array<iPoint2,3> coords = this->getCoords(triangle);
        drawTriangle(coords[0], triangle[0].z, coords[1], triangle[1].z, coords[2], triangle[2].z);
    }

    std::string buffer;
    buffer.reserve(rows * cols);

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            if (renderStyle) {
                buffer += getAnsi(this->plot[j][i]);
            } else {
                buffer += getChar(this->plot[j][i]);
            }
        }
        // Add newline except after the last row
        if (j != rows - 1) {
            buffer += '\n';
        }
    }

    std::cout << buffer;
    std::cout.flush();  // Ensure output is immediately displayed
}

void Plotter::cube(float r) {
    this->addLine(-0.57735f * r, -0.57735f * r, -0.57735f * r,  0.57735f * r, -0.57735f * r, -0.57735f * r);
    this->addLine(-0.57735f * r, -0.57735f * r,  0.57735f * r,  0.57735f * r, -0.57735f * r,  0.57735f * r);
    this->addLine(-0.57735f * r,  0.57735f * r, -0.57735f * r,  0.57735f * r,  0.57735f * r, -0.57735f * r);
    this->addLine(-0.57735f * r,  0.57735f * r,  0.57735f * r,  0.57735f * r,  0.57735f * r,  0.57735f * r);

    this->addLine(-0.57735f * r, -0.57735f * r, -0.57735f * r, -0.57735f * r,  0.57735f * r, -0.57735f * r);
    this->addLine(-0.57735f * r, -0.57735f * r,  0.57735f * r, -0.57735f * r,  0.57735f * r,  0.57735f * r);
    this->addLine( 0.57735f * r, -0.57735f * r, -0.57735f * r,  0.57735f * r,  0.57735f * r, -0.57735f * r);
    this->addLine( 0.57735f * r, -0.57735f * r,  0.57735f * r,  0.57735f * r,  0.57735f * r,  0.57735f * r);

    this->addLine(-0.57735f * r, -0.57735f * r, -0.57735f * r, -0.57735f * r, -0.57735f * r,  0.57735f * r);
    this->addLine(-0.57735f * r,  0.57735f * r, -0.57735f * r, -0.57735f * r,  0.57735f * r,  0.57735f * r);
    this->addLine( 0.57735f * r, -0.57735f * r, -0.57735f * r,  0.57735f * r, -0.57735f * r,  0.57735f * r);
    this->addLine( 0.57735f * r,  0.57735f * r, -0.57735f * r,  0.57735f * r,  0.57735f * r,  0.57735f * r);
}
void Plotter::filledCube(float r) {
    float a = 0.577 * r;

    this->addPolygon({{
        {-a, -a, -a}, 
        { a, -a, -a}, 
        { a,  a, -a}, 
        {-a,  a, -a}
    }});

    this->addPolygon({{
        {-a, -a,  a}, 
        {-a,  a,  a}, 
        { a,  a,  a}, 
        { a, -a,  a}
    }});

    this->addPolygon({{
        {-a, -a, -a}, 
        {-a, -a,  a}, 
        { a, -a,  a}, 
        { a, -a, -a}
    }});

    this->addPolygon({{
        {-a,  a, -a}, 
        { a,  a, -a}, 
        { a,  a,  a}, 
        {-a,  a,  a}
    }});

    this->addPolygon({{
        {-a, -a, -a}, 
        {-a,  a, -a}, 
        {-a,  a,  a}, 
        {-a, -a,  a}
    }});

    this->addPolygon({{
        { a, -a, -a}, 
        { a, -a,  a}, 
        { a,  a,  a}, 
        { a,  a, -a}
    }});
}

void Plotter::icosahedron(float r) {
    this->addLine(0.0f * r, 0.526f * r, 0.851f * r, 0.526f * r, 0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, 0.526f * r, 0.851f * r, -0.526f * r, 0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, 0.526f * r, -0.851f * r, 0.526f * r, 0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, 0.526f * r, -0.851f * r, -0.526f * r, 0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, -0.526f * r, 0.851f * r, 0.526f * r, -0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, -0.526f * r, 0.851f * r, -0.526f * r, -0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, -0.526f * r, -0.851f * r, 0.526f * r, -0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, -0.526f * r, -0.851f * r, -0.526f * r, -0.851f * r, 0.0f * r);
    this->addLine(0.0f * r, 0.526f * r, 0.851f * r, 0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(0.0f * r, 0.526f * r, 0.851f * r, -0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(0.0f * r, 0.526f * r, -0.851f * r, 0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(0.0f * r, 0.526f * r, -0.851f * r, -0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(0.0f * r, -0.526f * r, 0.851f * r, 0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(0.0f * r, -0.526f * r, 0.851f * r, -0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(0.0f * r, -0.526f * r, -0.851f * r, 0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(0.0f * r, -0.526f * r, -0.851f * r, -0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(0.526f * r, 0.851f * r, 0.0f * r, 0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(0.526f * r, 0.851f * r, 0.0f * r, 0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(0.526f * r, -0.851f * r, 0.0f * r, 0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(0.526f * r, -0.851f * r, 0.0f * r, 0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(-0.526f * r, 0.851f * r, 0.0f * r, -0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(-0.526f * r, 0.851f * r, 0.0f * r, -0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(-0.526f * r, -0.851f * r, 0.0f * r, -0.851f * r, 0.0f * r, 0.526f * r);
    this->addLine(-0.526f * r, -0.851f * r, 0.0f * r, -0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(0.0f * r, 0.526f * r, 0.851f * r, 0.0f * r, -0.526f * r, 0.851f * r);
    this->addLine(0.0f * r, 0.526f * r, -0.851f * r, 0.0f * r, -0.526f * r, -0.851f * r);
    this->addLine(0.526f * r, 0.851f * r, 0.0f * r, -0.526f * r, 0.851f * r, 0.0f * r);
    this->addLine(0.526f * r, -0.851f * r, 0.0f * r, -0.526f * r, -0.851f * r, 0.0f * r);
    this->addLine(0.851f * r, 0.0f * r, 0.526f * r, 0.851f * r, 0.0f * r, -0.526f * r);
    this->addLine(-0.851f * r, 0.0f * r, 0.526f * r, -0.851f * r, 0.0f * r, -0.526f * r);
}

void Plotter::filledIcosahedron(float r) {
    float a = 0.526 * r;
    float b = 0.851 * r;

    this->addTriangle(0, -a, -b, 0, a, -b, -b, 0, -a);
    this->addTriangle(0, -a, b, -b, 0, a, 0, a, b);
    this->addTriangle(0, -a, -b, a, -b, 0, b, 0, -a);
    this->addTriangle(0, a, b, -a, b, 0, a, b, 0);
    this->addTriangle(-a, b, 0, -b, 0, a, -b, 0, -a);

    this->addTriangle(a, b, 0, b, 0, -a, b, 0, a);
    this->addTriangle(0, -a, -b, -b, 0, -a, -a, -b, 0);
    this->addTriangle(0 ,-a, b, b, 0, a, a, -b, 0);
    this->addTriangle(0, a, -b, b, 0, -a, a, b, 0);
    this->addTriangle(0, -a, b, a, -b, 0, -a, -b, 0);

    this->addTriangle(0, a, -b, a, b, 0, -a, b, 0);
    this->addTriangle(0, -a, b, -a, -b, 0, -b, 0, a);
    this->addTriangle(0, a, -b, -a, b, 0, -b, 0, -a);
    this->addTriangle(0, a, b, a, b, 0, b, 0 ,a);
    this->addTriangle(-a, -b, 0, -b, 0, -a, -b, 0, a);

    this->addTriangle(a, -b, 0, b, 0, a, b, 0, -a);
    this->addTriangle(0, -a, -b, -a, -b, 0, a, -b, 0);
    this->addTriangle(0, a, b, -b, 0, a, -a, b, 0);
    this->addTriangle(0, -a, -b, b, 0, -a, 0, a, -b);
    this->addTriangle(0, -a, b, 0, a, b, b, 0, a);
} 
void Plotter::end() {
    std::cout << "\x1B[2J\x1B[H";
}