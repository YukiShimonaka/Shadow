#include "testApp.h"

// --------------------------------------------------------------
//  Global Variables
// --------------------------------------------------------------

bool testApp::draw_flag = FALSE;

// as cording flag
int test_flag = 0;
int flow = 0;

// --------------------------------------------------------------
//  Members
// --------------------------------------------------------------
int snow::down_count[SNOW];
int snow::snowxpos_flg[SNOW];
int snow::snowypos_flg[SNOW];
float snow::snow_xpos[SNOW];
float snow::snow_ypos[SNOW];
float snow::xflow[SNOW];
float snow::snow_noise[SNOW];
//int snow::rotate_flg = 0;
//float snow::alfa = 0.5;
int snow::l = 0;

//double ground::mak_posx = 0;
//double ground::mak_posy= 0;
int ground::gr_count = 0;
int ground::gr_flg = 0;
double ground::groot_count = 0;
int ground::draw_flg = 0;
double ground::gr_xpos = 0;
double ground::gr_ypos = 0;
float ground::copy_array_right[];
float ground::copy_array_left[];

int grass::flg = 0;
float grass::w = 0;
float grass::h = 0;

void snow::draw()
{
    for (snow::l = 0; snow::l < SNOW; snow::l++) {
        if (test_flag == 1 && snow::snowxpos_flg[snow::l] == 0) {
            snow::snow_xpos[snow::l] = ofRandom(-500, ofGetWidth() + 500);
            snow::snow_noise[snow::l] = ofRandom(-3.0, 0);
            snow::snowxpos_flg[snow::l] = 1;
        }
        if (test_flag == 1 && snow::snowypos_flg[snow::l] == 0) {
            snow::snow_ypos[snow::l] = ofRandom(0, ofGetHeight());
            snow::snowypos_flg[snow::l] = 1;
        }
        
        if (test_flag == 1 && snow::snowxpos_flg[snow::l] == 1) {
            ofPushMatrix();
            /*
            if (snow::rotate_flg == 2) {
                    snow::flow_right();
            }
            if (snow::rotate_flg == 1) {
                    snow::flow_left();
            }
            */
            snow::xflow[snow::l] += flow;
            ofTranslate(snow::snow_xpos[snow::l] + snow::xflow[snow::l], snow::snow_ypos[snow::l] + snow::down_count[snow::l], 0.0);
            ofSetColor(0);
            ofCircle(0, 0, 3);
            ofPopMatrix();
            
            if ((snow::snow_ypos[snow::l] + snow::down_count[snow::l]) <= ofGetHeight()) {
                snow::down_count[snow::l] = snow::down_count[snow::l] + 5 + snow::snow_noise[snow::l];
            }
            else if ((snow::snow_ypos[snow::l] + snow::down_count[snow::l]) > ofGetHeight()) {
                snow::down_count[snow::l] = 0;
                snow::snowxpos_flg[snow::l] = 0;
                snow::snow_ypos[snow::l] = 0;
                snow::xflow[snow::l] = 0;
                snow::snow_noise[snow::l] = 0;
            }
        }
    }
}

