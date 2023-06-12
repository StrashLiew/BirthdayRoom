#include "Balloon.h"
#include <iostream>
#include <math.h>
Balloon::Balloon(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z)
{
    this->max_size = 1.8;
    // make sure it doesn't spawn outside of scene
    this->x = pos_x - this->max_size < -floorLength ? this->max_size - floorLength: pos_x + this->max_size > floorLength ? floorLength - this->max_size: pos_x;
    this->y = pos_y - 2.0 * this->max_size < 0.0? 2.0 * this->max_size: pos_y + 2.0 * this->max_size > wallHeight? wallHeight - 2.0 * this->max_size: pos_y;
    this->z = pos_z - this->max_size < -floorLength ? this->max_size - floorLength: pos_z + this->max_size > floorLength ? floorLength - this->max_size: pos_z;
    this->radius = 0.5;
    for(int i=0; i<3; i++) // random r, g, b
        this->color[i] = GLfloat(5+rand()%6)/10.0;
    this->velocity_y = 0.0;
    this->velocity_x = GLfloat(rand()%31 - 15) / 1000.0; // random x velocity [-0.015, 0.015]
    this->velocity_z = GLfloat(rand()%31 - 15) / 1000.0; // random z velocity [-0.015, 0.015]
    this->acceleration_y = 0.003;
    this->inflating = true;
    this->inflate_speed = 0.1;
}

Balloon::~Balloon()
{
    //std::cout << "Balloon destructor invoked." << std::endl;
}

void Balloon::draw()
{
    glColor3fv(color);
    for(double theta=0.0; theta<180.0; theta+=2.0){ // made out of 90 spheres
        glPushMatrix();
            glTranslatef(x, y, z);
            double radian = theta * 3.14159 / 180.0;
            glTranslatef(0.0, radius * cos(radian) * ((theta > 90.0)? 1.0: 0.2), 0.0);
            glutSolidSphere(radius*sin(radian), 30, 30);
        glPopMatrix();
    }

    //Balloon tie
    glPushMatrix();
        glTranslatef(x, y, z); // follows balloon
        glTranslatef(0.0, -radius * 1.5, 0.0); // below balloon
        glRotatef(-90.0, 1.0, 0.0, 0.0); // so it faces up
        glutSolidCone(radius * 0.2, radius * 0.2, 10, 10);
    glPopMatrix();

    //Balloon particles
    for(unsigned int i=0; i<particles.size(); i++){
        particles[i].draw();
    }
}

void Balloon::inflates()
{
    if(this->inflating){
        this->radius += this->inflate_speed;
    }
    if(this->radius >= this->max_size){
        this->letGo();
    }
}

void Balloon::levitate()
{
    handleParticles();
    if(this->outOfRange())
        return;
    this->x += this->velocity_x;
    this->y += this->velocity_y;
    this->velocity_y += this->acceleration_y;

}

bool Balloon::outOfRange()
{
    return this->y - 2.0*this->radius > wallHeight;
}

void Balloon::letGo()
{
    this->inflating = false;
    glutTimerFunc(25, balloon_levitate, 0);
}

void Balloon::handleParticles()
{
    if(rand()%3<1) // spawn by 33% chance
        particles.push_back(Particle(x+GLfloat(rand()%10)/100.0, y-1.6*radius, z+GLfloat(rand()%10)/100.0, color)); // random displacement from x and z
    int erase_until = -1;
    for(unsigned int i=0; i<particles.size(); i++){
        particles[i].shrinks();
        if(particles[i].tooSmall()) // exists too small particle to be erased
            erase_until = i;
    }
    if(erase_until != -1){ // erase too small particle
        particles.erase(particles.begin(), particles.begin() + erase_until);
        erase_until = -1;
    }
}

void balloon_inflates(int val)
{
    if(balloon == NULL || !balloon->inflating)
        return;

    balloon->inflates();
    if(balloon->inflating)
        glutTimerFunc(25, balloon_inflates, 0);
}

void balloon_levitate(int val)
{
    if(balloon->inflating)
        return;
    balloon->levitate();
    if(balloon->outOfRange()){ // deallocate balloon
        delete balloon;
        balloon = NULL;
    }
    else{
        glutTimerFunc(25, balloon_levitate, 0);
    }
}
