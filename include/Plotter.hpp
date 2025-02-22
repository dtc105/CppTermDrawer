#ifndef PLOTTER_H
#define PLOTTER_H

#include <array>
#include <vector>
#include <windows.h>
#include <math.h>
#include <iostream>
#include "fPoint3.hpp"

struct iPoint2 {
    int x, y;
};

class Plotter {
    private:
        std::vector<fPoint3> points; // List of all points being plotted
        std::vector<std::array<fPoint3, 2>> lines; // List of all lines being plotted
        std::vector<std::array<fPoint3, 3>> triangles; // List of all triangles being plotted
        int cols; // Amount of cols in the terminal
        int rows; // Amount of rows in the terminal
        float distance; // Distance camera is from (0,0,0).  If distance == -1 then the view is isometric
        float zoom; // "Zoom" factor of camera, effectively scales the end coords of all points proportionally
        std::vector<std::vector<int>> plot; // The "string" being printed, holds a 2d matrix in order to print the image
        bool renderStyle; // The style to render with, if True then ascii else ansi
        
        // @brief Calculates what should be draw for each point
        void calculatePlot();

        // @brief converts a 3d point of floats to a 2d point of ints for plotting to the display
        // @param p the point to convert
        // @return the 2d representation of the point
        iPoint2 to2d(fPoint3 point);

    public:
        // @brief Will draw a 2d representation of 3d lines and polygons in the terminal
        // @param distance the distance the camera is from (0,0,0)
        // @param zoom the factor all points will be scaled by
        Plotter(float distance = 1.0f, float zoom = 1.0f);

        void loadObj(std::string fileName);

        // @brief Adds a point to be drawn
        // @param p the point
        void addPoint(fPoint3 p);
        
        // @brief Adds a point to be drawn
        // @param x the x coordinate of the point
        // @param y the y coordinate of the point
        // @param z the z coordinate of the point
        void addPoint(float x, float y, float z);

        // @brief Adds a line to be drawn
        // @param ps the points defining the line
        void addLine(std::array<fPoint3, 2> ps);
        
        // @brief Adds a line to be drawn
        // @param x0 the x coordinate of the first point defining the line
        // @param y0 the y coordinate of the first point defining the line
        // @param z0 the z coordinate of the first point defining the line
        // @param x1 the x coordinate of the second point defining the line
        // @param y1 the y coordinate of the second point defining the line
        // @param z1 the z coordinate of the second point defining the line
        void addLine(float x0, float y0, float z0, float x1, float y1, float z1);

        // @brief Adds a triangle to be drawn
        // @param vs the 3 vertices the define a triangle
        void addTriangle(std::array<fPoint3, 3> vs);
        
        // @brief Adds a triangle to be drawn
        // @param x0 the x coordinate of the first point defining the triangle
        // @param y0 the y coordinate of the first point defining the triangle
        // @param z0 the z coordinate of the first point defining the triangle
        // @param x1 the x coordinate of the second point defining the triangle
        // @param y1 the y coordinate of the second point defining the triangle
        // @param z1 the z coordinate of the second point defining the triangle
        // @param x2 the x coordinate of the third point defining the triangle
        // @param y2 the y coordinate of the third point defining the triangle
        // @param z2 the z coordinate of the third point defining the triangle
        void addTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2);
        
        // @brief Adds a double triangle which allows it to be rendered from any angle
        // @param vs the 3 vertices the define a triangle
        void addDoubleTriangle(std::array<fPoint3, 3> vs);
        
        // @brief Adds a double triangle which allows it to be rendered from any angle
        // @param x0 the x coordinate of the first point defining the triangle
        // @param y0 the y coordinate of the first point defining the triangle
        // @param z0 the z coordinate of the first point defining the triangle
        // @param x1 the x coordinate of the second point defining the triangle
        // @param y1 the y coordinate of the second point defining the triangle
        // @param z1 the z coordinate of the second point defining the triangle
        // @param x2 the x coordinate of the third point defining the triangle
        // @param y2 the y coordinate of the third point defining the triangle
        // @param z2 the z coordinate of the third point defining the triangle
        void addDoubleTriangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2);

        // @brief Adds a n sided irregular polygon to be drawn
        // @param vs the points defining the polygon
        void addPolygon(std::vector<fPoint3> vs);
    
        // @brief Adds a n sided irregular polygon to be drawn and will be rendered from any angle
        // @param vs the points defining the polygon
        void addDoublePolygon(std::vector<fPoint3> vs);

        // @brief Clears the plot to avoid artifacts from previous frames
        void resetPlot();

        // @brief Draws a line between two end points
        // @param p0 the first point defining the line
        // @param z0 the z value of the first point
        // @param p1 the second point defining the line
        // @param z1 the z value of the second point
        void drawLine(iPoint2 p0, float z0, iPoint2 p1, float z1);

        // @brief Draws a triangle from three points
        // @param p0 the first point defining the triangle
        // @param z0 the z value of the first point
        // @param p1 the second point defining the triangle
        // @param z1 the z value of the second point
        // @param p2 the third point defining the triangle
        // @param z2 the z value of the third point
        void drawTriangle(iPoint2 p0, float z0, iPoint2 p1, float z1, iPoint2 p2, float z2);

        // @brief Puts a pixel at (x,y)
        // @param x the horizontal part of the coordinate
        // @param y the vertical part of the coordinate
        void putPixel(int x, int y, int z);

        // @breif Returns a single point after being rotated about the x-axis by theta radians
        // @param p the point to rotate
        // @param theta the amount to rotate the point by in radians
        fPoint3 rotatePointX(fPoint3 p, float theta);
        
        // @breif Returns a single point after being rotated about the y-axis by theta radians
        // @param p the point to rotate
        // @param theta the amount to rotate the point by in radians
        fPoint3 rotatePointY(fPoint3 p, float theta);
        
        // @breif Returns a single point after being rotated about the z-axis by theta radians
        // @param p the point to rotate
        // @param theta the amount to rotate the point by in radians
        fPoint3 rotatePointZ(fPoint3 p, float theta);

        // @brief Rotates all points along the x axis
        // @param theta the angle in radians of which to rotate the points
        void rotateX(float theta);

        // @brief Rotates all points along the y axis
        // @param theta the angle in radians of which to rotate the points
        void rotateY(float theta);

        // @brief Rotates all points along the Z axis
        // @param theta the angle in radians of which to rotate the points
        void rotateZ(float theta);

        // @brief increases the zoom of the renderer
        // @param increase the amount to increase the zoom by
        void increaseZoom(float change);
        
        // @brief increases the distance of the renderer
        // @param increase the amount to increase the distance by
        void increaseDistance(float change);

        // @brief Toggles the rendering style between ascii and ansi
        void toggleRender();
        
        // @brief Calculates what should be draw for each cell then draws the points
        void draw();

        // @brief Draws the skeleton of a cube with radius r
        // @param r the radius of the cube
        void cube(float r);

        // @brief Draws a cube with radius r
        // @param r the radius of the cube
        void filledCube(float r);

        // @brief Draws the skeleton of a icosahedron with radius r
        // @param r the radius of the icosahedron
        void icosahedron(float r);

        // @brief Draws a icosahedron with radius r
        // @param r the radius of the icosahedron
        void filledIcosahedron(float r);

        // @brief Draws the dtc logo
        void duck();

        // @brief Draws the kodi logo
        void fox();

        // @brief Clears the display
        void end();
};



#endif