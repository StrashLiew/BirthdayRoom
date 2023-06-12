#ifndef GIFTBOX_H
#define GIFTBOX_H

#include <GL/glut.h>

extern const GLfloat wallHeight;
extern const GLfloat floorLength;
extern bool lights;
extern float setGlow(float, float);
// camera position
extern float x;
extern float y;
extern float z;
class GiftBox
{
    public:
        GiftBox();
        void drawWrap();
        void drawBoxCover();
        void drawRibbon();
        void drawGiftBox();
        void jumps();
        bool isJumping();

    private:
        GLfloat box_x;
        GLfloat box_y;
        GLfloat box_z;
        GLfloat cover_color[3];
        GLfloat box_color[3];
        GLfloat gravity;
        GLfloat jump_height;
        GLfloat jump_speed;
        GLfloat current_height;
        GLfloat current_velocity;
};

#endif // GIFTBOX_H
