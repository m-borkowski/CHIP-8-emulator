#include "chip8.h"
#include "chip8.cpp"
#include <stdio.h>
#include <GL/glut.h>

int scaler = 10;
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

int display_width = SCREEN_WIDTH * scaler;
int display_height = SCREEN_HEIGHT * scaler;

emulator::chip8 Chip8;

void display();
void keyUp(unsigned char key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void updateQuads(const emulator::chip8 &c8);
void drawPixel(int x, int y);

int main(int argc, char **argv)
{
    if (!Chip8.load(".//data//pong.c8"))
        return 1;

    int my_argc = 0;
    glutInit(&my_argc, NULL);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(display_width, display_height);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("CHIP-8");

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMainLoop();
    return 0;
}

void display()
{
    Chip8.emulateCycle();

    if (Chip8.drawFlag)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        updateQuads(Chip8);

        glutSwapBuffers();
        Chip8.drawFlag = false;
    }
}

void keyDown(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);

    if (key == '1')
        Chip8.key[0x0] = 1;
    else if (key == '2')
        Chip8.key[0x1] = 1;
    else if (key == '3')
        Chip8.key[0x2] = 1;
    else if (key == '4')
        Chip8.key[0x3] = 1;
    else if (key == 'q')
        Chip8.key[0x4] = 1;
    else if (key == 'w')
        Chip8.key[0x5] = 1;
    else if (key == 'e')
        Chip8.key[0x6] = 1;
    else if (key == 'r')
        Chip8.key[0x7] = 1;
    else if (key == 'a')
        Chip8.key[0x8] = 1;
    else if (key == 's')
        Chip8.key[0x9] = 1;
    else if (key == 'd')
        Chip8.key[0xA] = 1;
    else if (key == 'f')
        Chip8.key[0xB] = 1;
    else if (key == 'z')
        Chip8.key[0xC] = 1;
    else if (key == 'x')
        Chip8.key[0xD] = 1;
    else if (key == 'c')
        Chip8.key[0xE] = 1;
    else if (key == 'v')
        Chip8.key[0xF] = 1;
}

void keyUp(unsigned char key, int x, int y)
{
    if (key == '1')
        Chip8.key[0x0] = 0;
    else if (key == '2')
        Chip8.key[0x1] = 0;
    else if (key == '3')
        Chip8.key[0x2] = 0;
    else if (key == '4')
        Chip8.key[0x3] = 0;
    else if (key == 'q')
        Chip8.key[0x4] = 0;
    else if (key == 'w')
        Chip8.key[0x5] = 0;
    else if (key == 'e')
        Chip8.key[0x6] = 0;
    else if (key == 'r')
        Chip8.key[0x7] = 0;
    else if (key == 'a')
        Chip8.key[0x8] = 0;
    else if (key == 's')
        Chip8.key[0x9] = 0;
    else if (key == 'd')
        Chip8.key[0xA] = 0;
    else if (key == 'f')
        Chip8.key[0xB] = 0;
    else if (key == 'z')
        Chip8.key[0xC] = 0;
    else if (key == 'x')
        Chip8.key[0xD] = 0;
    else if (key == 'c')
        Chip8.key[0xE] = 0;
    else if (key == 'v')
        Chip8.key[0xF] = 0;
}

void updateQuads(const emulator::chip8 &c8)
{
    for (int y = 0; y < 32; ++y)
        for (int x = 0; x < 64; ++x)
        {
            if (c8.gfx[(y * 64) + x] == 0)
                glColor3f(0.0f, 0.0f, 0.0f);
            else
                glColor3f(1.0f, 1.0f, 1.0f);
            drawPixel(x, y);
        }
}

void drawPixel(int x, int y)
{
    glBegin(GL_QUADS);
    glVertex3f((x * scaler) + 0.0f, (y * scaler) + 0.0f, 0.0f);
    glVertex3f((x * scaler) + 0.0f, (y * scaler) + scaler, 0.0f);
    glVertex3f((x * scaler) + scaler, (y * scaler) + scaler, 0.0f);
    glVertex3f((x * scaler) + scaler, (y * scaler) + 0.0f, 0.0f);
    glEnd();
}