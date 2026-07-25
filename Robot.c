#include <stdio.h> 
#include <stdlib.h> 
#include <GL/glut.h> // Header File For The GLUT Library
#include <GL/gl.h> // Header File For The OpenGL32 Library
#include <GL/glu.h> // Header File For The GLu32 Library
#include <unistd.h> // Header file for sleeping.
#include <math.h> 
#include <string.h> // Header file for string operations

/* ascii code for the escape key */
#define ESCkey 27
#define img_width 500
#define img_height 100

GLUquadric* obj;

/* define color */  
GLfloat white[4]   ={1.0, 1.0, 1.0, 1.0};
GLfloat gray[4] = {0.5, 0.5, 0.5, 1.0}; 
GLfloat blue[4] = {0.0, 0.0, 1.0, 1.0};
GLfloat green1[4]  ={0.8, 1.0, 0.8, 1.0};
GLfloat blue1[4]  ={0.1, 0.1, 1.0, 1.0};
GLfloat blue2[4]  ={0.2, 0.2, 1.0, 1.0};
GLfloat blue3[4]  ={0.3, 0.3, 1.0, 1.0};
GLfloat yellow1[4]={0.1, 0.1, 0.0, 1.0};
GLfloat yellow2[4]={0.2, 0.2, 0.0, 1.0};
GLfloat pink6[4] ={0.8, 0.55, 0.6, 1.0};
GLfloat yellow5[4]={0.8, 0.8, 0.0, 1.0};
GLfloat abu2[4]={0.5,0.5,0.5,1.0};
GLfloat gray1[4]  ={0.1, 0.1, 0.1, 1.0};
GLfloat gray2[4]  ={0.2, 0.2, 0.2, 1.0};
GLfloat gray3[4]  ={0.3, 0.3, 0.3, 1.0};
GLfloat gray4[4]  ={0.4, 0.4, 0.4, 1.0};
GLfloat gray5[4]  ={0.5, 0.5, 0.5, 1.0};
GLfloat gray6[4]  ={0.6, 0.6, 0.6, 1.0};
GLfloat gray7[4]  ={0.7, 0.7, 0.7, 1.0};
GLfloat gray8[4]  ={0.8, 0.8, 0.7, 1.0};
GLfloat gray9[4]  ={0.9, 0.9, 0.7, 1.0};

float PI = 3.14159265358;
float RTD = 180.0 / PI;
float q1 = 0.0;
float q2 = 0.0;
float xmove = 0.0; 
float ymove = 0.0;
float theta = -90;

float dx = 0.0, dy = 0.0, dshi = 0.0;
float rv = 0.0;
float rX = 0.0, rY = 0.0;
float shi = 0.0;
float shi_old = 0.0;
float dq1 = 0.0, dq2 = 0.0;
bool grid = false;

unsigned int textureNumber;
int window, backtopWindow, sensorWindow, irWindow;
int sensor = 0;
int nilaisensor = 0;
char track[12] = "track.ppm";
char mode[7] = "pwm";

// Sensor values
#define sen8 475
#define sen7 410
#define sen6 346
#define sen5 282
#define sen4 217
#define sen3 153
#define sen2 89
#define sen1 24

#define vbat 5.0
#define ThetaVolRatio 0.6667

unsigned char image_raw[img_height+1][img_width+1];
unsigned char* data;
int ir1, ir2, ir3, ir4, ir5, ir6, ir7, ir8;

void Jacobian(float &dx, float &dy, float &dshi, float dq2, float dq1, float shi) {
    dx = 0.025 / 2.0 * cos(shi) * (dq2 + dq1);
    dy = 0.025 / 2.0 * sin(shi) * (dq2 + dq1);
    dshi = 0.025 / 0.18 * (dq2 - dq1);
}

void lighting(void){
    GLfloat light_ambient[] =  {0.2, 0.2, 0.2, 1.0};
    GLfloat light_diffuse[] =  {0.4, 0.4, 0.4, 1.0};
    GLfloat light_specular[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat light_position[] = {2, 0.1, 7, 1.0};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void model_cylinder(GLUquadricObj * object, GLdouble lowerRadius, GLdouble upperRadius, GLdouble length, GLint res, GLfloat *color1, GLfloat *color2){
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color1);
      glTranslatef(0,0,-length/2);
        gluCylinder(object, lowerRadius, upperRadius, length, 20, res);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color2);
      gluDisk(object, 0.01, lowerRadius, 20, res); 
      glTranslatef(0, 0, length);
      gluDisk(object, 0.01, upperRadius, 20, res); 
    glPopMatrix();
  }

