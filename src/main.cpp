#include <iostream>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include "Plotter.hpp"

#define TAU = 6.283181030717910864769210287;

int main() {
    Plotter plotter = Plotter(10.0f, 4.0f);

    //* Icosahedron
    plotter.addLine(0, 2.5f, 4.045f, 2.5f, 4.045f, 0);
    plotter.addLine(0, 2.5f, 4.045f, -2.5f, 4.045f, 0);
    plotter.addLine(0, 2.5f, -4.045f, 2.5f, 4.045f, 0);
    plotter.addLine(0, 2.5f, -4.045f, -2.5f, 4.045f, 0);
    plotter.addLine(0, -2.5f, 4.045f, 2.5f, -4.045f, 0);
    plotter.addLine(0, -2.5f, 4.045f, -2.5f, -4.045f, 0);
    plotter.addLine(0, -2.5f, -4.045f, 2.5f, -4.045f, 0);
    plotter.addLine(0, -2.5f, -4.045f, -2.5f, -4.045f, 0);
    plotter.addLine(0, 2.5f, 4.045f, 4.045f, 0, 2.5f);
    plotter.addLine(0, 2.5f, 4.045f, -4.045f, 0, 2.5f);
    plotter.addLine(0, 2.5f, -4.045f, 4.045f, 0, -2.5f);
    plotter.addLine(0, 2.5f, -4.045f, -4.045f, 0, -2.5f);
    plotter.addLine(0, -2.5f, 4.045f, 4.045f, 0, 2.5f);
    plotter.addLine(0, -2.5f, 4.045f, -4.045f, 0, 2.5f);
    plotter.addLine(0, -2.5f, -4.045f, 4.045f, 0, -2.5f);
    plotter.addLine(0, -2.5f, -4.045f, -4.045f, 0, -2.5f);
    plotter.addLine(2.5f, 4.045f, 0, 4.045f, 0, 2.5f);
    plotter.addLine(2.5f, 4.045f, 0, 4.045f, 0, -2.5f);
    plotter.addLine(2.5f, -4.045f, 0, 4.045f, 0, 2.5f);
    plotter.addLine(2.5f, -4.045f, 0, 4.045f, 0, -2.5f);
    plotter.addLine(-2.5f, 4.045f, 0, -4.045f, 0, 2.5f);
    plotter.addLine(-2.5f, 4.045f, 0, -4.045f, 0, -2.5f);
    plotter.addLine(-2.5f, -4.045f, 0, -4.045f, 0, 2.5f);
    plotter.addLine(-2.5f, -4.045f, 0, -4.045f, 0, -2.5f);
    plotter.addLine(0, 2.5f, 4.045f, 0, -2.5f, 4.045f);
    plotter.addLine(0, 2.5f, -4.045f, 0, -2.5f, -4.045f);
    plotter.addLine(2.5f, 4.045f, 0, -2.5f, 4.045f, 0);
    plotter.addLine(2.5f, -4.045f, 0, -2.5f, -4.045f, 0);
    plotter.addLine(4.045f, 0, 2.5f, 4.045f, 0, -2.5f);
    plotter.addLine(-4.045f, 0, 2.5f, -4.045f, 0, -2.5f);

    //* Square
    // plotter.addLine(-5, -5, -5,  5, -5, -5);
    // plotter.addLine(-5, -5,  5,  5, -5,  5);
    // plotter.addLine(-5,  5, -5,  5,  5, -5);
    // plotter.addLine(-5,  5,  5,  5,  5,  5);

    // plotter.addLine(-5, -5, -5, -5,  5, -5);
    // plotter.addLine(-5, -5,  5, -5,  5,  5);
    // plotter.addLine( 5, -5, -5,  5,  5, -5);
    // plotter.addLine( 5, -5,  5,  5,  5,  5);

    // plotter.addLine(-5, -5, -5, -5, -5,  5);
    // plotter.addLine(-5,  5, -5, -5,  5,  5);
    // plotter.addLine( 5, -5, -5,  5, -5,  5);
    // plotter.addLine( 5,  5, -5,  5,  5,  5);

    plotter.draw();
    while (true) {
        if (_kbhit()) { // Check if a key is pressed
            if (GetAsyncKeyState('W') & 0x8000) { // W key
                plotter.rotateX(0.1f);
            }
            if (GetAsyncKeyState('A') & 0x8000) { // A key
                plotter.rotateY(-0.1f);
            }
            if (GetAsyncKeyState('S') & 0x8000) { // S key
                plotter.rotateX(-0.1f);
            }
            if (GetAsyncKeyState('D') & 0x8000) { // D key
                plotter.rotateY(0.1f);
            }
            if (GetAsyncKeyState('Q') & 0x8000) { // Q key (quit)
                plotter.rotateZ(-0.1f);
            }
            if (GetAsyncKeyState('E') & 0x8000) { // E key (example action)
                plotter.rotateZ(0.1f);
            }
            if (GetAsyncKeyState('Z') & 0x8000) break;
            plotter.draw();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    plotter.end();
    
    return 0;
}