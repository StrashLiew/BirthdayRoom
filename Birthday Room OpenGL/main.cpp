#define STB_IMAGE_IMPLEMENTATION
#define PI 3.141592

#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <time.h>
#include "Balloon.h"
#include "stb_image.h"
#include "Particle.h"
#include "Confetti.h"
#include "GiftBox.h"
const GLfloat floorLength = 22.0;
const GLfloat wallHeight = 20.0;
GLsizei g_width = 640; // width of the window
GLsizei g_height = 640; //height of the window
float ratio = float(g_width)/g_height; //ratio of window

float yaw = 0.0; // angle of rotation Y axis for the camera direction
float pitch = 0.0;// angle of rotation X axis for the camera direction
GLdouble fov = 45.0; // field of view

float lx=0.0f, ly=0.0f, lz=-1.0f; //vector representing the camera's direction
float x=0.0f, y=5.0f,  z=5.0f; // position of the camera
float movementSpeed = 0.5; //movement speed of camera

//flags that control animations
bool candleLit = true, smoke = true, blow = false, colorTurn = false, lights = true, genText = false;
bool displayConfetti = true, spawningConfetti = true, allowJumping = true;

float smokeX1 = -0.1, smokeX2 = 0.01, smokeX3 = 0.0; //represent the control points of the smoke skipping the first control point
float bannerR = 1.0, bannerG = 1.0, bannerB = 0.0;//represent the colors of the words on banner

static float lightPos[4] = {0.0f, 10.0f, 0.0f, 1.0};
static float lightAmb[4] ={0.5, 0.5, 0.5, 1.0};
static float lightDiff[4]={0.5, 0.5, 0.5, 1.0};
static float lightSpec[4]={0.3, 0.3, 0.3, 1.0};
static float dimAmb[4] ={0.1, 0.1, 0.35, 1.0};
static float dimDiff[4] ={0.1, 0.1, 0.4, 1.0};

static float matAmb[4]  = {0.2, 0.2, 0.2, 1.0};
static float matDiff[4] ={0.4, 0.4, 0.0, 1.0};
static float matSpec[4] ={0.4, 0.4, 0.0, 1.0};
static float matShininess[1] = {1.0};

Balloon * balloon = NULL;
std::vector<GiftBox> gifts;

void animateGiftBox()
{
    int determinant = allowJumping? 1: 0;
    for(unsigned int i=0; i<gifts.size(); i++){
        if(gifts[i].isJumping() || rand()%50<determinant){ // jumps occasionally
            gifts[i].jumps();
        }
    }
}

bool checkBound(float futureX, float futureY, float futureZ){ // check if camera is out of boundary
    if(futureX<-floorLength+0.01||futureX>floorLength-0.01){
        return false;
    }
    if(futureZ<-floorLength||futureZ>floorLength-0.01){
        return false;
    }
    if(futureY<0+0.1||futureY>wallHeight-0.3){
        return false;
    }
    return true;
}

float setGlow(float glow, float color){
    return color / glow;
}

void drawCylinder(float rad, float height){
    //the body of the cylinder
    glBegin(GL_QUAD_STRIP);
    for(int j = 0; j<=360; j+=10){
        glVertex3f(rad*cos(PI/180*j), 0.0f, rad*sin(PI/180*j));
        glVertex3f(rad*cos(PI/180*j), height ,rad*sin(PI/180*j));
    }
    glEnd();

    //the circle top of the cylinder
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f,height,0.0f);
    for(int j = 0; j<=360; j += 5){
        glVertex3f(rad*cos(PI/180*j),height ,rad*sin(PI/180*j));
    }
    glEnd();
}

// draw table
void drawTableBody(){
    glColor3f(0.5f,0.25f,0.25f); //brown
     //draw table body  bottom
        glBegin(GL_QUADS);
            glVertex3f(0.0f,0.0f,0.0f);
            glVertex3f(8.0f,0.0f,0.0f);
            glVertex3f(8.0f,0.0f,2.8f);
            glVertex3f(0.0f,0.0f,2.8f);
        glEnd();

        //draw table body top
        glBegin(GL_QUADS);
            glVertex3f(0.0f,0.5f,0.0f);
            glVertex3f(8.0f,0.5f,0.0f);
            glVertex3f(8.0f,0.5f,2.8f);
            glVertex3f(0.0f,0.5f,2.8f);
        glEnd();

     //draw table body back side
        glBegin(GL_QUADS);
            glVertex3f(0.0f,0.0f,0.0f);
            glVertex3f(8.0f,0.0f,0.0f);
            glVertex3f(8.0f,0.5f,0.0f);
            glVertex3f(0.0f,0.5f,0.0f);
        glEnd();

        //draw table body front side
        glBegin(GL_QUADS);
            glVertex3f(0.0f,0.0f,2.8f);
            glVertex3f(8.0f,0.0f,2.8f);
            glVertex3f(8.0f,0.5f,2.8f);
            glVertex3f(0.0f,0.5f,2.8f);
        glEnd();

    //draw table body left  side
        glBegin(GL_QUADS);
            glVertex3f(0.0f,0.0f,0.0f);
            glVertex3f(0.0f,0.5f,0.0f);
            glVertex3f(0.0f,0.5f,2.8f);
            glVertex3f(0.0f,0.0f,2.8f);
        glEnd();

        //draw table body  right side
        glBegin(GL_QUADS);
            glVertex3f(8.0f,0.0f,0.0f);
            glVertex3f(8.0f,0.5f,0.0f);
            glVertex3f(8.0f,0.5f,2.8f);
            glVertex3f(8.0f,0.0f,2.8f);
        glEnd();

}

