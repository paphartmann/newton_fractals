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

double coeffs[4];
double complex true_roots[3] = {
    -1+I, 1+I, 1-I
};

double screen_range = 32.0;
double pos_x = 0.0;
double pos_y = 0.0;

double complex ratio_z_dz(double complex z) {
    double complex p = 0.0, q = 0.0;
    for (int i = 0; i < 4; i++) {
        q = z * q + p;
        p = z * p + coeffs[i];
    }
    return p/q;
}

double complex newton(double complex zn) {
    for (uint16_t i = 0; i < 100; i++) {
        zn = zn - ratio_z_dz(zn);
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
            double complex n_res = newton(CMPLX(x, y));
            pixels[(BUFFER_LINE*i)+j] =
                (struct rgb) {
                    exp2(-8*cabs(n_res - true_roots[0])),
                    exp2(-8*cabs(n_res - true_roots[1])),
                    exp2(-8*cabs(n_res - true_roots[2]))
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

void aberth() {
    double complex w[3];
    for (uint16_t i = 0; i < 1000; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            const double complex slp = ratio_z_dz(true_roots[j]);
            double complex sum = 0;
            for (uint8_t k = 0; k < 3; k++) {
                if (j != k) {
                    sum += 1/(true_roots[j]-true_roots[k]);
                }
            }
            w[j] = slp / (1-slp*sum);
            true_roots[j] = true_roots[j] - w[j];
        }
    }
    for (int i = 0; i < 3; i++) {
        printf("%f + %fi\n", creal(true_roots[i]), cimag(true_roots[i]));
    }
}

int main(int argc, char *argv[]) {
    coeffs[0] = strtod(argv[1],NULL);
    coeffs[1] = strtod(argv[2],NULL);
    coeffs[2] = strtod(argv[3],NULL);
    coeffs[3] = strtod(argv[4],NULL);

    aberth();

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
