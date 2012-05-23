#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetVerticalSync(true);
	ofBackgroundHex(0xfdefc2);
    ofTrueTypeFont::setGlobalDpi(72);
    ofSetFrameRate(60);
	    loaduser = false;
    user1load = false;
    user2load = false;
    numInvaders = 20;
    verdana22.loadFont("verdana.ttf", 22, true, true);
	verdana22.setLineHeight(18.0f);
	verdana22.setLetterSpacing(1.037);
    loadSettings("settings.xml");
    receiver.setup( port );
	box2d.init();
	box2d.setGravity(0, 0);
	box2d.createGround();
	box2d.setFPS(30.0);
    box2d.createBounds(0, 0,ofGetScreenWidth(), 0);

    score1 = 0;
    score2 = 0;
    counter = 0;
    count = 0;
    paddlewidth = ofGetWidth()/50;
	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);

    ofxBox2dRect player1;
   player1.setPhysics(0.1, 1.0, 0.0);
    player1.setup(box2d.getWorld(), 0, ofGetHeight()-10, paddlewidth, paddlewidth, b2_kinematicBody);
   player1.setData(new Data());
  //  paddle1.body->SetUserData(paddle1);
    Data * sd1 = (Data*)player1.getData();
    sd1->soundID = ofRandom(0, N_SOUNDS);
    sd1->hit	= false;		
    sd1->type = 1;
    players.push_back(player1);	

    ofxBox2dRect player2;
    player2.setPhysics(0.1, 1.0, 0.0);
    player2.setup(box2d.getWorld(), ofGetWidth(), ofGetHeight()-10, paddlewidth, paddlewidth, b2_kinematicBody);
 //   paddle2.body->SetUserData(paddle2);
    player2.setData(new Data());
    Data * sd2 = (Data*)player2.getData();
    sd2->soundID = ofRandom(0, N_SOUNDS);
    sd2->hit	= false;	
	sd2->type = 1;
    players.push_back(player2);	

    // load the 8 sfx soundfile
	for (int i=0; i<N_SOUNDS; i++) {
		sound[i].loadSound("sfx/"+ofToString(i)+".mp3");
		sound[i].setMultiPlay(true);
		sound[i].setLoop(false);
	}
    
    vimeologo.loadImage("images/vimeo_logo.png");
    
    for(int i=0; i< 13; i++){
        ofVideoPlayer * v = new ofVideoPlayer();
        v->loadMovie("movies/fingers" + ofToString(i) +".mov");
        v->play();
        invaderVideos.push_back(v);
    }
    rows = 3;
    columns = 4;
    yincrement = 25;
    xincrement = 25;
    xlimit = ofGetWidth()-20;
    xmin = 20;
    
    for(int i=0; i < columns; i++){
        vector <Invader> row;
        for(int j=0; j < rows; j++){
        Invader v;
        v.setPhysics(1.0, 0.5, 0.3);
        v.setup(box2d.getWorld(), float(((ofGetWidth()/2 - 60)/columns)*i + (ofGetWidth()/2 + 60)), float(((ofGetHeight() - ofGetHeight()/2)/(rows))*j + 60), 40,40, b2_staticBody);
        v.setupTheCustomData();
        v.movie = invaderVideos[i%13];
        row.push_back(v);
        }
         rightInvaders.push_back(row);
    }


    for(int i=0; i < columns; i++){
        vector <Invader> row;
        for(int j=0; j < rows; j++){
            Invader v;
            v.setPhysics(1.0, 0.5, 0.3);
            v.setup(box2d.getWorld(), float(((ofGetWidth()/2 - 60)/columns)*i + 60), float(((ofGetHeight() - ofGetHeight()/2)/(rows))*j + 60), 40,40, b2_staticBody);
            v.setupTheCustomData();
            v.movie = invaderVideos[i%13];
            row.push_back(v);
        }
        leftInvaders.push_back(row);
    }
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e) {
	}

//--------------------------------------------------------------
void testApp::contactEnd(ofxBox2dContactArgs &e) {
}



