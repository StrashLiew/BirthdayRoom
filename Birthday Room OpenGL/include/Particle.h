#ifndef PARTICLE_H
#define PARTICLE_H
#include <GL/glut.h>
#include <iostream>
class Particle
{
public:
    Particle(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat* the_color);
    void draw();
    void shrinks();
    bool tooSmall();
//    ~Particle(){std::cout << "Particle destructor invoked" <<std::endl;}
private:
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat radius;
    GLfloat color[3];
    GLfloat shrinking_speed;
};

#endif // PARTICLE_H
