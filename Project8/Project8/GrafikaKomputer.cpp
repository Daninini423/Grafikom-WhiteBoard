#include <iostream>
#include <GL/freeglut.h>
#include <cmath>
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Kamera (polar)
float camAngleY = 15.0f;
float camAngleX = 0.0f;
float camDist = 5.0f;  // Jarak kamera (dipakai untuk zoom)

// Gelombang
float wavePhase = 0.0f;
float waveSpeed = 0.04f;
float waveAmplitude = 0.025f;
float waveYoffset = 1.0f;

// Lighting
bool light0On = true;
float lightIntensity = 1.0f;

const float BOARD_W = 3.0f;
const float BOARD_H = 1.8f;
const float BOARD_D = 0.02f;

// ----------------------------------
void setupLighting() {
    glEnable(GL_LIGHTING);

    GLfloat globalAmb[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);             // fv (float (vector) 
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);           // i (integer)

    GLfloat light0Pos[] = { 3.0f, 4.0f, 3.0f, 1.0f };
    GLfloat lightAmb[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat lightDifSpec[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifSpec);

    if (light0On) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

// ----------------------------------
void drawBlackboardModel() {
    float halfW = BOARD_W / 2, halfH = BOARD_H / 2, halfD = BOARD_D / 2;

    glColor3f(0.02f, 0.28f, 0.06f);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(-halfW, -halfH, halfD);
    glVertex3f(halfW, -halfH, halfD);
    glVertex3f(halfW, halfH, halfD);
    glVertex3f(-halfW, halfH, halfD);
    glEnd();

    glColor3f(0.6f, 0.6f, 0.6f);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glVertex3f(-halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, -halfD);
    glVertex3f(halfW, halfH, -halfD);
    glVertex3f(-halfW, halfH, -halfD);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-halfW, halfH, -halfD);
    glVertex3f(halfW, halfH, -halfD);
    glVertex3f(halfW, halfH, halfD);
    glVertex3f(-halfW, halfH, halfD);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, halfD);
    glVertex3f(-halfW, -halfH, halfD);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(-halfW, -halfH, -halfD);
    glVertex3f(-halfW, -halfH, halfD);
    glVertex3f(-halfW, halfH, halfD);
    glVertex3f(-halfW, halfH, -halfD);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glVertex3f(halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, halfD);
    glVertex3f(halfW, halfH, halfD);
    glVertex3f(halfW, halfH, -halfD);
    glEnd();

    glColor3f(0.45f, 0.45f, 0.45f);
    float trayW = BOARD_W * 0.9f;
    float trayH = 0.06f;
    float trayD = 0.18f;
    float frontZ = BOARD_D / 2 + trayD / 2 + 0.005f;
    float trayY = -BOARD_H / 2 - trayH / 2 - 0.03f;

    glPushMatrix();
    glTranslatef(0.0f, trayY, frontZ);
    glScalef(trayW, trayH, trayD);
    glutSolidCube(1.0);
    glPopMatrix();
}

// ----------------------------------
void drawWaveLine() {
    float frontZ = BOARD_D / 2 + 0.015f;
    float lineLength = BOARD_W * 0.8f;
    int segments = 250;

    glDisable(GL_LIGHTING);
    glLineWidth(3.0f);
    glColor3f(0, 0, 0);

    glPushMatrix();
    glTranslatef(0.0f, BOARD_H / 2 - waveYoffset, frontZ);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float t = (float)i / segments;
        float x = (t - 0.5f) * lineLength;

        float freq = 15.0f;
        float y = sinf(wavePhase + t * freq * 2.0f * M_PI) * waveAmplitude;

        glVertex3f(x, y, 0);
    }
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

// ----------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    setupLighting();

    // Hitung posisi kamera
    float camX = camDist * cos(camAngleX * M_PI / 180.0f) * sin(camAngleY * M_PI / 180.0f);
    float camY = camDist * sin(camAngleX * M_PI / 180.0f);
    float camZ = camDist * cos(camAngleX * M_PI / 180.0f) * cos(camAngleY * M_PI / 180.0f);

    gluLookAt(camX, camY, camZ,
        0, 0, 0,
        0, 1, 0);

    drawBlackboardModel();
    drawWaveLine();

    glutSwapBuffers();
}

// ----------------------------------
void idle() {
    wavePhase += waveSpeed;
    if (wavePhase > 2 * M_PI) wavePhase -= 2 * M_PI;

    glutPostRedisplay();
}

// ----------------------------------
void keyboardDown(unsigned char key, int, int) {

    // Rotasi kamera
    if (key == 'a' || key == 'A') camAngleY -= 3.0f;
    else if (key == 'd' || key == 'D') camAngleY += 3.0f;
    else if (key == 'w' || key == 'W') camAngleX += 3.0f;
    else if (key == 's' || key == 'S') camAngleX -= 3.0f;

    // Zoom (fitur baru)
    else if (key == '+') camDist -= 0.2f;
    else if (key == '-') camDist += 0.2f;

    // Batas zoom
    if (camDist < 1.5f) camDist = 1.5f;
    if (camDist > 15.0f) camDist = 15.0f;

    // Batas rotasi vertikal
    if (camAngleX > 89) camAngleX = 89;
    if (camAngleX < -89) camAngleX = -89;

    // Amplitudo gelombang
    else if (key == '[') waveAmplitude -= 0.005f;
    else if (key == ']') waveAmplitude += 0.005f;

    else if (key == 'l' || key == 'L') {
        light0On = !light0On;  // toggle light
    }
        
    else if (key == 27) exit(0);
}

void specialKeys(int key, int, int) {
    if (key == GLUT_KEY_UP) waveSpeed += 0.01f;
    else if (key == GLUT_KEY_DOWN) waveSpeed -= 0.01f;
    else if (key == GLUT_KEY_LEFT) waveYoffset += 0.05f;
    else if (key == GLUT_KEY_RIGHT) waveYoffset -= 0.05f;
}

// ----------------------------------
void initGL() {
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1000.0 / 650.0, 0.1, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

// ----------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 650);
    glutCreateWindow("3D Blackboard - FIX + ZOOM");

    initGL();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboardDown);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
