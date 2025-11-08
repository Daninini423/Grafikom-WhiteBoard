// main.cpp - 3D Blackboard with metallic edges visible from front
// Controls: Hold R/T to rotate, Arrow Up/Down to adjust wave speed
#include <iostream>
using namespace std;

#include <GL/freeglut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float boardRotY = 0.0f;    //Sudut rotasi papan (dalam derajat)
int rotateDir = 0;            //Arah rotasi papan   // -1 = muter dari arah jam berlawan. 0 = berhenti. 1 = muter searah jarum jam
float wavePhase = 0.0f; 
float waveSpeed = 0.04f;       // Kecepatan animasi nya
float waveAmplitude = 0.025f;   // tinggi gelombang awal
float waveYoffset = 0.25f;      // posisi vertikal awal (dekat atas papan)

// --- Variabel global untuk kontrol pencahayaan ---
bool light0On = true;
bool light1On = true;
float lightIntensity = 1.0f;



const float BOARD_W = 3.0f;         // Lebar papan
const float BOARD_H = 1.8f;         // Tinggi
const float BOARD_D = 0.02f;        // kedalaman

void setupLighting() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Warna dasar ambient global (pencahayaan ruangan)
    GLfloat globalAmb[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Cahaya utama (putih terang dari kanan atas depan)
    GLfloat light0Pos[] = { 3.0f, 4.0f, 3.0f, 1.0f };
    GLfloat lightAmb[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // cahaya dasar lembut
    GLfloat lightDifSpec[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };

    // Atur properti cahaya 0
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifSpec);

    // Aktifkan hanya satu sumber cahaya
    if (light0On) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);

    // Aktifkan pewarnaan dan culling
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      
}





void drawBox(float w, float h, float d) {
    glPushMatrix();
    glScalef(w, h, d);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawBlackboardModel() {
    float halfW = BOARD_W / 2;
    float halfH = BOARD_H / 2;
    float halfD = BOARD_D / 2;

    // -------- Permukaan depan (warna hijau papan tulis) --------
    glColor3f(0.02f, 0.28f, 0.06f); // hijau tua
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-halfW, -halfH, halfD);
    glVertex3f(halfW, -halfH, halfD);
    glVertex3f(halfW, halfH, halfD);
    glVertex3f(-halfW, halfH, halfD);
    glEnd();

    // -------- Sisi-sisi lainnya (warna abu metalik) --------
    glColor3f(0.6f, 0.6f, 0.6f);

    // belakang
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, -halfD);
    glVertex3f(halfW, halfH, -halfD);
    glVertex3f(-halfW, halfH, -halfD);
    glEnd();

    // atas
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfW, halfH, -halfD);
    glVertex3f(halfW, halfH, -halfD);
    glVertex3f(halfW, halfH, halfD);
    glVertex3f(-halfW, halfH, halfD);
    glEnd();

    // bawah
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, halfD);
    glVertex3f(-halfW, -halfH, halfD);
    glEnd();

    // kiri
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-halfW, -halfH, -halfD);
    glVertex3f(-halfW, -halfH, halfD);
    glVertex3f(-halfW, halfH, halfD);
    glVertex3f(-halfW, halfH, -halfD);
    glEnd();

    // kanan
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(halfW, -halfH, -halfD);
    glVertex3f(halfW, -halfH, halfD);
    glVertex3f(halfW, halfH, halfD);
    glVertex3f(halfW, halfH, -halfD);
    glEnd();

    // -------- Tray (tempat kapur) --------
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


void drawWaveLine() {
    float frontZ = BOARD_D / 2 + 0.002f;
    float lineLength = BOARD_W * 0.8f;
    int segments = 250;

    glDisable(GL_LIGHTING);
    glLineWidth(3.5f);
    glColor3f(0.0f, 0.0f, 0.0f); // warna hitam (kapur)

    glPushMatrix();
    glTranslatef(0.0f, BOARD_H / 2 - waveYoffset, frontZ); // posisi vertikal dikontrol
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float t = (float)i / segments;
        float x = (t - 0.5f) * lineLength;
        float freq = 10.0f; // tetap rapat
        float y = sinf(wavePhase + t * freq * 2.0f * (float)M_PI) * waveAmplitude;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glPopMatrix();

    glEnable(GL_LIGHTING);
}





