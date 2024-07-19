#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include <array>
#include <thread>

#include "Server.h"
#include "Render.h"
#include "Console_Chess.h"

#define White "\033[0m"
#define Black "\033[38;2;68;68;68m"
#define Base "\033[37m"

#define PORT 4848

using namespace std;

//secret commands:
//outer wilds: play the music babbyyyy
//

void InitGrid();
void RenderGrid();
bool Game(string input, bool outside);
bool Move(int figure, vector<int> cords, int take, bool outside);
bool Check_moves(int figure, vector<int> cords, int take);
bool Bishop(vector<int> cords);
bool Rook(vector<int> cords);
void CheckCheck();
bool isInBounds(int x, int y);
void Bishop_Check(int x, int y);
void Rook_Check(int x, int y);
void AddChat(string messageT);
void AddHistory(string moveT);

string input;
string moveses = "";
int turn;
bool gaming = true;
bool is_x = false;
int step_x = 0;
int step_y = 0;
int max_dis = 0;
int alt_y;
int side = 0;
int y_alt = 0;
int x_alt = 0;

int gridW = 40;
int chatW;
int consoleWidth;
int pos;
int MTS = 28; // max text size

string epty = "     ";
array<string, 18> chat = { "" };
array<string, 18> history = { "" };
int columnC = 17;

CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

vector<int> cords;

class Piece {
public:
    int id;
    char ch;

    Piece(int set_id, char character) {
        id = set_id;
        ch = character;
    }
};

vector<Piece> pieces = {
    Piece(0, ' '),
    Piece(1, 'P'),
    Piece(2, 'N'),
    Piece(3, 'B'),
    Piece(4, 'R'),
    Piece(5, 'Q'),
    Piece(6, 'K')
};

unordered_map<char, int> moves = {{'a',0}, {'b',1}, {'c',2}, {'d',3},
                                    {'e',4}, {'f',5}, {'g',6}, {'h',7},
                                    {'1',7}, {'2',6}, {'3',5}, {'4',4},
                                    {'5',3}, {'6',2}, {'7',1}, {'8',0}};
unordered_map<char, int> moves_b = {{'a',7}, {'b',6}, {'c',5}, {'d',4},
                                    {'e',3}, {'f',2}, {'g',1}, {'h',0},
                                    {'1',0}, {'2',1}, {'3',2}, {'4',3},
                                    {'5',4}, {'6',5}, {'7',6}, {'8',7}};
unordered_map<char, int> piece_names = { {'p',1}, {'n',2}, {'b',3}, {'r',4},{'q',5}, {'k',6}};

vector<vector<vector<int>>> grid(8, vector<vector<int>>(8, vector<int>(2, 0)));
vector<vector<int>> check_grid(8, vector<int>(8, 0));

