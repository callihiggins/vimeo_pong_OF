#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetVerticalSync(true);
	ofBackgroundHex(0xfdefc2);

	box2d.init();
	box2d.setGravity(0, 10);
	box2d.createGround();
	box2d.setFPS(30.0);
    box2d.createBounds(0, 0,ofGetScreenWidth(), 0);

	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);

    ofxBox2dRect paddle1;
    paddle1.setPhysics(0.1, 1.0, 0.0);
    paddle1.setup(box2d.getWorld(), 0, 0, 10,ofGetHeight()/4, b2_kinematicBody);
    paddle1.setData(new Data());
  //  paddle1.body->SetUserData(paddle1);
    Data * sd1 = (Data*)paddle1.getData();
    sd1->soundID = ofRandom(0, N_SOUNDS);
    sd1->hit	= false;		
    sd1->type = 1;
    paddles.push_back(paddle1);	

    ofxBox2dRect paddle2;
    paddle2.setPhysics(0.1, 1.0, 0.0);
    paddle2.setup(box2d.getWorld(), ofGetWidth(), ofGetHeight(), 10,ofGetHeight()/4, b2_kinematicBody);
 //   paddle2.body->SetUserData(paddle2);
    paddle2.setData(new Data());
    Data * sd2 = (Data*)paddle2.getData();
    sd2->soundID = ofRandom(0, N_SOUNDS);
    sd2->hit	= false;	
	sd2->type = 1;
    paddles.push_back(paddle2);	

    // load the 8 sfx soundfile
	for (int i=0; i<N_SOUNDS; i++) {
		sound[i].loadSound("sfx/"+ofToString(i)+".mp3");
		sound[i].setMultiPlay(true);
		sound[i].setLoop(false);
	}
    vimeologo.loadImage("images/vimeo_logo.png");
    
    for(int i=0; i < 12; i++){
       WinningVideo v;
        v.setPhysics(1.0, 0.5, 0.3);
       // v.body->SetUserData(v);
        v.setup(box2d.getWorld(), float(i*100 + 80), float(ofRandom(20, ofGetHeight() - 40)), 40,40, b2_staticBody);
        v.setupTheCustomData();
        winningvideos.push_back(v);
        
    }
}


//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e) {
	if(e.a != NULL && e.b != NULL) { 
		
		// if we collide with the ground we do not
		// want to play a sound. this is how you do that
		if((e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_polygon) || (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_circle )) {
			
			Data * aData = (Data*)e.a->GetBody()->GetUserData();
			Data * bData = (Data*)e.b->GetBody()->GetUserData();
      
            if(aData){
                if((aData->type == 0 && bData->type == 1)|| (bData->type == 0 && aData->type == 1)) {
                sound[aData->soundID].play();                
                }
                
                if(aData->type == 0 && bData->type == 2){
                    aData->hit = true;
                    bData->hit = true;
                    printf("collision!");
                }
                
                if(bData->type == 0 && aData->type == 2){
                    aData->hit = true;  
                    bData->hit = true;
                    printf("collision!");

                    
                }
            }
            
            

            //WHY DOESN'T THIS WORK???
//            if(bData){
//                if((aData->type == 0 && bData->type == 1)|| (bData->type == 0 && aData->type == 1)) {
//                    bData->bHit = true;
//                    sound[bData->soundID].play();
//                }
//            }
        
       
      			
//			if(bData) {
//				bData->bHit = true;
//				sound[bData->soundID].play();
//			}
		}
	}
}

//--------------------------------------------------------------
void testApp::contactEnd(ofxBox2dContactArgs &e) {
//	if(e.a != NULL && e.b != NULL) { 
//		
//		Data * aData = (Data*)e.a->GetBody()->GetUserData();
//		Data * bData = (Data*)e.b->GetBody()->GetUserData();
//		
//		if(aData) {
//			aData->hit = false;
//		}
//		
//		if(bData) {
//			bData->hit = false;
//		}
//	}
}



//--------------------------------------------------------------
void testApp::update() {
	
	box2d.update();
    
    for(int i=0; i<paddles.size(); i++) {
        paddles[i].setPosition(paddles[i].getPosition().x, mouseY);
        paddles[i].update();
	}
    for(int i=0; i<circles.size(); i++) {
        b2Vec2 p = circles[i].body->GetLocalCenter();
        circles[i].body->ApplyForce(b2Vec2( 0, -3 ), p);
	}
    for(int i=0; i<winningvideos.size(); i++) {
        Data * theData = (Data*)winningvideos[i].getData();
        if(theData->hit == true){
             printf("hit! %d\n", i);
            for(int j=0; j < 12; j++){
                NominatedVideo v;
                v.setPhysics(1.0, 0.0, 0.5);
                //            // v.body->SetUserData(v);
                v.setup(box2d.getWorld(), winningvideos[i].getPosition().x, winningvideos[i].getPosition().y, 20, 20, b2_dynamicBody);
                v.setupTheCustomData(theData->id);
                v.setVelocity(int(ofRandom(0, 5)), int(ofRandom(0, 5)));
                nomvideos.push_back(v);
            }
             
          theData->hit = false;  
          box2d.getWorld()->DestroyBody(winningvideos[i].body);
            winningvideos.erase(winningvideos.begin()+i);   //HOW TO DELETE FROM VECTOR LIST?
        }
        

           
	}


	
}


//--------------------------------------------------------------
void testApp::draw() {
	
	for(int i=0; i<circles.size(); i++) {
	Data * data = (Data*)circles[i].getData();
        ofSetColor(255);
        ofEnableAlphaBlending();
      //  ofRectMode(OF_RECTMODE_CORNER);
        vimeologo.draw(circles[i].getPosition().x, circles[i].getPosition().y, circles[i].getRadius() * 2, circles[i].getRadius() * 2); 

	}
	

	for(int i=0; i<paddles.size(); i++) {
		ofFill();
		Data * data = (Data*)paddles[i].getData();
        ofSetHexColor(0x4ccae9);
		paddles[i].draw();
	}
    
    for(int i=0; i<winningvideos.size(); i++) {
        winningvideos[i].draw();
       	}

    
    for(int i=0; i<nomvideos.size(); i++) {
        nomvideos[i].draw();
        
	}

    
	

	string info = "";
	info += "FPS: "+ofToString(ofGetFrameRate(), 1)+"\n";
    info += "FPS: "+ofToString(circles.size(), 1)+"\n";
	ofSetHexColor(0x444342);
	ofDrawBitmapString(info, 30, 30);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	if(key == 't') ofToggleFullscreen();
	if(key == 'b' || 'B') {
        ofxBox2dCircle  c;
		c.setPhysics(0.1, 1.0, 1.0);
		c.setup(box2d.getWorld(), mouseX, mouseY, 30);
       
       // c.body->SetGravityScale(0);
        float sgn = ofRandom(-1, 1);
        float vx = copysign(20,sgn);
        sgn = ofRandom(-1, 1);
        float vy = copysign(20,sgn);
        c.setVelocity(vx, vy);
		c.setData(new Data());
        b2Vec2 gravity = box2d.world->GetGravity();
        b2Vec2 p = c.body->GetLocalCenter();
        c.body->ApplyForce(b2Vec2( 0, -200 ), p);
		Data * sd = (Data*)c.getData();
		sd->soundID = ofRandom(0, N_SOUNDS);
		sd->hit	= false;		
        sd->type = 0;
		circles.push_back(c);	
        
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){
}

