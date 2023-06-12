#include "Confetti.h"

Confetti::Confetti(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z)
{
    this->x = pos_x;
    this->y = pos_y;
    this->z = pos_z;
    for(int i=0; i<3; i++)
        this->color[i] = GLfloat(5+rand()%6)/10.0; // random r, g, b [0.5, 1.0] brighter color
    this->radius = GLfloat(rand()%9/100.0) + 0.08; // random size [0.08, 0.16]
    this->falling_speed = 0.05;
    this->rotate_x = 0.0;
    this->rotate_y = 0.0;
    this->rotate_z = 0.0;
    //different axis different random rotation speed [-3, 3]
    this->rotate_x_speed = GLfloat(rand()%7 - 3);
    this->rotate_y_speed = GLfloat(rand()%7 - 3);
    this->rotate_z_speed = GLfloat(rand()%7 - 3);
}

void Confetti::drawConfetti()
{
    if(this->outOfRange())
        return;
    glColor3fv(color);

    glPushMatrix();
        glTranslatef(this->x, this->y, this->z);
        glRotatef(rotate_x, 1.0, 0.0, 0.0);
        glRotatef(rotate_y, 0.0, 1.0, 0.0);
        glRotatef(rotate_z, 0.0, 0.0, 1.0);
        glBegin(GL_POLYGON);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(this->radius, 0.0, 0.0);
            glVertex3f(this->radius, this->radius, 0.0);
            glVertex3f(0.0, this->radius, 0.0);
        glEnd();
    glPopMatrix();
}

void Confetti::falls()
{
    this->y -= this->falling_speed; // y - dy

    this->rotate_x += this->rotate_x_speed; // angle_x + d(angle_x)
    this->rotate_x -= (this->rotate_x > 360.0)? 360.0: 0.0; // angle = angle % 360

    this->rotate_y += this->rotate_y_speed; // angle_y + d(angle_y)
    this->rotate_y -= (this->rotate_y > 360.0)? 360.0: 0.0; // angle = angle % 360

    this->rotate_z += this->rotate_z_speed; // angle_z + d(angle_z)
    this->rotate_z -= (this->rotate_z > 360.0)? 360.0: 0.0; // angle = angle % 360
}

bool Confetti::outOfRange() // under ground
{
    return this->y < 0.0;
}
