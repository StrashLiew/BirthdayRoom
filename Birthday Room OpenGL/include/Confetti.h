#ifndef CONFETTI_H
#define CONFETTI_H
#include <GL/glut.h>

class Confetti
{
public:
    Confetti(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z);
    void drawConfetti();
    void falls();
    bool outOfRange();

private:
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat radius;
    GLfloat color[3];
    GLfloat falling_speed;
    GLfloat rotate_x;
    GLfloat rotate_y;
    GLfloat rotate_z;
    GLfloat rotate_x_speed;
    GLfloat rotate_y_speed;
    GLfloat rotate_z_speed;
};

#endif // CONFETTI_H
