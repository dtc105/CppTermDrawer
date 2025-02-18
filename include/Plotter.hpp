#ifndef PLOTTER_H
#define PLOTTER_H

#include <array>
#include <vector>
#include <windows.h>
#include <math.h>
#include <iostream>


class Plotter {
    private:
        std::vector<std::array<float, 6>> lines; // List of all lines being plotted with each array being 6 floats (x0,y0,z0) and (x1,y1,z1)
        int cols; // Amount of cols in the terminal
        int rows; // Amount of rows in the terminal
        float distance; // Distance camera is from (0,0,0).  If distance == -1 then the view is isometric
        float zoom; // "Zoom" factor of camera, effectively scales the end coords of all points proportionally
        std::vector<std::vector<char>> plot; // The "string" being printed, holds a 2d matrix in order to print the image

        // @brief Calculates what should be draw for each point
        void calculatePlot();
    
    public:
        // @brief Will tries its best to draw all points given in the terminal
        // @param distance the distance the camera is from (0,0,0)
        // @param zoom the factor all points will be scaled by
        Plotter(float distance = 1.0f, float zoom = 1.0f);

        void addLine(std::array<float, 6> endpoints);
        void addLine(float x0, float y0, float z0, float x1, float y1, float z1);

        void resetPlot();

        // @brief Draws a line between two end points
        // @param x0 the x coordinates of one of the endpoints
        // @param y0 the y coordinates of one of the endpoints
        // @param x1 the x coordinates of the other endpoint
        // @param x1 the y coordinates of the other endpoint
        void drawLine(int x0, int y0, int x1, int y1);

        // @brief Puts a pixel at (x,y)
        // @param x the horizontal part of the coordinate
        // @param y the vertical part of the coordinate
        void putPixel(int x, int y);

        // @brief Adds a point to plot
        // @param point the point to add
        void addPoint(std::array<float,3> point);

        std::array<int,4> getCoords(std::array<float, 6> endpoints);

        // @brief Rotates all points along the x axis
        // @param theta the angle in radians of which to rotate the points
        void rotateX(float theta);

        // @brief Rotates all points along the y axis
        // @param theta the angle in radians of which to rotate the points
        void rotateY(float theta);

        // @brief Rotates all points along the Z axis
        // @param theta the angle in radians of which to rotate the points
        void rotateZ(float theta);
        
        // @brief Calculates what should be draw for each cell then draws the points
        void draw();

        void end();
};



#endif