void model_box(GLfloat width, GLfloat depth, GLfloat height, GLfloat *color1, GLfloat *color2, GLfloat *color3, int color)
  {
     width=width/2.0;depth=depth/2.0;height=height/2.0;
     glBegin(GL_QUADS);
  // top
      if (color==1) 
          glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color1);
      glVertex3f(-width,-depth, height);
      glVertex3f( width,-depth, height);
      glVertex3f( width, depth, height);
      glVertex3f(-width, depth, height);
     glEnd();
     glBegin(GL_QUADS);
  // bottom
      if (color==1) 
          glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color1);
      glVertex3f(-width,-depth,-height);
      glVertex3f( width,-depth,-height);
      glVertex3f( width, depth,-height);
      glVertex3f(-width, depth,-height);
     glEnd();
     glBegin(GL_QUAD_STRIP);
  // sides
      if (color==1) 
          glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color2);
      glVertex3f(-width,-depth,height);
      glVertex3f(-width,-depth,-height);
      glVertex3f(width,-depth,height);
      glVertex3f(width,-depth,-height);
      glVertex3f(width,depth,height);
      glVertex3f(width,depth,-height);
      glVertex3f(-width,depth,height);
      glVertex3f(-width,depth,-height);
      glVertex3f(-width,-depth,height);
     glEnd();
  }

int loadGLTexture(const char *filename, int width, int height) {
    // Open texture data
    free(data);

    // data = gImageLoadPPM(filename, &width, &height);
    
    // Pastikan ukuran file tidak besar hanya 500x500
    FILE *fileImage = fopen(filename, "r");
    if (fileImage == NULL) return 0;

    // Allocate buffer
    data = (unsigned char*) malloc(width * height * 3);

    // Read texture data
    fread(data, width * height * 3, 1, fileImage);
    fclose(fileImage);

    unsigned int textureID;
    int border = 0;
    int depth = width * height * 3;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture colors should replace the original color values
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // GL_MODULATE mengikuti warna dasar

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    return textureID;
}

float newx(float x, float y) {
    return rX + x * cos(shi) - y * sin(shi);
}

float newy(float x, float y) {
    return rY + x * sin(shi) + y * cos(shi);
}

void drawOneLine(double x1, double y1, double x2, double y2) {
    glBegin(GL_LINES); 
    glVertex3f((x1), (y1), 0.0); 
    glVertex3f((x2), (y2), 0.0); 
    glEnd();
}

void drawCylinder(float x, float y, float z, float radius, float height){
    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(90, 0, 1, 0); // Menegakkan roda
    gluCylinder(quad, radius, radius, height, 20, 20);
    glPopMatrix();
    gluDeleteQuadric(quad);
}

void drawRobot(void){
    glPushMatrix();
    glRotatef(theta, 0, 0, 1);
    glTranslatef(xmove, ymove, 0.0);

    //Bagian Belakang
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray);
    glBegin(GL_QUADS);
    glVertex3f(-0.09, -0.1625, 0.02);
    glVertex3f(0.09, -0.1625, 0.02);
    glVertex3f(0.09, -0.1375, 0.02);
    glVertex3f(-0.09, -0.1375, 0.02);
    glEnd();

    //Bagian Tengah 1
    glBegin(GL_QUADS);
    glVertex3f(-0.04, -0.1375, 0.02);
    glVertex3f(-0.03, -0.1375, 0.02);
    glVertex3f(-0.03, 0.0, 0.02);
    glVertex3f(-0.04, 0.0, 0.02);
    glEnd();

    //Bagian Tengah 2
    glBegin(GL_QUADS);
    glVertex3f(0.04, -0.1375, 0.02);
    glVertex3f(0.03, -0.1375, 0.02);
    glVertex3f(0.03, 0.0, 0.02);
    glVertex3f(0.04, 0.0, 0.02);
    glEnd();

    //Bagian Tengah 3
    glBegin(GL_QUADS);
    glVertex3f(-0.03, -0.065, 0.02);
    glVertex3f(0.03, -0.065, 0.02);
    glVertex3f(0.03, -0.06, 0.02);
    glVertex3f(-0.03, -0.06, 0.02);
    glEnd();

    //Bagian Depan
    glBegin(GL_QUADS);
    glVertex3f(-0.05, 0.0, 0.02);
    glVertex3f(0.05, 0.0, 0.02);
    glVertex3f(0.05, 0.02, 0.02);
    glVertex3f(-0.05, 0.02, 0.02);
    glEnd();

    // Wheels (Menegakkan roda dan memberikan sumbu putar yang sesuai)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
    drawCylinder(-0.1, -0.15, 0.02, 0.0125, 0.01);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
    drawCylinder(0.09, -0.15, 0.02, 0.0125, 0.01);

    glPopMatrix();
}

