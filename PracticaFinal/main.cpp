#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Common.h"
#include "Block.h"
#include "Game.h"
#include "RgbImage.h"

#define SCREEN_SIZE     1000, 500
#define SCREEN_POSITION 800,  400
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
void drawBlocks();
void drawPause();
void drawPlane(GLfloat size);
void drawBlock(Block* block);
void drawSubBlock(SubBlock* sub);
void drawBasicBlock(bool withBorder = true);
void initLights();
void initTextures();
void selectColor(uint8 color);
void generateRandomBlock();
void renderText(float x, float y, void *font, const unsigned char* string);
void drawPoints();

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

bool selected = false;

bool soundPaused = true;

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
    
    PlaySoundTetris(TEXT("../src/main.wav"), nullptr, SND_LOOP | SND_ASYNC);
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
    initTextures();
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

    {
        GLfloat Ia0[] = { 0.5, 0.5, 0.5, 1.0 };
        GLfloat Id0[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat Is0[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat point[] = { 4.0, 4.0, 5.0, 1.0};
        glLightfv(GL_LIGHT0, GL_AMBIENT , Ia0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE , Id0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, Is0);
	    glLightfv(GL_LIGHT0, GL_POSITION, point);
        glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION , 0.50f);
        glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION   , 0.01f);
        glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
        glEnable(GL_LIGHT0);
    }

    {
        GLfloat Ia0[] = { 0.5, 0.5, 0.5, 1.0 };
        GLfloat Id0[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat Is0[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat point[] = { -4.0, 4.0, 5.0, 1.0};
        glLightfv(GL_LIGHT1, GL_AMBIENT , Ia0);
        glLightfv(GL_LIGHT1, GL_DIFFUSE , Id0);
        glLightfv(GL_LIGHT1, GL_SPECULAR, Is0);
	    glLightfv(GL_LIGHT1, GL_POSITION, point);
        glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION , 0.50f);
        glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION   , 0.01f);
        glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);
        glEnable(GL_LIGHT1);
    }
}

const GLuint numTextures = 3;
GLuint textureName[numTextures];

void initTextures()
{
    glEnable(GL_TEXTURE_2D);
    glGenTextures(numTextures-1, textureName);
    
    const char *filename[numTextures] = { "../src/textura.bmp", "../src/tetris.bmp", "../src/texturaSelect.bmp" };

    for(unsigned i = 0; i < numTextures; i++)
    {
        // Cargamos la textura
        glBindTexture(GL_TEXTURE_2D, textureName[i]);
        RgbImage texture(filename[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.GetNumCols(), texture.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.ImageData());
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture.GetNumCols(), texture.GetNumRows(), GL_RGB, GL_UNSIGNED_BYTE, texture.ImageData());
    
        // Configuramos la textura
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        /*glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);*/
    }
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
    case 'm':
        soundPaused = !soundPaused;
        if (!soundPaused)
            PlaySoundTetris(nullptr, nullptr, 0);
        else
            PlaySoundTetris(TEXT("../src/main.wav"), nullptr, SND_LOOP | SND_ASYNC);
        break;
    case 's':
        selected = !selected;
        break;
    case 'r':
        cameraPos[0] = 2.0f;
        cameraPos[1] = 3.0f;
        cameraPos[2] = 10.0f;
        lookat[0] = 2.0f;
        lookat[1] = 3.0f;
        lookat[2] = -8.0f;
        break;
    case 'c':
        game->ChangeBlock();
        break;
    case ' ':
        game->RotateActiveBlock();
        break;
    case 13: // Enter
    case 27: // ESC
        stopped = !stopped;
        if (stopped)
            game->PauseGame();
        else
            game->ResumeGame();
        break;
    case '+':
        game->SetLevel(game->GetLevel() + 1);
        break;
    case '-':
        game->SetLevel(game->GetLevel() - 1);
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
    case GLUT_KEY_UP:
        game->DropBlock();
        break;
    case GLUT_KEY_DOWN:
        game->IncreaseBlockSpeed();
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
        if ((glutGet(GLUT_ELAPSED_TIME) - lastClickTime) < DOUBLE_CLICK_TIME)
            stopped = !stopped;

        lastClickTime = glutGet(GLUT_ELAPSED_TIME);
    }

    DEBUG_LOG("MOUSE: key: %d, state: %d, x: %d, y: %d \n", key, state, x, y);
}

