#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Common.h"
#include "Block.h"
#include "Game.h"
//#include "RgbImage.h"

#define SCREEN_SIZE     1000, 500
#define SCREEN_POSITION   50,  50
#define SCREEN_COLOR     0.0, 0.0, 0.0, 0.0
#define DOUBLE_CLICK_TIME 250

void initFunc();
void funReshape(int w, int h);
void funDisplay();
void funIdle();
void funKeyboardUp(unsigned char key, int x, int y);
void funSpecial(int key, int x, int y);
void funMouse(int key, int state, int x, int y);
void funMotion(int x, int y);
void funMotionPassive(int x, int y);
void funMouseWheel(int wheel, int direction, int x, int y);
void drawFrame();
void drawPanel();
void drawNumber(uint8 number);
void drawBlocks();
void drawBlock(Block* block);
void drawSubBlock(SubBlock* sub);
void drawBasicBlock(bool withBorder = true);
void initLights();
void selectColor(uint8 color);
void generateRandomBlock();

GLfloat cameraPos[3]            = { 2.0, 3.0, 10.0 };
GLfloat lookat[3]               = { 2.0, 3.0, -8.0 };
GLfloat up[3]                   = { 0.0, 1.0,  0.0 };

GLfloat currentRotationZ        = 0.0;

GLfloat ambientLightIntensity[]   = { 0.2f, 0.2f, 0.2f, 0.2f };

uint8 color = COLOR_WHITE;

int32 oldX = 0, oldY = 0;

uint32 lastClickTime = 0;

uint64 nextMoveTime = 0;

Game* game = nullptr;

bool stopped = false;

int main(int argc, char** argv) {
    
    srand(unsigned int(time(nullptr)));

    // Inicializamos OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Inicializamos la Ventana
    glutInitWindowSize(SCREEN_SIZE);
    glutInitWindowPosition(SCREEN_POSITION);
    glutCreateWindow("Practica Final");

    // Inicializaciones específicas
    initFunc();

    // Configuración CallBacks
    glutReshapeFunc(funReshape);
    glutDisplayFunc(funDisplay);
    glutKeyboardUpFunc(funKeyboardUp);
    glutSpecialFunc(funSpecial);
    glutMouseFunc(funMouse);
    glutMotionFunc(funMotion);
    glutPassiveMotionFunc(funMotionPassive);
    glutIdleFunc(funIdle);
    glutMouseWheelFunc(funMouseWheel);

    game = Game::CreateNewGame();
    if (!game)
        return(1);

    game->StartGame();

    // Bucle principal
    glutMainLoop();

    // Destrucción de objetos
    return(0);
}

void initFunc() {

    // Inicializamos GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        DEBUG_LOG("Error: %s\n", glewGetErrorString(err));
    }
    DEBUG_LOG("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    // Configuracion de parametros fijos
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    initLights();
    //initTextures();
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glPolygonOffset(1.0, 1.0);
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_NORMALIZE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    lastClickTime = glutGet(GLUT_ELAPSED_TIME);
}

void initLights()
{
    glEnable(GL_LIGHTING);

    // Luz ambiente global
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLightIntensity);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    // Brillo del sol
    GLfloat Ia0[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat Id0[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat Is0[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat point[] = { 4.0, 4.0, 0.0, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT , Ia0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE , Id0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Is0);
	glLightfv(GL_LIGHT0, GL_POSITION, point);
    glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION , 0.50f);
    glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION   , 0.01f);
    glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
    glEnable(GL_LIGHT0);
}

void funReshape(int w, int h) {

    // Configuramos el Viewport
    glViewport(0, 0, w, h);

    // Configuracion del modelo de proyeccion (P)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 50.0);
}

void funKeyboardUp(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ' ':
        game->RotateActiveBlock();
        break;
    case 13: // Enter
        stopped = !stopped;
        break;
    default:
        break;
    }

    DEBUG_LOG("KEYBOARD: key: %c, x: %d, y: %d \n", key, x, y);
}

void funSpecial(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN:
        game->DropBlock();
        break;
    case GLUT_KEY_RIGHT:
        game->MoveBlock(true);
        break;
    case GLUT_KEY_LEFT:
        game->MoveBlock(false);
        break;
    default:
        break;
    }

    DEBUG_LOG("KEYBOARD SPECIAL: key: %d, x: %d, y: %d \n", key, x, y);
}

void funMouse(int key, int state, int x, int y)
{
    oldX = x;
    oldY = y;

    if (state == GLUT_UP)
    {   
        //if ((glutGet(GLUT_ELAPSED_TIME) - lastClickTime) < DOUBLE_CLICK_TIME)
        //    doubleClick();

        lastClickTime = glutGet(GLUT_ELAPSED_TIME);
    }

    DEBUG_LOG("MOUSE: key: %d, state: %d, x: %d, y: %d \n", key, state, x, y);
}

void funMotionPassive(int x, int y)
{
    oldX = x;
    oldY = y;
    
    //DEBUG_LOG("PASSIVE: x: %d, y: %d \n", x, y);
}

void funMotion(int x, int y)
{
    cameraPos[0] -= float(oldX - x) / 100.0f;
    lookat[0] = float(oldX - x) / 100.0f;
    cameraPos[1] += float(oldY - y) / 100.0f;
    lookat[1] = float(oldY - y) / 100.0f;

    oldX = x;
    oldY = y;

    DEBUG_LOG("MOTION: x: %d, y: %d \n", x, y);
}

