//
//  File.cpp
//  ARToolKit
//
//  Created by shimonaka on 2014/04/23.
//
//

#include "File.h"

// ============================================================================
//	Constants and types.
// ============================================================================

#define VIEW_SCALEFACTOR		0.025		// 1.0 ARToolKit unit becomes 0.025 of my OpenGL units.
#define VIEW_DISTANCE_MIN		0.1			// Objects closer to the camera than this will not be displayed.
#define VIEW_DISTANCE_MAX		100.0		// Objects further away from the camera than this will not be displayed.

// For cases in which we have multiple OpenGL contexts, never more than this many.
#define CONTEXTSACTIVECOUNT		2
#define CONTEXTSACTIVECOUNTMAX	CONTEXTSACTIVECOUNT

// Structure to keep track of per-camera variables.
typedef struct {
	int							apiContextIndex;	// API-specific index into an array of display contexts.
	ARParam						ARTCparam;			// Camera parameter.
	AR2VideoParamT				*ARTVideo;			// Video parameters
	ARUint8						*ARTImage;			// Most recent image.
	int							ARTThreshhold;		// Threshold for marker detection.
	long						callCountMarkerDetect;	// Frames received.
	double						patt_trans[3][4];	// Marker transformation.
	int                         patt_found;			// Whether marker transformation is valid.
	ARGL_CONTEXT_SETTINGS_REF	arglSettings;		// Settings from ARGL.
} CONTEXT_INFO;

// ============================================================================
//	Global variables.
// ============================================================================

static GLuint *gDrawListBox = NULL;

CONTEXT_INFO *gContextsActive;
int gContextsActiveCount = 0;

// ARToolKit globals.
static long			gCallCountGetImage = 0;
static int			gPatt_id;
static double		gPatt_width     = 80.0;
static double		gPatt_centre[2] = {0.0, 0.0};

// Other globals.
int             xsize, ysize;

// hikaku you
double          pre_patt_trans[3];
double          mark_11 = 0;
double          mark_31 = 0;

// mainloop time of doing count
int             do_count = 0;
// as cording count
int             test_count = 0;
// as cording flag
int             test_flag = 0;

// ============================================================================
//	Members
// ============================================================================

double ground::mak_posx = 0;
double ground::mak_posy= 0;
int ground::gr_count = 0;
int ground::gr_flg = 0;
double ground::groot_count = 0;
int ground::draw_flg = 0;
double ground::gr_xpos = 0;
double ground::gr_ypos = 0;
float ground::copy_array_right[];
float ground::copy_array_left[];

int snow::down_count[SNOW];
int snow::snowxpos_flg[SNOW];
int snow::snowypos_flg[SNOW];
float snow::snow_xpos[SNOW];
float snow::snow_ypos[SNOW];
int snow::snow_cou[SNOW];
float snow::xflow[SNOW];
float snow::snow_noise[SNOW];
float snow::fall_flg = 0;
int snow::rotate_flg = 0;
double snow::cosx_plus = 1.0;
double snow::cosx_minus = 1.0;
float snow::alfa = 0.5;
int snow::l = 0;
double snow::cosine = 0;


void circle::make(void)
{
    const float PAI = 3.14159265358979;
    
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++) {
        rate = (double)i / n;
        x = r * cos(2.0 * PAI * rate);
        y = r * sin(2.0 * PAI * rate);
        glVertex3f(x/100, y/100, 0.0);
    }
    glEnd();
}

void circle::draw()
{
    glPushMatrix();
    glTranslatef(0.0, 0.4, 0.0);
    
    circle::make();
    
    glTranslatef(0.1, -0.2, 0.0);
    circle::make();
    
    glTranslatef(-0.2, 0.0, 0.0);
    circle::make();
    
    glPopMatrix();
}

