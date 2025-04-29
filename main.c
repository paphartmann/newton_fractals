#include <GL/glut.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_LINE 800

struct rgb {
    float r, g, b;
};

struct rgb pixels[BUFFER_LINE * BUFFER_LINE];

uint8_t degree;
long double complex coeffs[7];
long double complex true_roots[6] = {
    -1-I, -1+I, 1+I, 1-I, I, -I
};

const struct rgb colors[6] = {
    {0, 0, 1},
    {0, 1, 0},
    {0, 1, 1},
    {1, 0, 0},
    {1, 0, 1},
    {1, 1, 0}
};

long double screen_range = 32.0;
long double pos_x = 0.0;
long double pos_y = 0.0;

long double complex ratio_z_dz(long double complex z) {
    long double complex p = 0.0, q = 0.0;
    for (int i = 0; i <= degree; i++) {
        q = z * q + p;
        p = z * p + coeffs[i];
    }
    return p/q;
}

long double complex newton(long double complex zn) {
    for (uint8_t i = 0; i < UINT8_MAX; i++) {
        zn = zn - ratio_z_dz(zn);
    }
    return zn;
}

void drawGraph() {
    // struct timespec begin;
    // clock_gettime(CLOCK_REALTIME, &begin);
    const long double step_size = screen_range/BUFFER_LINE;
    #pragma omp parallel for
    for (int i = 0; i < BUFFER_LINE; i++) {
        for (int j = 0; j < BUFFER_LINE; j++) {
            long double x = (j*step_size) - (screen_range/2) + pos_x;
            long double y = (i*step_size) - (screen_range/2) + pos_y;
            long double complex n_res = newton(CMPLX(x, y));
            struct rgb rgb;

            for (uint8_t k = 0; k < degree; k++) {
                if (cabs(true_roots[k] - n_res) < 0.05) {
                    rgb = colors[k];
                    break;
                }
            }

            pixels[(BUFFER_LINE*i)+j] = rgb;
        }
    }
    // struct timespec end;
    // clock_gettime(CLOCK_REALTIME, &end);
    // printf("%ld ms to calculate the newton methods\n", ((end.tv_sec - begin.tv_sec)*1000) + (end.tv_nsec - begin.tv_nsec) / 1000000);

    glDrawPixels(BUFFER_LINE, BUFFER_LINE, GL_RGB, GL_FLOAT, pixels);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case ' ':
        screen_range /= 2.0;
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
    // printf("side of the screen rectangle is %.30f\n\n", screen_range);
}

void draw() {
    drawGraph();
    glFlush();
}

void aberth() {
    long double complex w[degree];
    for (uint16_t i = 0; i < 1024; i++) {
        for (uint8_t j = 0; j < degree; j++) {
            const long double complex slp = ratio_z_dz(true_roots[j]);
            long double complex sum = 0;
            for (uint8_t k = 0; k < degree; k++) {
                if (j != k) {
                    sum += 1/(true_roots[j]-true_roots[k]);
                }
            }
            w[j] = slp / (1-slp*sum);
            true_roots[j] = true_roots[j] - w[j];
        }
    }

    for (int i = 0; i < degree; i++) {
        if (cimag(true_roots[i]) < 0) {
            printf("root #%d found: %.20f %.20fi\n", i, creal(true_roots[i]), cimag(true_roots[i]));
        } else {
            printf("root #%d found: %.20f + %.20fi\n", i, creal(true_roots[i]), cimag(true_roots[i]));
        }
    }
}

int main(int argc, char *argv[]) {
    degree = argc-2;
    if (argc == 1) {
	coeffs[0] = 1;
	coeffs[1] = 0;
	coeffs[2] = -2;
	coeffs[3] = 2;
	degree = 3;
    } else if (degree > 6) {
	return -1;
    } else {
    	for (uint8_t i = 0; i <= degree; i++) {
    	    long double real, imag;
    	    int its = sscanf(argv[i+1], "%Lf+%Lfi", &real, &imag);
    	    if (its == 2) {
    	        coeffs[i] = CMPLX(real,imag);
    	        printf("%f + %fi\n", creal(coeffs[i]), cimag(coeffs[i]));
    	        continue;
    	    }
    	    real = 0.0;
    	    imag = 0.0;
    	    its = sscanf(argv[i+1], "%Lf%Lfi\n", &real, &imag);
    	    if (its == 2) {
    	        coeffs[i] = CMPLX(real,imag);
    	        printf("%f + %fi\n", creal(coeffs[i]), cimag(coeffs[i]));
    	        continue;
    	    }
    	    real = 0.0;
    	    sscanf(argv[i+1], "%Lf", &real);
    	    coeffs[i] = real;
    	    printf("%f + %fi\n", creal(coeffs[i]), cimag(coeffs[i]));
    	}
    }
    aberth();

    glutInit(&argc, argv);
    glutInitDisplayMode(0);
    glutInitWindowSize(800, 800);
    glutCreateWindow("My personal window");

    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
}