void disp_robot(void){
    glPushMatrix();
      glTranslatef(rX, rY, 0.02);
      glRotatef(shi*RTD, 0, 0, 1);
      model_box(0.02, 0.18, 0.01, gray8, gray8 ,gray8, 1);
      glPushMatrix();
      glTranslatef(0.145, 0, 0);
      model_box(0.02, 0.1, 0.01, gray8, gray8 ,gray8, 1);
      glTranslatef(-0.07, 0.02, 0);   
      model_box(0.14, 0.01, 0.01, gray8, gray8 ,gray8, 1);
      glTranslatef(0, -0.04, 0);
      model_box(0.14, 0.01, 0.01, gray8, gray8 ,gray8, 1);
      glPopMatrix();
      glTranslatef(0.0, 0.09, 0.005);
      glRotatef(-90, 1, 0, 0);
      model_cylinder(obj, 0.025, 0.025, 0.01, 2, yellow2 , blue2);
      glTranslatef(0.0, 0.0, -0.18);
      glRotatef(180, 1, 0, 0);
      model_cylinder(obj, 0.025, 0.025, 0.01, 2, yellow2 , blue2);
     glPopMatrix();
  }

void disp_floor(bool grid, unsigned int textureNumber) {
    int i,j,flagc=1;

    glPushMatrix();
        GLfloat dx=4.5,dy=4.5;
        GLint amount=15;
        GLfloat x_min=-dx/2.0, x_max=dx/2.0, x_sp=(GLfloat) dx/amount, y_min=-dy/2.0, y_max=dy/2.0, y_sp=(GLfloat) dy/amount;
  
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
        for(i = 0; i<=48; i++){
       drawOneLine(-2.4+0.1*i, -2.4,       -2.4+0.1*i,  2.4);
       drawOneLine(-2.4,       -2.4+0.1*i,  2.4,       -2.4+0.1*i);
    }
    glPopMatrix();

   // Mengaktifkan tekstur untuk lantai
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureNumber); // Mengikat tekstur dari track.ppm
            glColor3f(1.0f, 1.0f, 1.0f); // Pastikan warna tidak mengubah tekstur
            glBegin(GL_POLYGON);
            // glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f, -1.0f, 0);
            // glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f, -1.0f, 0);
            // glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f,  1.0f, 0);
            // glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f,  1.0f, 0);
            glTexCoord2f(0,1); glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(0,0); glVertex3f(-1.0f, 1.0f, 0.0f);
            glTexCoord2f(1,0); glVertex3f(1.0f, 1.0f, 0.0f);
            glTexCoord2f(1,1); glVertex3f(1.0f, -1.0f, 0.0f);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void display_main() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear The Screen and The Depth Buffer
    
    disp_floor(true, textureNumber);
    // drawRobot();
    disp_robot();

    glutSwapBuffers();
}