void ground::draw(void)
{
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    float array_right[ground::draw_flg + 1];
    float array_left[ground::draw_flg + 1];
    
    for (int n = 0; n < ground::draw_flg; n++) {
        array_right[n]=ground::copy_array_right[n];
        array_left[n]=ground::copy_array_left[n];
    }
    
    array_right[ground::draw_flg]=random::GetRandom_plus(-0.2, 0.2);
    array_left[ground::draw_flg]=random::GetRandom_plus(-0.2, 0.2);
    
    for (int i = 0; i < ground::draw_flg + 1; i++) {
        glVertex2d(ground::gr_xpos / 100 + 0.2 * i, (ysize / 1.2 - ground::gr_ypos) - (ysize / 120) + array_right[i]); // KOKO! (BUFFALO = 12)(CCD = 15)
        glVertex2d(ground::gr_xpos / 100 + 0.2 * (i + 1), (ysize / 1.2 - ground::gr_ypos) - (ysize / 120) + array_right[i + 1]);
        glVertex2d(ground::gr_xpos / 100 + 0.2 * (i + 1), 0 - ysize / 2);
        glVertex2d(ground::gr_xpos / 100 + 0.2 * i, 0 - ysize / 2);
        
        glVertex2d(ground::gr_xpos / 100 - 0.2 * i, (ysize / 1.2 - ground::gr_ypos) - (ysize / 120) + array_left[i]); // KOKO! (BUFFALO = 12)(CCD = 15)
        glVertex2d(ground::gr_xpos / 100 - 0.2 * (i + 1), (ysize / 1.2 - ground::gr_ypos) - (ysize / 120) + array_left[i + 1]);
        glVertex2d(ground::gr_xpos / 100 - 0.2 * (i + 1), 0 - ysize / 2);
        glVertex2d(ground::gr_xpos / 100 - 0.2 * i, 0 - ysize / 2);
    }
    
    for (int m = 0; m < ground::draw_flg + 1; m++) {
        ground::copy_array_right[m] = array_right[m];
        ground::copy_array_left[m] = array_left[m];
    }
    
    glEnd();
}

void ground::draw_flag()
{
    /*
     if (test_flag == 1 && ground::gr_flg == 0) {
     ground::gr_xpos = ground::mak_posx;
     ground::gr_ypos = ground::mak_posy;
     ground::gr_flg = 1;
     }
     */
    if (test_flag == 1 && ground::gr_flg == 1) {
        ground::draw();
        if (ground::draw_flg < 1000) {
            ground::draw_flg ++;
        }
        if (ground::gr_count < xsize) {
            ground::gr_count += 6; // KOKO! (BUFFALO = 40)(CCD = 6)
            ground::groot_count++;
        }
        glColor3f(1.0, 1.0, 1.0);
    }
}

void snow::flow_right(void)
{
    if (snow::cosx_plus >= 0.995 && snow::cosx_plus < 1.0) {
        snow::xflow[l] += 0;
    }
    else if (snow::cosx_plus >= 0.98 && snow::cosx_plus < 0.995) {
        snow::xflow[l] += (snow::alfa / 2);
    }
    else if (snow::cosx_plus >= 0.97 && snow::cosx_plus < 0.98) {
        snow::xflow[l] += snow::alfa;
    }
    else if (snow::cosx_plus >= 0.96 && snow::cosx_plus < 0.97) {
        snow::xflow[l] += (snow::alfa * 2);
    }
    else if (snow::cosx_plus >= 0.95 && snow::cosx_plus < 0.96) {
        snow::xflow[l] += (snow::alfa * 3);
    }
    else if (snow::cosx_plus >= 0.94 && snow::cosx_plus < 0.95) {
        snow::xflow[l] += (snow::alfa * 4);
    }
    else if (snow::cosx_plus >= 0.93 && snow::cosx_plus < 0.94) {
        snow::xflow[l] += (snow::alfa * 5);
    }
    else if (snow::cosx_plus >= 0.92 && snow::cosx_plus < 0.93) {
        snow::xflow[l] += (snow::alfa * 6);
    }
    else if (snow::cosx_plus >= 0.91 && snow::cosx_plus < 0.92) {
        snow::xflow[l] += (snow::alfa * 7);
    }
    else if (snow::cosx_plus >= 0.90 && snow::cosx_plus < 0.91) {
        snow::xflow[l] += (snow::alfa * 8);
    }
    else if (snow::cosx_plus >= 0.88 && snow::cosx_plus < 0.89) {
        snow::xflow[l] += (snow::alfa * 9);
    }
    else if (snow::cosx_plus >= 0.87 && snow::cosx_plus< 0.88) {
        snow::xflow[l] += (snow::alfa * 10);
    }
    else if (snow::cosx_plus >= 0.86 && snow::cosx_plus < 0.87) {
        snow::xflow[l] += (snow::alfa * 11);
    }
    else if (snow::cosx_plus >= 0.85 && snow::cosx_plus< 0.86) {
        snow::xflow[l] += (snow::alfa * 12);
    }
    else if (snow::cosx_plus >= 0.84 && snow::cosx_plus < 0.85) {
        snow::xflow[l] += (snow::alfa * 13);
    }
    else if (snow::cosx_plus >= 0.825 && snow::cosx_plus < 0.84) {
        snow::xflow[l] += (snow::alfa * 14);
    }
    else if (snow::cosx_plus < 0.825) {
        snow::xflow[l] += (snow::alfa * 15);
    }
    
}

