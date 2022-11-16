#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cstring>
using namespace std;

#define tick 500000 // 틱레이트 마이크로초 단위 1초에 10틱

bool fail;                                               // 실패 여부
int stage;                                               // stage 구분
int gatecount;                                           // gate 통과했는지 확인
int chk_grow, chk_posion, chk_gate, len_body;            // item 획득 횟수, gate 통과 횟수, body 길이
int target_grow, target_poison, target_gate, target_len; // 목표
int window_X, window_Y;
bool all_clear; // all 스테이지 clear
double game_timer;

vector<pair<int, int>> snake; // snake x, y좌표

vector<pair<int, int>> item; // item 좌표
vector<int> item_timer;      // item_timer
int item_Cnt;                // item 갯수

int dir; // 방향 U = 0, R = 1, D = 2, L = 3
enum
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};
enum Object
{
    FIELD,
    WALL,
    IMMUNE_WALL,
    SNAKE_HEAD,
    SNAKE_BODY,
    ITEM,
    POISON,
    GATE
};

vector<pair<int, int>> direction = { // UP, RIGHT, DOWN, LEFT
    {-1, 0},
    {0, 1},
    {1, 0},
    {0, -1}};
vector<pair<int, int>> door(2); // gate좌표 저장

int map[4][21][21] =
    {
        {
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        },
        {
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        },
        {
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        },
        {
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 1, 1, 2, 0, 0, 1, 0, 0, 2, 1, 1, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
            {1, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 1, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 1, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 1, 1, 2, 0, 0, 1, 0, 0, 2, 1, 1, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        }};

void init_set();    // 게임 시작시 함수 초기화
void inputKey();    // key 입력 받는 함수
void move();        // 좌표 이동 함수
void show_window(); // 화면 출력 함수
void makeItem();
void makeGate();      // gate를 만드는 함수
void timer();         // item timer 관리 함수
void setstage(int n); // stage 초기화 함수
void setgoal();
void check_target();

void init_set()
{
    initscr();                            // 터미널 초기화
    start_color();                        // 컬러모드 사용
    nodelay(stdscr, true);                // 키 입력 기다리지 않음
    noecho();                             // 입력받은 키 출력하지 않음
    curs_set(0);                          // 커서 숨기기
    cbreak();                             // 입력받은 키 바로 활용
    keypad(stdscr, TRUE);                 // 특수 키 사용 가능
    getmaxyx(stdscr, window_Y, window_X); // 창 크기 저장

    fail = false;
    all_clear = false;
    stage = 0;

    setstage(stage);

    // colors
    init_pair(WALL, COLOR_WHITE, COLOR_WHITE);
    init_pair(IMMUNE_WALL, COLOR_BLUE, COLOR_BLUE);
    init_pair(SNAKE_HEAD, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(SNAKE_BODY, COLOR_CYAN, COLOR_CYAN);
    init_pair(ITEM, COLOR_GREEN, COLOR_GREEN);
    init_pair(POISON, COLOR_RED, COLOR_RED);
    init_pair(GATE, COLOR_MAGENTA, COLOR_MAGENTA);
}

void inputKey()
{
    int key = getch();

    switch (key)
    {
    case KEY_UP:
        if (dir == DOWN)
            fail = true;
        else
            dir = UP;
        break;
    case KEY_DOWN:
        if (dir == UP)
            fail = true;
        else
            dir = DOWN;
        break;
    case KEY_LEFT:
        if (dir == RIGHT)
            fail = true;
        else
            dir = LEFT;
        break;
    case KEY_RIGHT:
        if (dir == LEFT)
            fail = true;
        else
            dir = RIGHT;
        break;
    }
}

void move()
{
    if (gatecount > 0)
    {
        if (gatecount == 1)
        {
            map[stage][door[0].first][door[0].second] = WALL;
            map[stage][door[1].first][door[1].second] = WALL;
            chk_gate += 1; //획득개수 체크
            makeGate();
        }
        gatecount--;
    }
    int last = snake.size();

    auto it = snake[last - 1]; // item 먹었을 때 사용하기 위한 좌표 저장

    map[stage][snake[last - 1].first][snake[last - 1].second] = FIELD; // 몸통 마지막 좌표 지우기
    for (int i = last - 1; i > 0; i--)                                 // 몸통 앞으로 한 칸 씩 복제
        snake[i] = snake[i - 1];

    snake[0].first += direction[dir].first;
    snake[0].second += direction[dir].second;

    if (map[stage][snake[0].first][snake[0].second] == GATE) // Gate 통과시
    {
        gatecount = last;

        int exitx = door[0].second;
        int exity = door[0].first;

        if (snake[0].first == door[0].first && snake[0].second == door[0].second)
        {
            exitx = door[1].second;
            exity = door[1].first;
        }

        int array[4] = {0, 1, 3, 2};
        for (int i = 0; i < 4; i++)
        {
            int y = exity + direction[(dir + array[i]) % 4].first;
            int x = exitx + direction[(dir + array[i]) % 4].second;
            if (y <= 20 && y >= 0)
                if (x <= 20 && x >= 0)
                    if (map[stage][y][x] != WALL && map[stage][y][x] != IMMUNE_WALL && map[stage][y][x] != GATE)
                    {
                        snake[0] = {y, x};
                        dir = (dir + array[i]) % 4;
                        break;
                    }
        }
    }

    int getItem = 0;                                                                                                      // 뱀 이동 후 아이템 생성 함수 호출하기 위한 변수
    if (map[stage][snake[0].first][snake[0].second] == SNAKE_BODY || map[stage][snake[0].first][snake[0].second] == WALL) // Game Rule #1 실패 조건
    {
        fail = true;
    }
    else if (map[stage][snake[0].first][snake[0].second] == ITEM) // growth item 획득시
    {
        snake.push_back({it.first, it.second}); // 몸통 뒤에 좌표 추가
        getItem = 1;
        chk_grow += 1; // item 개수 확인용

        if (len_body < snake.size())
            len_body = snake.size();
    }
    else if (map[stage][snake[0].first][snake[0].second] == POISON) // poison item 획득시
    {
        if (last == 3) // 길이가 3이면 실패
        {
            fail = true;
        }
        else
        {
            map[stage][snake[last - 1].first][snake[last - 1].second] = FIELD; // 몸통 마지막 좌표 지우기
            snake.pop_back();
            getItem = 1;
            chk_posion += 1; // item 개수 확인용
        }
    }

    map[stage][snake[0].first][snake[0].second] = SNAKE_HEAD; // 수정된 snake의 좌표 전달
    for (int i = 1; i < snake.size(); i++)
        map[stage][snake[i].first][snake[i].second] = SNAKE_BODY;

    if (getItem == 1)
    {
        auto it_item = item.begin();
        auto it_timer = item_timer.begin();
        for (; it_item != item.end(); it_timer++, it_item++)
        {
            if (map[stage][(*it_item).first][(*it_item).second] == SNAKE_HEAD)
            {
                item.erase(it_item);
                item_timer.erase(it_timer);
                item_Cnt--;
                break;
            }
        }
        makeItem();
    }
    check_target();
}

void show_window()
{
    clear();
    for (int j = 0; j < 21; j++)
    {
        printw("\n");
        for (int z = 0; z < 21; z++)
        {
            if (map[stage][j][z] == IMMUNE_WALL)
            {
                attron(COLOR_PAIR(IMMUNE_WALL));
                printw("IW");
                attroff(COLOR_PAIR(IMMUNE_WALL));
            }
            else if (map[stage][j][z] == WALL)
            {
                attron(COLOR_PAIR(WALL));
                printw("WW");
                attroff(COLOR_PAIR(WALL));
            }
            else if (map[stage][j][z] == FIELD)
            {
                printw("  ");
            }
            else if (map[stage][j][z] == SNAKE_HEAD)
            {
                attron(COLOR_PAIR(SNAKE_HEAD));
                printw("HH");
                attroff(COLOR_PAIR(SNAKE_HEAD));
            }
            else if (map[stage][j][z] == SNAKE_BODY)
            {
                attron(COLOR_PAIR(SNAKE_BODY));
                printw("BB");
                attroff(COLOR_PAIR(SNAKE_BODY));
            }
            else if (map[stage][j][z] == ITEM)
            {
                attron(COLOR_PAIR(ITEM));
                printw("II");
                attroff(COLOR_PAIR(ITEM));
            }
            else if (map[stage][j][z] == POISON)
            {
                attron(COLOR_PAIR(POISON));
                printw("PP");
                attroff(COLOR_PAIR(POISON));
            }
            else if (map[stage][j][z] == GATE)
            {
                attron(COLOR_PAIR(GATE));
                printw("GG");
                attroff(COLOR_PAIR(GATE));
            }
        }
    }
    mvprintw(1, 45, "Score Board");                        //점수표
    mvprintw(2, 45, "B: %d / %d", snake.size(), len_body); //뱀의 몸길이
    mvprintw(3, 45, "+: %d", chk_grow);                    // growth item 획득 개수
    mvprintw(4, 45, "-: %d", chk_posion);                  // poison item 획득 개수
    mvprintw(5, 45, "G: %d", chk_gate);                    // gate 통과 횟수
    mvprintw(6, 45, "T: %ds", (int)game_timer);
    mvprintw(9, 45, "Mission Board "); //미션표
    mvprintw(10, 45, "B: %d", target_len);
    if (len_body >= target_len)
        printw(" (V)");
    else
        printw(" ( )");

    mvprintw(11, 45, "+: %d", target_grow);
    if (chk_grow >= target_grow)
        printw(" (V)");
    else
        printw(" ( )");

    mvprintw(12, 45, "-: %d", target_poison);
    if (chk_posion >= target_poison)
        printw(" (V)");
    else
        printw(" ( )");

    mvprintw(13, 45, "G: %d", target_gate);
    if (chk_gate >= target_gate)
        printw(" (V)");
    else
        printw(" ( )");

    refresh();
}

void makeItem()
{
    while (item_Cnt < 3)
    {
        int itemy = rand() % 19 + 1;
        int itemx = rand() % 19 + 1;
        if (map[stage][itemy][itemx] != FIELD)
            continue;
        item.push_back({itemy, itemx});
        item_timer.push_back(20);
        if (rand() % 2 == 0)
        {
            map[stage][itemy][itemx] = ITEM;
            item_Cnt++;
        }
        else
        {
            map[stage][itemy][itemx] = POISON;
            item_Cnt++;
        }
    }
}

void makeGate()
{
    int gate_x;
    int gate_y;

    for (int i = 0; i < 2; i++)
    {
        while (1)
        {
            gate_x = (rand() % 21); //랜덤으로 gate1의 x,y좌표 설정
            gate_y = (rand() % 21);
            if (map[stage][gate_y][gate_x] == WALL)
            {
                map[stage][gate_y][gate_x] = GATE; // wall이면 gate로 변경
                door[i] = {gate_y, gate_x};
                break;
            }
        }
    }
}

void timer()
{
    auto it_time = item_timer.begin();
    for (; it_time != item_timer.end(); it_time++)
        (*it_time)--;

    it_time = item_timer.begin();
    auto it_item = item.begin();
    for (; it_time != item_timer.end();)
    {
        if (*it_time == 0)
        {
            map[stage][(*it_item).first][(*it_item).second] = FIELD;
            it_time = item_timer.erase(it_time);
            it_item = item.erase(it_item);
            item_Cnt--;
        }
        else
        {
            it_time++, it_item++;
        }
    }
    game_timer += 0.5;
    if (game_timer == 10)
        makeGate();

    makeItem();
    move();
}

void setstage(int n)
{
    clear();
    mvprintw(window_Y / 2, (window_X - 7) / 2, "Stage %d", stage + 1);
    refresh();
    usleep(2000000);

    chk_gate = 0;
    chk_grow = 0;
    chk_posion = 0;
    len_body = 3;
    game_timer = 0;

    item_Cnt = 0;
    gatecount = 0;

    vector<pair<int, int>>().swap(snake);

    item.clear();
    item_timer.clear();

    switch (stage)
    {
    case 0:
        snake.push_back({9, 9});
        snake.push_back({9, 10});
        snake.push_back({9, 11});
        dir = LEFT;
        break;
    case 1:
        snake.push_back({9, 10});
        snake.push_back({10, 10});
        snake.push_back({11, 10});
        dir = UP;
        break;
    case 2:
        snake.push_back({9, 9});
        snake.push_back({9, 10});
        snake.push_back({9, 11});
        dir = LEFT;
        break;
    case 3:
        snake.push_back({19, 9});
        snake.push_back({19, 10});
        snake.push_back({19, 11});
        dir = LEFT;
        break;
    }

    makeItem();
    setgoal();
}

void setgoal()
{
    target_len = (rand() % 4) + 5;
    target_grow = (rand() % 5) + 3;
    target_poison = (rand() % 3) + 1;
    target_gate = (rand() % 5) + 1;
}

void check_target()
{
    if (chk_gate >= target_gate && chk_grow >= target_grow && chk_posion >= target_poison && len_body >= target_len)
    {
        if (stage < 3)
        {
            clear(); // window 모든 곳을 빈칸 처리
            mvprintw(window_Y / 2, (window_X - 20) / 2, "Stage %d Clear!", stage + 1);
            mvprintw(window_Y / 2 + 1, (window_X - 25) / 2, "Go To The Next Stage!!");
            refresh();
            usleep(2000000);
            stage++;
            setstage(stage);
        }
        else
        {
            clear();
            mvprintw(window_Y / 2, (window_X - 22) / 2, "Clear All Stages");
            mvprintw(window_Y / 2 + 1, (window_X - 15) / 2, "Good Job!");
            refresh();
            usleep(2000000);
            fail = true;
            all_clear = true;
        }
    }
}

int main()
{
    srand(time(NULL));
    init_set();
    while (!fail)
    {
        inputKey();
        timer();
        show_window();
        usleep(tick);
    }
    if (all_clear == false)
    {
        clear();
        mvprintw(window_Y / 2, (window_X - 11) / 2, "You Fail!");
        refresh();
        usleep(2000000);
    }
    endwin();
    return 0;
}