void funMotionPassive(int x, int y)
{
    /*oldX = x;
    oldY = y;*/
    
    //DEBUG_LOG("PASSIVE: x: %d, y: %d \n", x, y);
}

void funMotion(int x, int y)
{
    if (!stopped)
    {
        cameraPos[0] -= float(oldX - x) / 300.0f;
        cameraPos[1] += float(oldY - y) / 300.0f;

        oldX = x;
        oldY = y;
    }

    DEBUG_LOG("MOTION: x: %d, y: %d \n", x, y);
}

#define MAX_ZOOM 7.0f
#define MIN_ZOOM 10.0f

void funMouseWheel(int wheel, int direction, int x, int y)
{
    cameraPos[2] = std::min<GLfloat>(MIN_ZOOM, std::max<GLfloat>(MAX_ZOOM, cameraPos[2] - direction * 0.3f));
    DEBUG_LOG("MOUSEWHEEL: wheel: %d, direction: %d, x: %d, y: %d, positionZ: %f \n", wheel, direction, x, y, cameraPos[2]);
}

void funDisplay()
{
    drawFrame();
}

void funIdle()
{
    if (!stopped)
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

    // Posicionamos la cámara (V)
    gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2],
                 lookat[0],    lookat[1],    lookat[2],
                     up[0],        up[1],        up[2]);
    
    glScaled(0.5f, 0.5f, 0.5f);
    drawPanel();
    drawBlocks();
    glScaled(1.0f, 1.0f, 1.0f);

    if (stopped)
        drawPause();

    drawPoints();

    //DEBUG_LOG("points = %u : %s", game->GetPoints(), std::to_string(game->GetPoints()));
    
    // Intercambiamos los buffers
    glutSwapBuffers();
}