void ground::draw(){
    ofSetColor(0);
    float array_right[ground::draw_flg + 2];
    float array_left[ground::draw_flg + 2];
    
    for (int n = 0; n < ground::draw_flg; n++) {
        array_right[n] = ground::copy_array_right[n];
        array_left[n] = ground::copy_array_left[n];
    }
    
    if (ground::draw_flg == 0) {
        array_right[ground::draw_flg] = 0;
        array_left[ground::draw_flg] = 0;
    }
    else{
        array_right[ground::draw_flg] = ofRandom(-3, 3);
        array_right[ground::draw_flg + 1] = ofRandom(0, 0);
        array_left[ground::draw_flg] = ofRandom(-3, 3);
        array_left[ground::draw_flg + 1] = ofRandom(0, 0);
    }
    
    float rapid = 16; // landing speed
    float p = 80; // positioning
    
    for (int l = 0; l < ground::draw_flg + 1; l++) {
        ofBeginShape();
        ofVertex(ground::gr_xpos + rapid * l, ground::gr_ypos + p + array_right[l]); // KOKO! (BUFFALO = 12)(CCD = 15)
        ofVertex(ground::gr_xpos + rapid * (l + 1), ground::gr_ypos + p + array_right[l + 1]);
        ofVertex(ground::gr_xpos + rapid * (l + 1), ofGetHeight());
        ofVertex(ground::gr_xpos + rapid * l, ofGetHeight());
        ofEndShape();

        ofBeginShape();
        ofVertex(ground::gr_xpos - rapid * l, ground::gr_ypos + p + array_left[l]); // KOKO! (BUFFALO = 12)(CCD = 15)
        ofVertex(ground::gr_xpos - rapid * (l + 1), ground::gr_ypos + p + array_left[l + 1]);
        ofVertex(ground::gr_xpos - rapid * (l + 1), ofGetHeight());
        ofVertex(ground::gr_xpos - rapid * l, ofGetHeight());
        ofEndShape();
    }
    
    for (int m = 0; m < ground::draw_flg + 1; m++) {
        ground::copy_array_right[m] = array_right[m];
        ground::copy_array_left[m] = array_left[m];
    }
}

void ground::flag(float w, float h)
{
    if (test_flag == 1 && ground::gr_flg == 0) {
         ground::gr_xpos = w;
         ground::gr_ypos = h;
         ground::gr_flg = 1;
     }
     
    if (test_flag == 1 && ground::gr_flg == 1) {
        ground::draw();
        if (ground::draw_flg < 160) {
            ground::draw_flg ++;
        }
        if (ground::gr_count < ofGetWidth()) {
            ground::gr_count += 8; // KOKO! (BUFFALO = 40)(CCD = 6)
            ground::groot_count++;
        }
    }
}

void grass::grass0(float x, float y){
	ofBezier(x, y + p, x, y-a/6 + p, x-a/2, y-a/4 + p, x-a/2, y-a/4 + p);
	ofBezier(x, y + p, x, y-a/6 + p, x+a/2, y-a/4 + p, x+a/2, y-a/4 + p);
}

void grass::grass1(float x, float y){
	ofBezier(x, y + p, x, y-a/5 + p, x-a/3*2, y-a/4 + p, x-a/3*2, y-a/4 + p);
	ofBezier(x, y + p, x, y-a/5 + p, x+a/3*2, y-a/4 + p, x+a/3*2, y-a/4 + p);
	ofBezier(x, y + p, x, y-a/3 + p, x-a/4, y-a/5*3 + p, x-a/4, y-a/5*3 + p);
	ofBezier(x, y + p, x, y-a/3 + p, x+a/4, y-a/5*3 + p, x+a/4, y-a/5*3 + p);
}

void grass::grass1_left(float x, float y){
	ofBezier(x, y + p, x, y-a/5 + p, x-a/3*2, y-a/4 + p, x-a/3*2, y-a/4 + p);
	ofBezier(x, y + p, x, y-a/3 + p, x-a/4, y-a/5*3 + p, x-a/4, y-a/5*3 + p);
	ofBezier(x, y + p, x, y-a/3 + p, x+a/4, y-a/5*3 + p, x+a/4, y-a/5*3 + p);
}

void grass::grass1_right(float x, float y){
	ofBezier(x, y + p, x, y-a/5 + p, x+a/3*2, y-a/4 + p, x+a/3*2, y-a/4 + p);
	ofBezier(x, y + p, x, y-a/3 + p, x-a/4, y-a/5*3 + p, x-a/4, y-a/5*3 + p);
	ofBezier(x, y + p, x, y-a/3 + p, x+a/4, y-a/5*3 + p, x+a/4, y-a/5*3 + p);
}

