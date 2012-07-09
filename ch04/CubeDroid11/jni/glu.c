#include <EGL/egl.h>
#include <GLES/gl.h>
#include "glu.h"

#undef PI
#define PI 3.1415926535897932f

/* Following gluLookAt implementation is adapted from the
 * Mesa 3D Graphics library. http://www.mesa3d.org
 */
void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
               GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy,
               GLfloat upz) {
  GLfloat m[16];
  GLfloat x[3], y[3], z[3];
  GLfloat mag;

  /* Make rotation matrix */

  /* Z vector */
  z[0] = eyex - centerx;
  z[1] = eyey - centery;
  z[2] = eyez - centerz;
  mag = (float) sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
  if (mag) { /* mpichler, 19950515 */
    z[0] /= mag;
    z[1] /= mag;
    z[2] /= mag;
  }

  /* Y vector */
  y[0] = upx;
  y[1] = upy;
  y[2] = upz;

  /* X vector = Y cross Z */
  x[0] = y[1] * z[2] - y[2] * z[1];
  x[1] = -y[0] * z[2] + y[2] * z[0];
  x[2] = y[0] * z[1] - y[1] * z[0];

  /* Recompute Y = Z cross X */
  y[0] = z[1] * x[2] - z[2] * x[1];
  y[1] = -z[0] * x[2] + z[2] * x[0];
  y[2] = z[0] * x[1] - z[1] * x[0];

  /* mpichler, 19950515 */
  /* cross product gives area of parallelogram, which is < 1.0 for
   * non-perpendicular unit-length vectors; so normalize x, y here
   */

  mag = (float) sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
  if (mag) {
    x[0] /= mag;
    x[1] /= mag;
    x[2] /= mag;
  }

  mag = (float) sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
  if (mag) {
    y[0] /= mag;
    y[1] /= mag;
    y[2] /= mag;
  }

#define M(row,col)  m[col*4+row]
  M(0, 0) = x[0];
  M(0, 1) = x[1];
  M(0, 2) = x[2];
  M(0, 3) = 0.0;
  M(1, 0) = y[0];
  M(1, 1) = y[1];
  M(1, 2) = y[2];
  M(1, 3) = 0.0;
  M(2, 0) = z[0];
  M(2, 1) = z[1];
  M(2, 2) = z[2];
  M(2, 3) = 0.0;
  M(3, 0) = 0.0;
  M(3, 1) = 0.0;
  M(3, 2) = 0.0;
  M(3, 3) = 1.0;
#undef M
  {
    int a;
    GLfixed fixedM[16];
    for (a = 0; a < 16; ++a)
      fixedM[a] = (GLfixed) (m[a] * 65536);
    glMultMatrixx(fixedM);
  }

  /* Translate Eye to Origin */
  glTranslatex((GLfixed) (-eyex * 65536), (GLfixed) (-eyey * 65536),
               (GLfixed) (-eyez * 65536));
}

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
  GLfloat xmin, xmax, ymin, ymax;

  ymax = zNear * (GLfloat) tan(fovy * PI / 360);
  ymin = -ymax;
  xmin = ymin * aspect;
  xmax = ymax * aspect;

  glFrustumx((GLfixed) (xmin * 65536), (GLfixed) (xmax * 65536),
             (GLfixed) (ymin * 65536), (GLfixed) (ymax * 65536),
             (GLfixed) (zNear * 65536), (GLfixed) (zFar * 65536));
}