void funMouseWheel(int wheel, int direction, int x, int y)
{
    cameraPos[2] -= GLfloat(direction * 0.3);
    DEBUG_LOG("MOUSEWHEEL: wheel: %d, direction: %d, x: %d, y: %d, positionZ: %f \n", wheel, direction, x, y, cameraPos[2]);
}

void funDisplay()
{
    drawFrame();
}

void funIdle()
{
    if (stopped)
        return;

    game->Update();

    drawFrame();
}

void drawFrame()
{
    // Borramos el buffer de color y el de profundidad
    glClearColor(SCREEN_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Posicionamos la cámara (V)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //drawInterface();

    // Posicionamos la cámara (V)
    gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2],
                 lookat[0],    lookat[1],    lookat[2],
                     up[0],        up[1],        up[2]);
    
    glScaled(0.5f, 0.5f, 0.5f);
    drawPanel();
    drawBlocks();

    drawNumber(110);
    
    // Intercambiamos los buffers
    glutSwapBuffers();
}

void drawBlocks()
{
    // Draw the active falling block
    if (game->GetActiveBlock())
        drawBlock(game->GetActiveBlock());

    // Draw other subBlocks
    for (SubBlock* sub : game->GetSubBlockList())
        drawSubBlock(sub);
}

void drawBlock(Block* block)
{
    if (!block)
        return;

    if (block->GetType() > MAX_BLOCK_TYPE)
    {
        DEBUG_LOG("Block type not supported: type (%d)", block->GetType());
        exit(1);
    }
    
    color = block->GetColor();
    std::vector<SubBlock*> subBlocks = block->GetSubBlocks();

    float correction[2] = {0.0f, 0.0f};

    switch(block->GetType())
    {
    case TYPE_CUBE:
        correction[0] = 1.0f;
        correction[1] = 1.0f;
        break;
    case TYPE_PRISM:
        correction[0] = 2.0f;
        break;
    case TYPE_T:
    case TYPE_Z:
    case TYPE_L:
        correction[0] = 1.0f;
        break;
    default:
        break;
    }

    glPushMatrix();
    
    glTranslatef(block->GetPositionX() + correction[0], block->GetPositionY() + correction[1], block->GetPositionZ());

    // Cube should not rotate
    if (block->GetType() != TYPE_CUBE)
        glRotatef(block->GetRotation(), 0.0f, 0.0f, 1.0f);

    if (correction[0] != 0.0f || correction[1] != 0.0f)
        glTranslatef(-correction[0], -correction[1], 0.0f);

    for (SubBlock* sub : subBlocks)
    {
        glPushMatrix();
        glTranslatef(sub->GetPositionX(), sub->GetPositionY(), sub->GetPositionZ());
        drawBasicBlock();
        glPopMatrix();
    }
    glPopMatrix();
}

void drawSubBlock(SubBlock* sub)
{
    color = sub->GetColor();
    glPushMatrix();
    glTranslatef(sub->GetPositionX(), sub->GetPositionY(), sub->GetPositionZ());
    drawBasicBlock();
    glPopMatrix();
}

void selectColor(uint8 color)
{
    switch(color)
    {
    case COLOR_WHITE:
    {
        GLfloat Kad[] = {1.0, 0.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(1.0, 1.0, 1.0);
        break;
    }
    case COLOR_BLACK:
    {
        GLfloat Kad[] = {0.0, 0.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.0, 0.0, 0.0);
        break;
    }
    case COLOR_RED:
    {
        GLfloat Kad[] = {1.0, 0.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(1.0, 0.0, 0.0);
        break;
    }
    case COLOR_BLUE:
    {
        GLfloat Kad[] = {0.0, 0.0, 1.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.0, 0.0, 1.0);
        break;
    }
    case COLOR_GREEN:
    {
        GLfloat Kad[] = {0.0, 1.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.0, 1.0, 0.0);
        break;
    }
    case COLOR_YELLOW:
    {
        GLfloat Kad[] = {1.0, 1.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(1.0, 1.0, 0.0);
        break;
    }
    case COLOR_PINK:
    {
        GLfloat Kad[] = {1.0, 0.0, 0.5, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(1.0, 0.0, 0.5);
        break;
    }
    case COLOR_ORANGE:
    {
        GLfloat Kad[] = {1.0, 0.5, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(1.0, 0.5, 0.0);
        break;
    }
    case COLOR_GRAY:
    {
        GLfloat Kad[] = {0.753f, 0.753f, 0.753f, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.753f, 0.753f, 0.753f);
        break;
    }
    default:
        break;
    }
}

void drawBasicBlock(bool withBorder /*=true*/)
{
    selectColor(color);
    glutSolidCube(1.0f);

    if (withBorder)
    {
        glLineWidth(2.0);
        selectColor(COLOR_BLACK);
        glutWireCube(1.0f);
    }
}

void drawPanel()
{
    uint8 oldColor = color;
    color = COLOR_GRAY;
    glPushMatrix();
    {
        glTranslatef(0.0, -1.0, 0.0);
        for (uint8 i = 0; i < MAX_WIDTH; i++)
        {
            drawBasicBlock();
            glTranslatef(1.0, 0.0, 0.0);
        }
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-1.0, -1.0, 0.0);
        for (uint8 i = 0; i < MAX_HEIGHT; i++)
        {
            drawBasicBlock();
            glTranslatef(0.0, 1.0, 0.0);
        }
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(MAX_WIDTH, -1.0, 0.0);
        for (uint8 i = 0; i < MAX_HEIGHT; i++)
        {
            drawBasicBlock();
            glTranslatef(0.0, 1.0, 0.0);
        }
    }
    glPopMatrix();
    color = oldColor;
}

void drawNumber(uint8 number)
{
    glRasterPos2f(0.0f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, number);
}
