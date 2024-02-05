#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024
#define BLOCK_SIZE 16

int width = 128;
int height = 128;
int grid_size = 8;
BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), 128, 128, 1, 32, BI_RGB };
HWND window;
HDC WindowDC;

typedef struct {
    unsigned char b, g, r, a;
}pixel;

typedef struct {
    int x, y;
}vector2;

typedef struct portal{
    vector2 pos;
    struct portal* link;
}portal_t;

enum {
    EMPTY = 1, START, EXIT, BRICK, FRAGILE, BOUNCE, CRATE, PORTAL1, PORTAL2, PORTAL3, PORTAL4
};

pixel* vram;
char* grid;
vector2 player_pos;
vector2 player_movement;
bool ismoving;
bool justmoved;
int level_counter;
int table[128];
int teleport_counter;

pixel* texture_bricks;
pixel* texture_cracked_bricks;
pixel* texture_exit;
pixel* texture_bounce;
pixel* texture_crate;
pixel* texture_player;
pixel* texture_portal1;
pixel* texture_portal2;
pixel* texture_portal3;
pixel* texture_portal4;

int windowMessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
WNDCLASSA window_class = { .lpfnWndProc = windowMessageHandler,.lpszClassName = "class",.lpszMenuName = "class" };
unsigned int screen_width;
unsigned int screen_height;

portal_t portal1, portal2, portal3, portal4;

void open_level_file(char* filename)
{
    char* temp_buffer;
    int temp_buffer_size;
    char* buffer;
    int file_size;
    int file_width = 0;
    HANDLE file = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    file_size = GetFileSize(file, 0);
    temp_buffer_size = 2 * ((int)sqrt(file_size / 2) + 1);
    temp_buffer = malloc(temp_buffer_size);
    ReadFile(file, temp_buffer, temp_buffer_size, 0, 0);
    SetFilePointer(file, 0, 0, 0);
    printf("temp: %s\n", temp_buffer);
    for (int i = 0; i < temp_buffer_size && temp_buffer[i] != '\n'; i++)
    {
        if (table[temp_buffer[i]])
        {
            file_width++;
        }
    }
    printf("%i\n\n", file_width);
    free(temp_buffer);
    if (file_width != grid_size)
    {
        grid_size = file_width;
        width = BLOCK_SIZE * grid_size;
        height = BLOCK_SIZE * grid_size;
        bmi = (BITMAPINFO){ sizeof(BITMAPINFOHEADER), width, height, 1, 32, BI_RGB };
        free(vram);
        vram = malloc(width * height * sizeof(pixel));
    }
    buffer = malloc(file_size);
    ReadFile(file, buffer, file_size, 0, 0);
    printf("%s\n", buffer);
    for (int i = 0, j = 0; i < file_size; i++)
    {
        vector2 pos = { j % grid_size, grid_size - 1 - j / grid_size };
        int location = pos.x + pos.y * grid_size;
        if (!table[buffer[i]])
        {
            continue;
        }
        if (table[buffer[i]] == START)
        {
            player_pos = pos;
            grid[location] = EMPTY;
            j++;
            continue;
        }
        if (table[buffer[i]] == PORTAL1)
        {
            portal1.pos = pos;
            portal2.link = &portal1;
        }
        if (table[buffer[i]] == PORTAL2)
        {
            portal2.pos = pos;
            portal1.link = &portal2;
        }
        if (table[buffer[i]] == PORTAL3)
        {
            portal3.pos = pos;
            portal4.link = &portal3;
        }
        if (table[buffer[i]] == PORTAL4)
        {
            portal4.pos = pos;
            portal3.link = &portal4;
        }
        grid[location] = table[buffer[i]];
        j++;
    }
    CloseHandle(file);
    //free(buffer);
}

void reset_movement()
{
    player_movement = (vector2){ 0, 0 };
    ismoving = false;
    teleport_counter = 0;
}

