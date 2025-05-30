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

int getch_compatible() {
#ifdef _WIN32
    return _getch();
#elif defined(__linux__)
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

bool kbhit_compatible() {
#ifdef _WIN32
    return _kbhit();
#elif defined(__linux__)
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
#endif
}

void Tetris::clearInputBuffer() {
    while (kbhit_compatible()) getch_compatible(); 
}

void Tetris::handlePause() {
    while (paused) {
        if (kbhit_compatible()) {
            int c = getch_compatible();
            if (c == 'o' || c == 'O') SaveGame();
            if (c == 'r' || c == 'R') {
                paused = 0;
                return;
            }
        }
    }
}

string Tetris::movePtr(int val) {
    if (val == glb) return " -> ";
    return "   ";
}

void Tetris::InsertName() {
    int cnt = 0;
    do {
        if (cnt++ > 0) cout << warning << "\n";
        cout << "Please insert your name: ";
        getline(cin, name);
        system("cls");
    } while (name.size() < 1 || name.size() > 10);
    tmpName = name;
    name += string(WIDTH + 2 - name.size(), ' ');
    nameIter = name.size();
}

int Tetris::handleEnter() {
    if (kbhit_compatible()) {
        int c = getch_compatible();
        if (c == 13) return 1;
        else if (c == 'S' || c == 's') return 2;
        else if (c == 'W' || c == 'w') return 3;
        else if (c == 224 || c == 0) {
            int v = getch_compatible();
            if (v == 72) return 3; 
            else if (v == 80) return 2;
        }
    }
    return -1;
}

int visibleLength(const string& str) {
    string cleaned = std::regex_replace(str, std::regex("\033\\[[0-9;]*m"), "");
    
    int length = 0;
    for (char ch : cleaned) {
        if ((unsigned char)ch >= 0xC0) length += 2; 
        else length++;
    }
    return length;
}

void printInFrame(const string& content, int width = 57) {
    string mhm = "\033[48;5;245m--\033[0m";
    int visible = visibleLength(content);
    int padding = max(0, width - visible);

    cout << mhm << content << string(padding, ' ') << mhm << endl;
}


void Tetris::Help() {
    string press = "\033[0m  Press \033[31m";
    string endd = "\033[0m";

    printInFrame(press + "a, A, " + char(17) + "\033[0m to move to \033[1;35mleft\033[0m" + endd);
    printInFrame(press + "d, D, " + char(16) + "\033[0m to move to \033[34mright\033[0m" + endd);
    printInFrame(press + "s, S, " + char(31) + "\033[0m to move to \033[33mdown faster\033[0m" + endd);
    printInFrame(press + "w, W, " + char(30) + "\033[0m to \033[32mrotate\033[0m" + endd);
    printInFrame(press + "p, P, " + "\033[0m  to \033[36mpause\033[0m" + endd);
    printInFrame(press + "r, R, " + "\033[0m  to \033[30mresume\033[0m" + endd);
    printInFrame(press + "o, O, " + "\033[0m  to \033[35msave game\033[0m" + endd);
    printInFrame(press + "SPACE " + "\033[0m  to \033[96mdrop instantly\033[0m" + endd);
    printInFrame("");
    printInFrame("\033[35m                 PRESS ANY KEY TO GO BACK\033[0m");

    cout << "\033[48;5;245m-------------------------------------------------------------\033[0m\n";
    while (kbhit_compatible() == false) {}
}


int Tetris::Option() {
    if (glb == 0) {
        if (sizeTable != maxSizeTable) sizeTable = maxSizeTable;
        else sizeTable = 3;
    } else if (sizeTable == maxSizeTable && glb <= 3) {
        if (glb == 1) speed = 400;
        else if(glb == 2) speed = 300;
        else if (glb == 3) speed = 150;
        return -1; // Mã break
    } else if (glb == (5 - maxSizeTable + sizeTable)) Help();
    else if (glb == 4 - maxSizeTable + sizeTable) {
        ifstream fin("data.txt");
        string line;
        bool ok = false;
        while(getline(fin, line)) {
            if (line == "current=1") {
                ok = true;
                for (int i = 0; i < HEIGHT + 2; i++) {
                    getline(fin, line);
                    int k = 0;
                    for (int j = 0; j < WIDTH + 2; j++) {
                        string num = "";
                        while(k < line.size() && '0' <= line[k] && line[k] <= '9') num += line[k++];
                        gameBoard.setBoardXY(i, j, stoi(num));
                        k++; //Skip space
                    }
                }
                getline(fin, line);
                name = line;
                tmpName = line;
                getline(fin, line);
                speed = stoi(line);
                getline(fin, line);
                score = stoi(line);
                getline(fin, line);
                int a, b, c, d;
                sscanf(line.c_str(), "%d %d %d %d", &a, &b, &c, &d);
                setTetromino(a,b,c,d);
            }
        }
        fin.close();
        if (ok == true) return 2; // read successfully
        else return -2;
    }
    return 0;
}

void Tetris::LevelAndContinue() {
    string mhm = "\033[48;5;245m-------------------------------------------------------------\033[0m\n";
    while (1) {
        cout << "\033[36m" << char(30) << " to go up using W, w or " << char(30) << " arrow" << "\033[0m" << endl;
        cout << "\033[36m" << char(31) << " to go down using S, s or " << char(31) << " arrow" << "\033[0m" << endl;
        cout << "\033[33mEnter to choose\033[0m\n" << mhm;
        printInFrame(movePtr(0) + "\033[35m Choose your level\033[0m");

        if (sizeTable == maxSizeTable) {
            printInFrame(movePtr(1) + "\033[32m 1. Easy\033[0m");
            printInFrame(movePtr(2) + "\033[33m 2. Medium\033[0m");
            printInFrame(movePtr(3) + "\033[31m 3. Hard\033[0m");
        }

        printInFrame(movePtr(4 - (maxSizeTable - sizeTable)) + "\033[34m Continue last playing\033[0m");
        printInFrame(movePtr(5 - (maxSizeTable - sizeTable)) + "\033[36m Help - How to play?!!?!?!?\033[0m");
        cout << mhm;

        int val = handleEnter();
        if (val == 1) {
            int opt = Option();
            if (opt == -1 || opt == 2) {
                system("cls");
                break;
            } else if (opt == -2) {
                printInFrame("");
                printInFrame("\033[31m                     No data available\033[0m");
                printInFrame("");
                printInFrame("\033[35m                 PRESS ANY KEY TO GO BACK\033[0m");
                cout << mhm;
                while (kbhit_compatible() == false) {}
            }
        }
        else if (val == 2) glb = (glb + 1) % sizeTable;
        else if (val == 3) glb = (glb - 1 + sizeTable) % sizeTable;

        clearInputBuffer();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        system("cls");
    }
}

void Tetris::spawnTetromino() {
    int id = rand() % 7;
    current.setShape(tetrominoShapes[id]);
    current.setID(id);
    current.setX(WIDTH / 2 - 2);
    current.setY(0);
    current.setColor(rand() % 7 + 1);
}

void Tetris::setTetromino(int fid, int fcolor, int nid, int ncolor) {
    current.setShape(tetrominoShapes[fid]);
    current.setID(fid);
    current.setColor(fcolor);
    current.setX(WIDTH / 2 - 2);
    current.setY(0);
    nextT.setShape(tetrominoShapes[nid]);
    nextT.setID(nid);
    nextT.setColor(ncolor);
    nextT.setX(WIDTH / 2 - 2);
    nextT.setY(0);
}

void Tetris::spawnNextTetromino() {
    int id = rand() % 7;
    nextT.setShape(tetrominoShapes[id]);
    nextT.setID(id);
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
    name = "";
    LevelAndContinue();
    if (name == "") InsertName();
    srand(time(0));
    if (current.getID() == -1) spawnTetromino();
    if (nextT.getID() == -1) spawnNextTetromino();
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
                system("cls");
                GameOver();
                break;
            }
        }
        clearInputBuffer();

        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
}

