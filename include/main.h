#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <thread>
#include <iomanip>
#include <cmath> // thay vì math.h

#define VectorMatrix vector<vector<int>>

#ifdef _WIN32
    #include <conio.h>
    #include <chrono>
#endif

#ifdef __linux__
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <stdio.h>
#endif

#endif
