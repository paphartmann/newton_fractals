#include <GL/glut.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>

#define BUFFER_LINE 800

struct rgb {
    float r, g, b;
};

struct rgb pixels[BUFFER_LINE * BUFFER_LINE];

double complex true_roots[3];
double coeffs[4];

double screen_range = 32.0;
double pos_x = 0.0;
double pos_y = 0.0;

double complex slope(double complex z) {
    double complex p = 0.0, q = 0.0;
    for (int i = 0; i < 4; i++) {
        q = z * q + p;
        p = z * p + coeffs[i];
    }
    return p/q;
}

double complex newton(double complex zn) {
    for (uint16_t i = 0; i < 100; i++) {
        zn = zn - slope(zn);
    }
    return zn;
}

void drawGraph() {
    //struct timespec begin;
    //clock_gettime(CLOCK_REALTIME, &begin);
    double step_size = screen_range/BUFFER_LINE;
    #pragma omp parallel for
    for (int i = 0; i < BUFFER_LINE; i++) {
        for (int j = 0; j < BUFFER_LINE; j++) {
            double x = (j*step_size) - (screen_range/2) + pos_x;
            double y = (i*step_size) - (screen_range/2) + pos_y;
            double complex n_res = newton(CMPLXL(x, y));
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
        screen_range /= 2.0;
	printf("%.30f\n", screen_range);
	break;
    case 'w':
	pos_y += screen_range/16.0;
	break;
    case 'a':
	pos_x -= screen_range/16.0;
	break;
    case 's':
	pos_y -= screen_range/16.0;
	break;
    case 'd':
	pos_x += screen_range/16.0;
	break;
    }
    glutPostRedisplay();
}

void draw() {
    drawGraph();
    glFlush();
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        puts("this program only supports cubic polynomials because its easier to implement");
        return -1;
    }
    coeffs[0] = strtod(argv[1],NULL);
    coeffs[1] = strtod(argv[2],NULL);
    coeffs[2] = strtod(argv[3],NULL);
    coeffs[3] = strtod(argv[4],NULL);

    glutInit(&argc, argv);
    glutInitDisplayMode(0);
    glutInitWindowSize(800, 800);
    glutCreateWindow("My personal window");

    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);

    while (~0) {
        glutMainLoop();
    }
}