//--------------------------------------------------------------
void testApp::update() {
	
	box2d.update();
    
    //OSC STUFF
    while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		if ( m.getAddress() == "/joysticks" )
		{
			// both the arguments are int32's
			joystick1 = m.getArgAsInt32( 0 );
            joystick2 = m.getArgAsInt32( 1 );
        }
        if ( m.getAddress() == "/user" )
		{
			// both the arguments are int32's
			user = m.getArgAsString( 0 );
            loaduser = true;
        }
    }
        
   
        if(loaduser && whichuser == 0){
            user1.loadImage(user);
            whichuser = 1;
            user1load = true;
            loaduser = false;
        }
        
        if(loaduser && whichuser == 1){
            user2.loadImage(user);
            whichuser = 0;
            user2load = true;
            loaduser = false;
        }
    

    for(int i=0; i<invaderVideos.size(); i++){ 
        invaderVideos[i]->idleMovie();
    }
    
    
   //MOVE THE PLAYERS
        mapped_joystick1 = int(ofMap(joystick1, 0, 360, 0, ofGetHeight()));
        mapped_joystick2 = int(ofMap(joystick2, 0, 360, 0, ofGetHeight()));
    
            b2Vec2 pos1 =  players[0].body->GetPosition();
            b2Vec2 target1 = b2Vec2(mouseX/OFX_BOX2D_SCALE, pos1.y );
         // b2Vec2 target1 = b2Vec2(mapped_joystick1/OFX_BOX2D_SCALE, pos1.y );
            target1.x = ofClamp(target1.x, 0, (ofGetWidth()/2)/OFX_BOX2D_SCALE - (paddlewidth/OFX_BOX2D_SCALE));
            b2Vec2 diff1 = b2Vec2(target1.x-pos1.x,target1.y-pos1.y);
            diff1.operator*=(2);
            players[0].body->SetLinearVelocity(diff1);

            b2Vec2 pos2 =  players[1].body->GetPosition();
            b2Vec2 target2 = b2Vec2(mouseX/OFX_BOX2D_SCALE, pos2.y );
         // b2Vec2 target2 = b2Vec2(mapped_joystick2/OFX_BOX2D_SCALE, pos2.y );
            target2.x = ofClamp(target2.x, (ofGetWidth()/2)/OFX_BOX2D_SCALE + (paddlewidth/OFX_BOX2D_SCALE), ofGetWidth()/OFX_BOX2D_SCALE);
            b2Vec2 diff2 = b2Vec2(target2.x-pos2.x,target2.y-pos2.y);
            diff2.operator*=(2);
            players[1].body->SetLinearVelocity(diff2);

    
     if(count%50 == 1){
             for(int j=0; j<rows;j++){
                 for(int i=0; i<columns;i++){
                    leftInvaders[i][j].setPosition(leftInvaders[i][j].getPosition().x + xincrement, leftInvaders[i][j].getPosition().y);
                 leftInvaders[i][j].update();
                 rightInvaders[i][j].setPosition(rightInvaders[i][j].getPosition().x + xincrement, rightInvaders[i][j].getPosition().y);
                 rightInvaders[i][j].update();
              
             }
            }
        }
    for(int j=0; j<rows;j++){
        for(int i=0; i<columns;i++){
    if (rightInvaders[i][j].getPosition().x >= xlimit || leftInvaders[i][j].getPosition().x <= xmin ){
        printf("incremement!");
        for(int l=0; l<rows;l++){
            for(int k=0; k<columns;k++){
                rightInvaders[k][l].setPosition(rightInvaders[k][l].getPosition().x + xincrement*-1, rightInvaders[k][l].getPosition().y + yincrement);
                rightInvaders[k][l].update();
                leftInvaders[k][l].setPosition(leftInvaders[k][l].getPosition().x + xincrement*-1, leftInvaders[k][l].getPosition().y + yincrement);
                leftInvaders[k][l].update();
            }
        }
        xincrement = xincrement * -1;
        }
    }
    }
   

      
      /*  for(int i=0; i<winningvideos.size(); i++) {
        Data * theData = (Data*)winningvideos[i].getData();
        if(theData->hit == true){
                       //delete winning videos after they've been hit 
            theData->hit = false;  
            winningvideos[i].movie->stop();
            delete winningvideos[i].movie;
            box2d.getWorld()->DestroyBody(winningvideos[i].body);
            wvideos.erase(wvideos.begin()+i);
            winningvideos.erase(winningvideos.begin()+i);   //HOW TO DELETE FROM VECTOR LIST?
            
        }
  	}*/
    count++;
}

