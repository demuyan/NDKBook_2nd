#ifndef _GLU_H_
#define _GLU_H_

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
        GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz);

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

#endif