void drawLegs(float height){
    glColor3f(0.66f, 0.33f,0.33f); //light brown

    glBegin(GL_QUADS); //front
            glVertex3f(0.0f,0.0f,0.0f);
            glVertex3f(0.2f,0.0f,0.0f);
            glVertex3f(0.2f,height,0.0f);
            glVertex3f(0.0f,height,0.0f);
    glEnd();

    glBegin(GL_QUADS); //back
            glVertex3f(0.0f,0.0f,0.2f);
            glVertex3f(0.2f,0.0f,0.2f);
            glVertex3f(0.2f,height,0.2f);
            glVertex3f(0.0f,height,0.2f);
    glEnd();

    glBegin(GL_QUADS); //left
            glVertex3f(0.0f,0.0f,0.0f);
            glVertex3f(0.0f,0.0f,0.2f);
            glVertex3f(0.0f,height,0.2f);
            glVertex3f(0.0f,height,0.0f);
    glEnd();

    glBegin(GL_QUADS); //right
            glVertex3f(0.2f,0.0f,0.0f);
            glVertex3f(0.2f,0.0f,0.2f);
            glVertex3f(0.2f,height,0.2f);
            glVertex3f(0.2f,height,0.0f);
    glEnd();
}

void drawSupport(){
    glColor3f(0.66f, 0.33f,0.33f); //light brown
     glBegin(GL_QUADS); //front
            glVertex3f(0.0f,0.0f, 0.0f);
            glVertex3f(0.2f,0.0f, 0.0f);
            glVertex3f(0.2f,1.41f,0.0f);
            glVertex3f(0.0f,1.41f,0.0f);
    glEnd();

    glBegin(GL_QUADS); //back
            glVertex3f(0.0f,0.0f, 0.2f);
            glVertex3f(0.2f,0.0f, 0.2f);
            glVertex3f(0.2f,1.41f,0.2f);
            glVertex3f(0.0f,1.41f,0.2f);
    glEnd();

    glBegin(GL_QUADS); //left
            glVertex3f(0.0f,0.0f, 0.0f);
            glVertex3f(0.0f,0.0f, 0.2f);
            glVertex3f(0.0f,1.41f,0.2f);
            glVertex3f(0.0f,1.41f,0.0f);
    glEnd();

    glBegin(GL_QUADS); //right
            glVertex3f(0.2f,0.0f, 0.0f);
            glVertex3f(0.2f,0.0f, 0.2f);
            glVertex3f(0.2f,1.41f,0.2f);
            glVertex3f(0.2f,1.41f,0.0f);
    glEnd();
}

void drawTable(){

    //draw table body
    glPushMatrix();
    glTranslatef(-4.0f,3.0f,-1.25f);
    drawTableBody();
    glPopMatrix();

    //draw table legs x4
    for(float j = -0.9; j < 0.8; j+= 1.6){
        for(float i = -3.9; i <=3.7 ; i+=7.6){
            glPushMatrix();
            glTranslatef(i,0.0f,j);
            drawLegs(3.0f);
            glPopMatrix();
        }
    }

    //draw leg supports for left legs
    for(float j = -0.9; j < 0.8; j+=1.6){
        glPushMatrix();
        glTranslatef(-3.84f,2.28f,j);
        glRotatef(45, 0.0f,0.0f,-1.0f);
        drawSupport();
        glPopMatrix();
    }

    //draw leg supports for right legs
    for(float j = -0.9; j < 0.8; j+=1.6){
        glPushMatrix();
        glTranslatef(3.7f,2.28f,j);
        glRotatef(45, 0.0f,0.0f,1.0f);
        drawSupport();
        glPopMatrix();
    }
}

//draw chair
void drawChair(){
    glColor3f(0.5f,0.25f,0.25f); //brown

    glPushMatrix();
    glTranslatef(0.0f,2.5,0.0f);
    drawCylinder(1,0.25);  //draw chair body
    for(int i = 0; i<=360; i+= 2){
        glBegin(GL_POLYGON); //draw bottom of chair
         glVertex3f(1*cos(PI/180*i),0,1*sin(PI/180*i));
        glEnd();
    }
    glPopMatrix();

    //draw table legs x4
    for(float j = -0.65; j < 0.6; j+= 1.15){
        for(float i = -0.75; i < 0.6 ; i+=1.25){
            glPushMatrix();
            glTranslatef(i,0.0f,j);
            drawLegs(2.5);
            glPopMatrix();
        }
    }
}

