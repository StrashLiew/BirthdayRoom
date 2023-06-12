#include "GiftBox.h"
#include <math.h>
GiftBox::GiftBox()
{
    this->box_x = GLfloat(rand()%43 - 21); // [-21, 21]
    this->box_y = 0.0;
    this->box_z = GLfloat(rand()%43 - 21); // [-21, 21]
    for(int i=0; i<3; i++){
        this->box_color[i] = GLfloat(5+rand()%6)/10.0;
        this->cover_color[i] = GLfloat(5+rand()%6)/10.0;
    }
    this->gravity = 0.008;
    this->jump_height = GLfloat(0.5 + rand()%10 / 10.0); // random maximum height [0.5, 1.4]
    this->jump_speed = sqrt(2 * this->gravity * this->jump_height); // v = sqrt(2*g*h) free fall
    this->current_height = 0.0;
    this->current_velocity = 0.0;
}

void GiftBox::drawWrap(){

    glBegin(GL_QUADS); //back
        glVertex3f(0.31f,0.0f,0.0f);
        glVertex3f(0.71f,0.0f,0.0f);
        glVertex3f(0.71f,1.0f,0.0f);
        glVertex3f(0.31f,1.0f,0.0f);
    glEnd();

     glBegin(GL_QUADS); //front
        glVertex3f(0.31f,0.0f,1.02f);
        glVertex3f(0.71f,0.0f,1.02f);
        glVertex3f(0.71f,1.0f,1.02f);
        glVertex3f(0.31f,1.0f,1.02f);
    glEnd();

    glBegin(GL_QUADS); //left
        glVertex3f(0.0f,0.0f,0.31f);
        glVertex3f(0.0f,0.0f,0.71f);
        glVertex3f(0.0f,1.0f,0.71f);
        glVertex3f(0.0f,1.0f,0.31f);
    glEnd();

    glBegin(GL_QUADS); //right
        glVertex3f(1.02f,0.0f,0.31f);
        glVertex3f(1.02f,0.0f,0.71f);
        glVertex3f(1.02f,1.0f,0.71f);
        glVertex3f(1.02f,1.0f,0.31f);
    glEnd();
}

void GiftBox::drawBoxCover(){

    glBegin(GL_QUADS); //back
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(1.2f,0.0f,0.0f);
        glVertex3f(1.2f,0.2f,0.0f);
        glVertex3f(0.0f,0.2f,0.0f);
    glEnd();

    glBegin(GL_QUADS); //front
        glVertex3f(0.0f,0.0f,1.2f);
        glVertex3f(1.2f,0.0f,1.2f);
        glVertex3f(1.2f,0.2f,1.2f);
        glVertex3f(0.0f,0.2f,1.2f);
    glEnd();

    glBegin(GL_QUADS); //left
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(0.0f,0.0f,1.2f);
        glVertex3f(0.0f,0.2f,1.2f);
        glVertex3f(0.0f,0.2f,0.0f);
    glEnd();

    glBegin(GL_QUADS); //right
        glVertex3f(1.2f,0.0f,0.0f);
        glVertex3f(1.2f,0.0f,1.2f);
        glVertex3f(1.2f,0.2f,1.2f);
        glVertex3f(1.2f,0.2f,0.0f);
    glEnd();

    glBegin(GL_QUADS); //bottom
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(0.0f,0.0f,1.2f);
        glVertex3f(1.2f,0.0f,1.2f);
        glVertex3f(1.2f,0.0f,0.0f);
    glEnd();

    glBegin(GL_QUADS); //top
        glVertex3f(0.0f,0.2f,0.0f);
        glVertex3f(0.0f,0.2f,1.2f);
        glVertex3f(1.2f,0.2f,1.2f);
        glVertex3f(1.2f,0.2f,0.0f);
    glEnd();
}

void GiftBox::drawRibbon(){

    glPushMatrix();
        glTranslatef(0.2, 0.1, 0.0);
        glRotatef(45.0, 0.0, 0.0, 1.0);
        glScalef(2.0, 1.0, 1.0);
        glutSolidTorus(0.1, 0.08, 10, 10);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-0.2, 0.1, 0.0);
        glRotatef(-45.0, 0.0, 0.0, 1.0);
        glScalef(2.0, 1.0, 1.0);
        glutSolidTorus(0.1, 0.08, 10, 10);
    glPopMatrix();

}

void GiftBox::drawGiftBox(){
    float glow = lights? 1.4: 0.2;

    glPushMatrix();
        glTranslatef(this->box_x, this->box_y + this->current_height, this->box_z);
        glRotatef(atan(-(this->box_z-z)/(this->box_x-x))*180.0/3.14159 + 90.0, 0.0, 1.0, 0.0); // faces user

        //draw box body
        glColor3fv(this->box_color);
        glPushMatrix();
            glTranslatef(0.0f,0.5f,0.0f);
            glutSolidCube(1.0f);
        glPopMatrix();


        //draw box wraps on front back
        glColor3f(setGlow(glow,1),0.2f,0.2f); //slightly brighter red
        glPushMatrix();
            glTranslatef(-0.51f,0.0f,-0.51f); //front
            drawWrap();
        glPopMatrix();


        //draw box cover
        glColor3fv(this->cover_color);
        glPushMatrix();
            glTranslatef(-0.6f,1.0f,-0.6f); //top
            drawBoxCover();
        glPopMatrix();


        //draw ribbon on top of gift box
        glColor3f(setGlow(glow,1),0.2f,0.2f); //slightly brighter red
        glPushMatrix();
            glTranslatef(0.0f,1.1f,0.0f);
            drawRibbon();
        glPopMatrix();

    glPopMatrix();

}

void GiftBox::jumps()
{
    if(!this->isJumping()) // if currently not jumping, jumps
        this->current_velocity = this->jump_speed;
    this->current_height += this->current_velocity; // h + dh
    this->current_velocity -= this->gravity; // v + dv
    if(this->current_height <= 0.0){ // lands
        this->current_height = 0.0;
        this->current_velocity = 0.0;
    }
}

bool GiftBox::isJumping()
{
    return this->current_height > 0;
}
