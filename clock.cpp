#include "clock.h"

//-----------------------------------------------------------------------------
// Name : Deg2Vector
//-----------------------------------------------------------------------------
void Deg2Vector(float VecDeg, float VecSize, Point *p)
{
    p->x = (GLfloat) sin(Deg2Rad(VecDeg)) * VecSize;
    p->y = (GLfloat) cos(Deg2Rad(VecDeg)) * VecSize;
}

//-----------------------------------------------------------------------------
// Name : Deg2Rad
//-----------------------------------------------------------------------------
float Deg2Rad(float deg)
{
    return deg*Pi/180.0;
}

//-----------------------------------------------------------------------------
// Name : Clock (constructor)
//-----------------------------------------------------------------------------
Clock::Clock()
{
    //initializing points and circle array
    int n;
    for(n=0; n < POINTS_VERTEX_NUM; n++)
    {
        Deg2Vector( (n+1)*360/POINTS_VERTEX_NUM, 0.7 ,&points_vertexes[n]);
    }
    
    for(n=0; n < CIRCLE_VERTEX_NUM; n++)
    {
	Deg2Vector( (n+1)*360/CIRCLE_VERTEX_NUM, 0.9 ,&circle_vertexes[n]);
    }
}

//-----------------------------------------------------------------------------
// Name : Clock (destructor)
//-----------------------------------------------------------------------------
Clock::~Clock()
{
}

//-----------------------------------------------------------------------------
// Name : draw 
//-----------------------------------------------------------------------------
void Clock::draw()
{
    int n;
    GLenum er;
	
    //clearing the background
    glClearColor(0.2f,0.8f,0.7f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //drawing points
    glColor3f(0.1f,0.5f,0.1f);
    glPointSize(4);
    
    glBegin(GL_POINTS);
    for(n=0 ; n<POINTS_VERTEX_NUM ; n++)
    {
        glVertex2f(points_vertexes[n].x,points_vertexes[n].y);
    }
    glEnd();

    //drawing circle
    glLineWidth(2);
    glColor3f(0.1f,0.4f,0.8f);
    
    glBegin(GL_LINE_LOOP);
    for(n=0 ; n<CIRCLE_VERTEX_NUM ; n++)
    {
        glVertex2f(circle_vertexes[n].x,circle_vertexes[n].y);
    }
    glEnd();

    //drawing clock lines
    glColor3f(0.0f,0.0f,0.8f);
    glBegin(GL_LINES);
        glVertex2f(0,0);
        glVertex2f(0.4,0);
        glVertex2f(0,0);
        glVertex2f(0,0.6);
    glEnd();

    //drawing middle point
    glColor3f(0.1f,0.5f,0.1f);
    glPointSize(6);
    glBegin(GL_POINTS);
        glVertex2f(0,0);
    glEnd();

    //forces execution of OpenGL functions in finite time
    //glXSwapBuffers (dpy, win);
    //glFlush();

    //check for errors
	er = glGetError();  //get errors. 0 for no error, find the error codes in: https://www.opengl.org/wiki/OpenGL_Error
	if(er)
            std::cout << "error " << er << std::endl;
}