void display_backtop() {
    float rZ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 2.0, 0.2, 8.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    lighting();

    glShadeModel(GL_SMOOTH);

    // Mengatur tampilan kamera dari atas belakang
    // Koordinat kamera terhadap basis robot
    float sense_x = -0.2, sense_y = 0, sense_z = 0.3;
    float floor_x = 0.3, floor_y = 0.0, floor_z = 0.0;

    // Konversi ke koordinat dunia dengan mempertimbangkan rotasi robot
    float cam_x = rX + (sense_x * cos(shi) - sense_y * sin(shi));
    float cam_y = rY + (sense_x * sin(shi) + sense_y * cos(shi));
    float cam_z = rZ + sense_z;

    float look_x = rX + (floor_x * cos(shi) - floor_y * sin(shi));
    float look_y = rY + (floor_x * sin(shi) + floor_y * cos(shi));
    float look_z = rZ + floor_z;

    // Menentukan perspektif dengan gluLookAt()
    gluLookAt(cam_x, cam_y, cam_z, // Posisi kamera
              look_x, look_y, look_z, // Titik yang dilihat
              0.0, 0.0, 1.0); // Arah atas (z positif)
    // Draw the floor and robot
    disp_floor(true, textureNumber);
    // drawRobot();
    disp_robot();
    glutSwapBuffers();
}

void display_ir(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(img_width, img_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, image_raw);
    glutSwapBuffers();
}

void display_sensor() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Inisialisasi koordinat dasar lantai dan sensor
    float floor_x = 0.145 + 0.005, floor_y = 0, floor_z = 0;
    float sense_x = 0.145, sense_y = 0, sense_z = 0.2;

    float floor_x_ = newx(floor_x, floor_y);
    float floor_y_ = newy(floor_x, floor_y);
    float sense_x_ = newx(sense_x, sense_y);
    float sense_y_ = newy(sense_x, sense_y);

    glFrustum(-0.05, 0.05, 0.01, -0.01, 0.19, 1);
    gluLookAt(sense_x_, sense_y_, sense_z, floor_x_, floor_y_, floor_z, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    disp_floor(true, textureNumber);
    lighting();
    glShadeModel(GL_SMOOTH);
    
    glutSwapBuffers();

    glPixelTransferf(GL_RED_SCALE, 0.3333 * 0.2);
    glPixelTransferf(GL_GREEN_SCALE, 0.3333 * 0.2);
    glPixelTransferf(GL_BLUE_SCALE, 0.3333 * 0.2);

    glReadPixels(0, 0, img_width, img_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, image_raw);

    ir8 = (image_raw[50][sen8] < 50) ? 1 : 0;
    ir7 = (image_raw[50][sen7] < 50) ? 1 : 0;
    ir6 = (image_raw[50][sen6] < 50) ? 1 : 0;
    ir5 = (image_raw[50][sen5] < 50) ? 1 : 0;
    ir4 = (image_raw[50][sen4] < 50) ? 1 : 0;
    ir3 = (image_raw[50][sen3] < 50) ? 1 : 0;
    ir2 = (image_raw[50][sen2] < 50) ? 1 : 0;
    ir1 = (image_raw[50][sen1] < 50) ? 1 : 0;

    image_raw[50][sen8] = ir8 * 255;
    image_raw[50][sen7] = ir7 * 255;
    image_raw[50][sen6] = ir6 * 255;
    image_raw[50][sen5] = ir5 * 255;
    image_raw[50][sen4] = ir4 * 255;
    image_raw[50][sen3] = ir3 * 255;
    image_raw[50][sen2] = ir2 * 255;
    image_raw[50][sen1] = ir1 * 255;
}

void keyboard(unsigned char key, int i, int j){
    if (key == ESCkey) exit(1);
}

void main_window() {
    // Main Window size
    glutInitWindowSize(800, 400);  // Set the window's initial width & height
    glutInitWindowPosition(40, 100);  // Position the window's initial top-left corner

    // Open a window
    window = glutCreateWindow("Simple Window");  // Create a window with the given title

    // Clear background to (Red, Green, Blue, Alpha)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 2.0, 0.2, 8);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.2, -1.5, 2.0, 0.0, 0.2, 0.2, 0.0, 0.0, 1.0);
    
    lighting();

    glShadeModel(GL_SMOOTH);

    glutDisplayFunc(&display_main);
    glutKeyboardFunc(&keyboard);
}

void backtop_window(void) {
    // Menentukan ukuran dan posisi jendela kamera back-top
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(200, 100);
 
    // Membuka jendela dengan nama "Back-top Camera"
    backtopWindow = glutCreateWindow("Back-top Camera");
 
    // Mengatur warna latar belakang jendela
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
 
    // Menautkan fungsi tampilan 
    glutDisplayFunc(&display_backtop);
}