void grass::draw(){
    if (ground::gr_count > 30) {
        grass0(50 + grass::w, grass::h);
        grass1(50 + grass::w, grass::h);
    }
    if (ground::gr_count > 80) {
        grass1_left(-120 + grass::w, grass::h);
        grass1_right(100 + grass::w, grass::h);
    }
    if (ground::gr_count > 160) {
        grass1(-250 + grass::w, grass::h);
        grass0(-250 + grass::w, grass::h);
        grass1_left(-180 + grass::w, grass::h);
        grass1_right(270 + grass::w, grass::h);
    }
    if (ground::gr_count > 220) {
        grass1(-450 + grass::w, grass::h);
        grass0(-450 + grass::w, grass::h);
        grass1_left(-360 + grass::w, grass::h);
        grass1_right(370 + grass::w, grass::h);
    }
    if (ground::gr_count > 270) {
        grass1_left(-560 + grass::w, grass::h);
        grass1_right(540 + grass::w, grass::h);
    }
    if (ground::gr_count > 320) {
        grass1_left(-670 + grass::w, grass::h);
        grass1_right(620 + grass::w, grass::h);
    }
    if (ground::gr_count > 380) {
        grass1_left(-750 + grass::w, grass::h);
        grass1_right(780 + grass::w, grass::h);
    }
    if (ground::gr_count > 450) {
        grass1_left(-910 + grass::w, grass::h);
        grass1_right(890 + grass::w, grass::h);
    }
    if (ground::gr_count > 540) {
        grass1_left(-1100 + grass::w, grass::h);
        grass1_right(1060 + grass::w, grass::h);
    }
    if (ground::gr_count > 620) {
        grass1_left(-1240 + grass::w, grass::h);
        grass1_right(1250 + grass::w, grass::h);
    }
    if (ground::gr_count > 680) {
        grass1_left(-1410 + grass::w, grass::h);
        grass1_right(1370 + grass::w, grass::h);
    }
}

void tree::main(int x, int y, float a, int b, float w, float h){
    float s = 1.2; // scale
    ofSetRectMode(OF_RECTMODE_CENTER);
	ofSetColor(0);
	ofEllipse(x * s + w - 2, y * s + h - 45, a*2 * s, a/3*4 * s);
	ofCircle(x-(a/4 * s) + w, y-(a/4 * s) + h - 80, a/2 * s);
	ofCircle(x+(a/4 * s) + w, y-(a/3 * s) + h - 80, a/5*3 * s);
	ofCircle(x-(a/5*3 * s) + w, y+(a/4 * s) + h - 10, a/2 * s);
	ofCircle(x+(a/5*3 * s) + w, y+(a/3 * s) + h - 10, a/5*3 * s);
	ofCircle(x-(a/5 * s) + w, y+(a/3 * s) + h, a/5*3 * s);
	ofCircle(x+(a/5 * s) + w, y+(a/3 * s) + h, a/5*3 * s);
    ofRect(w, h + 50, 13.5, 85);
    ofSetRectMode(OF_RECTMODE_CORNER);
}

