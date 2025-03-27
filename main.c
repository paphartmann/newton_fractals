#include <GL/glut.h>
#include <complex.h>
#include <math.h>

#define BUFFER_LINE 800

struct rgb {
    float r, g, b;
};

struct rgb pixels[BUFFER_LINE * BUFFER_LINE];

const long double complex true_roots[3] = {
    -1.769292354238631415240409L,
    0.8846461771193157076202047L - (0.5897428050222055016472807L * I),
    0.8846461771193157076202047L + (0.5897428050222055016472807L * I)
};

long double screen_range = 32.0L;
long double pos_x = 0.0L;
long double pos_y = 0.0L;

long double complex f(long double complex z) {
    return 2 + (z * (-2 + (z*z)));
}

long double complex df(long double complex z) {
    return -2 + (3*z*z);
}

long double complex newton(long double complex zn) {
    for (uint16_t i = 0; i < 100; i++) {
        zn = zn - (f(zn)/df(zn));
    }
    return zn;
}

#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>

void drawGraph() {
    //struct timespec begin;
    //clock_gettime(CLOCK_REALTIME, &begin);
    
    #pragma omp parallel for
    for (int i = 0; i < BUFFER_LINE; i++) {
        for (int j = 0; j < BUFFER_LINE; j++) {
            long double step_size = screen_range/BUFFER_LINE;
            long double x = (j*step_size) - (screen_range/2) + pos_x;
            long double y = (i*step_size) - (screen_range/2) + pos_y;
            long double complex n_res = newton(CMPLXL(x, y));
            pixels[(BUFFER_LINE*i)+j] =
                (struct rgb) {
                    exp2(-8*cabsl(n_res - true_roots[0])),
                    exp2(-8*cabsl(n_res - true_roots[1])),
                    exp2(-8*cabsl(n_res - true_roots[2]))
                };
        }
    }

    //struct timespec end;
    //clock_gettime(CLOCK_REALTIME, &end);
    //printf("%ld ms\n", ((end.tv_sec - begin.tv_sec)*1000) + (end.tv_nsec - begin.tv_nsec) / 1000000);

    glDrawPixels(BUFFER_LINE, BUFFER_LINE, GL_RGB, GL_FLOAT, pixels);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case ' ':
        screen_range /= 2;
	printf("%.30Lf\n", screen_range);
	break;
    case 'w':
	pos_y += screen_range/16;
	break;
    case 'a':
	pos_x -= screen_range/16;
	break;
    case 's':
	pos_y -= screen_range/16;
	break;
    case 'd':
	pos_x += screen_range/16;
	break;
    }
    glutPostRedisplay();
}

void draw() {
    drawGraph();
    glFlush();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(0);
    glutInitWindowSize(800, 800);
    glutCreateWindow("My personal window");

    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);

    while (GLU_TRUE) {
        glutMainLoop();
    }
}
