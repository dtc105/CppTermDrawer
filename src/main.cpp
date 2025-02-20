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

    plotter.filledIcosahedron(5);

    plotter.draw();
    while (true) {
        if (_kbhit()) { // Check if a key is pressed
            if (GetAsyncKeyState('W') & 0x8000) { // W key
                plotter.rotateX(-0.1f);
            } else if (GetAsyncKeyState('S') & 0x8000) { // S key
                plotter.rotateX(0.1f);
            }

            if (GetAsyncKeyState('A') & 0x8000) { // A key
                plotter.rotateY(0.1f);
            } else if (GetAsyncKeyState('D') & 0x8000) { // D key
                plotter.rotateY(-0.1f);
            }

            if (GetAsyncKeyState('Q') & 0x8000) { // Q key
                plotter.rotateZ(-0.1f);
            } else if (GetAsyncKeyState('E') & 0x8000) { // E key
                plotter.rotateZ(0.1f);
            }

            if (GetAsyncKeyState('I') & 0x8000) { // I key
                plotter.changeZoomBy(0.1f);
            } else if (GetAsyncKeyState('O') & 0x8000) { // O key
                plotter.changeZoomBy(-0.1f);
            }
            
            if (GetAsyncKeyState('J') & 0x8000) { // J key
                plotter.changeDistanceBy(-0.1f);
            } else if (GetAsyncKeyState('K') & 0x8000) { // K key
                plotter.changeDistanceBy(0.1f);
            }

            if (GetAsyncKeyState('T') & 0x8000) { // T key
                plotter.toggleRender();
            }

            if (GetAsyncKeyState('Z') & 0x8000) break;
            plotter.draw();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    plotter.end();
    
    return 0;
}