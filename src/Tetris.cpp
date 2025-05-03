#include "Tetris.h"

vector<vector<vector<int>>> tetrominoShapes = {
    { {1,1,1,1,1,1,1,1}},                   // I

    { {1,1,1,1},
      {1,1,1,1} },                 // O

    { {0,0,1,1,0,0}
     ,{1,1,1,1,1,1} },             // T
    
    { {0,0,0,0,1,1}
     ,{1,1,1,1,1,1} },             // L
    
    { {1,1,0,0,0,0},
      {1,1,1,1,1,1} },             // J
    
    { {0,0,1,1,1,1}, 
      {1,1,1,1,0,0} },             // S

    { {1,1,1,1,0,0},
      {0,0,1,1,1,1} },              // Z
};

string colorBlock(int color) {
    stringstream ss;
    switch (color) {
        case 1: ss << "\033[41m \033[0m"; break;
        case 2: ss << "\033[42m \033[0m"; break;
        case 3: ss << "\033[43m \033[0m"; break;
        case 4: ss << "\033[44m \033[0m"; break;
        case 5: ss << "\033[45m \033[0m"; break;
        case 6: ss << "\033[46m \033[0m"; break;
        case 7: ss << "\033[47m \033[0m"; break;
    }
    return ss.str();
}

void Tetris::clearInputBuffer() {
    while (_kbhit()) _getch(); 
}

void Tetris::handlePause() {
    while (paused) {
        if (_kbhit()) {
            int c = _getch();
            if (c == 'r' || c == 'R') {
                paused = 0;
                return;
            }
        }
    }
}

void Tetris::InsertName() {
    int cnt = 0;
    do {
        if (cnt++ > 0) cout << warning << "\n";
        cout << "Please insert your name: ";
        getline(cin, name);
        system("cls");
    } while (name.size() < 1 || name.size() > 10);
    name += string(WIDTH + 2 - name.size(), ' ');
    nameIter = name.size();
}

void Tetris::spawnTetromino() {
    int id = rand() % 7;
    current.getShape() = tetrominoShapes[id];
    current.setX(WIDTH / 2 - 2);
    current.setY(0);
    current.setColor(rand() % 7 + 1);
}

void Tetris::spawnNextTetromino() {
    int id = rand() % 7;
    nextT.getShape() = tetrominoShapes[id];
    nextT.setX(WIDTH / 2 - 2);
    nextT.setY(0);
    nextT.setColor(rand() % 7 + 1);
}

bool Tetris::collides(int dx, int dy) {
    auto shape = current.getShape();
    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[0].size(); ++x) {
            if (shape[y][x]) {
                int newX = current.getX() + x + dx;
                int newY = current.getY() + y + dy;
                if (newX <= 1 || newX >= WIDTH || newY >= HEIGHT + 1)
                    return true;
                if (newY >= 0 && gameBoard.getBoardXY(newY, newX))
                    return true;
            }
        }
    }
    return false;
}

Tetris::Tetris() {
    gameBoard = Board();
    current = Tetromino();
    nextT = Tetromino();
    InsertName();
    srand(time(0));
    spawnTetromino();
    spawnNextTetromino();
}

void Tetris::Playing() {
    while (true) {
        draw();
        handleInput();
        handlePause();
        
        if (!collides(0, 1)) current.setY(current.getY() + 1);
        else {
            merge();
            clearLines();
            clearInputBuffer();
            current = nextT;
            spawnNextTetromino();
            if (collides(0, 0)) {
                cout << "Game Over!\n";
                break;
            }
        }
        clearInputBuffer();

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

void Tetris::handleInput() {
    static auto lastInput = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastInput).count() > 100) {
        if (_kbhit()) {
            int c = _getch();
            int x = current.getX();
            int y = current.getY();

            if (c == 224 || c == 0) {
                int special = _getch();
                switch (special) {
                    case 72: rotate(); break;                    
                    case 80: if (!collides(0, 1)) current.setY(y + 1); break;
                    case 75: if (!collides(-2, 0)) current.setX(x - 2); break;
                    case 77: if (!collides(2, 0)) current.setX(x + 2); break;
                }
            } else {
                switch (c) {
                    case 'p': case 'P': paused = 1; break;
                    case 'r': case 'R': paused = 0; break;
                    case 'w': case 'W': rotate(); break;
                    case 's': case 'S': if (!collides(0, 1)) current.setY(y + 1); break;
                    case 'a': case 'A': if (!collides(-2, 0)) current.setX(x - 2); break;
                    case 'd': case 'D': if (!collides(2, 0)) current.setX(x + 2); break;
                    case ' ': while (!collides(0, 1)) current.setY(current.getY() + 1); break;
                }
            }
            lastInput = now;
        }
    }
}

string Tetris::centerWithColor(const string& content, int totalWidth) {
    int visibleLen = 0;
    bool inEscape = false;
    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '\033') inEscape = true;
        else if (inEscape && content[i] == 'm') inEscape = false;
        else if (!inEscape) visibleLen++;
    }

    int padLeft = (totalWidth - visibleLen) / 2;
    int padRight = totalWidth - visibleLen - padLeft;
    return string(padLeft, ' ') + content + string(padRight, ' ');
}