//draw candle and smoke
void drawSmoke(){
    glPointSize(0.8);
    glColor3f(0.5f, 0.5f, 0.5f); //rey
    //control points of Bezier curve
    float ctrlpoints[4][3] = {{ 0, 0, 0 }, { smokeX1, 0.22, 0 }, {smokeX2, 0.88, 0}, {smokeX3, 1.0f, 0 }};
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
    glBegin(GL_POINTS); //left side of flame
    for (int i = 0; i <= 100; i++)
        glEvalCoord1f((GLfloat)i / 80.0);
    glEnd();
}

void animateSmoke(){
    if(blow == false){
        return;
    }
    else{
        smokeX1 += 0.0025;
        smokeX2 -= 0.0025;
        smokeX3 -= 0.0025;
        if(smokeX1 > 5.0){
            smoke = false;
        }
    }
}

void drawCandle(){
    //candle body
    //glColor3f(1.0f,0.0f,0.0f); //red
    glColor3f(setGlow(0.1,1),setGlow(0.1,0),setGlow(0.1,0));
    drawCylinder(0.05f, 0.5f);

    glTranslatef(0.0f,0.5f,0.0f);
    //candle flame
    if(candleLit == true){
        glPointSize(1.0);
        //control points of Bezier curve
        float ctrlpoints[4][3] = {{ 0, 0, 0 }, { -0.1, 0.05, 0 }, {0.02, 0.075, 0}, { 0, 0.25f, 0 }};
        glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
       glPushMatrix();
       glColor3f(setGlow(0.35,1), setGlow(0.35,0.61),setGlow(0.35,0.22)); //orange glow
       for(float angle = 0; angle <360; angle+=1 ){
            glRotatef(angle,0.0f,1.0f,0.0f);
            glBegin(GL_POLYGON); //left side of flame
            for (int i = 0; i <= 150; i++)
                glEvalCoord1f((GLfloat)i / 150.0);
            glEnd();
       }
        glPopMatrix();
    }
    if(smoke == true){
        glTranslatef(0.0f,0.25f,0.0f);
        drawSmoke();
    }
};

//draw cake
void drawIcing(float rad, float height){
    glColor3f(setGlow(0.01,1),1,1);
    glPushMatrix();
        glTranslatef(0.0, GLfloat(height), 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glutSolidTorus(0.035, rad, 10, 20);
    glPopMatrix();
}

void drawCake(){

    glPushMatrix();
    glTranslatef(0.0f,3.51f,0.0f);

    //base on bottom of cake
    float rad = 1.1;
    float layerHeight = 0.0;
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_POLYGON);
    for(int j = 0; j<=360; j+=10){
        glVertex3f( rad*cos(PI/180*j), 0.0f, rad*sin(PI/180*j));
    }
    glEnd();

    //draw icing on bottom of cake
    drawIcing(rad-0.09, layerHeight);

    //bottom layer cake
    rad = 1;
    layerHeight = 1.0f;
    glColor3f(0.957f,0.576f,0.957f); //light pink
    drawCylinder(rad, layerHeight); //cake body
    drawIcing(rad, layerHeight);  //draw icing

    //mid layer cake
    rad = 0.8;
    layerHeight = 0.8f;
    glColor3f(0.957f,0.576f,0.957f);
    glTranslatef(0.0f, 1.0f, 0.0f);
    drawCylinder(rad, layerHeight); //cake body
    drawIcing(rad, layerHeight);//draw icing

    //top layer cake
    rad = 0.5;
    layerHeight = 0.5f;
    glColor3f(0.957f,0.576f,0.957f);
    glTranslatef(0.0f, 0.8f, 0.0f);
    drawCylinder(rad, layerHeight); //cake body
    drawIcing(rad, layerHeight); //draw icing

    glTranslatef(0.0f, 0.5f, 0.0f);
    drawCandle();
    glPopMatrix();
}

//draw cup
void drawCup(){
    glColor3f(0.6f,0.8f,0.8f);
    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.3,0.8);

    //draw water inside cup
    glColor3f(0.5f,1.0f,1.0f);
    glTranslatef(0.0f,0.2f,0.0f);
    glBegin(GL_POLYGON);
    for(int j = 0; j<=360; j += 1){
        glVertex3f(0.3*cos(PI/180*j),0,0.3*sin(PI/180*j));
    }
    glEnd();
    glPopMatrix();
}

