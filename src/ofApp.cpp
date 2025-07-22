#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableDepthTest();

    //DPHelmet.load("DaftPunkHelmet/Daft-Punk.fbx");
    DPHelmet.load("Guitars/Billie_Joes_Blue_Guitar_01.glb");
    //DPHelmet.load("Fox/Fox_05.fbx");

    /*DPHelmet.setScaleNormalization(false);
    DPHelmet.setScale(10.0f, 10.0f, 10.0f);  // adjust scale as needed
    DPHelmet.setPosition(0, 0, 0);*/
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    
    ofEnableLighting();

	ofEnableSeparateSpecularLight();
    ofSetGlobalAmbientColor(ofFloatColor(.9F, .9F, .9F));
	light.setAmbientColor(ofFloatColor(.9F,.9F,.9F)); // gray ambient light
    light.setPosition(00,-1000,-1000);
    light.enable();
    light.draw();
    //light.setSpotlight(256.0F);
    //light.setDiffuseColor(ofFloatColor(0,0,0));       // turn off diffuse
    //light.setSpecularColor(ofFloatColor(0,0,0));      // turn off specular

	ofPushMatrix();

	DPHelmet.drawFaces();
	ofPopMatrix();
	
	//light.disable();
	ofDisableLighting();
	ofDisableSeparateSpecularLight();

    //DPHelmet.drawWireframe();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