void Tetris::outsider(int i) {
    string le = "\t\t\t\033[45m||\033[0m";
    string ri = "\033[45m||\033[0m";
    string press = "\t\tPress \033[31m";
    string border = "\t\t\t\033[46m" + string(width_score, '-') + "\033[0m";
    string endd = "\033[0m";

    auto shape = nextT.getShape();
    auto color = nextT.getColor();

    if (i == 1 || i == 12 || i == 7) cout << border;
    if (i == 2) cout << le << centerWithColor("NEXT", width_score - 4) << ri;
    if (i == 3 || i == 6 || i == 8 || i == 11) cout << le << string(width_score - 4, ' ') << ri;
    string tmp;
    if (i == 4) {
        for (int y = 0; y < shape[0].size(); ++y) {
            if (shape[0][y]) tmp += colorBlock(color);
            else tmp += " ";
        }
        cout << le << centerWithColor(tmp, width_score - 4) << ri;
    }
    if (i == 5) {
        if (shape[0].size() < 8) {
            for (int y = 0; y < shape[0].size(); ++y) {
                if (shape[1][y]) tmp += colorBlock(color);
                else tmp += " ";
            }
        } 
        cout << le << centerWithColor(tmp, width_score - 4) << ri;
    }

    if (i == 9) {
        string title = "SCORES";
        if (updateScoreSuccess > 0 && updateScoreSuccess % 2)
            title = "\033[31m" + title + endd;

        cout << le << centerWithColor(title, width_score - 4) << ri;
    }
    if (i == 10) {
        string scoreStr = to_string(score);
        if (updateScoreSuccess > 0 && updateScoreSuccess % 2)
            scoreStr = "\033[31m" + scoreStr + endd;

        updateScoreSuccess--;

        cout << le << centerWithColor(scoreStr, width_score - 4) << ri;
    }
    if (i == 14) cout << press << "a, A, " 
                    << char(17) << "\033[0m to move to \033[1;35mleft" << endd;

    if (i == 15) cout << press << "d, D, " 
                    << char(16) << "\033[0m to move to \033[34mright" << endd;

    if (i == 16) cout << press << "s, S, "
                    << char(31) << "\033[0m to move to \033[33mdown faster" << endd;

    if (i == 17) cout << press << "w, W, " 
                    << char(30) << "\033[0m to \033[32mrotate" << endd;
    
    if (i == 18) cout << press << "p, P, " 
                    << "\033[0m  to \033[36mpause" << endd;
    
    if (i == 19) cout << press << "r, R, " 
                    << "\033[0m  to \033[30mresume" << endd;
    
    if (i == 20) cout << press << "SPACE " 
                    << "\033[0m  to \033[96mdrop instantly" << endd;
}

void Tetris::draw() {
    system("cls"); 
    cout << "+" << centerWithColor(hello, WIDTH) << "+\n";
    
    cout << "\033[1;35m" + name.substr(nameIter) << "\033[0m";
    cout << "\033[1;35m" << name.substr(0, nameIter) << "\033[0m\n";

    nameIter = (nameIter - 1 + name.size()) % name.size();
    for (int i = 0; i < HEIGHT + 2; ++i) {
        for (int j = 0; j < WIDTH + 2; ++j) {
            if (i == 0 || i == HEIGHT + 1) {
                if (j % 4 <= 1) cout << "\033[30;42m";
                else cout << "\033[30;44m";
                cout << "-";
                cout << "\033[0m";
                continue;
            }
            if (j <= 1 && i != 0) {
                cout << "\033[34;47m|";
                cout << "\033[0m";
                continue;
            }
            if (j >= WIDTH && i != HEIGHT + 1) {
                cout << "\033[34;47m|";
                cout << "\033[0m";
                if (j == WIDTH + 1) outsider(i);
                continue;
            }
            bool drawn = false;
            auto shape = current.getShape();
            // Vẽ khối đang rơi
            for (int y = 0; y < shape.size(); ++y) {
                for (int x = 0; x < shape[0].size(); ++x) {
                    if (shape[y][x] &&
                        current.getY() + y == i &&
                        current.getX() + x == j) {
                        cout << colorBlock(current.getColor());
                        drawn = true;
                    }
                }
            }
            if (!drawn)
                cout << (gameBoard.getBoardXY(i, j) ? colorBlock(gameBoard.getBoardXY(i, j)) : " ");
        }
        cout << "\n";
    }
}

void Tetris::merge() {
    auto shape = current.getShape();
    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[0].size(); ++x) {
            if (shape[y][x]) {
                gameBoard.setBoardXY(current.getY() + y, current.getX() + x, current.getColor());
            }
        }
    }
    int ran = rand() % 11;
    score += ceil(ran / 10.0 * 5 + 1);
}

void Tetris::rotate() {
    auto shape = current.getShape();
    int m = shape.size();          
    int n = shape[0].size();       

    vector<vector<int>> rotated(n, vector<int>(m));  
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            rotated[j][m - 1 - i] = shape[i][j];
    
    vector<vector<int>> tmp(n / 2, vector<int>(2 * m));
    for (int i = 0; i < n / 2; i++) {
        for (int j = 0; j < 2 * m; j += 2) {
            tmp[i][j] = rotated[i * 2][j / 2];
            tmp[i][j + 1] = rotated[i * 2][j / 2]; 
        }
    }
    rotated = tmp;

    auto oldShape = shape;
    current.setShape(rotated);
    if (collides(0, 0)) 
        current.setShape(oldShape); // Quay không hợp lệ
}

void Tetris::clearLines() {
    int val = 0;
    for (int i = 0; i < HEIGHT + 2; ++i) {
        bool full = true;
        for (int j = 2; j < WIDTH; ++j)
            if (gameBoard.getBoardXY(i, j) == 0)
                full = false;
        if (full) {
            auto tmpBoard = gameBoard.getBoard();
            for (int k = i; k > 0; --k)
                gameBoard.RemoveRow(k);
            gameBoard.resetRow(0);
            val++;
            updateScoreSuccess = 5;
        }
    }
    score += val * val * val;
}