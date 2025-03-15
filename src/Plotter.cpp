#define NOMINMAX
#include <array>
#include <vector>
#include <windows.h>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include "Plotter.hpp"

Plotter::Plotter(float distance, float zoom) {
    // Setting given initial values
    this->distance = distance;
    this->zoom = zoom;
    this->renderStyle = false;
    
    this->savingText = false;
    this->frameNum = 0;

    // Get terminal size
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    this->cols = csbi.srWindow.Right - csbi.srWindow.Left;
    this->rows = csbi.srWindow.Bottom - csbi.srWindow.Top;
    // End get terminal size

    // Set the array to be 'empty'
    this->plot.resize(rows, std::vector<int>(cols, 2147483646));
}


void Plotter::loadObj(std::string fileName) {
    std::fstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "No file \"" << fileName << "\" found.\n";
        return;
    }

    std::cout << "Reading \"" << fileName << "\"...\n"; 

    std::vector<fPoint3> vertices;
    std::vector<std::vector<fPoint3>> faces;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            fPoint3 newVertex;
            ss >> newVertex.x >> newVertex.y >> newVertex.z;

            vertices.push_back(newVertex);
        } else if (type == "f") {
            std::vector<fPoint3> face;
            std::string vertexData;
            
            while (ss >> vertexData) {
                std::stringstream vss(vertexData);
                std::string vertexIndex;
                std::getline(vss, vertexIndex, '/');
                face.push_back(vertices[std::stoi(vertexIndex) - 1]);
            }

            faces.push_back(face);
        }
    }

    std::cout << "Done read, plotting...\n";

    for (const auto& face : faces) {
        this->addDoublePolygon(face);
    }

    std::cout << "Finished!\n";
}

void Plotter::addPoint(fPoint3 p) {
    this->points.push_back(p);
}

void Plotter::addPoint(float x, float y, float z) {
    this->points.push_back({x, y, z});
}

void Plotter::addLine(std::array<fPoint3, 2> ps) {
    this->lines.push_back(ps);
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

void Plotter::addDoubleTriangle(std::array<fPoint3, 3> vertices) {
    this->addTriangle(vertices);
    this->addTriangle({{vertices[0], vertices[2], vertices[1]}});
}

void Plotter::addDoubleTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2) {
    this->addTriangle(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    this->addTriangle(x0, y0, z0, x2, y2, z2, x1, y1, z1);
}

void Plotter::addPolygon(std::vector<fPoint3> vertices) {
    if (vertices.size() < 3) return;

    for (int i = 1; i < (int)vertices.size() - 1; i++) {
        this->addTriangle({{vertices[0], vertices[i], vertices[i + 1]}});
    }
}

void Plotter::addDoublePolygon(std::vector<fPoint3> vertices) {
    if (vertices.size() < 3) return;

    for (int i = 1; i < (int)vertices.size() - 1; i++) {
        this->addDoubleTriangle({{vertices[0], vertices[i], vertices[i + 1]}});
    }
}

void Plotter::resetPlot() {
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            plot[j][i] = INT_MAX;
        }
    }
}

iPoint2 Plotter::to2d(fPoint3 p) {
    float x = p.x;
    float y = p.y;
    float z = p.z;

    float scaler = this->distance * this->zoom / (this->distance + z);

    int xIndex = x * scaler + (this->cols - 1) / 2;
    int yIndex = y * scaler + (this->rows - 1) / 2;

    return {xIndex, yIndex};
}