void snow::flow_left(void)
{
    if (snow::cosx_minus >= 0.995 && snow::cosx_minus < 1.0) {
        snow::xflow[l] -= 0;
    }
    else if (snow::cosx_plus >= 0.98 && snow::cosx_plus < 0.995) {
        snow::xflow[l] -= (snow::alfa / 2);
    }
    else if (snow::cosx_minus >= 0.97 && snow::cosx_minus < 0.98) {
        snow::xflow[l] -= snow::alfa;
    }
    else if (snow::cosx_minus >= 0.96 && snow::cosx_minus < 0.97) {
        snow::xflow[l] -= (snow::alfa * 2);
    }
    else if (snow::cosx_minus >= 0.95 && snow::cosx_minus < 0.96) {
        snow::xflow[l] -= (snow::alfa * 3);
    }
    else if (snow::cosx_minus >= 0.94 && snow::cosx_minus < 0.95) {
        snow::xflow[l] -= (snow::alfa * 4);
    }
    else if (snow::cosx_minus >= 0.93 && snow::cosx_minus < 0.94) {
        snow::xflow[l] -= (snow::alfa * 5);
    }
    else if (snow::cosx_minus >= 0.92 && snow::cosx_minus < 0.93) {
        snow::xflow[l] -= (snow::alfa * 6);
    }
    else if (snow::cosx_minus >= 0.91 && snow::cosx_minus < 0.92) {
        snow::xflow[l] -= (snow::alfa * 7);
    }
    else if (snow::cosx_minus >= 0.90 && snow::cosx_minus < 0.91) {
        snow::xflow[l] -= (snow::alfa * 8);
    }
    else if (snow::cosx_minus >= 0.88 && snow::cosx_minus < 0.89) {
        snow::xflow[l] -= (snow::alfa * 9);
    }
    else if (snow::cosx_minus >= 0.87 && snow::cosx_minus < 0.88) {
        snow::xflow[l] -= (snow::alfa * 10);
    }
    else if (snow::cosx_minus >= 0.86 && snow::cosx_minus < 0.87) {
        snow::xflow[l] -= (snow::alfa * 11);
    }
    else if (snow::cosx_minus >= 0.85 && snow::cosx_minus < 0.86) {
        snow::xflow[l] -= (snow::alfa * 12);
    }
    else if (snow::cosx_minus >= 0.84 && snow::cosx_minus < 0.85) {
        snow::xflow[l] -= (snow::alfa * 13);
    }
    else if (snow::cosx_minus >= 0.825 && snow::cosx_minus < 0.84) {
        snow::xflow[l] -= (snow::alfa * 14);
    }
    else if (snow::cosx_minus < 0.825) {
        snow::xflow[l] -= (snow::alfa * 15);
    }
}

void snow::draw()
{
    circle draw;
    
    //    draw.r = 2;
    for (snow::l = 0; snow::l < SNOW; snow::l++) {
        if (test_flag == 1 && snow::snowxpos_flg[snow::l] == 0) {
            snow::snow_xpos[snow::l] = random::GetRandom(0, 1 + 0);
            snow::snow_noise[snow::l] = random::GetRandom(-1.0, 1.0);
            snow::snowxpos_flg[snow::l] = 1;
        }
        if (test_flag == 1 && snow::snowypos_flg[snow::l] == 0) {
            snow::snow_ypos[snow::l] = random::GetRandom(0, 1);
            snow::snowypos_flg[snow::l] = 1;
        }
        
        if (test_flag == 1 && snow::snowxpos_flg[snow::l] == 1) {
            glPushMatrix();
            if (snow::rotate_flg == 2) {
                //                snow::flow_right();
            }
            if (snow::rotate_flg == 1) {
                //                snow::flow_left();
            }
            glTranslatef(snow::snow_xpos[snow::l] - snow::xflow[snow::l], (10/*ysize*/ - snow::snow_ypos[snow::l]) - snow::down_count[snow::l], 0.0); // KOKO! (BUFFALO = -200)(CCD = 0)
            glColor3f(0.0, 0.0, 0.0);
            
            draw.make();
            
            glFlush();
            glPopMatrix();
            if (((10/*ysize*/ - snow::snow_ypos[snow::l]) - snow::down_count[snow::l]) >= 0) { // KOKO! (BUFFALO = -200)(CCD = 0)
                snow::down_count[snow::l] = snow::down_count[snow::l] + 0.05 - snow::snow_noise[snow::l]; // KOKO! (BUFFALO = 20)(CCD = 3)
            }
            else if (((10/*ysize*/ - snow::snow_ypos[snow::l]) - snow::down_count[snow::l]) < 0) { // KOKO! (BUFFALO = -200)(CCD = 0)
                snow::down_count[snow::l] = 0;
                snow::snowxpos_flg[snow::l] = 0;
                snow::snow_ypos[snow::l] = 0;
                snow::xflow[snow::l] = 0;
                snow::snow_noise[snow::l] = 0;
            }
        }
    }
}


