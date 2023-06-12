#ifndef BALLOON_H
#define BALLOON_H
#include <GL/glut.h>
#include <vector>
#include "Particle.h"

extern const GLfloat wallHeight;
extern const GLfloat floorLength;
class Balloon
{
public:
    Balloon(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z);
    void draw();
    void inflates();
    void levitate();
    void letGo();
    bool outOfRange();
    void handleParticles();
    ~Balloon();
     bool inflating;

private:
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat radius;
    GLfloat color[3];
    GLfloat acceleration_y;
    GLfloat velocity_y;
    GLfloat velocity_x;
    GLfloat velocity_z;
    GLfloat inflate_speed;
    GLfloat max_size;
    std::vector<Particle> particles;
};
extern Balloon *balloon;
void balloon_levitate(int val);
void balloon_inflates(int val);

#endif // BALLOON_H
