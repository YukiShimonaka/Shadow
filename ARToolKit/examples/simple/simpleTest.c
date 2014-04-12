#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <math.h>

//
// Camera configuration.
//
#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

int             xsize, ysize;
int             thresh = 100;
int             count = 0;

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

char           *patt_name      = "Data/patt.hiro";
int             patt_id;
double          patt_width     = 80.0;
double          patt_center[2] = {0.0, 0.0};
double          patt_trans[3][4];

// hikaku you
double          pre_patt_trans[3];
double          mark_11 = 0;
double          mark_31 = 0;
// ground
double          mak_posx = 0;
double          mak_posy = 0;
int             gr_count = 0;
int             gr_flg = 0;
double          gr_pos;
double          groot_count = 0;

// snow
#define SNOW    200 // the number of snow 
int             down_count[SNOW];
int             snowxpos_flg[SNOW];
int             snowypos_flg[SNOW];
float           snow_xpos[SNOW];
float           snow_ypos[SNOW];
int             snow_cou[SNOW];
float           fall_flg;
float           xflow[SNOW];
float           snow_noise[SNOW];
double          cosx_plus = 1.0;
double          cosx_minus = 1.0;
int             rotate_flg;
float           alfa = 2.5; // KOKO! (BUFFALO = 2.5)(CCD = 0.5)


// capture flag
static int      capFlag = 0;
// mainloop time of doing count
int             do_count = 0;
// as cording count
int             test_count = 0;
// as cording flag
int test_flag = 0;


static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mouseEvent(int button, int state, int x, int y);
static void   mainLoop(void);
static void   draw(double cosine/*int flg*/);


#define WIDTH   200
#define HEIGHT  200

#define PAI 3.14159265358979
#define PART 100  //time of part

/* snow------------------------*/
#define snow_num 500

float   GetRandom(float min, float max);

void	snow();
void    update_snow();
void    draw_snow(int a, float sx, float sy, float snow_scale);

float   snow_x[snow_num],snow_y[snow_num],snow_z[snow_num];
int     num[snow_num];
void    fall(float x,float y,float z);
/*-----------------------------*/

int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));
    
	glutInit(&argc, argv);
	init();
    
    arVideoCapStart();
    argMainLoop( mouseEvent, keyEvent, mainLoop );
	return (0);
}

static void   keyEvent( unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
    }
}

static void   mouseEvent(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN) {
        if(button == GLUT_LEFT_BUTTON) {
            arVideoCapStop();
            capFlag = 1;
        }
    
        else if (button == GLUT_RIGHT_BUTTON) {
            capFlag = 0;
            arVideoCapStart();
        }
    }    
}