void fan::draw(void)
{
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);  // stem
    glVertex3d(-0.02, -0.1, 0.0);
    glVertex3d(0.02, -0.1, 0.0);
    glVertex3d(0.02, 0.1, 0.0);
    glVertex3d(-0.02, 0.1, 0.0);
    glEnd();
    
    // circle
    glPushMatrix();
    glTranslatef(0.0, 0.3, 0.0);
    
    draw_circle.make();
    
    glPopMatrix();
}

void root::draw()
{
    // center
    glPushMatrix();
    glTranslatef(0.0, -0.00, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.019, -0.1, 0.0);
    glVertex3f(0.019, -0.1, 0.0);
    glVertex3f(0.0, -0.3, 0.0);
    glEnd();
    glPopMatrix();
    // left
    glPushMatrix();
    glTranslatef(0.0, -0.00, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.02, -0.08, 0.0);
    glVertex3f(-0.01, -0.1, 0.0);
    glVertex3f(-0.08, -0.2, 0.0);
    glEnd();
    glPopMatrix();
    // right
    glPushMatrix();
    glTranslatef(0.0, -0.00, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.02, -0.08, 0.0);
    glVertex3f(0.01, -0.1, 0.0);
    glVertex3f(0.08, -0.2, 0.0);
    glEnd();
    glPopMatrix();
}

// ============================================================================
//	Functions
// ============================================================================

static int DrawInit(int contextsActiveCountMax)
{
	// Allocate room for display lists for all contexts.
	if (gDrawListBox) return (FALSE); // Sanity check.
	if ((gDrawListBox = (GLuint *)calloc(contextsActiveCountMax, sizeof(GLuint))) == NULL) {
		return (FALSE);
	}
	return (TRUE);
}

static int DrawSetup(int contextIndex)
{
    root ro_draw;
    circle ci_draw;
    fan fa_draw;
    
	if (!gDrawListBox[contextIndex]) {
		gDrawListBox[contextIndex] = glGenLists (3);
		glNewList(1/*gDrawListBox[contextIndex]*/, GL_COMPILE);
        
        fa_draw.draw();
        
        glEndList();
        
        glNewList(2/*gDrawListBox[contextIndex]*/, GL_COMPILE);
        
        glColor3f(0.0, 0.0, 0.0);
        // stem
        glBegin(GL_QUADS);
        glVertex3f(-0.02, -0.1, 0.0);
        glVertex3f(0.02, -0.1, 0.0);
        glVertex3f(0.02, 0.1, 0.0);
        glVertex3f(-0.02, 0.1, 0.0);
        glEnd();
        
        ci_draw.draw();
        
		glEndList ();
        
        glNewList(3, GL_COMPILE);
        ro_draw.draw();
        glEndList();
	}
	return (TRUE);
}

// Something to look at, draw a rotating colour cube.
static void Draw(int contextIndex)
{
	glPushMatrix(); // Save world coordinate system.
	glTranslatef(0.0, 0.0, 0.5); // Place base of cube on marker surface.
	glDisable(GL_LIGHTING);	// Just use colours.
    if (test_flag == 0) {
        glCallList(1/*gDrawListBox[contextIndex]*/);	// Draw the cube.
    }
    if (test_flag > 0) {
        glCallList(2/*gDrawListBox[contextIndex]*/);	// Draw the cube.
        if (ground::groot_count > 5) {
            glCallList(3);
        }
    }
	glPopMatrix();	// Restore world coordinate system.
}

/*
 static void DrawUpdate(void)
 {
 
 }
 */

static int DrawCleanup(int contextIndex)
{
	if (contextIndex >= gContextsActiveCount) return (FALSE); // Sanity check.
	
	// Destroy display lists...
	if (gDrawListBox[contextIndex]) {
		glDeleteLists(gDrawListBox[contextIndex], 1);
		gDrawListBox[contextIndex] = 0;
	}
	return (TRUE);
}

static int DrawFinal(void)
{
	if (gDrawListBox) {
		free(gDrawListBox);
		gDrawListBox = NULL;
	}
	return (TRUE);
}