void Plotter::drawLine(iPoint2 p0, float z0, iPoint2 p1, float z1) {
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

fPoint3 Plotter::rotatePointX(fPoint3 p, float theta) {
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    return {p.x, p.y * cosRes - p.z * sinRes, p.y * sinRes + p.z * cosRes};
}

fPoint3 Plotter::rotatePointY(fPoint3 p, float theta) {
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    return {p.x * cosRes + p.z * sinRes, p.y, -p.x * sinRes + p.z * cosRes};
}

fPoint3 Plotter::rotatePointZ(fPoint3 p, float theta) {
    float cosRes = cos(theta);
    float sinRes = sin(theta);

    return {p.x * cosRes - p.y * sinRes, p.x * sinRes + p.y * cosRes, p.z};
}

void Plotter::rotateX(float theta) { 
    for (int i = 0; i < (int)this->points.size(); i++) {
        this->points[i] = this->rotatePointX(this->points[i], theta);
    } 

    for (int i = 0; i < (int)this->lines.size(); i++) {
        this->lines[i][0] = this->rotatePointX(this->lines[i][0], theta);
        this->lines[i][1] = this->rotatePointX(this->lines[i][1], theta);
    }     
    
    for (int i = 0; i < (int)this->triangles.size(); i++) {
        this->triangles[i][0] = this->rotatePointX(this->triangles[i][0], theta);
        this->triangles[i][1] = this->rotatePointX(this->triangles[i][1], theta);
        this->triangles[i][2] = this->rotatePointX(this->triangles[i][2], theta);
    }
}

void Plotter::rotateY(float theta) { 
    for (int i = 0; i < (int)this->points.size(); i++) {
        this->points[i] = this->rotatePointY(this->points[i], theta);
    } 

    for (int i = 0; i < (int)this->lines.size(); i++) {
        this->lines[i][0] = this->rotatePointY(this->lines[i][0], theta);
        this->lines[i][1] = this->rotatePointY(this->lines[i][1], theta);
    }     
    
    for (int i = 0; i < (int)this->triangles.size(); i++) {
        this->triangles[i][0] = this->rotatePointY(this->triangles[i][0], theta);
        this->triangles[i][1] = this->rotatePointY(this->triangles[i][1], theta);
        this->triangles[i][2] = this->rotatePointY(this->triangles[i][2], theta);
    }
}

void Plotter::rotateZ(float theta) { 
    for (int i = 0; i < (int)this->points.size(); i++) {
        this->points[i] = this->rotatePointZ(this->points[i], theta);
    } 

    for (int i = 0; i < (int)this->lines.size(); i++) {
        this->lines[i][0] = this->rotatePointZ(this->lines[i][0], theta);
        this->lines[i][1] = this->rotatePointZ(this->lines[i][1], theta);
    }     
    
    for (int i = 0; i < (int)this->triangles.size(); i++) {
        this->triangles[i][0] = this->rotatePointZ(this->triangles[i][0], theta);
        this->triangles[i][1] = this->rotatePointZ(this->triangles[i][1], theta);
        this->triangles[i][2] = this->rotatePointZ(this->triangles[i][2], theta);
    }
}

const float maxZoom = 50.0f;
void Plotter::increaseZoom(float change) {
    if (this->zoom + change <= 0 || this->zoom + change > maxZoom) return;
    this->zoom += change;
}

const float maxDistance = 50.0f;
void Plotter::increaseDistance(float change) {
    if (this->distance + change <= 0 || this->distance + change > maxDistance) return;
    this->distance += change;
}

void Plotter::toggleRender() {
    this->renderStyle = !this->renderStyle;
}

const std::string chars = "@%#*+=-:. "; 
const float zMin = -5.0f;   
const float zMax = 5.0f; 
char getChar(int z) {
    if (z <= zMin || z >= zMax) return ' ';

    float zNorm = (z - zMin) / (zMax - zMin);

    int index = static_cast<int>(zNorm * (chars.size() - 1));

    return chars[index];
}

std::string getAnsi(int z) {
    if (z <= zMin || z >= zMax) return " ";

    // int value = 255 - static_cast<int>((z - zMin) / (zMax - zMin) * 255);
    // std::string strValue = std::to_string(value);

    return "█";

    // return "\033[38;2;" + strValue + ";" + strValue + ";" + strValue + "m" + (char)219u + "\033[0m";
}

void saveText(const std::string& text, int frameNum) {
    std::string filename = "./frames/text/frame_" + std::to_string(frameNum) + ".txt";
    std::ofstream file(filename);
    file << text;
}

void Plotter::toggleSaving() {
    this->savingText = !this->savingText;
}

void Plotter::draw() {
    // Move cursor to the starting position
    std::cout << "\033[H";

    // Hide cursor
    std::cout << "\033[?25l";

    this->resetPlot();

    for (const auto& point : this->points) {
        iPoint2 coord = this->to2d(point);
        this->putPixel(coord.x, coord.y, point.z);
    }

    for (const auto& line : this->lines) {
        iPoint2 aCoord = this->to2d(line[0]);
        iPoint2 bCoord = this->to2d(line[1]);
        this->drawLine(aCoord, line[0].z, bCoord, line[1].z);
    }

    for (const auto& triangle : this->triangles) {
        iPoint2 aCoord = this->to2d(triangle[0]);
        iPoint2 bCoord = this->to2d(triangle[1]);
        iPoint2 cCoord = this->to2d(triangle[2]);
        drawTriangle(aCoord, triangle[0].z, bCoord, triangle[1].z, cCoord, triangle[2].z);
    }

    std::string buffer;
    buffer.reserve(rows * cols);

    for (int j = rows - 1; j >= 0; j--) {
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

    if (this->savingText) {
        saveText(buffer, this->frameNum);
        this->frameNum++;
    }
}

void Plotter::cube(float r) {
    float a = 0.57735 * r;

    this->addLine(-a, -a, -a,  a, -a, -a);
    this->addLine(-a, -a,  a,  a, -a,  a);
    this->addLine(-a,  a, -a,  a,  a, -a);
    this->addLine(-a,  a,  a,  a,  a,  a);

    this->addLine(-a, -a, -a, -a,  a, -a);
    this->addLine(-a, -a,  a, -a,  a,  a);
    this->addLine( a, -a, -a,  a,  a, -a);
    this->addLine( a, -a,  a,  a,  a,  a);

    this->addLine(-a, -a, -a, -a, -a,  a);
    this->addLine(-a,  a, -a, -a,  a,  a);
    this->addLine( a, -a, -a,  a, -a,  a);
    this->addLine( a,  a, -a,  a,  a,  a);
}

void Plotter::filledCube(float r) {
    float a = 0.57735 * r;

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
    float a = 0.526 * r;
    float b = 0.851f * r;

    this->addLine(0, a, b, a, b, 0);
    this->addLine(0, a, b, -a, b, 0);
    this->addLine(0, a, -b, a, b, 0);
    this->addLine(0, a, -b, -a, b, 0);
    this->addLine(0, -a, b, a, -b, 0);
    this->addLine(0, -a, b, -a, -b, 0);
    this->addLine(0, -a, -b, a, -b, 0);
    this->addLine(0, -a, -b, -a, -b, 0);
    this->addLine(0, a, b, b, 0, a);
    this->addLine(0, a, b, -b, 0, a);
    this->addLine(0, a, -b, b, 0, -a);
    this->addLine(0, a, -b, -b, 0, -a);
    this->addLine(0, -a, b, b, 0, a);
    this->addLine(0, -a, b, -b, 0, a);
    this->addLine(0, -a, -b, b, 0, -a);
    this->addLine(0, -a, -b, -b, 0, -a);
    this->addLine(a, b, 0, b, 0, a);
    this->addLine(a, b, 0, b, 0, -a);
    this->addLine(a, -b, 0, b, 0, a);
    this->addLine(a, -b, 0, b, 0, -a);
    this->addLine(-a, b, 0, -b, 0, a);
    this->addLine(-a, b, 0, -b, 0, -a);
    this->addLine(-a, -b, 0, -b, 0, a);
    this->addLine(-a, -b, 0, -b, 0, -a);
    this->addLine(0, a, b, 0, -a, b);
    this->addLine(0, a, -b, 0, -a, -b);
    this->addLine(a, b, 0, -a, b, 0);
    this->addLine(a, -b, 0, -a, -b, 0);
    this->addLine(b, 0, a, b, 0, -a);
    this->addLine(-b, 0, a, -b, 0, -a);
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

void Plotter::duck() {
    this->addDoublePolygon({{
        {0,-2,0},
        {1,-2,0},
        {1,2,0},
        {0,2,0}
    }});

    this->addDoublePolygon({{
        {-2,0,0},
        {-1,0,0},
        {-1,1,0},
        {-2,1,0}
    }});

    this->addDoublePolygon({{
        {-1,1,0},
        {1,1,0},
        {1,2,0},
        {-1,2,0}
    }});

    this->addDoublePolygon({{
        {-1,0,0},
        {-1,-2,0},
        {2,-2,0},
        {2,0,0}
    }});
}

void Plotter::fox() {
    this->addDoublePolygon({{
        {-2, 2, 0},
        {-2, 0, 0},
        {-1, 0, 0},
        {-1, 2, 0}
    }});

    this->addDoublePolygon({{
        {2, 2, 0},
        {2, 0, 0},
        {1, 0, 0},
        {1, 2, 0}
    }});

    this->addDoublePolygon({{
        {-2, 0, 0},
        {2, 0, 0},
        {2, 1, 0},
        {-2, 1, 0}
    }});

    this->addDoublePolygon({{
        {-1, 1, 0},
        {-1, -2, 0},
        {1, -2, 0},
        {1, 1, 0}
    }});

    this->addDoublePolygon({{
        {-2, -2, 0},
        {2, -2, 0},
        {2, -1, 0},
        {-2, -1, 0}
    }});
}

void Plotter::end() {
    std::cout << "\x1B[2J\x1B[H";
}