/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             j, k;
//    static int      swFlag = 1;
    
    // left--
    // kasou point
    double          ver_pos_l[2];
    // far
    double          far_a1_l, far_b1_l, far_a2_l, far_b2_l;
    // 2 jou
    double          far_a1_2_l, far_b1_2_l, far_a2_2_l, far_b2_2_l;
    // wa
    double          far_deno_l;
    // root
    double          far_mole_l;
    double          root_a_l;
    double          root_b_l;

    // right--
    // kasou point
    double          ver_pos_r[2];
    // far
    double          far_a1_r, far_b1_r, far_a2_r, far_b2_r;
    // 2 jou
    double          far_a1_2_r, far_b1_2_r, far_a2_2_r, far_b2_2_r;
    // wa
    double          far_deno_r;
    // root
    double          far_mole_r;
    double          root_a_r;
    double          root_b_r;
    
    double          cosine;
    
    // snow
    int             i, n = PART;
    float           a, b;
    double          rate;
    int             l = 0;
    
    /* not doing when stop capturing*/
    if(capFlag > 0) {
        return;
    }

    /* grab a vide frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count == 0 ) arUtilTimerReset();
    count++;

    argDrawMode2D();
//    argDispImage( dataPtr, 0,0 );
    glColor3f(1.0, 1.0, 1.0);
    argDispImage(0, 0, 0 );  //(pointer, draw window number, same)

    // draw ground-------------------------------------------------------------------------------------------------
    if (test_flag == 1 && gr_flg == 0) {
        gr_pos = mak_posy;
        gr_flg = 1;
    }
    if (test_flag == 1 && gr_flg == 1) {
        glBegin(GL_QUADS);
        glColor3f(0.0, 0.0, 0.0);
        glVertex2d(mak_posx + gr_count, (ysize - gr_pos) - (ysize / 12)); // KOKO! (BUFFALO = 12)(CCD = 15)
        glVertex2d(mak_posx - gr_count, (ysize - gr_pos) - (ysize / 12));
        glVertex2d(mak_posx - gr_count, 0);
        glVertex2d(mak_posx + gr_count, 0);
        glEnd();
//        printf("gr_count = %d\n", gr_count);
//        if (gr_count <= 10) {
//            gr_count += 5;
//        }
        if (gr_count < xsize) {
            gr_count += 40; // KOKO! (BUFFALO = 40)(CCD = 6)
            groot_count++;
        }
        glColor3f(1.0, 1.0, 1.0);
    }
    // draw ground-------------------------------------------------------------------------------------------------
    
    // draw snow---------------------------------------------------------------------------------------------------
    for (l = 0; l < SNOW; l++) {
        if (test_flag == 1 && snowxpos_flg[l] == 0) {
            snow_xpos[l] = GetRandom(-500, xsize + 500);
            snow_noise[l] = GetRandom(-5.0, 5.0);
            snowxpos_flg[l] = 1;
        }
        if (test_flag == 1 && snowypos_flg[l] == 0) {
            snow_ypos[l] = GetRandom(0, ysize);
            snowypos_flg[l] = 1;
        }
        
        if (test_flag == 1 && snowxpos_flg[l] == 1) {
        glPushMatrix();
            if (rotate_flg == 2) {
                if (cosx_plus >= 0.98 && cosx_plus < 1.0) {
                    xflow[l] += (alfa / 2);
                }
                else if (cosx_plus >= 0.95 && cosx_plus < 0.98) {
                    xflow[l] += alfa;
                }
                else if (cosx_plus >= 0.925 && cosx_plus < 0.95) {
                    xflow[l] += (alfa * 2);
                }
                else if (cosx_plus >= 0.90 && cosx_plus < 0.925) {
                    xflow[l] += (alfa * 3);
                }
                else if (cosx_plus >= 0.875 && cosx_plus < 0.90) {
                    xflow[l] += (alfa * 4);
                }
                else if (cosx_plus >= 0.85 && cosx_plus < 0.875) {
                    xflow[l] += (alfa * 5);
                }
                else if (cosx_plus >= 0.825 && cosx_plus < 0.85) {
                    xflow[l] += (alfa * 6);
                }
                else if (cosx_plus < 0.825) {
                    xflow[l] += (alfa * 7);
                }
            }
            if (rotate_flg == 1) {
                if (cosx_minus >= 0.98 && cosx_minus < 1.0) {
                    xflow[l] -= 0;
                }
                else if (cosx_minus >= 0.95 && cosx_minus < 0.98) {
                    xflow[l] -= alfa;
                }
                else if (cosx_minus >= 0.925 && cosx_minus < 0.95) {
                    xflow[l] -= (alfa * 2);
                }
                else if (cosx_minus >= 0.90 && cosx_minus < 0.925) {
                    xflow[l] -= (alfa * 3);
                }
                else if (cosx_minus >= 0.875 && cosx_minus < 0.90) {
                    xflow[l] -= (alfa * 4);
                }
                else if (cosx_minus >= 0.85 && cosx_minus < 0.875) {
                    xflow[l] -= (alfa * 5);
                }
                else if (cosx_minus >= 0.825 && cosx_minus < 0.85) {
                    xflow[l] -= (alfa * 6);
                }
                else if (cosx_minus < 0.825) {
                    xflow[l] -= (alfa * 7);
                }
            }
            glTranslatef(snow_xpos[l] - xflow[l], (ysize - snow_ypos[l]) - down_count[l], 0.0); // KOKO! (BUFFALO = -200)(CCD = 0)
            glColor3f(0.0, 0.0, 0.0);
            glBegin(GL_POLYGON);
            for (i = 0; i < n; i++) {
                rate = (double)i / n;
                a = 3 * cos(2.0 * PAI * rate);
                b = 3 * sin(2.0 * PAI * rate);
                glVertex2f(a, b);
            }
            glEnd();
            glFlush();
            glPopMatrix();
            if (((ysize - snow_ypos[l]) - down_count[l]) >= 0) { // KOKO! (BUFFALO = -200)(CCD = 0)
                down_count[l] = down_count[l] + 20 - snow_noise[l]; // KOKO! (BUFFALO = 20)(CCD = 3)
            }
            else if (((ysize - snow_ypos[l]) - down_count[l]) < 0) { // KOKO! (BUFFALO = -200)(CCD = 0)
                down_count[l] = 0;
                snowxpos_flg[l] = 0;
                snow_ypos[l] = 0;
                xflow[l] = 0;
                snow_noise[l] = 0;
            }
        }
    }

    glColor3f(1.0, 1.0, 1.0);
    // draw snow---------------------------------------------------------------------------------------------------
    
    /* detect the markers in the video frame */
    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }

    arVideoCapNext();

    /* check for object visibility */
    k = -1;
    for( j = 0; j < marker_num; j++ ) {
        if( patt_id == marker_info[j].id ) {
            if( k == -1 ) k = j;
            else if( marker_info[k].cf < marker_info[j].cf ) k = j;
        }
    }
    if( k == -1 ) {
//        swFlag = -swFlag;
        test_count = 0;
        test_flag = 0;
        do_count = 0;
        gr_count = 0;
        gr_flg = 0;
        groot_count = 0;
        for (l = 0; l < SNOW; l++) {
            snowxpos_flg[l] = 0;
            snowypos_flg[l] = 0;
            down_count[l] = 0;
            xflow[l] = 0;
        }
        cosx_minus = 1.0;
        cosx_plus = 1.0;
        rotate_flg = 0;
        mark_11 = 0;
        mark_31 = 0;
        argSwapBuffers();
        return;
    }

    /* get the transformation between the marker and the real camera */
    arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);

    if ((marker_info[k].vertex[0][0] - marker_info[k].vertex[3][0] > -10 && marker_info[k].vertex[0][0] - marker_info[k].vertex[3][0] < 10) || (marker_info[k].vertex[2][0] - marker_info[k].vertex[3][0] > -10 && marker_info[k].vertex[2][0] - marker_info[k].vertex[3][0] < 10)) {
        mark_11 = marker_info[k].vertex[1][1];
        mark_31 = marker_info[k].vertex[3][1];
    }

    printf("%5.2f %5.2f %5.2f\n", patt_trans[0][3], patt_trans[1][3], patt_trans[2][3]);
