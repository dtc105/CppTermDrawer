#include <iostream>
#include <thread>
#include <chrono>
#include "Plotter.hpp"

#define TAU = 6.283181030717910864769210287;


int main() {
    Plotter plotter = Plotter(20.0f, 10.0f);

    plotter.addLine(0, 0.5f, 0.809f, 0.5f, 0.809f, 0);
    plotter.addLine(0, 0.5f, 0.809f, -0.5f, 0.809f, 0);
    plotter.addLine(0, 0.5f, -0.809f, 0.5f, 0.809f, 0);
    plotter.addLine(0, 0.5f, -0.809f, -0.5f, 0.809f, 0);
    plotter.addLine(0, -0.5f, 0.809f, 0.5f, -0.809f, 0);
    plotter.addLine(0, -0.5f, 0.809f, -0.5f, -0.809f, 0);
    plotter.addLine(0, -0.5f, -0.809f, 0.5f, -0.809f, 0);
    plotter.addLine(0, -0.5f, -0.809f, -0.5f, -0.809f, 0);
    plotter.addLine(0, 0.5f, 0.809f, 0.809f, 0, 0.5f);
    plotter.addLine(0, 0.5f, 0.809f, -0.809f, 0, 0.5f);
    plotter.addLine(0, 0.5f, -0.809f, 0.809f, 0, -0.5f);
    plotter.addLine(0, 0.5f, -0.809f, -0.809f, 0, -0.5f);
    plotter.addLine(0, -0.5f, 0.809f, 0.809f, 0, 0.5f);
    plotter.addLine(0, -0.5f, 0.809f, -0.809f, 0, 0.5f);
    plotter.addLine(0, -0.5f, -0.809f, 0.809f, 0, -0.5f);
    plotter.addLine(0, -0.5f, -0.809f, -0.809f, 0, -0.5f);
    plotter.addLine(0.5f, 0.809f, 0, 0.809f, 0, 0.5f);
    plotter.addLine(0.5f, 0.809f, 0, 0.809f, 0, -0.5f);
    plotter.addLine(0.5f, -0.809f, 0, 0.809f, 0, 0.5f);
    plotter.addLine(0.5f, -0.809f, 0, 0.809f, 0, -0.5f);
    plotter.addLine(-0.5f, 0.809f, 0, -0.809f, 0, 0.5f);
    plotter.addLine(-0.5f, 0.809f, 0, -0.809f, 0, -0.5f);
    plotter.addLine(-0.5f, -0.809f, 0, -0.809f, 0, 0.5f);
    plotter.addLine(-0.5f, -0.809f, 0, -0.809f, 0, -0.5f);
    plotter.addLine(0, 0.5f, 0.809f, 0, -0.5f, 0.809f);
    plotter.addLine(0, 0.5f, -0.809f, 0, -0.5f, -0.809f);
    plotter.addLine(0.5f, 0.809f, 0, -0.5f, 0.809f, 0);
    plotter.addLine(0.5f, -0.809f, 0, -0.5f, -0.809f, 0);
    plotter.addLine(0.809f, 0, 0.5f, 0.809f, 0, -0.5f);
    plotter.addLine(-0.809f, 0, 0.5f, -0.809f, 0, -0.5f);

    while (true) {
        plotter.draw();
        plotter.rotateX(0.05f);
        plotter.rotateY(0.05f);
        plotter.rotateZ(0.05f);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    return 0;
}