void drawBlocks()
{
    // Draw the active falling block
    if (game->GetActiveBlock())
        drawBlock(game->GetActiveBlock());

    // Draw the next block
    if (game->GetNextBlock())
        drawBlock(game->GetNextBlock());

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
    
    glEnable(GL_TEXTURE_2D);
    if (selected)
    {
        glBindTexture(GL_TEXTURE_2D, textureName[2]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, textureName[0]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPushMatrix();
    {
        glTranslatef(block->GetPositionX() + correction[0], block->GetPositionY() + correction[1], block->GetPositionZ());

        // Cube should not rotate
        //if (block->GetType() != TYPE_CUBE)
        //    glRotatef(block->GetRotation(), 0.0f, 0.0f, 1.0f);

        if (correction[0] != 0.0f || correction[1] != 0.0f)
            glTranslatef(-correction[0], -correction[1], 0.0f);

        for (SubBlock* sub : subBlocks)
        {
            glPushMatrix();
            glTranslatef(sub->GetPositionX(), sub->GetPositionY(), sub->GetPositionZ());
            drawBasicBlock();
            glPopMatrix();
        }
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawSubBlock(SubBlock* sub)
{
    color = sub->GetColor();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureName[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPushMatrix();
    glTranslatef(sub->GetPositionX(), sub->GetPositionY(), sub->GetPositionZ());
    drawBasicBlock();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void selectColor(uint8 color)
{
    switch(color)
    {
    case COLOR_WHITE:
    {
        GLfloat Kad[] = {1.0, 1.0, 1.0, 1.0};
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
        GLfloat Kad[] = {0.863f, 0.078f, 0.235f, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.863f, 0.078f, 0.235f);
        break;
    }
    case COLOR_BLUE:
    {
        GLfloat Kad[] = {0.118f, 0.565f, 1.000f, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.118f, 0.565f, 1.000f);
        break;
    }
    case COLOR_GREEN:
    {
        GLfloat Kad[] = {0.235f, 0.702f, 0.443f, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.235f, 0.702f, 0.443f);
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
        GLfloat Kad[] = {0.653f, 0.653f, 0.653f, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.753f, 0.753f, 0.753f);
        break;
    }
    case COLOR_CYAN:
    {
        GLfloat Kad[] = {0.902f, 0.902f, 0.980f, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Kad);
        glColor3f(0.902f, 0.902f, 0.980f);
        break;
    }
    default:
        break;
    }
}

void drawCube(GLfloat size)
{
    GLfloat dimension = size / 2.0f;

   // draw a cube (6 quadrilaterals)
    glBegin(GL_QUADS);				// start drawing the cube.
 
	    // Front Face
	    glTexCoord2f(0.0f, 0.0f); glVertex3f(-dimension, -dimension,  dimension);	// Bottom Left Of The Texture and Quad
	    glTexCoord2f(1.0f, 0.0f); glVertex3f( dimension, -dimension,  dimension);	// Bottom Right Of The Texture and Quad
	    glTexCoord2f(1.0f, 1.0f); glVertex3f( dimension,  dimension,  dimension);	// Top Right Of The Texture and Quad
	    glTexCoord2f(0.0f, 1.0f); glVertex3f(-dimension,  dimension,  dimension);	// Top Left Of The Texture and Quad
 
	    // Back Face
	    glTexCoord2f(1.0f, 0.0f); glVertex3f(-dimension, -dimension, -dimension);	// Bottom Right Of The Texture and Quad
	    glTexCoord2f(1.0f, 1.0f); glVertex3f(-dimension,  dimension, -dimension);	// Top Right Of The Texture and Quad
	    glTexCoord2f(0.0f, 1.0f); glVertex3f( dimension,  dimension, -dimension);	// Top Left Of The Texture and Quad
	    glTexCoord2f(0.0f, 0.0f); glVertex3f( dimension, -dimension, -dimension);	// Bottom Left Of The Texture and Quad
 
	    // Top Face
	    glTexCoord2f(0.0f, 1.0f); glVertex3f(-dimension,  dimension, -dimension);	// Top Left Of The Texture and Quad
	    glTexCoord2f(0.0f, 0.0f); glVertex3f(-dimension,  dimension,  dimension);	// Bottom Left Of The Texture and Quad
	    glTexCoord2f(1.0f, 0.0f); glVertex3f( dimension,  dimension,  dimension);	// Bottom Right Of The Texture and Quad
	    glTexCoord2f(1.0f, 1.0f); glVertex3f( dimension,  dimension, -dimension);	// Top Right Of The Texture and Quad
 
	    // Bottom Face
	    glTexCoord2f(1.0f, 1.0f); glVertex3f(-dimension, -dimension, -dimension);	// Top Right Of The Texture and Quad
	    glTexCoord2f(0.0f, 1.0f); glVertex3f( dimension, -dimension, -dimension);	// Top Left Of The Texture and Quad
	    glTexCoord2f(0.0f, 0.0f); glVertex3f( dimension, -dimension,  dimension);	// Bottom Left Of The Texture and Quad
	    glTexCoord2f(1.0f, 0.0f); glVertex3f(-dimension, -dimension,  dimension);	// Bottom Right Of The Texture and Quad
 
	    // Right face
	    glTexCoord2f(1.0f, 0.0f); glVertex3f( dimension, -dimension, -dimension);	// Bottom Right Of The Texture and Quad
	    glTexCoord2f(1.0f, 1.0f); glVertex3f( dimension,  dimension, -dimension);	// Top Right Of The Texture and Quad
	    glTexCoord2f(0.0f, 1.0f); glVertex3f( dimension,  dimension,  dimension);	// Top Left Of The Texture and Quad
	    glTexCoord2f(0.0f, 0.0f); glVertex3f( dimension, -dimension,  dimension);	// Bottom Left Of The Texture and Quad
 
	    // Left Face
	    glTexCoord2f(0.0f, 0.0f); glVertex3f(-dimension, -dimension, -dimension);	// Bottom Left Of The Texture and Quad
	    glTexCoord2f(1.0f, 0.0f); glVertex3f(-dimension, -dimension,  dimension);	// Bottom Right Of The Texture and Quad
	    glTexCoord2f(1.0f, 1.0f); glVertex3f(-dimension,  dimension,  dimension);	// Top Right Of The Texture and Quad
	    glTexCoord2f(0.0f, 1.0f); glVertex3f(-dimension,  dimension, -dimension);	// Top Left Of The Texture and Quad
 
    glEnd();					// Done Drawing The Cube
 
}

void drawPause()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureName[1]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPushMatrix();
    {
        glTranslatef(4.0f, 6.0f, 10.0f);
        glScalef(4.0f, 4.0f, 4.0f);
        glBegin(GL_QUADS);
	        glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, -1.0f,  1.0f);
	        glTexCoord2f(1.0f, 0.0f); glVertex3f( 2.5f, -1.0f,  1.0f);
	        glTexCoord2f(1.0f, 1.0f); glVertex3f( 2.5f,  1.0f,  1.0f);
	        glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.5f,  1.0f,  1.0f);
        glEnd();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawPlane(GLfloat size)
{
    GLfloat dimension = size / 2.0f;
    glBegin(GL_QUADS);
	    glTexCoord2f(0.0f, 0.0f); glVertex3f(-dimension, -dimension,  dimension);
	    glTexCoord2f(1.0f, 0.0f); glVertex3f( dimension, -dimension,  dimension);
	    glTexCoord2f(1.0f, 1.0f); glVertex3f( dimension,  dimension,  dimension);
	    glTexCoord2f(0.0f, 1.0f); glVertex3f(-dimension,  dimension,  dimension);
    glEnd();
}

void drawBasicBlock(bool withBorder /*=true*/)
{
    selectColor(color);
    
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    drawCube(1.0f);

    glDisable(GL_TEXTURE_2D);
    //glutSolidCube(1.0f);


    /*if (withBorder)
    {
        glLineWidth(2.0);
        selectColor(COLOR_BLACK);
        glutWireCube(1.0f);
    }*/
}

void drawPanel()
{
    uint8 oldColor = color;
    color = COLOR_GRAY;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureName[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

    glPushMatrix();
    {
        glTranslatef(DISPLAY_NEXT_BLOCK_X, DISPLAY_NEXT_BLOCK_Y, 0.0);
        for (uint8 i = 0; i < DISPLAY_NEXT_BLOCK_HEIGHT; i++)
        {
            drawBasicBlock();
            glTranslatef(0.0, 1.0, 0.0);
        }
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(DISPLAY_NEXT_BLOCK_X + DISPLAY_NEXT_BLOCK_WITDH, DISPLAY_NEXT_BLOCK_Y, 0.0);
        for (uint8 i = 0; i < DISPLAY_NEXT_BLOCK_HEIGHT + 1; i++)
        {
            drawBasicBlock();
            glTranslatef(0.0, 1.0, 0.0);
        }
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(DISPLAY_NEXT_BLOCK_X, DISPLAY_NEXT_BLOCK_Y, 0.0);
        for (uint8 i = 0; i < DISPLAY_NEXT_BLOCK_WITDH; i++)
        {
            drawBasicBlock();
            glTranslatef(1.0, 0.0, 0.0);
        }
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(DISPLAY_NEXT_BLOCK_X, DISPLAY_NEXT_BLOCK_Y + DISPLAY_NEXT_BLOCK_HEIGHT, 0.0);
        for (uint8 i = 0; i < DISPLAY_NEXT_BLOCK_WITDH; i++)
        {
            drawBasicBlock();
            glTranslatef(1.0, 0.0, 0.0);
        }
    }
    glPopMatrix();
    color = oldColor;
    glDisable(GL_TEXTURE_2D);
}

void renderText(float x, float y, void *font, const unsigned char* string)
{
    glRasterPos3f(x, y, 0.0f);
    glutBitmapString(font, string);
}

void drawPoints()
{
    unsigned char points[BUFFER_SIZE];
    std::string pointsString = "· Puntuacion: " + std::to_string(game->GetPoints()) + "\n· Nivel: " + std::to_string(game->GetLevel()) + "\n· Velocidad: " + std::to_string(game->GetSpeed());
    strcpy_s((char*)points, BUFFER_SIZE, pointsString.c_str());
    renderText(POINTS_X, POINTS_Y, GLUT_BITMAP_9_BY_15, points);
}
