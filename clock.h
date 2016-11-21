#ifndef  _CLOCK_H
#define  _CLOCK_H

#include <GL/gl.h>
#include <iostream>
#include <math.h>

#define Pi 3.141592654
#define POINTS_VERTEX_NUM 12 
#define CIRCLE_VERTEX_NUM 72

//vertex array
typedef struct POINT{
	GLfloat x;
	GLfloat y;
} Point;

void     Deg2Vector (float VecDeg, float VecSize, Point *p);
float    Deg2Rad    (float deg); 

class Clock
{
public:
    Clock();
    virtual ~Clock();
    
    void draw();
    
private:
    Point points_vertexes[POINTS_VERTEX_NUM];
    Point circle_vertexes[CIRCLE_VERTEX_NUM];
};

#endif  //_CLOCK_H