//    printf("\n");
    printf("%5.2f %5.2f %5.2f\n", pre_patt_trans[0], pre_patt_trans[1], pre_patt_trans[2]);
    printf("\n");
    
    mak_posx = marker_info[k].pos[0];
    mak_posy = marker_info[k].pos[1];
    
    // hikaku sena...
    if (do_count > 0 && test_count < 15) {  // KOKO! (BUFFALO = 15)(CCD = 50)
        if ((0 <= (patt_trans[0][3] - pre_patt_trans[0]) && (patt_trans[0][3] - pre_patt_trans[0]) < 3) || (0 >= (patt_trans[0][3] - pre_patt_trans[0]) && (patt_trans[0][3] - pre_patt_trans[0]) > -3)) {  // KOKO! (BUFFALO = 3)(CCD = 0.5)
            // only 1 jikan start
            // now - start > 3 de tree
            test_count++;
            printf("%f\n", patt_trans[0][3] - pre_patt_trans[0]);
            printf("%d\n", test_count);
            printf("\n");
        }
        else{
            // now =
            test_count = 0;
            test_flag = 0;
            printf("%f\n", patt_trans[0][3] - pre_patt_trans[0]);
            printf("%d\n", test_count);
            printf("\n");
        }
    }
    if (test_count >= 15) { // KOKO! (BUFFALO = 15)(CCD = 50)
        test_flag = 1;
    }

    pre_patt_trans[0] = patt_trans[0][3];
    pre_patt_trans[1] = patt_trans[1][3];
    pre_patt_trans[2] = patt_trans[2][3];
    do_count++;
    

    // vertex = konoji kata
    printf("%f %f %f %f\n", marker_info[k].vertex[0][0], marker_info[k].vertex[0][1], marker_info[k].vertex[1][0], marker_info[k].vertex[1][1]);
    printf("%f %f %f %f\n", marker_info[k].vertex[2][0], marker_info[k].vertex[2][1], marker_info[k].vertex[3][0], marker_info[k].vertex[3][1]);
    printf("\n");
    // [0][0] = [3][0], [0][1] = [1][1], [1][0] = [2][0], [2][1] = [3][1]

    
    // left turn-----------------------------------------------
    if (marker_info[k].vertex[1][1] >= marker_info[k].vertex[3][1] && marker_info[k].vertex[0][0] >= marker_info[k].vertex[2][0]) {
        if (mark_11 >= mark_31) {
        ver_pos_l[0] = marker_info[k].vertex[1][0];
        ver_pos_l[1] = marker_info[k].vertex[0][1];
//        printf("ver_pos = %f %f\n", ver_pos[0], ver_pos[1]);
//        printf("\n");
        
        // far between 2 points ----------------------------------------------------------------
        // sa
        far_a1_l = marker_info[k].vertex[1][0] - marker_info[k].vertex[0][0];
        far_a2_l = marker_info[k].vertex[1][1] - marker_info[k].vertex[0][1];
        far_b1_l = ver_pos_l[0] - marker_info[k].vertex[0][0];
        far_b2_l = ver_pos_l[1] - marker_info[k].vertex[0][1];
        // 2 jou
        far_a1_2_l = far_a1_l * far_a1_l;
        far_a2_2_l = far_a2_l * far_a2_l;
        far_b1_2_l = far_b1_l * far_b1_l;
        far_b2_2_l = far_b2_l * far_b2_l;
        // bumbo
        far_mole_l = far_a1_l * far_b1_l;
        root_a_l = sqrt(far_a1_2_l + far_a2_2_l);
        root_b_l = sqrt(far_b1_2_l + far_b2_2_l);
        far_deno_l = root_a_l * root_b_l;
        // cos(theta)
        cosine = far_mole_l / far_deno_l;
        
//        printf("root_a_l = %f\n", sqrt(far_a1_2_l +far_a2_2_l));
//        printf("root_b_l = %f\n", sqrt(far_b1_2_l + far_b2_2_l));
//        printf("far_mole_l = %f\n", far_mole_l);
//        printf("far_deno_l = %f\n", far_deno_l);
        printf("cos_l = %f\n", cosine);
//        printf("theta = %f\n", acos(far_mole_l / far_deno_l));
        printf("\n");
        
        cosx_minus = cosine;
        rotate_flg = 1;
        }
    }
    // left turn---------------------------------------------

    // right turn-----------------------------------------------
    if (marker_info[k].vertex[1][1] < marker_info[k].vertex[3][1] && marker_info[k].vertex[0][0] < marker_info[k].vertex[2][0]) {
        if (mark_11 < mark_31) {
        ver_pos_r[0] = marker_info[k].vertex[0][0];
        ver_pos_r[1] = marker_info[k].vertex[3][1];
//        printf("ver_pos = %f %f\n", ver_pos[0], ver_pos[1]);
//        printf("\n");
        
        // far between 2 points ----------------------------------------------------------------
        // sa
        far_a1_r = marker_info[k].vertex[0][0] - marker_info[k].vertex[3][0];
        far_a2_r = marker_info[k].vertex[0][1] - marker_info[k].vertex[3][1];
        far_b1_r = ver_pos_r[0] - marker_info[k].vertex[3][0];
        far_b2_r = ver_pos_r[1] - marker_info[k].vertex[3][1];
        // 2 jou
        far_a1_2_r = far_a1_r * far_a1_r;
        far_a2_2_r = far_a2_r * far_a2_r;
        far_b1_2_r = far_b1_r * far_b1_r;
        far_b2_2_r = far_b2_r * far_b2_r;
        // bunbo
        far_mole_r = far_a1_r * far_b1_r;
        root_a_r = sqrt(far_a1_2_r + far_a2_2_r);
        root_b_r = sqrt(far_b1_2_r + far_b2_2_r);
        far_deno_r = root_a_r * root_b_r;
        // cos(theta)
        cosine = far_mole_r / far_deno_r;
        
//        printf("root_a_r = %f\n", sqrt(far_a1_2_r + far_a2_2_r));
//        printf("root_b_r = %f\n", sqrt(far_b1_2_r + far_b2_2_r));
//        printf("far_mole_r = %f\n", far_mole_r);
//        printf("far_deno_r = %f\n", far_deno_r);
        printf("cos_r = %f\n", cosine);
//        printf("theta = %f\n", acos(far_mole_r / far_deno_r));
        printf("\n");
        
        cosx_plus = cosine;
            rotate_flg = 2;
        }
    }
    // right turn---------------------------------------------
    
    draw(cosine/*swFlag*/);

    argSwapBuffers();
}