void Tetris::SaveGame()
{
    ofstream fout("data.txt"); 
    fout << "current=1\n";
    for (int i = 0; i < HEIGHT + 2; i++)
    {
        for (int j = 0; j < WIDTH + 2; j++)
        {
            fout << gameBoard.getBoardXY(i, j);
            if (j != WIDTH + 1)
                fout << " "; 
        }
        fout << "\n";
    }
    fout << name << "\n";
    fout << speed << "\n";
    fout << score << "\n";
    fout << current.getID() << " " << current.getColor() << " " << nextT.getID() << " " << nextT.getColor() << "\n";
    fout.close();
}

void Tetris::handleInput() {
    static auto lastInput = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastInput).count() > 100) {
        if (kbhit_compatible()) {
            int c = getch_compatible();
            int x = current.getX();
            int y = current.getY();

            if (c == 224 || c == 0) {
                int special = getch_compatible();
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
                    case 'o': case 'O': SaveGame(); break;
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
    if (i == 20) cout << press << "o, O, "
                    << "\033[0m  to \033[35msave game" << endd;

    if (i == 21) cout << press << "SPACE " 
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
                if (j == WIDTH + 1) outsider(i);
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

void Tetris::GameOver() {
    int val = 0;
    int t = 0;
    string arr[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", "\033[37m"};
    string tmp = "                               " + tmpName + " [finished with a score of] " + to_string(score);
    int iter = 0;
    while(1) {
        cout << arr[val];
        if (t == 0) std::cout << R"(
  /$$$$$$   /$$$$$$  /$$      /$$ /$$$$$$$$        /$$$$$$  /$$    /$$ /$$$$$$$$ /$$$$$$$
 /$$__  $$ /$$__  $$| $$$    /$$$| $$_____/       /$$__  $$| $$   | $$| $$_____/| $$__  $$
| $$  \__/| $$  \ $$| $$$$  /$$$$| $$            | $$  \ $$| $$   | $$| $$      | $$  \ $$
| $$ /$$$$| $$$$$$$$| $$ $$/$$ $$| $$$$$         | $$  | $$|  $$ / $$/| $$$$$   | $$$$$$$/
| $$|_  $$| $$__  $$| $$  $$$| $$| $$__/         | $$  | $$ \  $$ $$/ | $$__/   | $$__  $$
| $$  \ $$| $$  | $$| $$\  $ | $$| $$            | $$  | $$  \  $$$/  | $$      | $$  \ $$
|  $$$$$$/| $$  | $$| $$ \/  | $$| $$$$$$$$      |  $$$$$$/   \  $/   | $$$$$$$$| $$  | $$
 \______/ |__/  |__/|__/     |__/|________/       \______/     \_/    |________/|__/  |__/
    )" << "\033[30m" <<std::endl;

        if (t == 1) std::cout << R"(
  ______    ______   __       __  ________         ______   __     __  ________  _______  
 /      \  /      \ |  \     /  \|        \       /      \ |  \   |  \|        \|       \
|  $$$$$$\|  $$$$$$\| $$\   /  $$| $$$$$$$$      |  $$$$$$\| $$   | $$| $$$$$$$$| $$$$$$$\
| $$ __\$$| $$__| $$| $$$\ /  $$$| $$__          | $$  | $$| $$   | $$| $$__    | $$__| $$
| $$|    \| $$    $$| $$$$\  $$$$| $$  \         | $$  | $$ \$$\ /  $$| $$  \   | $$    $$
| $$ \$$$$| $$$$$$$$| $$\$$ $$ $$| $$$$$         | $$  | $$  \$$\  $$ | $$$$$   | $$$$$$$\
| $$__| $$| $$  | $$| $$ \$$$| $$| $$_____       | $$__/ $$   \$$ $$  | $$_____ | $$  | $$
 \$$    $$| $$  | $$| $$  \$ | $$| $$     \       \$$    $$    \$$$   | $$     \| $$  | $$
  \$$$$$$  \$$   \$$ \$$      \$$ \$$$$$$$$        \$$$$$$      \$     \$$$$$$$$ \$$   \$$
    )" << "\033[30m" <<std::endl;

        if (t == 2) std::cout << R"(
  ______    ______   __       __  ________         ______   __     __  ________  _______  
 /      \  /      \ /  \     /  |/        |       /      \ /  |   /  |/        |/       \
/$$$$$$  |/$$$$$$  |$$  \   /$$ |$$$$$$$$/       /$$$$$$  |$$ |   $$ |$$$$$$$$/ $$$$$$$  |
$$ | _$$/ $$ |__$$ |$$$  \ /$$$ |$$ |__          $$ |  $$ |$$ |   $$ |$$ |__    $$ |__$$ |
$$ |/    |$$    $$ |$$$$  /$$$$ |$$    |         $$ |  $$ |$$  \ /$$/ $$    |   $$    $$<
$$ |$$$$ |$$$$$$$$ |$$ $$ $$/$$ |$$$$$/          $$ |  $$ | $$  /$$/  $$$$$/    $$$$$$$  |
$$ \__$$ |$$ |  $$ |$$ |$$$/ $$ |$$ |_____       $$ \__$$ |  $$ $$/   $$ |_____ $$ |  $$ |
$$    $$/ $$ |  $$ |$$ | $/  $$ |$$       |      $$    $$/    $$$/    $$       |$$ |  $$ |
 $$$$$$/  $$/   $$/ $$/      $$/ $$$$$$$$/        $$$$$$/      $/     $$$$$$$$/ $$/   $$/
    )" << "\033[30m" <<std::endl;

        if (t == 3) std::cout << R"(
 $$$$$$\   $$$$$$\  $$\      $$\ $$$$$$$$\        $$$$$$\  $$\    $$\ $$$$$$$$\ $$$$$$$\
$$  __$$\ $$  __$$\ $$$\    $$$ |$$  _____|      $$  __$$\ $$ |   $$ |$$  _____|$$  __$$\
$$ /  \__|$$ /  $$ |$$$$\  $$$$ |$$ |            $$ /  $$ |$$ |   $$ |$$ |      $$ |  $$ |
$$ |$$$$\ $$$$$$$$ |$$\$$\$$ $$ |$$$$$\          $$ |  $$ |\$$\  $$  |$$$$$\    $$$$$$$  |
$$ |\_$$ |$$  __$$ |$$ \$$$  $$ |$$  __|         $$ |  $$ | \$$\$$  / $$  __|   $$  __$$<
$$ |  $$ |$$ |  $$ |$$ |\$  /$$ |$$ |            $$ |  $$ |  \$$$  /  $$ |      $$ |  $$ |
\$$$$$$  |$$ |  $$ |$$ | \_/ $$ |$$$$$$$$\        $$$$$$  |   \$  /   $$$$$$$$\ $$ |  $$ |
 \______/ \__|  \__|\__|     \__|\________|       \______/     \_/    \________|\__|  \__|
    )" << "\033[30m" <<std::endl;
        cout << endl << "           ";
        cout << "\033[1;35m" + tmp.substr(iter) << "\033[0m";
        cout << "\033[1;35m" << tmp.substr(0, iter) << "\033[0m\n";
        iter = (iter - 1 + tmp.size()) % tmp.size();
        val = (val + 1) % 7;
        t = (t + 1) % 4;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        system("cls");
    }
}