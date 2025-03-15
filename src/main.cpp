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
    Plotter plotter = Plotter(15.0f, 2.0f);

    // plotter.loadObj("C:/Users/qqaz4/source/repos/CppTermDrawer/C/ar.obj");
    plotter.icosahedron(5);

    plotter.draw();
    while (true) {
        if (_kbhit()) { // Check if a key is pressed
            if (GetAsyncKeyState('W') & 0x8000) { // W key
                plotter.rotateX(0.10471975512f);
            } else if (GetAsyncKeyState('S') & 0x8000) { // S key
                plotter.rotateX(-0.10471975512f);
            }

            if (GetAsyncKeyState('A') & 0x8000) { // A key
                plotter.rotateY(0.10471975512f);
            } else if (GetAsyncKeyState('D') & 0x8000) { // D key
                plotter.rotateY(-0.10471975512f);
            }

            if (GetAsyncKeyState('Q') & 0x8000) { // Q key
                plotter.rotateZ(0.10471975512f);
            } else if (GetAsyncKeyState('E') & 0x8000) { // E key
                plotter.rotateZ(-0.10471975512f);
            }

            if (GetAsyncKeyState('I') & 0x8000) { // I key
                plotter.increaseZoom(0.10471975512f);
            } else if (GetAsyncKeyState('O') & 0x8000) { // O key
                plotter.increaseZoom(-0.10471975512f);
            }
            
            if (GetAsyncKeyState('J') & 0x8000) { // J key
                plotter.increaseDistance(-0.10471975512f);
            } else if (GetAsyncKeyState('K') & 0x8000) { // K key
                plotter.increaseDistance(0.10471975512f);
            }

            if (GetAsyncKeyState('V') & 0x8000) {
                plotter.toggleSaving();
            }

            if (GetAsyncKeyState('T') & 0x8000) { // T key
                plotter.toggleRender();
            }

            if (GetAsyncKeyState('Z') & 0x8000) break;
            plotter.draw();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    plotter.end();
    
    return 0;
}