void tree::root(int x, int y , float a, float w, float h){
	int rw = a/8;
    float s = 2; // scale
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(0);
    ofPushMatrix();
    ofTranslate(1.6, -7.5);
	ofTriangle((x-a/6) / s + w, (y+a*2) / s + h, (x+a/10) / s + w, (y+a*2) / s + h, (x+a/20*3) / s + w, (y+a*4) / s + h);
	ofTriangle((x+a/10) / s + w, (y+a*3) / s + h, (x+a/10) / s + w, (y+a*3+rw) / s + h, (x+a/2) / s + w, (y+a*3.5) / s + h);
	ofTriangle((x-a/10) / s + w, (y+a*2) / s + h, (x+a/10) / s + w, (y+a*2) / s + h, (x-a/5) / s + w, (y+a*3.5) / s + h);
	ofTriangle(x / s + w, (y+a*2) / s + h, x / s + w, (y+a*2+rw) / s + h, (x-a*1.5) / s + w, (y+a*2+a/3) / s + h);
	ofTriangle(x / s + w, (y+a*2+a/20) / s + h, x / s + w, (y+a*2+rw+a/20) / s + h, (x+a*1.5) / s + w, (y+a*2+a/3+a/5) / s + h);
	ofTriangle((x+a/2) / s + w, (y+a*2+a/4) / s + h, (x+rw+a/2) / s + w, (y+a*2+a/4) / s + h, (x+a*1.3) / s + w, (y+a*2+a/4*3) / s + h);
	ofTriangle(x / s + w, (y+a*2+a/20-7) / s + h, x / s + w, (y+a*2+rw+a/20) / s + h, (x-a) / s + w, (y+a*2+a+a/10*2) / s + h);
	ofTriangle((x-a/16*3) / s + w, (y+a*2+a/3) / s + h, (x-a/16*3-rw) / s + w, (y+a*2+a/3+rw) / s + h, (x-a) / s + w, (y+a*2+a/5*3) / s + h);
	ofTriangle(x / s + w, (y+a*2+a/5) / s + h, x / s + w, (y+a*2+rw+a/5) / s + h, (x-a/2) / s + w, (y+a*2+a+a/5) / s + h);
	ofTriangle(x / s + w, (y+a*2+a/10*2) / s + h, x / s + w, (y+a*2+rw+a/10*2) / s + h, (x+a/4*3) / s + w, (y+a*2+a+a/10*2) / s + h);
	ofTriangle(x / s + w, (y+a*2+a/10*2) / s + h, x / s + w, (y+a*2+rw+a/10*2) / s + h, (x+a/4*3) / s + w, (y+a*2+a/2) / s + h);
	ofTriangle(x / s + w, (y+a*2+a/5*2) / s + h, x / s + w, (y+a*2+rw+a/5*2) / s + h, (x+a/2) / s + w, (y+a*2+a+a/5*2) / s + h);
	ofTriangle((x+a/20) / s + w, (y+a*2+a/5*3) / s + h, (x+a/20+rw) / s + w, (y+a*2+a/5*3+rw*1.5) / s + h, (x+a/2) / s + w, (y+a*2+a*0.8+a/5) / s + h);
    ofPopMatrix();
    ofSetRectMode(OF_RECTMODE_CORNER);
}

void fan::setup(float w, float h){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(0);
    ofCircle(w, h - 35, 50);
    ofRect(w, h + 50, 13.5, 85);
    ofSetRectMode(OF_RECTMODE_CORNER);
}

// --------------------------------------------------------------
//  Functions
// --------------------------------------------------------------

void testApp::setup(){
    ofBackground(255, 255, 255, 0);
	ofSetFrameRate(60);
	ofEnableSmoothing();
	ofEnableAlphaBlending();
    ofSetCircleResolution(64);
/*
    cam80.listDevices();
	cam80.setDeviceID(4);
	cam94.setDeviceID(5);

    cam80.setVerbose(true);
	cam94.setVerbose(true);

    cam80.initGrabber(CAM_WIDTH, CAM_HEIGHT);
	cam94.initGrabber(CAM_WIDTH, CAM_HEIGHT);

    color80.allocate(CAM_WIDTH, CAM_HEIGHT);
	color94.allocate(CAM_WIDTH, CAM_HEIGHT);
	gray80.allocate(CAM_WIDTH, CAM_HEIGHT);
	gray94.allocate(CAM_WIDTH, CAM_HEIGHT);
	diffImage.allocate(CAM_WIDTH, CAM_HEIGHT);
	arImage.allocate(CAM_WIDTH, CAM_HEIGHT);

    bLearnBackground = true;
*/
    treeImage.loadImage("tree.png");
	treeImage.allocate(treeImage.width, treeImage.height, OF_IMAGE_COLOR_ALPHA);

    treeImageCorners.push_back(ofPoint(62, 32));
	treeImageCorners.push_back(ofPoint(132, 32));
	treeImageCorners.push_back(ofPoint(132, 102));
	treeImageCorners.push_back(ofPoint(62, 102));

    threshold = 80;

    counter = 0;
    objX = mouseX;
    objY = mouseY;
    tempX = tempY = 0.0;
}