// Sets up fields ARTVideo, ARTCparam of gContextsActive[0] through gContextsActive[cameraCount - 1].
static int setupCameras(const int cameraCount, const char *cparam_names[], char *vconfs[])
{
	int i;
	ARParam wparam;
	
	for (i = 0; i < cameraCount; i++) {
		
		// Open the video path.
		if ((gContextsActive[i].ARTVideo = ar2VideoOpen(vconfs[i])) == NULL) {
			fprintf(stderr, "setupCameras(): Unable to open connection to camera %d.\n", i + 1);
			return (FALSE);
		}
		
		// Find the size of the window.
		if (ar2VideoInqSize(gContextsActive[i].ARTVideo, &xsize, &ysize) < 0) return (FALSE);
		fprintf(stderr, "setupCameras(): Camera %d image size (x,y) = (%d,%d)\n", i + 1, xsize, ysize);
        
		// Load the camera parameters, resize for the window and init.
		if (arParamLoad(cparam_names[i], 1, &wparam) < 0) {
			fprintf(stderr, "setupCameras(): Error loading parameter file %s for camera %d.\n", cparam_names[i], i + 1);
			return (FALSE);
		}
		arParamChangeSize(&wparam, xsize, ysize, &(gContextsActive[i].ARTCparam));
		arInitCparam(&(gContextsActive[i].ARTCparam));
		fprintf(stderr, "*** Camera %d parameter ***\n", i + 1);
		arParamDisp(&(gContextsActive[i].ARTCparam));
		gContextsActive[i].ARTThreshhold = 100;
		
		// Start the video capture for this camera.
		if (ar2VideoCapStart(gContextsActive[i].ARTVideo) != 0) {
			fprintf(stderr, "setupCameras(): Unable to begin camera data capture for camera %d.\n", i + 1);
			return (FALSE);
		}
	}
	return (TRUE);
}

static int setupMarker(const char *patt_name, int *patt_id)
{
	// Loading only 1 pattern in this example.
    if ((*patt_id = arLoadPatt(patt_name)) < 0) {
        fprintf(stderr, "setupMarker(): pattern load error !!\n");
        return (FALSE);
    }
	return (TRUE);
}

// Report state of ARToolKit global variables arFittingMode,
// arImageProcMode, arglDrawMode, arTemplateMatchingMode, arMatchingPCAMode.
static void debugReportMode(ARGL_CONTEXT_SETTINGS_REF	arglSettings)
{
	if (arFittingMode == AR_FITTING_TO_INPUT) {
		fprintf(stderr, "FittingMode (Z): INPUT IMAGE\n");
	} else {
		fprintf(stderr, "FittingMode (Z): COMPENSATED IMAGE\n");
	}
	
	if (arImageProcMode == AR_IMAGE_PROC_IN_FULL) {
		fprintf(stderr, "ProcMode (X)   : FULL IMAGE\n");
	} else {
		fprintf(stderr, "ProcMode (X)   : HALF IMAGE\n");
	}
	
	if (arglDrawModeGet(arglSettings) == AR_DRAW_BY_GL_DRAW_PIXELS) {
		fprintf(stderr, "DrawMode (C)   : GL_DRAW_PIXELS\n");
	} else if (arglTexmapModeGet(arglSettings) == AR_DRAW_TEXTURE_FULL_IMAGE) {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (FULL RESOLUTION)\n");
	} else {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (HALF RESOLUTION)\n");
	}
	
	if (arTemplateMatchingMode == AR_TEMPLATE_MATCHING_COLOR) {
		fprintf(stderr, "TemplateMatchingMode (M)   : Color Template\n");
	} else {
		fprintf(stderr, "TemplateMatchingMode (M)   : BW Template\n");
	}
	
	if (arMatchingPCAMode == AR_MATCHING_WITHOUT_PCA) {
		fprintf(stderr, "MatchingPCAMode (P)   : Without PCA\n");
	} else {
		fprintf(stderr, "MatchingPCAMode (P)   : With PCA\n");
	}
#ifdef APPLE_TEXTURE_FAST_TRANSFER
	fprintf(stderr, "arglAppleClientStorage is %d.\n", arglAppleClientStorage);
	fprintf(stderr, "arglAppleTextureRange is %d.\n", arglAppleTextureRange);
#endif // APPLE_TEXTURE_FAST_TRANSFER
}

// Function to clean up and then exit. Will be
// installed by atexit() and called when program exit()s.
static void Quit(void)
{
	int i;
	
	fprintf(stdout, "Quitting...\n");
    
	// OpenGL per-context cleanup.
	for (i = 0; i < gContextsActiveCount; i++) {
		if (gContextsActive[i].apiContextIndex) {
			glutSetWindow(gContextsActive[i].apiContextIndex);
			arglCleanup(gContextsActive[i].arglSettings);
			DrawCleanup(i);
			glutDestroyWindow(gContextsActive[i].apiContextIndex);
			gContextsActive[i].apiContextIndex = 0;
		}
		ar2VideoCapStop(gContextsActive[i].ARTVideo);
		ar2VideoClose(gContextsActive[i].ARTVideo);
	}
	gContextsActiveCount = 0;
	
	// Library finals (in reverse order to inits.)
	DrawFinal();
}

static void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 0x1B:						// Quit.
		case 'Q':
		case 'q':
			exit(0);
			break;
		default:
			break;
	}
}