//the HAPPY BIRTHDAY letters
void drawH(float height, float width){
    //draw H
    glBegin(GL_POLYGON); //left side
        glVertex3f(0.0f   , height, 1.5f);
        glVertex3f(0.0f   ,   0.0f, 1.5f);
        glVertex3f(width/4,   0.0f, 1.5f);
        glVertex3f(width/4, height, 1.5f);
    glEnd();
    glBegin(GL_POLYGON); //middle
        glVertex3f(width/4  ,  height/2-0.5, 1.5f);
        glVertex3f(width*3/4,  height/2-0.5, 1.5f);
        glVertex3f(width*3/4,  height/2+0.5, 1.5f);
        glVertex3f(width/4  ,  height/2+0.5, 1.5f);
   glEnd();
   glBegin(GL_POLYGON); //right
        glVertex3f(width*3/4,   0.0f, 1.5f);
        glVertex3f(width    ,   0.0f, 1.5f);
        glVertex3f(width    , height, 1.5f);
        glVertex3f(width*3/4, height, 1.5f);
    glEnd();
}

void drawA(float height, float width){
    //draw A
    glBegin(GL_POLYGON); // tilted left side
        glVertex3f(0.0f       ,0.0f  ,1.5f);
        glVertex3f(width/4    ,0.0f  ,1.5f);
        glVertex3f(width/2+0.2,height,1.5f);
        glVertex3f(width/4+0.2,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // tilted right side
        glVertex3f(width*3/4    ,0.0f  ,1.5f);
        glVertex3f(width        ,0.0f  ,1.5f);
        glVertex3f(width*3/4-0.2,height,1.5f);
        glVertex3f(width/2-0.2  ,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // middle bridge of A
        glVertex3f(width/4  ,height*4/10-0.5,1.5f);
        glVertex3f(width*3/4,height*4/10-0.5,1.5f);
        glVertex3f(width*3/4,height*4/10+0.5,1.5f);
        glVertex3f(width/4  ,height*4/10+0.5,1.5f);
    glEnd();
}

void drawP(float height, float width){
 //draw P
    glBegin(GL_POLYGON); //left side of P
        glVertex3f(0.0f   ,0.0f  ,1.5f);
        glVertex3f(width/4,0.0f  ,1.5f);
        glVertex3f(width/4,height,1.5f);
        glVertex3f(0.0f   ,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // top horizontal of P
        glVertex3f(width/4  ,height*4/5,1.5f);
        glVertex3f(width*3/4,height*4/5,1.5f);
        glVertex3f(width*3/4,height    ,1.5f);
        glVertex3f(width/4  ,height    ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // bottom horizontal of P
        glVertex3f(width/4  ,height*2/5,1.5f);
        glVertex3f(width*3/4,height*2/5,1.5f);
        glVertex3f(width*3/4,height*3/5,1.5f);
        glVertex3f(width/4  ,height*3/5,1.5f);
    glEnd();

    glBegin(GL_POLYGON); //right vertical of P
        glVertex3f(width*3/4,height*2/5,1.5f);
        glVertex3f(width    ,height*2/5,1.5f);
        glVertex3f(width    ,height    ,1.5f);
        glVertex3f(width*3/4,height    ,1.5f);
    glEnd();
}

void drawY(float height, float width){
     //draw Y
    glBegin(GL_POLYGON); //left branch of Y
        glVertex3f(width*3/8,height/2,1.5f); //bottom left going counterclockwise
        glVertex3f(width*5/8,height/2,1.5f);
        glVertex3f(width/4  ,height  ,1.5f);
        glVertex3f(0.0f     ,height  ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // right branch of Y
        glVertex3f(width*3/8,height/2,1.5f); //bottom left going counterclockwise
        glVertex3f(width*5/8,height/2,1.5f);
        glVertex3f(width    ,height  ,1.5f);
        glVertex3f(width*3/4,height  ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // middle part of Y
        glVertex3f(width*3/8,0.0f    ,1.5f); //bottom left going counterclockwise
        glVertex3f(width*5/8,0.0f    ,1.5f);
        glVertex3f(width*5/8,height/2,1.5f);
        glVertex3f(width*3/8,height/2,1.5f);
    glEnd();
}

void drawB(float height, float width){
     //draw B
    glBegin(GL_POLYGON); // left vertical side of B
        glVertex3f(0.0f     ,0.0f  ,1.5f); //bottom left going counterclockwise
        glVertex3f(width*3/8,0.0f  ,1.5f);
        glVertex3f(width*3/8,height,1.5f);
        glVertex3f(0.0f     ,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // top horizontal of B
        glVertex3f(width*3/8,height*4/5,1.5f); //bottom left going counterclockwise
        glVertex3f(width*5/8,height*4/5,1.5f);
        glVertex3f(width*5/8,height    ,1.5f);
        glVertex3f(width*3/8,height    ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // mid horizontal of B
        glVertex3f(width*3/8,height*2/5,1.5f); //bottom left going counterclockwise
        glVertex3f(width*6/8,height*2/5,1.5f);
        glVertex3f(width*6/8,height*3/5,1.5f);
        glVertex3f(width*3/8,height*3/5,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // bottom horizontal of B
        glVertex3f(width*3/8,0.0f    ,1.5f); //bottom left going counterclockwise
        glVertex3f(width*6/8,0.0f    ,1.5f);
        glVertex3f(width*6/8,height/5,1.5f);
        glVertex3f(width*3/8,height/5,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // right bottom vertical side of B
        glVertex3f(width*6/8,0.0f        ,1.5f); //bottom left going counterclockwise
        glVertex3f(width    ,height/10   ,1.5f);
        glVertex3f(width    ,height*2.5/5,1.5f);
        glVertex3f(width*6/8,height*3/5  ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // right top vertical side of B
        glVertex3f(width*5/8,height*3/5 ,1.5f); //bottom left going counterclockwise
        glVertex3f(width*6/8,height*3/5 ,1.5f);
        glVertex3f(width*6/8,height*9/10,1.5f);
        glVertex3f(width*5/8,height     ,1.5f);
    glEnd();
}

void drawI(float height, float width){
    //draw I
    glBegin(GL_POLYGON); //middle of I
        glVertex3f(width*3/8,0.0f  ,1.5f); //bottom left going counterclockwise
        glVertex3f(width*5/8,0.0f  ,1.5f);
        glVertex3f(width*5/8,height,1.5f);
        glVertex3f(width*3/8,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // bottom of I
        glVertex3f(0.0f ,0.0f    ,1.5f); //bottom left going counterclockwise
        glVertex3f(width,0.0f    ,1.5f);
        glVertex3f(width,height/5,1.5f);
        glVertex3f(0.0f ,height/5,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // top of I
        glVertex3f(0.0f ,height*4/5,1.5f); //bottom left going counterclockwise
        glVertex3f(width,height*4/5,1.5f);
        glVertex3f(width,height    ,1.5f);
        glVertex3f(0.0f ,height    ,1.5f);
    glEnd();
}

void drawR(float height, float width){
    //draw R
    glBegin(GL_POLYGON); //left side of R
        glVertex3f(0.0f     ,0.0f  ,1.5f);
        glVertex3f(width*3/8,0.0f  ,1.5f);
        glVertex3f(width*3/8,height,1.5f);
        glVertex3f(0.0f     ,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // top horizontal of R
        glVertex3f(width*3/8,height*4/5,1.5f);
        glVertex3f(width*5/8,height*4/5,1.5f);
        glVertex3f(width*5/8,height    ,1.5f);
        glVertex3f(width*3/8,height    ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // bottom horizontal of R
        glVertex3f(width*3/8,height*2/5,1.5f);
        glVertex3f(width*5/8,height*2/5,1.5f);
        glVertex3f(width*5/8,height*3/5,1.5f);
        glVertex3f(width*3/8,height*3/5,1.5f);
    glEnd();

    glBegin(GL_POLYGON); //right vertical of R
        glVertex3f(width*5/8,height*2/5,1.5f);
        glVertex3f(width    ,height*2/5,1.5f);
        glVertex3f(width    ,height    ,1.5f);
        glVertex3f(width*5/8,height    ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); //right diagonal of R
        glVertex3f(width*5/8,0.0f      ,1.5f);
        glVertex3f(width    ,0.0f      ,1.5f);
        glVertex3f(width*5/8,height*2/5,1.5f);
        glVertex3f(width*3/8,height*2/5,1.5f);
    glEnd();

}

void drawT(float height, float width){
    //draw T
    glBegin(GL_POLYGON); //middle of T
        glVertex3f(width*3/8,0.0f,1.5f); //bottom left going counterclockwise
        glVertex3f(width*5/8,0.0f,1.5f);
        glVertex3f(width*5/8,height,1.5f);
        glVertex3f(width*3/8,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // top of T
        glVertex3f(0.0f ,height*4/5,1.5f); //bottom left going counterclockwise
        glVertex3f(width,height*4/5,1.5f);
        glVertex3f(width,height    ,1.5f);
        glVertex3f(0.0f ,height    ,1.5f);
    glEnd();
}

void drawD(float height, float width){
    //draw D
    glBegin(GL_POLYGON); //left side of D
        glVertex3f(0.0f     ,0.0f  ,1.5f);
        glVertex3f(width*3/8,0.0f  ,1.5f);
        glVertex3f(width*3/8,height,1.5f);
        glVertex3f(0.0f     ,height,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // top horizontal of D
        glVertex3f(width*3/8,height*4/5,1.5f);
        glVertex3f(width*6/8,height*4/5,1.5f);
        glVertex3f(width*6/8,height    ,1.5f);
        glVertex3f(width*3/8,height    ,1.5f);
    glEnd();

    glBegin(GL_POLYGON); // bottom horizontal of D
        glVertex3f(width*3/8,0.0f    ,1.5f);
        glVertex3f(width*6/8,0.0f    ,1.5f);
        glVertex3f(width*6/8,height/5,1.5f);
        glVertex3f(width*3/8,height/5,1.5f);
    glEnd();

    glBegin(GL_POLYGON); //right vertical of D
        glVertex3f(width*6/8,0.0f       ,1.5f);
        glVertex3f(width    ,height/10  ,1.5f);
        glVertex3f(width    ,height*9/10,1.5f);
        glVertex3f(width*6/8,height     ,1.5f);
    glEnd();

}

//draw HAPPY BIRTHDAY
void drawBanner(){
    glColor3f(setGlow(0.1,bannerR),setGlow(0.1,bannerG),setGlow(0.1,bannerB));
    float height = 6, width = 4, blank = 1;
    glTranslatef(-(width+blank)*2, wallHeight/2, -floorLength+0.1);
    drawH(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawA(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawP(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawP(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawY(height,width);
    glTranslatef(-(width+blank)*6, -height-blank, 0.0f);
    drawB(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawI(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawR(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawT(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawH(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawD(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawA(height,width);
    glTranslatef(width+blank, 0.0f, 0.0f);
    drawY(height,width);
}

void animateBanner(){ //blinking words on banner
    if(lights == true){ //reset to yellow
        bannerR = 1.0;
        bannerG = 1.0;
        bannerB = 0.0;
        return;
    }
    if(bannerG > 1.0){
        colorTurn = true;
    }
    if(bannerG < -1.0){
        colorTurn = false;
    }

    if(colorTurn == true){
        bannerR -= 0.0025;
        bannerG -= 0.005;
        bannerB += 0.004;
    }
    if(colorTurn == false){
        bannerR += 0.0025;
        bannerG += 0.005;
        bannerB -= 0.004;
    }
}

//set lighting
void initLight(){
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    if(lights == true){
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
    }
    else{
        glLightfv(GL_LIGHT0, GL_AMBIENT, dimAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dimDiff);
    }
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, matDiff);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, matShininess);
}

// draw scene
void drawWall(){
    glEnable(GL_TEXTURE_2D); //enable texture mapping
    if(!genText){ //apply Texture once
        const char* filename = "C:/Users/User/Desktop/SWE2002106 SWE2002109/Textures/DARKBDAY.jpg";//path and filename
        int width = 0, height = 0, channels = 0;
        stbi_uc *image = stbi_load( filename, &width, &height, &channels, 0 );
        GLuint texture = 0;
        glGenTextures(1, &texture);//generate texture name
        glBindTexture(GL_TEXTURE_2D, texture);//fix to texture target
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//set pixel
        //set the texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free( image );
        genText = true;
    }
    if(lights == true){
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        }
    if(lights == false){
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    glColor3f(setGlow(0.1,bannerR),setGlow(0.1,bannerG),setGlow(0.1,bannerB));
    glBegin(GL_QUADS); //right of default camera
        glTexCoord2f(0.0, 1.0);glVertex3f( floorLength, 0.0f,        floorLength);
        glTexCoord2f(1.0, 1.0);glVertex3f( floorLength, 0.0f,       -floorLength);
        glTexCoord2f(1.0, 0.0);glVertex3f( floorLength, wallHeight, -floorLength);
        glTexCoord2f(0.0, 0.0);glVertex3f( floorLength, wallHeight,  floorLength);
    glEnd();
    glBegin(GL_QUADS); //front of default camera
		glTexCoord2f(0.0, 1.0);glVertex3f(-floorLength, 0.0f,       -floorLength);
		glTexCoord2f(1.0, 1.0);glVertex3f( floorLength, 0.0f,       -floorLength);
		glTexCoord2f(1.0, 0.0);glVertex3f( floorLength, wallHeight, -floorLength);
		glTexCoord2f(0.0, 0.0);glVertex3f(-floorLength, wallHeight, -floorLength);
	glEnd();
	glBegin(GL_QUADS); //behind default camera
        glTexCoord2f(0.0, 1.0);glVertex3f(-floorLength, 0.0f,        floorLength);
		glTexCoord2f(1.0, 1.0);glVertex3f( floorLength, 0.0f,        floorLength);
		glTexCoord2f(1.0, 0.0);glVertex3f( floorLength, wallHeight,  floorLength);
		glTexCoord2f(0.0, 0.0);glVertex3f(-floorLength, wallHeight,  floorLength);
	glEnd();

	glBegin(GL_QUADS); //left of default camera
		glTexCoord2f(0.0, 1.0);glVertex3f(-floorLength, 0.0f,        floorLength);
		glTexCoord2f(1.0, 1.0);glVertex3f(-floorLength, 0.0f,       -floorLength);
		glTexCoord2f(1.0, 0.0);glVertex3f(-floorLength, wallHeight, -floorLength);
		glTexCoord2f(0.0, 0.0);glVertex3f(-floorLength, wallHeight,  floorLength);
	glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawGroundAndCeiling(){
    // Draw ground
    glColor3f(0.4f, 0.4f, 0.4f);
	glBegin(GL_QUADS);
		glVertex3f(-floorLength, 0.0f, -floorLength);
		glVertex3f(-floorLength, 0.0f,  floorLength);
		glVertex3f( floorLength, 0.0f,  floorLength);
		glVertex3f( floorLength, 0.0f, -floorLength);
	glEnd();

    // Draw ceiling
    glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
		glVertex3f(-floorLength, wallHeight, -floorLength);
		glVertex3f(-floorLength, wallHeight,  floorLength);
		glVertex3f( floorLength, wallHeight,  floorLength);
		glVertex3f( floorLength, wallHeight, -floorLength);
	glEnd();
}

std::vector<Confetti> confetti_drops;
void animateConfetti() // and also handle confetti deletion
{
    int erase_until = -1;
    for(unsigned int i=0; i<confetti_drops.size(); i++)
    {
        confetti_drops[i].falls();
        if(confetti_drops[i].outOfRange())
            erase_until = i;
    }
    if(erase_until != -1)
        confetti_drops.erase(confetti_drops.begin(), confetti_drops.begin() + erase_until);
}

void drawConfettiDrop()
{
    for(unsigned int i=0; i<confetti_drops.size(); i++)
        confetti_drops[i].drawConfetti();
}

void spawnConfetti(int foo)
{
    int determinant = spawningConfetti? 1: 0;
    for(GLfloat grid_x=-floorLength; grid_x<floorLength; grid_x+=1.0)
    {
        for(GLfloat grid_z=-floorLength; grid_z<floorLength; grid_z+=1.0) // treat ceiling as 22 by 22 grid
        {
            if(rand()%50<determinant){ // spawn by chance
                GLfloat dx = 0.01*GLfloat(rand()%100); // displacement x
                GLfloat dz = 0.01*GLfloat(rand()%100); // displacement z
                confetti_drops.push_back(Confetti(grid_x + dx, wallHeight, grid_z + dz));
            }
        }
    }
    glutTimerFunc(400, spawnConfetti, 0);
}

void displayFunc()
{
    // Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable light 0
    glEnable( GL_LIGHT0 );

	// Reset transformations
	glLoadIdentity();
	lx = cos(pitch)*sin(yaw);
    ly = sin(pitch);
    lz = cos(pitch)*-cos(yaw);

	// Set the camera
	gluLookAt(	x,    y,     z,
		   	    x+lx, y+ly,  z+lz,
			    0.0f, 1.0f,  0.0f);
    initLight();

    glPushMatrix();
        drawWall();
    glPopMatrix();

    glPushMatrix();
        drawGroundAndCeiling();
    glPopMatrix();

	for(unsigned int i=0; i<gifts.size(); i++){
        gifts[i].drawGiftBox();
	}

	glPushMatrix();
        drawTable();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-2.5f, 3.76f, 0.0f);
        glutSolidTeapot(0.5);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(2.0f, 4.31f,0.0f);
        drawCup();
        glTranslatef(0.5f, 0.0f,1.0f);
        drawCup();
        glTranslatef(0.5f, 0.0f,-0.5f);
        drawCup();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 5.5f);
        drawChair();
        glTranslatef(0.0f, 0.0f, -11.0f);
        drawChair();
        glTranslatef(5.0f, 0.0f, 4.0f);
        drawChair();
        glTranslatef(-10.0f, 0.0f, 4.0f);
        drawChair();
    glPopMatrix();

    glPushMatrix();
        drawCake();
    glPopMatrix();

    glPushMatrix();
        drawBanner();
    glPopMatrix();

    if(balloon != NULL)
        balloon->draw();

    if(displayConfetti)
        drawConfettiDrop();

	glutSwapBuffers();

}

void reshapeFunc( GLsizei w, GLsizei h )
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0) {h = 1;}
    ratio = w * 1.0 / h;

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);

    // Reset Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective.
    gluPerspective(fov, ratio, 0.1f, 201.0f);

    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

void KeyBoardKey(unsigned char key, int xx, int yy){
    switch(key){
    case 'a': //strafe left
    case 'A':
        if(checkBound(x-cos(yaw),y,z-sin(yaw))){
            x -= cos(yaw);
            z -= sin(yaw);
          }
        break;
    case 'd': //strafe right
    case 'D':
        if(checkBound(x+cos(yaw),y,z+sin(yaw))){
            x += cos(yaw);
            z += sin(yaw);
          }
        break;
    case 'w': //move towards front direction
    case 'W':
        if(checkBound(x+lx,y+ly,z+lz)){
            x += lx;
            y += ly;
            z += lz;
          }
        break;
    case 's': //move away from front direction
    case 'S':
        if(checkBound(x-lx,y-ly,z-lz)){
            x -= lx;
            y -= ly;
            z -= lz;
          }
        break;
    case 'q': //rotate left
    case 'Q':
        yaw -= 0.04f;
        yaw = (yaw < 0.0)? 2 * PI - yaw: yaw;
        break;
    case 'e': //rotate right
    case 'E':
        yaw += 0.04f;
        yaw = (yaw > 2 * PI)? yaw - 2 * PI: yaw;
        break;
    case 't': //rotate up
    case 'T':
        pitch += (pitch + 0.04 > PI / 2)? 0.0: 0.04;
        break;
    case 'g': //rotate down
    case 'G':
        pitch -= (pitch - 0.04 < -PI / 2)? 0.0: 0.04;
        break;
    case 'r': //float upwards
    case 'R':
        if(checkBound(x,y+0.5,z)){
            y+=0.5f;
          }
        break;
    case 'f': //drop downwards
    case 'F':
        if(checkBound(x,y-0.5,z)){
            y-=0.5f;
          }
        break;
    case 'o':  // Set to Ortho
    case 'O':
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-2.0f,2.0f,-2.0f,2.0f, 0.1f, 201.0f);
        glMatrixMode(GL_MODELVIEW);
        break;
    case 'p':  // Set to Perspective
    case 'P':
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fov, ratio, 0.1f, 201.0f);
        glMatrixMode(GL_MODELVIEW);
        break;
    case 'b': //blow candle
    case 'B':
        candleLit = false;
        blow = true;
        break;
    case 'l': //on off lights
    case 'L':
        lights = !lights;
        break;
    case 'c':
    case 'C':
        // display confetti or not
        displayConfetti = !displayConfetti;
        break;
    case 'v':
    case 'V':
        // keep spawning or stop spawning
        spawningConfetti = !spawningConfetti;
        break;
    case 'z':
    case 'Z':
        // field of view ++
        fov += (fov < 90.0)? 5.0: 0.0;
        KeyBoardKey('p', 0, 0);
        break;
    case 'x':
    case 'X':
        // field of view --
        fov -= (fov > 30.0)? 5.0: 0.0;
        KeyBoardKey('p', 0, 0);
        break;
    case 'm':
    case 'M':
        // just for debugging
        std::cout << x << " " << y << " " << z << std::endl;
        break;
    case 'j':
    case 'J':
        // GiftBox stops jumping or not
        allowJumping = !allowJumping;
        break;
    }

    glutPostRedisplay();

}

void mouseFunc(int button, int state, int mouse_x, int mouse_y)
{
    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    GLint    viewport[4];
    GLdouble balloon_x;
    GLdouble balloon_y;
    GLdouble balloon_z;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    // find the exact location of the balloon
    gluUnProject(mouse_x, 640-mouse_y, 0.995,
                 modelMatrix, projMatrix, viewport,
                 &balloon_x, &balloon_y, &balloon_z);

    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){ // left click spawns balloon, hold to inflates
        if(balloon != NULL){ // deallocate memory
            delete balloon;
            balloon = NULL;
        }
        balloon = new Balloon(balloon_x, balloon_y, balloon_z);
        glutTimerFunc(25, balloon_inflates, 0);
    }
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){ // release left click to let go
        if(balloon != NULL && balloon->inflating){
            balloon->letGo();
        }
    }
}

void idle(int foo){
    animateSmoke();
    animateBanner();
    animateConfetti();
    animateGiftBox();
    glutTimerFunc(foo, idle, foo);
    glutPostRedisplay();
}

void initialize()
{
    srand(time(0));
    glClearColor( 0.0f, 0.0f,0.0f, 1.0f ); // set the GL clear color - use when the color buffer is cleared
    glEnable( GL_LIGHTING ); // enable lighting
    glLightModelf( GL_LIGHT_MODEL_LOCAL_VIEWER,1 );  // enable lighting for local
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );   // material have diffuse and ambient lighting
    glEnable( GL_COLOR_MATERIAL );     // enable color
    glEnable(GL_MAP1_VERTEX_3);// Allow 3D coordinate control points to the parameter point conversion switch
    glEnable(GL_DEPTH_TEST); //enable depth testing
    glShadeModel(GL_SMOOTH);
    for(int i=0; i<10; i++)
    {
        gifts.push_back(GiftBox());
    }

}

int main( int argc, char ** argv )
{
    // initialize GLUT
    glutInit( &argc, argv );
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window position
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow( "SWE206 Project" );
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );

    glutKeyboardFunc(KeyBoardKey);
    glutMouseFunc(mouseFunc);
    glutTimerFunc(5, idle, 5);
    glutTimerFunc(1000, spawnConfetti, 0);
    initialize();
    glutMainLoop();
    return 0;
}