void reset(bool add_level)
{
    reset_movement();
    level_counter += add_level;
    char level_name[20] = "levels/level";
    char temp_str1[5];
#pragma warning(disable : 4996)
    _itoa(level_counter, temp_str1, 10);
    char temp_str2[5] = ".txt";
    strncat(level_name, temp_str1, 5);
    strncat(level_name, temp_str2, 5);
    printf("%s\n", level_name);
    open_level_file(level_name);
}

int windowMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:

        break;
    case WM_RBUTTONDOWN:

        break;
    case WM_MOUSEWHEEL:;

        break;
    case WM_KEYDOWN:
        if (!ismoving)
        {
            switch (wParam)
            {
            case 'A':
                player_movement = (vector2){ -1, 0 };
                ismoving = true; justmoved = true;
                break;
            case 'W':
                player_movement = (vector2){ 0, 1 };
                ismoving = true; justmoved = true;
                break;
            case 'S':
                player_movement = (vector2){ 0, -1 };
                ismoving = true; justmoved = true;
                break;
            case 'D':
                player_movement = (vector2){ 1, 0 };
                ismoving = true; justmoved = true;
                break;
            case 'R':
                reset(false);
                break;
            case VK_SHIFT:
                reset(true);
                break;
            case VK_CONTROL:
                if (level_counter > 1)
                    level_counter--;
                reset(false);
                break;
            case VK_ESCAPE:
                ExitProcess(0);
            }
        }
        break;
    case WM_QUIT:
    case WM_CLOSE:
        ExitProcess(0);
    }
    return DefWindowProcA(window, msg, wParam, lParam);
}

void draw_texture(int x, int y, int size, pixel* texture)
{
    for (int y2 = 0; y2 < size; y2++)
    {
        for (int x2 = 0; x2 < size; x2++)
        {
            vram[(x + x2) + (y + y2) * width] = texture[x2 + y2 * (size + 1)];
        }
    }
}

void draw_lines()
{
    for (int i = 1; i <= grid_size; i++)
    {
        for (int j = 1; j <= width; j++)
        {
            vram[i * BLOCK_SIZE * width + j - 1 - width] = (pixel) { 255, 255, 255 };
            vram[i * BLOCK_SIZE + j * width - 1 - width] = (pixel) { 255, 255, 255 };
        }
    }
}

void init()
{
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biWidth = width;
    level_counter = 1;

    reset(false);
}

void render()
{
    for (int y = 0; y < grid_size; y++)
    {
        for (int x = 0; x < grid_size; x++)
        {
            switch (grid[x + y * grid_size])
            { 
            case BRICK:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_bricks);
                break;
            case FRAGILE:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_cracked_bricks);
                break;
            case BOUNCE:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_bounce);
                break;
            case CRATE:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_crate);
                break;
            case EXIT:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_exit);
                break;
            case PORTAL1:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_portal1);
                break;
            case PORTAL2:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_portal2);
                break; 
            case PORTAL3:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_portal3);
                break;
            case PORTAL4:
                draw_texture(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_portal4);
                break;
            }
        }
    }
    draw_texture(player_pos.x * BLOCK_SIZE, player_pos.y * BLOCK_SIZE, BLOCK_SIZE - 1, texture_player);
    StretchDIBits(WindowDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, width, height, vram, &bmi, 0, SRCCOPY);
    memset(vram, 0, width * height * sizeof(pixel));
    draw_lines();
}

bool check_bound(vector2 pos)
{
    bool left = pos.x == 0;
    bool right = pos.x == grid_size - 1;
    bool down = pos.y == 0;
    bool up = pos.y == grid_size - 1;
    return (left || right || up || down);
}