//--------------------------------------------------------------
void testApp::draw() {
    ofLine(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetHeight()); 
    
    
    for(int i=0; i<columns; i++) {
        for(int j=0; j<rows; j++) {
        leftInvaders[i][j].draw();
        rightInvaders[i][j].draw();
    }
        //DRAW PROFILE PHOTOS AS PLAYERS
        if(user1load){
            user1.draw(players[0].getPosition().x, players[0].getPosition().y, players[0].getWidth() * 2, players[0].getWidth() * 2);
            verdana22.drawString(ofToString(score1, 1), 100,20);
        }
        
        if(user2load){
            verdana22.drawString(ofToString(score2, 1), ofGetWidth() - 50,20);
            user2.draw(players[1].getPosition().x, players[1].getPosition().y, players[1].getWidth() * 2, players[1].getWidth() * 2);
        }
        //IF IMAGES AREN'T LOADED JUST DRAW THEM BLUE
        if (!user1load && !user2load){
            for(int i=0; i<players.size(); i++) {
                ofFill();
                Data * data = (Data*)players[i].getData();
                ofSetHexColor(0x4ccae9);
                players[i].draw();
            }
        }
        
        for(int i = 0; i<bullets.size(); i++){
             ofSetHexColor(0x4ccae9);
            bullets[i].draw();
        }
   
//    for(int i=0; i < rightInvaders.size(); i++){
//                    printf(" x location: %d: \n", int(rightInvaders[i].body->GetPosition().x));
//            printf(" y location: %d: \n", int(rightInvaders[i].body->GetPosition().y));
//        
//    }
//    
//    for(int i=0; i < leftInvaders.size(); i++){
//        printf(" x location: %d: \n", int(leftInvaders[i].body->GetPosition().x));
//        printf(" y location: %d: \n", int(leftInvaders[i].body->GetPosition().y));
//        
//    }


    
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    
    //EVENTUALLY KNOW WHICH PLAYER IS SHOOTING
    if (key == 'B' || key == 'b'){
    ofxBox2dCircle  c1;
    c1.setPhysics(0.1, 1.0, 0.1);
    c1.setup(box2d.getWorld(), players[0].getPosition().x, players[0].getPosition().y, 5);
    c1.setVelocity(0, 100);
    c1.setData(new Data());
    Data * sd1 = (Data*)c1.getData();
    sd1->soundID = ofRandom(0, N_SOUNDS);
    sd1->hit	= false;		
    sd1->type = 0;
    bullets.push_back(c1);

        ofxBox2dCircle  c2;
        c2.setPhysics(0.1, 1.0, 0.1);
        c2.setup(box2d.getWorld(), players[1].getPosition().x, players[1].getPosition().y, 5);
        
        c2.setVelocity(0, 100);
        c2.setData(new Data());
        Data * sd2 = (Data*)c2.getData();
        sd2->soundID = ofRandom(0, N_SOUNDS);
        sd2->hit	= false;		
        sd2->type = 0;
        bullets.push_back(c2);

        
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

void testApp::loadSettings(string fileString){
	string host_address;
	string host_address1;
	string host_address2;
	string filename;
	
	//--------------------------------------------- get configs
    ofxXmlSettings xmlReader;
	bool result = xmlReader.loadFile(fileString);
	if(!result) printf("error loading xml file\n");
	
	
	host_address = xmlReader.getValue("settings:master:address","test",0);
	port = xmlReader.getValue("settings:master:port",5204,0);
	host = (char *) malloc(sizeof(char)*host_address.length());
	strcpy(host, host_address.c_str());
    
		
	filename = xmlReader.getValue("settings:movie:","test",0);
	
	
//		int w = xmlReader.getValue("settings:dimensions:width", 640, 0);
	//int h = xmlReader.getValue("settings:dimensions:height", 480, 0);
	
		//ofSetWindowShape(w, h);

	
	if(xmlReader.getValue("settings:go_fullscreen", "false", 0).compare("true") == 0) {
		fullscreen = true;
		ofSetFullscreen(true);
	}
}


