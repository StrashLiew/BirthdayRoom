#include "Particle.h"

Particle::Particle(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat* the_color)
{
    this->x = pos_x;
    this->y = pos_y;
    this->z = pos_z;
    for(int i=0; i<3; i++)
        this->color[i] = the_color[i];
    this->radius = 0.2;
    this->shrinking_speed = 0.01;
}

void Particle::draw()
{
    if(tooSmall())
        return;
    glColor3fv(color);
    glPushMatrix();
        glTranslatef(this->x, this->y, this->z);
        glutSolidSphere(this->radius, 6, 4);
    glPopMatrix();
}

void Particle::shrinks()
{
    this->radius -= shrinking_speed;
}

bool Particle::tooSmall()
{
    return radius < 0.05;
}