void physics()
{
    init();
    for (;;)
    {
        int current_pos = player_pos.x + player_pos.y * grid_size;
        int next_pos = player_pos.x + player_movement.x + (player_pos.y + player_movement.y) * grid_size;
        int next_next_pos = player_pos.x + player_movement.x * 2 + (player_pos.y + player_movement.y * 2) * grid_size;
        if (grid[current_pos] == EXIT)
        {
            reset(true);
        }
        switch (grid[current_pos])
        {
        case PORTAL1:
            player_pos.x = portal2.pos.x;
            player_pos.y = portal2.pos.y;
            teleport_counter++;
            break;
        case PORTAL2:
            player_pos.x = portal1.pos.x;
            player_pos.y = portal1.pos.y;
            teleport_counter++;
            break;
        case PORTAL3:
            player_pos.x = portal4.pos.x;
            player_pos.y = portal4.pos.y;
            teleport_counter++;
            break;
        case PORTAL4:
            player_pos.x = portal3.pos.x;
            player_pos.y = portal3.pos.y;
            teleport_counter++;
            break;
        }
        if (check_bound(player_pos) || teleport_counter >= 8)
        {
            reset(false);
            continue;
        }
        switch (grid[next_pos])
        {
        case FRAGILE:
            grid[next_pos] = EMPTY;
            reset_movement();
            break;
        case BOUNCE:
            grid[next_pos] = EMPTY;
            player_movement = (vector2){ -player_movement.x, -player_movement.y };
            break;
        case BRICK:
            reset_movement();
            break;
        case CRATE:
            if (justmoved && !check_bound((vector2) { player_pos.x + player_movement.x, player_pos.y + player_movement.y }) && grid[next_next_pos] == EMPTY)
            {
                grid[next_pos] = EMPTY;
                grid[next_next_pos] = CRATE;
                player_pos.x += player_movement.x;
                player_pos.y += player_movement.y;
            }
            reset_movement();
            break;
        }
        player_pos.x += player_movement.x;
        player_pos.y += player_movement.y;
        justmoved = false;
        render();
        Sleep(100);
    }
}

pixel* open_file(char* filename)
{
    HANDLE file = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    char buffer[14];
    ReadFile(file, buffer, 14, 0, 0);
    int file_size = buffer[2];
    file_size += buffer[3] << 8;
    int start_dest = buffer[10];
    start_dest += buffer[11] << 8;
    SetFilePointer(file, start_dest, 0, 0);
    char* temp = malloc(file_size);
    pixel* image = malloc((file_size - start_dest) * 4 / 3);
    ReadFile(file, temp, file_size, 0, 0);
    for (int i = 0; i < (file_size - start_dest) / 3; i++)
    {
        image[i].b = temp[i * 3 + 0];
        image[i].g = temp[i * 3 + 1];
        image[i].r = temp[i * 3 + 2];
    }
    CloseHandle(file);
    free(temp);
    return (image);
}

int main() {
    vram = malloc(height * width * sizeof(pixel));
    grid = malloc(grid_size * grid_size * sizeof(char));
    if (!vram || !grid)
    {
        printf("help the stack has died");
        return -1;
    }
    screen_width = GetSystemMetrics(SM_CXSCREEN);
    screen_height = GetSystemMetrics(SM_CYSCREEN);
    RegisterClassA(&window_class);
    window = CreateWindowExA(0, "class", "hello", WS_VISIBLE | WS_POPUP, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, window_class.hInstance, 0);
    WindowDC = GetDC(window);
    MSG message;

    texture_bricks = open_file("sprites/brick.bmp");
    texture_cracked_bricks = open_file("sprites/cracked_brick.bmp");
    texture_exit = open_file("sprites/exit.bmp");
    texture_bounce = open_file("sprites/bounce.bmp");
    texture_crate = open_file("sprites/crate.bmp");
    texture_portal1 = open_file("sprites/portal_blue.bmp");
    texture_portal2 = open_file("sprites/portal_green.bmp");
    texture_portal3 = open_file("sprites/portal_red.bmp");
    texture_portal4 = open_file("sprites/portal_purple.bmp");
    texture_player = open_file("sprites/player.bmp");

    memset(table, 0, 128);
    table['.'] = EMPTY;
    table['S'] = START;
    table['B'] = BRICK;
    table['E'] = EXIT;
    table['F'] = FRAGILE;
    table['b'] = BOUNCE;
    table['C'] = CRATE;
    table['P'] = PORTAL1;
    table['p'] = PORTAL2;
    table['Q'] = PORTAL3;
    table['q'] = PORTAL4;

#pragma comment(lib,"winmm")
    timeBeginPeriod(1);
    CreateThread(0, 0, physics, 0, 0, 0);

    while (GetMessageA(&message, window, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}