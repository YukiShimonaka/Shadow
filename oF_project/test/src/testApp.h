#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

const int CAM_WIDTH = 320;
const int CAM_HEIGHT = 240;
const int leaf = 20;
const int SNOW = 100; // the number of snow
const int GR = 170;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        static bool draw_flag;
        int counter;
        float objX, objY;
        float disX, disY;
        float tempX, tempY;
    
        ofVideoGrabber cam80, cam94;
    
        ofxCvColorImage color80, color94;
        ofxCvGrayscaleImage gray80, gray94;
        ofxCvGrayscaleImage	backGround;
        ofxCvGrayscaleImage diffImage, arImage;
    
        ofxCvContourFinder contourFinder;
    
        ofImage treeImage;
        vector<ofPoint> treeImageCorners;
    
        bool bLearnBackground;
        int threshold;

};

class snow
{
public:
    void draw(void);
    void flow_right(void);
    void flow_left(void);
    
    static int             down_count[SNOW];
    static int             snowxpos_flg[SNOW];
    static int             snowypos_flg[SNOW];
    static float           snow_xpos[SNOW];
    static float           snow_ypos[SNOW];
    static float           xflow[SNOW];
    static float           snow_noise[SNOW];
//    static int             rotate_flg;
//    static float           alfa; // KOKO! (BUFFALO = 2.5)(CCD = 0.5)
    static int             l;
};

class ground
{
public:
    void flag(float w, float h);
    void draw(void);
    
//    static double          mak_posx;
//    static double          mak_posy;
    static int             gr_count;
    static int             gr_flg;
    static double          groot_count;
    static int             draw_flg;
    
private:
    static double          gr_xpos;
    static double          gr_ypos;
    static float           copy_array_right[GR];
    static float           copy_array_left[GR];
};

class grass {
public:
    void draw();
    
    static int flg;
    static float w;
    static float h;

private:
    void grass0(float x, float y);
    void grass1(float x, float y);
    void grass1_left(float x, float y);
    void grass1_right(float x, float y);

    float a = 40;
    float p = 84;
};

class tree{
public:
    void main(int x, int y, float a, int b, float w, float h);
    void root(int x, int y, float a, float w, float h);
    
};

class fan{
public:
    void setup(float w, float h);
};