void camera_window(void) {
    glutInitWindowSize(img_width, img_height);
    glutInitWindowPosition(500, 100);
 
    sensorWindow = glutCreateWindow("Camera Sensor");
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
 
    glutDisplayFunc(&display_sensor);
}

void ir_window(void) {
    glutInitWindowSize(img_width, img_height);
    glutInitWindowPosition(500, 100);
 
    irWindow = glutCreateWindow("Tampak Sensor");
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
 
    glutDisplayFunc(&display_ir);
}

void Animate(int k) {
    float dt = 0.1;
    float Kp = 0.02;
    float Ki = 0.01;
    float Kd = 0.01;
 
    static float rv = 0;
    static float control = 0;
    static int integratedSensor = 0;
    static float derivative = 0;
    static float baseSpeed = 1;
    static float error = 0;
    static float error_old = 0;
    float dq1 = 0; float dq2 = 0;
    
    if(strcmp(track, "track3.ppm") == 0) {
        sensor = ((ir1) ? -12 : 0) + ((ir2) ? -9 : 0) + ((ir3) ? -6 : 0) + ((ir4) ? -3 : 0) + ((ir5) ? 3 : 0) + ((ir6) ? 6 : 0) + ((ir7) ? 9 : 0) + ((ir8) ? 12 : 0);
    } else if(strcmp(track, "track2.ppm") == 0) {
        sensor = ((ir1) ? -8 : 0) + ((ir2) ? -6 : 0) + ((ir3) ? -4 : 0) + ((ir4) ? -2 : 0) + ((ir5) ? 2 : 0) + ((ir6) ? 4 : 0) + ((ir7) ? 6 : 0) + ((ir8) ? 8 : 0);
    } else if(strcmp(track, "track.ppm") == 0) {
        sensor = ((ir1) ? -4 : 0) + ((ir2) ? -3 : 0) + ((ir3) ? -2 : 0) + ((ir4) ? -1 : 0) + ((ir5) ? 1 : 0) + ((ir6) ? 2 : 0) + ((ir7) ? 3 : 0) + ((ir8) ? 4 : 0);
    }
    

    nilaisensor = ir1 + ir2 + ir3 + ir4 + ir5 + ir6 + ir7 + ir8; // Simulasi input PID

    if (nilaisensor != 0){
        error = (float) sensor;
        // float error = (float) sensor/nilaisensor;

        integratedSensor += error * dt;
        derivative = (error - error_old) / dt;
        error_old = error;

        control = Kp * error + Ki * integratedSensor + Kd * derivative; 

        dq1 = baseSpeed + control;
        dq2 = baseSpeed - control;

        if (dq1 > 1) dq1 = 1;
        if (dq2 > 1) dq2 = 1;
        if (dq1 < -1) dq1 = -1;
        if (dq2 < -1) dq2 = -1;

        q1 += dq1;
        q2 += dq2;

        shi += 0.025/0.18 * (q2-q1);
    } else{
        dq1 = baseSpeed;
        dq2 = baseSpeed;

        q1 += dq1;
        q2 += dq2;

        integratedSensor = 0;
        shi = shi_old;
    }

    // Hitung perubahan posisi dan orientasi menggunakan Jacobian
    Jacobian(dx, dy, dshi, dq2, dq1, shi);
    rv = dx * cos(shi) + dy * sin(shi);
    rX = rX + rv * cos((shi + shi_old) / 2.0);
    rY = rY + rv * sin((shi + shi_old) / 2.0);

    shi_old = shi;

    xmove = rX;
    ymove = rY;
    theta = shi*RTD;

    printf("Sensor: %d, Error: %f, Control: %f\n", sensor, error, control);
    printf("dq1: %f, dq2: %f\n", dq1, dq2);
    printf("rX: %f, rY: %f, shi: %f\n", rX, rY, shi);
} 