static void init( void )
{
    ARParam  wparam;
	
    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    if( (patt_id=arLoadPatt(patt_name)) < 0 ) {
        printf("pattern load error !!\n");
        exit(0);
    }

    /* open the graphics window */
    argInit( &cparam, 1.0, 0, 0, 0, 0 );
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

float GetRandom(float min, float max)
{
	return min + (float)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

static void draw(double cosine/*int flg*/)
{
    double    gl_para[16];
    GLfloat   mat_ambient[]     = {1.1, 0.95, 0.95, 0.0};  // color of object
    GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
//    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
//    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
//    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
        
    /* circle */
    int i, n = PART;
    float x, y, r = 20.0;
    double rate;

/*
    static const char img[] = "Data/tree.png";
    GLubyte image[WIDTH][HEIGHT][3];
    FILE *fp;

    if((fp = fopen(img, "rb")) != NULL) {
        fread(image, sizeof image, 1, fp);
        fclose(fp);
    }
    else{
        perror(img);
        printf("image not reading !\n");
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
*/
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /* load the camera transformation matrix */
    argConvGlpara(patt_trans, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );

//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_TEXTURE_2D);

    glNormal3b(0.0, 0.0, 1.0);
    
    glPushMatrix();
    glTranslatef(0.0, -17.0, 0.0);
    
    // 15 count later
    if (test_flag > 0) {
        glColor3f(0.0, 0.0, 0.0);
        // stem
        glBegin(GL_QUADS);
//        glColor3f(0.0, 0.0, 0.0);
    //    glTexCoord2d(0.0, 10.0);
        glVertex3d(-2.0, -10.0, 0.0);
    //    glTexCoord2d(10.0, 10.0);
        glVertex3d(2.0, -10.0, 0.0);
    //    glTexCoord2d(10.0, 0.0);
        glVertex3d(2.0, 10.0, 0.0);
    //    glTexCoord2d(0.0, 0.0);
        glVertex3d(-2.0, 10.0, 0.0);
        glEnd();
        glPopMatrix();
        
        // root-----------------------------------------
        if (groot_count > 5) {
            // center
            glPushMatrix();
            glTranslatef(0.0, -17.0, 0.0);
            glBegin(GL_TRIANGLES);
            glVertex3f(-1.9, -10.0, 0.0);
            glVertex3f(1.9, -10.0, 0.0);
            glVertex3f(0.0, -30.0, 0.0);
            glEnd();
            glPopMatrix();
            // left
            glPushMatrix();
            glTranslatef(0.0, -17.0, 0.0);
            glBegin(GL_TRIANGLES);
            glVertex3f(-2.0, -8.0, 0.0);
            glVertex3f(-1.0, -10.0, 0.0);
            glVertex3f(-8.0, -20.0, 0.0);
            glEnd();
            glPopMatrix();
            // right
            glPushMatrix();
            glTranslatef(0.0, -17.0, 0.0);
            glBegin(GL_TRIANGLES);
            glVertex3f(2.0, -8.0, 0.0);
            glVertex3f(1.0, -10.0, 0.0);
            glVertex3f(8.0, -20.0, 0.0);
            glEnd();
            glPopMatrix();

        }
        // root-----------------------------------------
        
        // circle
        glPushMatrix();
        glTranslatef(0.0, 20.0, 0.0);
        glBegin(GL_POLYGON);
        for (i = 0; i < n; i++) {
            rate = (double)i / n;
            x = r * cos(2.0 * PAI * rate);
            y = r * sin(2.0 * PAI * rate);
            glVertex3f(x, y, 0.0);
        }
        glEnd();
        glTranslatef(10.0, -20.0, 0.0);
        glBegin(GL_POLYGON);
        for (i = 0; i < n; i++) {
            rate = (double)i / n;
            x = r * cos(2.0 * PAI * rate);
            y = r * sin(2.0 * PAI * rate);
            glVertex3f(x, y, 0.0);
        }
        glEnd();
        glTranslatef(-20.0, 0.0, 0.0);
        glBegin(GL_POLYGON);
        for (i = 0; i < n; i++) {
            rate = (double)i / n;
            x = r * cos(2.0 * PAI * rate);
            y = r * sin(2.0 * PAI * rate);
            glVertex3f(x, y, 0.0);
        }
        glEnd();
        
        // over 30 turn
/*
        if (cosine <= 0.866) {
            glTranslatef(-60.0, 0.0, 0.0);
            glBegin(GL_POLYGON);
            for (i = 0; i < n; i++) {
                rate = (double)i / n;
                x = r * cos(2.0 * PAI * rate);
                y = r * sin(2.0 * PAI * rate);
                glVertex3f(x, y, 0.0);
            }
            glEnd();
            glTranslatef(140.0, 0.0, 0.0);
            glBegin(GL_POLYGON);
            for (i = 0; i < n; i++) {
                rate = (double)i / n;
                x = r * cos(2.0 * PAI * rate);
                y = r * sin(2.0 * PAI * rate);
                glVertex3f(x, y, 0.0);
            }
            glEnd();
        }
*/
        glPopMatrix();
 
        glFlush();
    }
    
    // initial
    if (test_flag == 0) {
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_QUADS);  // stem
        //    glTexCoord2d(0.0, 10.0);
        glVertex3d(-2.0, -10.0, 0.0);
        //    glTexCoord2d(10.0, 10.0);
        glVertex3d(2.0, -10.0, 0.0);
        //    glTexCoord2d(10.0, 0.0);
        glVertex3d(2.0, 10.0, 0.0);
        //    glTexCoord2d(0.0, 0.0);
        glVertex3d(-2.0, 10.0, 0.0);
        glEnd();
        
        glPopMatrix();
        // circle
        glPushMatrix();
        glTranslatef(0.0, 10.0, 0.0);
        glBegin(GL_POLYGON);
        for (i = 0; i < n; i++) {
            rate = (double)i / n;
            x = r * cos(2.0 * PAI * rate);
            y = r * sin(2.0 * PAI * rate);
            glVertex3f(x, y, 0.0);
        }
        glEnd();
        glFlush();
        
        glPopMatrix();
    }
//    if(flg > 0) {
//        glTranslatef( 0.0, 0.0, 25.0 );
//        glutSolidCube(50.0);
//    }
//    else{
//        glTranslatef( 0.0, 0.0, 30.0 );
//        glRotatef(90.0, 1.0, 0.0, 0.0);
//        glutSolidTeapot(50.0);
//    }

    glDisable(GL_TEXTURE_2D);
    
//    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}