static void Idle(void)
{
    circle loop;
    loop.r = 3;
    
	int i;
	static int ms_prev;
	int ms;
	float s_elapsed;
	ARUint8 *image;                                 // window image
    
	ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
    int             marker_num;						// Count of number of markers detected.
    int             j, k;
	
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
    
	// Find out how long since Idle() last ran.
	ms = glutGet(GLUT_ELAPSED_TIME);
	s_elapsed = (float)(ms - ms_prev) * 0.001;
	if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
	ms_prev = ms;
    
	gCallCountGetImage++; // Increment Idle() counter.
	
    if (test_flag == 1 && ground::gr_flg == 0) {
        ground::gr_xpos = ground::mak_posx;
        ground::gr_ypos = ground::mak_posy;
        ground::gr_flg = 1;
    }
    
	for (i = 0; i < gContextsActiveCount; i++) {
		
		// Grab a video frame.
		if ((image = ar2VideoGetImage(gContextsActive[i].ARTVideo)) != NULL) {
            // 2TIKAWAKOKOKANA..........................................................................................
			gContextsActive[i].ARTImage = image;	// Save the fetched image.
			
			gContextsActive[i].callCountMarkerDetect++; // Increment ARToolKit FPS counter.
			//fprintf(stderr, "Idle(): Got image #%ld from cam %d on attempt #%ld.\n", gContextsActive[i].callCountMarkerDetect, i + 1, gCallCountGetImage);
            
			// Detect the markers in the video frame.
			if (arDetectMarkerLite(gContextsActive[i].ARTImage, gContextsActive[i].ARTThreshhold, &marker_info, &marker_num) < 0) {
				exit(-1);
			}
			
            if (i >= 1) {
                // Check through the marker_info array for highest confidence
                // visible marker matching our preferred pattern.
                k = -1;
                for (j = 0; j < marker_num; j++) {
                    if (marker_info[j].id == gPatt_id) {
                        if (k == -1) k = j; // First marker detected.
                        else if (marker_info[j].cf > marker_info[k].cf) k = j; // Higher confidence marker detected.
                    }
                }
                
                if (k == -1) {
                    gContextsActive[i].patt_found = FALSE;
                    test_count = 0;
                    test_flag = 0;
                    do_count = 0;
                    ground::gr_count = 0;
                    ground::gr_flg = 0;
                    ground::groot_count = 0;
                    for (snow::l = 0; snow::l < SNOW; snow::l++) {
                        snow::snowxpos_flg[snow::l] = 0;
                        snow::snowypos_flg[snow::l] = 0;
                        snow::down_count[snow::l] = 0;
                        snow::xflow[snow::l] = 0;
                    }
                    snow::cosx_minus = 1.0;
                    snow::cosx_plus = 1.0;
                    snow::rotate_flg = 0;
                    mark_11 = 0;
                    mark_31 = 0;
                    ground::draw_flg  = 0;
                }
                else if(k != -1) {
                    // Get the transformation between the marker and the real camera into gPatt_trans1.
                    arGetTransMat(&(marker_info[k]), gPatt_centre, gPatt_width, gContextsActive[i].patt_trans);
                    gContextsActive[i].patt_found = TRUE;
                }
                
                if ((marker_info[k].vertex[0][0] - marker_info[k].vertex[3][0] > -10 && marker_info[k].vertex[0][0] - marker_info[k].vertex[3][0] < 10) || (marker_info[k].vertex[2][0] - marker_info[k].vertex[3][0] > -10 && marker_info[k].vertex[2][0] - marker_info[k].vertex[3][0] < 10)) {
                    mark_11 = marker_info[k].vertex[1][1];
                    mark_31 = marker_info[k].vertex[3][1];
                }
                
                printf("[0][3]=%5.2f [1][3]=%5.2f [2][3]=%5.2f\n", gContextsActive[i].patt_trans[0][3], gContextsActive[i].patt_trans[1][3], gContextsActive[i].patt_trans[2][3]);
                //    printf("\n");
                printf("[0]=%5.2f [1]=%5.2f [2]=%5.2f\n", pre_patt_trans[0], pre_patt_trans[1], pre_patt_trans[2]);
                printf("\n");
                
                ground::mak_posx = marker_info[k].pos[0];
                ground::mak_posy = marker_info[k].pos[1];
                
                // hikaku sena...
                if (do_count > 0 && test_count < 15) {  // KOKO! (BUFFALO = 15)(CCD = 50)
                    if ((0 <= (gContextsActive[i].patt_trans[0][3] - pre_patt_trans[0]) && (gContextsActive[i].patt_trans[0][3] - pre_patt_trans[0]) < 10) || (0 >= (gContextsActive[i].patt_trans[0][3] - pre_patt_trans[0]) && (gContextsActive[i].patt_trans[0][3] - pre_patt_trans[0]) > -10)) {  // KOKO! (BUFFALO = 3)(CCD = 0.5)
                        // only 1 jikan start
                        // now - start > 3 de tree
                        test_count++;
                        printf("%f\n", gContextsActive[i].patt_trans[0][3] - pre_patt_trans[0]);
                        printf("%d\n", test_count);
                        printf("\n");
                    }
                    else{
                        // now =
                        test_count = 0;
                        test_flag = 0;
                        printf("%f\n", gContextsActive[i].patt_trans[0][3] - pre_patt_trans[0]);
                        printf("%d\n", test_count);
                        printf("\n");
                    }
                }
                if (test_count >= 15) { // KOKO! (BUFFALO = 15)(CCD = 50)
                    test_flag = 1;
                }
                
                pre_patt_trans[0] = gContextsActive[i].patt_trans[0][3];
                pre_patt_trans[1] = gContextsActive[i].patt_trans[1][3];
                pre_patt_trans[2] = gContextsActive[i].patt_trans[2][3];
                do_count++;
                
                printf("do_count=%d\n",do_count);
                printf("test_count=%d\n",test_count);
                
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
                        snow::cosine = far_mole_l / far_deno_l;
                        
                        //        printf("root_a_l = %f\n", sqrt(far_a1_2_l +far_a2_2_l));
                        //        printf("root_b_l = %f\n", sqrt(far_b1_2_l + far_b2_2_l));
                        //        printf("far_mole_l = %f\n", far_mole_l);
                        //        printf("far_deno_l = %f\n", far_deno_l);
                        printf("cos_l = %f\n", snow::cosine);
                        //        printf("theta = %f\n", acos(far_mole_l / far_deno_l));
                        printf("\n");
                        
                        snow::cosx_minus = snow::cosine;
                        snow::rotate_flg = 1;
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
                        snow::cosine = far_mole_r / far_deno_r;
                        
                        //        printf("root_a_r = %f\n", sqrt(far_a1_2_r + far_a2_2_r));
                        //        printf("root_b_r = %f\n", sqrt(far_b1_2_r + far_b2_2_r));
                        //        printf("far_mole_r = %f\n", far_mole_r);
                        //        printf("far_deno_r = %f\n", far_deno_r);
                        printf("cos_r = %f\n", snow::cosine);
                        //        printf("theta = %f\n", acos(far_mole_r / far_deno_r));
                        printf("\n");
                        
                        snow::cosx_plus = snow::cosine;
                        snow::rotate_flg = 2;
                    }
                }
                // right turn---------------------------------------------
            }
            // Update drawing.
            //            DrawUpdate();
            
			glutPostWindowRedisplay(gContextsActive[i].apiContextIndex);
		}
	}
}

