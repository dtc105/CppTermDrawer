#ifndef PLOTTER_H
#define PLOTTER_H

#include <array>
#include <vector>
#include <windows.h>
#include <math.h>
#include <iostream>

struct iPoint2 {
    int x, y;
};

struct fPoint3 {
    float x, y, z;
};

class Plotter {
    private:
        std::vector<std::array<fPoint3, 2>> lines; // List of all lines being plotted
        std::vector<std::array<fPoint3, 3>> triangles; // List of all triangles being plotted
        int cols; // Amount of cols in the terminal
        int rows; // Amount of rows in the terminal
        float distance; // Distance camera is from (0,0,0).  If distance == -1 then the view is isometric
        float zoom; // "Zoom" factor of camera, effectively scales the end coords of all points proportionally
        std::vector<std::vector<int>> plot; // The "string" being printed, holds a 2d matrix in order to print the image
        bool renderStyle;

        // @brief Calculates what should be draw for each point
        void calculatePlot();

        std::array<iPoint2,2> getCoords(std::array<fPoint3, 2> endpoints);
        std::array<iPoint2,3> getCoords(std::array<fPoint3, 3> endpoints);

        // char getChar(int z);

    public:
        // @brief Will tries its best to draw all points given in the terminal
        // @param distance the distance the camera is from (0,0,0)
        // @param zoom the factor all points will be scaled by
        Plotter(float distance = 1.0f, float zoom = 1.0f);

        void addLine(std::array<fPoint3, 2> endpoints);
        void addLine(float x0, float y0, float z0, float x1, float y1, float z1);

        void addTriangle(std::array<fPoint3, 3> vertices);
        void addTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2);
        
        void addDoubleTriangle(std::array<fPoint3, 3> vertices);
        void addDoubleTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2);

        void addPolygon(std::vector<fPoint3> vertices);

        void resetPlot();

        // @brief Draws a line between two end points
        // @param x0 the x coordinates of one of the endpoints
        // @param y0 the y coordinates of one of the endpoints
        // @param x1 the x coordinates of the other endpoint
        // @param x1 the y coordinates of the other endpoint
        std::vector<std::vector<int>> drawLine(iPoint2 p0, float z0, iPoint2 p1, float z1);

        void drawTriangle(iPoint2 p0, float z0, iPoint2 p1, float z1, iPoint2 p2, float z2);
        // @brief Puts a pixel at (x,y)
        // @param x the horizontal part of the coordinate
        // @param y the vertical part of the coordinate
        void putPixel(int x, int y, int z);

        // @brief Rotates all points along the x axis
        // @param theta the angle in radians of which to rotate the points
        void rotateX(float theta);

        // @brief Rotates all points along the y axis
        // @param theta the angle in radians of which to rotate the points
        void rotateY(float theta);

        // @brief Rotates all points along the Z axis
        // @param theta the angle in radians of which to rotate the points
        void rotateZ(float theta);

        void changeZoomBy(float change);
        void changeDistanceBy(float change);
        void toggleRender();
        
        // @brief Calculates what should be draw for each cell then draws the points
        void draw();

        void cube(float r);
        void filledCube(float r);

        void icosahedron(float r);
        void filledIcosahedron(float r);

        void end();
};



#endif