//--------------------------------------------------------------
void testApp::update(){
    /*
    bool isNewFrame80(false);
	bool isNewFrame94(false);

    cam80.update();
	cam94.update();

    isNewFrame80 = cam80.isFrameNew();
	isNewFrame94 = cam94.isFrameNew();

    if(isNewFrame80){
		color80.setFromPixels(cam80.getPixels(), CAM_WIDTH, CAM_HEIGHT);
		gray80 = color80;
		diffImage = gray80;
    }
    
    if(isNewFrame94){
		color94.setFromPixels(cam94.getPixels(), CAM_WIDTH, CAM_HEIGHT);
		gray94 = color94;
		arImage = gray94;
	}

    if(bLearnBackground == true){
		backGround = gray80;
		bLearnBackground = false;
	}

    diffImage.absDiff(backGround, gray80);

    diffImage.threshold(threshold);

    contourFinder.findContours(diffImage, 200, (diffImage.width * diffImage.height), 3, false, false);
*/
    
}

//--------------------------------------------------------------
void testApp::draw(){
    fan fn_draw;
    tree tr_draw;
    ground gr_draw;
    grass gs_draw;
    snow sn_draw;
    
    ofSetColor(255, 255, 255);
//	gray80.draw(20, 20);
//	gray94.draw(20, 420);

//    diffImage.draw(360, 20);
//	contourFinder.draw(360, 20);

    disX = tempX - objX;
    disY = tempY - objY;
    objX = objX + disX;
    objY = objY + disY;
    
    ofPushMatrix();
    glMatrixMode( GL_MODELVIEW );
    if (draw_flag == TRUE) {
        counter++;
        if (counter > 50) {
            test_flag = 1;
            ofPushMatrix();
            ofTranslate(objX, objY);
            ofRotateZ(flow * 4);
            ofTranslate(-objX, -objY);
//            treeImage.draw(objX - 98, objY - 100);
            tr_draw.main(0, 0, -50, 100, objX, objY);
            if (ground::groot_count > 15) {
                tr_draw.root(0, 0, 100, objX, objY);
            }
            ofPopMatrix();
            sn_draw.draw();
            gr_draw.flag(objX, objY);
            if (grass::flg == 0) {
                grass::w = objX;
                grass::h = objY;
                grass::flg = 1;
            }
            gs_draw.draw();
        }
        if (counter <= 50) {
            fn_draw.setup(objX, objY);
        }
    }
    else{
        counter = 0;
        test_flag = 0;
        for (snow::l = 0; snow::l < SNOW; snow::l++) {
            snow::snowxpos_flg[snow::l] = 0;
            snow::snowypos_flg[snow::l] = 0;
            snow::down_count[snow::l] = 0;
            snow::xflow[snow::l] = 0;
        }
        ground::gr_flg = 0;
        ground::draw_flg = 0;
        ground::gr_count = 0;
        ground::groot_count = 0;
        grass::flg = 0;
    }
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key){
			
//		case OF_KEY_UP:
//			threshold ++;
//			if(threshold > 255) threshold = 255;
//			break;
            
//		case OF_KEY_DOWN:
//			threshold --;
//			if(threshold < 0) threshold = 0;
//			break;
			
			
		case OF_KEY_RIGHT:
            flow+=2;
//			arThreshold ++;
//			if(arThreshold > 255) arThreshold = 255;
			break;
			
		case OF_KEY_LEFT:
            flow-=2;
//			arThreshold --;
//			if(arThreshold < 0) arThreshold = 0;
			break;
			
//		case 'z':
//			cam80.videoSettings();
//			break;
			
//		case 'x':
//			cam94.videoSettings();
//			break;
			
			
//		case 'c':
//			bLearnBackground = true;
//			break;
			
		case ' ':
			if(ofGetWindowMode() == OF_WINDOW){
				ofSetFullscreen(true);
			}else{
				ofSetFullscreen(false);
			}
			break;
            
        default:
            break;
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    switch (button) {
        case OF_MOUSE_BUTTON_LEFT:
            tempX = mouseX;
            tempY = mouseY;
            break;
        case OF_MOUSE_BUTTON_RIGHT:
            ofRotateZ(10);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    switch (button) {
        case OF_MOUSE_BUTTON_LEFT:
            draw_flag = !draw_flag;
            tempX = mouseX;
            tempY = mouseY;
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    switch (button) {
        case OF_MOUSE_BUTTON_LEFT:
            draw_flag = !draw_flag;
            break;
            
        default:
            break;
    }    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