//
//	The function is called on events when the visibility of a
//	GLUT window changes (including when it first becomes visible).
//
static void Visibility(int visible)
{
	if (visible == GLUT_VISIBLE) {
		glutIdleFunc(Idle);
	} else {
		glutIdleFunc(NULL);
	}
}

//
//	This function is called when the
//	GLUT window is resized.
//
static void Reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
	// Call through to anyone else who needs to know about window sizing here.
}

//
// This function is called when a window needs redrawing.
//
static void DisplayPerContext(const int drawContextIndex)
{
    GLdouble p[16];
	GLdouble m[16];
    
    ground gr_draw;
    snow sn_draw;
	
	// Select correct buffer for this context.
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the buffers for new frame.
	
    // kokoyadeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee in
    if (drawContextIndex == 0) {
        glClearColor(1, 1, 1, 1); // debug you
        arglDispImage(0,    // 0 = debug, gContextsActive[drawContextIndex].ARTImage = true
                      &(gContextsActive[drawContextIndex].ARTCparam),
                      1.0,
                      gContextsActive[drawContextIndex].arglSettings);	// zoom = 1.0.
    }
    else if (drawContextIndex == 1) {
        arglDispImage(gContextsActive[drawContextIndex].ARTImage,
                      &(gContextsActive[drawContextIndex].ARTCparam),
                      1.0,
                      gContextsActive[drawContextIndex].arglSettings);	// zoom = 1.0.
    }
    // kokoyadeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee out
 	ar2VideoCapNext(gContextsActive[drawContextIndex].ARTVideo);
	gContextsActive[drawContextIndex].ARTImage = NULL; // Image data is no longer valid after calling ar2VideoCapNext().
	
	// Projection transformation.
	arglCameraFrustumRH(&(gContextsActive[drawContextIndex].ARTCparam), VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(p);
	glMatrixMode(GL_MODELVIEW);
    
	// Viewing transformation.
	glLoadIdentity();
	// Lighting and geometry that moves with the camera should go here.
	// (I.e. must be specified before viewing transformations.)
	//none
    
    // kokoyadeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee in
	if (gContextsActive[1/*drawContextIndex*/].patt_found) {
		
		// Calculate the camera position relative to the marker.
		// Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
		arglCameraViewRH(gContextsActive[1/*drawContextIndex*/].patt_trans, m, VIEW_SCALEFACTOR);
		glLoadMatrixd(m);
		
		// All lighting and geometry to be drawn relative to the marker goes here.
        if (drawContextIndex == 0) {
            Draw(drawContextIndex);
        }
        // kokoyadeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee out
        
	} // patt_found
	
	// Any 2D overlays go here.
	//none
    //    if (drawContextIndex == 0) {
    glEnable(GL_TEXTURE_2D);
    
    gr_draw.draw_flag();
    sn_draw.draw();
    
    glDisable(GL_TEXTURE_2D);
    //    }
	// Drawing for this context complete.
}

// Linear search through all active contexts to find context index for the current glut window.
int getContextIndexForCurrentGLUTWindow(void)
{
	int i, window;
	
	if ((window = glutGetWindow()) != 0) {
		for (i = 0; i < gContextsActiveCount; i++) {
			if (gContextsActive[i].apiContextIndex == window) return (i);
		}
	}
	return (-1);
}

static void Display(void)
{
	int contextIndex;
    
	if ((contextIndex = getContextIndexForCurrentGLUTWindow()) != -1) {
		DisplayPerContext(contextIndex);
		glutSwapBuffers();
	}
}

int main(int argc, char** argv)
{
	int i;
	char windowTitle[32] = {0};
	const char *cparam_names[] = { // Camera parameter names.
		"Data/camera_para.dat",
		"Data/camera_para.dat",
	};
	char *vconfs[] = {					// Camera configuration.
#if defined(_WIN32)
		"Data\\WDM_camera_flipV.xml",
		"Data\\WDM_camera_flipV.xml",
#elif defined(__APPLE__)
		"",
		"",
#else
		"-dev=/dev/video0 -channel=0 -palette=YUV420P -width=320 -height=240",
		"-dev=/dev/video1 -channel=0 -palette=YUV420P -width=320 -height=240",
#endif
	};
	const char *patt_name  = "Data/patt.hiro";
	
	// ----------------------------------------------------------------------------
	// Library inits.
	//
    
    srand((unsigned int)time(NULL));
    
	glutInit(&argc, argv);
    
	// Register a cleanup function to be called upon exit().
	if (atexit(Quit) < 0) {
		fprintf(stderr, "main(): Unable to register exit function.\n");
		exit(-1); // Bail out if we can't even register our exit function.
	}
    
	// Initialise drawing libraries.
	if (!DrawInit(CONTEXTSACTIVECOUNTMAX)) {
		fprintf(stderr, "main(): DrawCubeInit returned error.\n");
		exit(-1);
	}
	
	// ----------------------------------------------------------------------------
	// Hardware setup.
	//
    
	if ((gContextsActive = (CONTEXT_INFO *)calloc(CONTEXTSACTIVECOUNTMAX, sizeof(CONTEXT_INFO))) == NULL) exit(-1);
	if (!setupCameras(CONTEXTSACTIVECOUNT, cparam_names, vconfs)) {
		fprintf(stderr, "main(): Unable to set up %d AR cameras.\n", CONTEXTSACTIVECOUNT);
		exit(-1);
	}
	gContextsActiveCount = CONTEXTSACTIVECOUNT;
    
	// ----------------------------------------------------------------------------
	// Library setup.
	//
	
	// Per- GL context setup.
	for (i = 0; i < gContextsActiveCount; i++ ) {
		
		// Set up GL context(s) for OpenGL to draw into.
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(gContextsActive[i].ARTCparam.xsize, gContextsActive[i].ARTCparam.ysize);
		glutInitWindowPosition(10 + 100*i, 20 + 100*i); // First window at 20,10, subsequent windows staggered.
		sprintf(windowTitle, "Video source %i", i);
		if ((gContextsActive[i].apiContextIndex = glutCreateWindow(windowTitle)) < 1) {
			fprintf(stderr, "main(): Unable to create window.\n");
			exit(-1);
		}
		glutDisplayFunc(Display);
		glutReshapeFunc(Reshape);
		glutVisibilityFunc(Visibility);
		glutKeyboardFunc(Keyboard);
		
		DrawSetup(i);
        
		if ((gContextsActive[i].arglSettings = arglSetupForCurrentContext()) == NULL) {
			fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
			exit(-1);
		}
		debugReportMode(gContextsActive[i].arglSettings);
		glEnable(GL_DEPTH_TEST);
	}
	arUtilTimerReset();
	
	if (!setupMarker(patt_name, &gPatt_id)) {
		fprintf(stderr, "main(): Unable to set up AR marker in context %d.\n", i);
		exit(-1);
	}	
    
	// Register GLUT event-handling callbacks.
	// NB: Idle() is registered by Visibility.
	glutMainLoop();
    
	return (0);
}