int knight_moves[8][2] = { {1, 2}, {1, -2}, {-1, 2}, {-1, -2},{2, 1}, {2, -1}, {-2, 1}, {-2, -1} };
int king_moves[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

int main()
{
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    consoleWidth = csbi.dwSize.X;
    SetFontSize(32);
    Fullscreen();
    NoResize();
    initalize_server();
    InitGrid();

    thread sendMess(message);
    thread reciveMess(getmessage);

    sendMess.join();
    reciveMess.join();

    closesocket(clientSock);

    WSACleanup();
}

void InitGrid() {
    for (size_t y= 0; y < 8; y++)
    {
        for (size_t x = 0; x < 8; x++)
        {
            if (y == 1 || y == 6) grid[x][y][0] = pieces[1].id;
            else if (y == 0 || y == 7) {
                if (x == 0 || x == 7) grid[x][y][0] = pieces[4].id;
                else if (x == 1 || x == 6) grid[x][y][0] = pieces[2].id;
                else if (x == 2 || x == 5) grid[x][y][0] = pieces[3].id;
                else if (x == 3) grid[x][y][0] = pieces[5].id;
                else if (x == 4) grid[x][y][0] = pieces[6].id;
            }

            if (side == 0) {
                if (y < 2) grid[x][y][1] = 2; // 2 = black
                else if (y > 5) grid[x][y][1] = 1; // 1 = white
            }
            else {
                if (y < 2) grid[x][y][1] = 1;
                else if (y > 5) grid[x][y][1] = 2; 
            }
        }
    }
}

void RenderGrid() {
    system("CLS");

    cout << endl << endl;

    //chat
    string text = "Chat";
    int textL = text.length();
    chatW = (consoleWidth - gridW) / 2;
    pos = (chatW - textL) / 2;
    for (int i = 0; i < pos; ++i) cout << " ";
    cout << text;
    for (int i = 0; i < pos; ++i) cout << " ";

    //opponent
    text = "Opponent: Someone";
    textL = text.length();
    pos = (gridW - textL) / 2;
    for (int i = 0; i < pos; ++i) cout << " ";
    cout << text;
    for (int i = 0; i < pos; ++i) cout << " ";

    //history
    text = "History";
    textL = text.length();
    pos = (chatW - textL) / 2; 
    for (int i = 0; i < pos; ++i) cout << " ";
    cout << text << endl;

    chatW -= 10;

    //border start
    cout << epty;
    for (int i = 0; i < chatW; ++i) cout << "_";
    cout << epty;
    for (int i = 0; i < gridW; ++i) cout << " ";
    cout << epty;
    for (int i = 0; i < chatW; ++i) cout << "_";
    cout << endl;

    chatW -= 2;

    pos = (consoleWidth - gridW) / 2;
    for (int y = -2; y < 16; y++)
    {
        text = chat[columnC];
        textL = text.length();
        pos = chatW - textL;
        cout << epty << "|" << text;
        for (size_t i = 0; i < pos; i++) cout << " ";
        cout << "|" << epty;
        if (y % 2 == 0 && y >= 0) {
            alt_y = y / 2;
            for (size_t x = 0; x < 8; x++) {
                cout << Base << " [";
                if (grid[x][alt_y][1] == 2) cout << Black;
                else cout << White;
                cout << pieces[grid[x][alt_y][0]].ch;
                cout << Base << "] ";
            }
        }
        else for (size_t i = 0; i < gridW; i++) cout << " ";
        text = history[columnC];
        textL = text.length();
        pos = (chatW - textL) / 2;
        cout << epty << "|";
        for (size_t i = 0; i < pos; i++) cout << " ";
        cout << text;
        if ((chatW - textL) % 2 == 1) pos++;
        for (size_t i = 0; i < pos; i++) cout << " ";
        if(y != 15)columnC--;
        cout << "|" << endl;
    }
    columnC = 17;

    //border end
    chatW += 2;
    cout << epty;
    for (int i = 0; i < chatW; ++i) cout << "^";
    cout << epty;
    for (int i = 0; i < gridW; ++i) cout << " ";
    cout << epty;
    for (int i = 0; i < chatW; ++i) cout << "^";
    cout << endl;
}

bool Game(string input, bool outside) {
    if (outside) input.pop_back();
    if (input == "exit") ExitProcess(0);
    if (input[0] == '/') {
        input = input.substr(1);
        if (input.size() <= 28) {
            AddChat(input);
            RenderGrid();
            if (!outside) return true;
        }
    }
    else
    {
        if ((turn == 0 && !outside) || (turn == 1 && outside)) {
            try {
                input.erase(remove(input.begin(), input.end(), ' '), input.end());
                if (input.size() < 5 || input.size() > 6) throw 1;
                int big_index = 3;
                int figure = piece_names[input[0]];
                if (input.find("x") != string::npos) big_index = 4;
                if (side == 0) cords = { moves[input[1]], moves[input[2]], moves[input[big_index]], moves[input[big_index + 1]] };
                else cords = { moves_b[input[1]], moves_b[input[2]], moves_b[input[big_index]], moves_b[input[big_index + 1]] };
                bool correct = Move(figure, cords, big_index, outside);
                if (turn == 0) {
                    turn = 1;
                    moveses.append(input);
                }
                else turn = 0;
                if ((side == 0 && turn == 0) || (side == 1 && turn == 1)) {
                    moveses.append(" ; " + input);
                    AddHistory(moveses);
                    moveses = "";
                }
                if (correct) RenderGrid();
                if (correct && !outside) {
                    return true;
                }
            }
            catch (int e) {
                RenderGrid();
                return false;
            }
        }
    }
    RenderGrid();
    return false;
}

bool Move(int figure, vector<int> cords, int take, bool outside) {
    if (grid[cords[0]][cords[1]][0] != figure) return false;
    if (outside || Check_moves(figure, cords, take)) {
        grid[cords[2]][cords[3]][0] = grid[cords[0]][cords[1]][0];
        grid[cords[2]][cords[3]][1] = grid[cords[0]][cords[1]][1];
        grid[cords[0]][cords[1]][0] = 0;
        grid[cords[0]][cords[1]][1] = 0;
        CheckCheck();
        return true;
    }
    return false;
}

bool Check_moves(int figure, vector<int> cords, int take) {
    if ((cords[2] > grid.size() || cords[2] < 0) || (cords[3] > grid[0].size() || cords[3] < 0)) return false;
    if (take == 4 && grid[cords[2]][cords[3]][0] == 0) return false;
    if (take == 3 && grid[cords[2]][cords[3]][0] != 0) return false;
    if (grid[cords[2]][cords[3]][1] == grid[cords[0]][cords[1]][1]) return false;
    switch (figure) {
        case 1:
            
            if (cords[3] < cords[1] - 2 || cords[3] > cords[1]) return false;
            if (take == 3 && (grid[cords[2]][cords[3]][0] != 0 || cords[0] != cords[2])) return false;
            if (take == 4 && ((cords[0] + 1 != cords[2] && cords[0] - 1 != cords[2]) || cords[3] != cords[1] - 1 || grid[cords[2]][cords[3]][0] == 0)) return false;
            if (cords[1] != 6 && cords[3] < cords[1] - 1) return false;
            break;
        case 2:
            if ((abs(cords[0] - cords[2]) != 2 || abs(cords[1] - cords[3]) != 1) && (abs(cords[0] - cords[2]) != 1 || abs(cords[1] - cords[3]) != 2)) return false;
            break;
        case 3:
            return Bishop(cords);
            break;
        case 4:
            return Rook(cords);
            break;
        case 5:
            if ((abs(cords[0] - cords[2]) != abs(cords[1] - cords[3])) && cords[0] != cords[2] && cords[1] != cords[3]) return false;
            if (abs(cords[0] - cords[2]) != abs(cords[1] - cords[3])) return Rook(cords);
            else return Bishop(cords);
            break;
        case 6:
            if (abs(cords[0] - cords[2]) > 1 || abs(cords[1] - cords[3]) > 1) return false;
            if (check_grid[cords[2]][cords[3]] == 1) return false;
            break;
    }
    return true;
}

bool Bishop(vector<int> cords) {
    if (abs(cords[0] - cords[2]) != abs(cords[1] - cords[3])) return false;
    step_x = (cords[2] > cords[0]) ? 1 : -1;
    step_y = (cords[3] > cords[1]) ? 1 : -1;
    max_dis = abs(cords[0] - cords[2]);
    for (int dist = 1; dist < max_dis; dist++) if (grid[cords[0] + (step_x * dist)][cords[1] + (step_y * dist)][0] != 0) return false;
}

bool Rook(vector<int> cords) {
    if (cords[0] != cords[2] && cords[1] != cords[3]) return false;
    is_x = (cords[0] == cords[2]) ? true : false;
    max_dis = (is_x) ? abs(cords[1] - cords[3]) : abs(cords[0] - cords[2]);
    step_x = (is_x) ? (cords[3] > cords[1]) ? 1 : -1 : (cords[2] > cords[0]) ? 1 : -1;
    for (int dist = 1; dist < max_dis; dist++) if ((is_x ? grid[cords[0]][cords[1] + (step_x * dist)][0] : grid[cords[0] + (step_x * dist)][cords[1]][0]) != 0) return false;

}

void CheckCheck() {
    for (size_t y = 0; y < 8; y++)
    {
        for (size_t x = 0; x < 8; x++) {
            if (grid[x][y][1] != 0 && grid[x][y][1] != side+1) {
                switch (grid[x][y][0]) {
                    case 1:
                        if (y < 7 && y > 0) {
                            if (x < 7) check_grid[x + 1][y + 1] = 1;
                            if (x > 0) check_grid[x - 1][y + 1] = 1;
                        }
                        break;
                    case 2:
                        for (size_t i = 0; i < 8; i++) {
                            int x_n = x + knight_moves[i][0];
                            int y_n = y + knight_moves[i][1];
                            if (isInBounds(x_n, y_n)) check_grid[x_n][y_n] = 1;
                        }
                        break;
                    case 3:
                        Bishop_Check(x, y);
                        break;
                    case 4:
                        Rook_Check(x, y);
                        break;
                    case 5:
                        Bishop_Check(x, y);
                        Rook_Check(x, y);
                        break;
                    case 6:
                        for (size_t i = 0; i < 8; i++) {
                            int x_n = x + king_moves[i][0];
                            int y_n = y + king_moves[i][1];
                            if (isInBounds(x_n, y_n)) check_grid[x_n][y_n] = 1;
                        }
                        break;
                }
            }
        }
    }
}

bool isInBounds(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

void Bishop_Check(int x, int y) {
    for (size_t i = 1; i < 8; i++) {
        bool blocked[4] = { false, false, false, false };
        if (isInBounds(x + i, y + i) && !blocked[0]) (grid[x + i][y + i][0] != 0) ? blocked[0] = true : check_grid[x + i][y + i] = 1;
        if (isInBounds(x + i, y - i) && !blocked[1]) (grid[x + i][y - i][0] != 0) ? blocked[1] = true : check_grid[x + i][y - i] = 1;
        if (isInBounds(x - i, y + i) && !blocked[2]) (grid[x - i][y + i][0] != 0) ? blocked[2] = true : check_grid[x - i][y + i] = 1;
        if (isInBounds(x - i, y - i) && !blocked[3]) (grid[x - i][y - i][0] != 0) ? blocked[3] = true : check_grid[x - i][y - i] = 1;
    }
}

void Rook_Check(int x, int y) {
    for (size_t i = 1; i < 8; i++) {
        bool blocked[4] = { false, false, false, false };
        if (isInBounds(x, y + i) && !blocked[0]) (grid[x][y + i][0] != 0) ? blocked[0] = true : check_grid[x][y + i] = 1;
        if (isInBounds(x, y - i) && !blocked[1]) (grid[x][y - i][0] != 0) ? blocked[1] = true : check_grid[x][y - i] = 1;
        if (isInBounds(x + i, y) && !blocked[2]) (grid[x + i][y][0] != 0) ? blocked[2] = true : check_grid[x + i][y] = 1;
        if (isInBounds(x - i, y) && !blocked[3]) (grid[x - i][y][0] != 0) ? blocked[3] = true : check_grid[x - i][y] = 1;
    }
}

void AddChat(string messageT) {
    for (size_t i = 17; i > 0; i--) chat[i] = chat[i-1];
    chat[0] = messageT;
}

void AddHistory(string moveT) {
    for (size_t i = 17; i > 0; i--) history[i] = history[i - 1];
    history[0] = moveT;
}