void animate(int k) {
    float dt = 0.1;
    // float Kp = 0.15;
    float Kp = 0.1862;
    float Ki = 0.01;
    float Kd = 0.01;

    static float rv = 0;
    static float control = 0;
    static int integratedSensor = 0;
    static float derivative = 0;
    // static float baseSpeed = 0.5;
    static float baseSpeed = 1;
    static float error = 0;
    static float error_old = 0;
    float dq1 = 0; float dq2 = 0;
    // float K_line = 0.6; // tuning gain
    float K_line = 0.631; // tuning gain
    static float theta_cmd = 0.0;
    static float vinput = 0.0;
    static float dutyCycle = 0.1;
    static float theta_motor = 0.0;
    static float theta_motor_old = 0.0;
    static float dtheta = 0.0;
    
    sensor = ((ir1) ? -1 : 0) + ((ir2) ? -1 : 0) + ((ir3) ? -1 : 0) + ((ir4) ? -1 : 0) + ((ir5) ? 1 : 0) + ((ir6) ? 1 : 0) + ((ir7) ? 1 : 0) + ((ir8) ? 1 : 0);
    nilaisensor = ir1 + ir2 + ir3 + ir4 + ir5 + ir6 + ir7 + ir8; // Simulasi input PID

    if (nilaisensor != 0){
        theta_cmd = (float) sensor / nilaisensor * K_line; // Menghitung sudut target berdasarkan sensor
        error = theta_cmd - theta_motor_old; // Menghitung error sudut antara target dan aktual

        integratedSensor += error * dt;
        derivative = (error - error_old) / dt;
        error_old = error;

        control = Kp * error + Ki * integratedSensor + Kd * derivative; 

        dutyCycle = control / vbat;
        if (dutyCycle > 1.0) dutyCycle = 1.0;
        if (dutyCycle < -1.0) dutyCycle = -1.0;
        vinput = dutyCycle * vbat;

        theta_motor = ThetaVolRatio * vinput; // Menghitung sudut motor berdasarkan duty cycle
        dtheta = (theta_motor - theta_motor_old) / dt;
        theta_motor_old = theta_motor;

        dq1 = baseSpeed + dtheta;
        dq2 = baseSpeed - dtheta;

        // if (dq1 > 0.5) dq1 = 0.5;
        // if (dq2 > 0.5) dq2 = 0.5;
        // if (dq1 < -0.5) dq1 = -0.5;
        // if (dq2 < -0.5) dq2 = -0.5;

        if (dq1 > 1) dq1 = 1;
        if (dq2 > 1) dq2 = 1;
        if (dq1 < -1) dq1 = -1;
        if (dq2 < -1) dq2 = -1;
        
        q1 += dq1;
        q2 += dq2;

        shi += 0.025/0.18 * (q2-q1);
    } else{
        dq1 = baseSpeed;
        dq2 = baseSpeed;

        q1 += dq1;
        q2 += dq2;

        integratedSensor = 0;
        shi = shi_old;
    }

    // Hitung perubahan posisi dan orientasi menggunakan Jacobian
    Jacobian(dx, dy, dshi, dq2, dq1, shi);
    rv = dx * cos(shi) + dy * sin(shi);
    rX = rX + rv * cos((shi + shi_old) / 2.0);
    rY = rY + rv * sin((shi + shi_old) / 2.0);

    shi_old = shi;

    xmove = rX;
    ymove = rY;
    theta = shi*RTD;

    printf("Sensor: %d, Error: %f, Control: %f\n", sensor, error, control);
    printf("dq1: %f, dq2: %f\n", dq1, dq2);
    printf("rX: %f, rY: %f, shi: %f\n", rX, rY, shi);
} 

void Sim_main(void) {
    static int count=0;
    count++;
    glutSetWindow(window);
    if(strcmp(mode, "servo") == 0) {
        Animate(count);
    } else if(strcmp(mode, "pwm") == 0) {
        animate(count);
    }
    display_main();

    glutSetWindow(backtopWindow);
    display_backtop();

    glutSetWindow(sensorWindow);
    display_sensor();

    glutSetWindow(irWindow);
    display_ir();

    usleep(100000);
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    obj = gluNewQuadric();
    
    // Initialize our windows
    main_window();
    textureNumber = loadGLTexture(track, 500, 500);
    backtop_window();

    textureNumber = loadGLTexture(track, 500, 500);
    camera_window();

    textureNumber = loadGLTexture(track, 500, 500);
    ir_window();

    glutIdleFunc(&Sim_main); 
    glutMainLoop();
    return 0;
}