void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(3.2f, 1.6f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glRotatef(boardRotY, 0.0f, 1.0f, 0.0f);
    drawBlackboardModel();
    drawWaveLine();
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void idle() {

    wavePhase += waveSpeed;
    if (wavePhase > 2.0f * M_PI) wavePhase -= 2.0f * M_PI;

    if (rotateDir != 0) {
        boardRotY += rotateDir * 0.25f;
        if (boardRotY > 360.0f) boardRotY -= 360.0f;
        if (boardRotY < -360.0f) boardRotY += 360.0f;
    }

    setupLighting(); // agar perubahan cahaya langsung diterapkan

    glutPostRedisplay();
}

// Tombol panah dan kontrol tambahan
void specialKeys(int key, int, int) {
    if (key == GLUT_KEY_UP) {
        waveSpeed += 0.01f;
        if (waveSpeed > 0.2f) waveSpeed = 0.2f;
    }
    else if (key == GLUT_KEY_DOWN) {
        waveSpeed -= 0.01f;
        if (waveSpeed < 0.005f) waveSpeed = 0.005f;
    }
    else if (key == GLUT_KEY_LEFT) {
        waveYoffset += 0.05f; // naik
        if (waveYoffset > BOARD_H) waveYoffset = BOARD_H;
    }
    else if (key == GLUT_KEY_RIGHT) {
        waveYoffset -= 0.05f; // turun
        if (waveYoffset < -BOARD_H / 2) waveYoffset = -BOARD_H / 2;
    }
}

void keyboardUp(unsigned char key, int, int) {
    if ((key == 'r' || key == 'R') && rotateDir == 1)
        rotateDir = 0;
    else if ((key == 't' || key == 'T') && rotateDir == -1)
        rotateDir = 0;
}


// Tombol biasa untuk amplitude dan rotasi
void keyboardDown(unsigned char key, int, int) {
    if (key == 'r' || key == 'R') rotateDir = 1;
    else if (key == 't' || key == 'T') rotateDir = -1;
    else if (key == '[') { // perkecil gelombang
        waveAmplitude -= 0.005f;
        if (waveAmplitude < 0.005f) waveAmplitude = 0.005f;
    }
    else if (key == ']') { // perbesar gelombang
        waveAmplitude += 0.005f;
        if (waveAmplitude > 0.08f) waveAmplitude = 0.08f;
    }
    else if (key == 27) exit(0);
    else if (key == '1') {
        light0On = !light0On;
        cout << "Light 0 " << (light0On ? "ON" : "OFF") << endl;
    }
    else if (key == '2') {
        light1On = !light1On;
        cout << "Light 1 " << (light1On ? "ON" : "OFF") << endl;
    }
    else if (key == '+') {
        lightIntensity += 0.1f;
        if (lightIntensity > 2.0f) lightIntensity = 2.0f;
        cout << "Light Intensity: " << lightIntensity << endl;
    }
    else if (key == '-') {
        lightIntensity -= 0.1f;
        if (lightIntensity < 0.1f) lightIntensity = 0.1f;
        cout << "Light Intensity: " << lightIntensity << endl;
    }
    else if (key == 'l' || key == 'L') {
        // Reset lighting ke default
        lightIntensity = 1.0f;
        light0On = true;
        light1On = true;
        cout << "Lighting reset ke default." << endl;
    }


}


void initGL() {
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    setupLighting();
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    cout << "=== Controls ===" << endl;
    cout << "R/T : Rotate board" << endl;
    cout << "Arrow Up/Down : Adjust wave speed" << endl;
    cout << "[ / ] : Adjust wave amplitude" << endl;
    cout << "Left/Right : Move wave up/down" << endl;
    cout << "1/2 : Toggle lights" << endl;
    cout << "+/- : Adjust light intensity" << endl;
    cout << "L : Reset lighting" << endl;
    cout << "ESC : Exit" << endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 650);
    glutCreateWindow("3D Blackboard - Visible Metallic Edges & Adjustable Wave Speed");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
