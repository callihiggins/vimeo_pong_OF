#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetVerticalSync(true);
	ofBackgroundHex(0xfdefc2);
    ofTrueTypeFont::setGlobalDpi(72);
    ofSetFrameRate(60);
    ofHideCursor();
    loadSettings("settings.xml");
	receiver.setup( port );
    whichuser = 0;
    loaduser = false;
    drawusers = false;
    user1load = false;
    user2load = false;
    countdownnum = 400;
    countdownnumbool = false;
    players = true;
    newballcount = 0;
    alpha = 125;
    alphaincrement = 1;
    visitor82.loadFont("visitor1.ttf", 82, true, true);
	visitor82.setLineHeight(18.0f);
	visitor82.setLetterSpacing(1.037);

    visitor42.loadFont("visitor1.ttf", 48, true, true);
	visitor42.setLineHeight(18.0f);
	visitor42.setLetterSpacing(1.037);
ofEnableAlphaBlending();
	box2d.init();
	box2d.setGravity(0, 10);
    bounds.set(765, 77, 1820, 920);
    box2d.createBounds(bounds);
	//box2d.createGround();
	box2d.setFPS(30.0);
        startScreen = true;
    startGameBool = false;
    score1 = 0;
    score2 = 0;
    counter = 0;
	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);

        vimeologo.loadImage("images/vimeo_logo.png");
    backgroundimg.loadImage("images/vimearcadeL.png");
    
    ofxBox2dRect paddle1;
    paddle1.setPhysics(0.1, 1.0, 0.0);
    paddle1.setup(box2d.getWorld(), bounds.x + 40, bounds.y, 20, bounds.height/8, b2_kinematicBody);
    paddle1.setData(new Data());
    //  paddle1.body->SetUserData(paddle1);
    Data * sd1 = (Data*)paddle1.getData();
    sd1->soundID = ofRandom(0, N_SOUNDS);
    sd1->hit	= false;		
    sd1->type = 1;
    paddles.push_back(paddle1);	
    
    ofxBox2dRect paddle2;
    paddle2.setPhysics(0.1, 1.0, 0.0);
    paddle2.setup(box2d.getWorld(), bounds.x + bounds.width - 30, bounds.y, 20, bounds.height/8, b2_kinematicBody);
    //   paddle2.body->SetUserData(paddle2);
    paddle2.setData(new Data());
    Data * sd2 = (Data*)paddle2.getData();
    sd2->soundID = ofRandom(0, N_SOUNDS);
    sd2->hit	= false;	
	sd2->type = 1;
    paddles.push_back(paddle2);	

   
       
   
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
                    //apply a force back at the ball when it hits a paddle
                    b2Vec2 veloc = e.b->GetBody()->GetLinearVelocity();
                    veloc.operator*=(1.5);
                    veloc.operator-();
                    e.b->GetBody()->SetLinearVelocity(veloc);
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
            
 		}
	}
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
        if ( m.getAddress() == "/user" && players && !startScreen)
		{
            user = m.getArgAsString( 0 );
            username =  m.getArgAsString( 1 );
            printf("got a user!");
            loaduser = true;
        }
        
        if ( m.getAddress() == "/leftshoot")
		{
            if(!startScreen && players){
                user = "images/guest.jpg";
                username = "Guest";
                loaduser = true;
            }
            if(startScreen){
                startScreen = false;
                players = true;
                break;
            }
            
        }
        if ( m.getAddress() == "/rightshoot") {
            if(!startScreen && players){
                user = "images/guest.jpg";
                username = "Guest";
                loaduser = true;
            }
            if(startScreen){
                startScreen = false;
                players = true;
                break;
            }
                       
        }

    }
    if(loaduser && players && whichuser == 0){
        user1.loadImage(user);
        username1 = username;
        whichuser = 1;
        user1load = true;
        loaduser = false;
    }
    
    if(loaduser && players && whichuser == 1){
        user2.loadImage(user);
        username2 = username;
        whichuser = 0;
        user2load = true;
        loaduser = false;
    }

        alpha  = alpha + alphaincrement;
        if(alpha > 200 || alpha < 100){
        alphaincrement = alphaincrement * -1;
        }
    
    if(countdownnumbool){
        printf("counting down: %d \n", countdownnum);
        countdownnum = countdownnum -1;
    }
    

       
    
    if(startGameBool){
   
    for(int i=0; i<wvideos.size(); i++){ 
    wvideos[i]->idleMovie();
    }
    
    for(int i=0; i<nvideos.size(); i++){ 
    nvideos[i]->idleMovie();
    }
    
   //MOVE THE PADDLES
        mapped_joystick1 = int(ofMap(joystick1, 0, 360, bounds.y + 77 + paddles[0].getHeight()/2 , bounds.y+ bounds.height - 77 - paddles[0].getHeight()/2)); 
        mapped_joystick2 = int(ofMap(joystick2, 0, 360, bounds.y + 77 + paddles[1].getHeight()/2, bounds.y+ bounds.height - 77 - paddles[1].getHeight()/2));
      
            b2Vec2 pos1 =  paddles[0].body->GetPosition();
            b2Vec2 target1 = b2Vec2(pos1.x, mapped_joystick1/OFX_BOX2D_SCALE);
        //     b2Vec2 target1 = b2Vec2(pos1.x, mouseY/OFX_BOX2D_SCALE);

        b2Vec2 diff1 = b2Vec2(target1.x-pos1.x,target1.y-pos1.y);
            diff1.operator*=(2);
            paddles[0].body->SetLinearVelocity(diff1);
            
            b2Vec2 pos2 =  paddles[1].body->GetPosition();
         //   b2Vec2 target2 = b2Vec2(pos2.x, mouseY/OFX_BOX2D_SCALE);
            b2Vec2 target2 = b2Vec2(pos2.x, mapped_joystick2/OFX_BOX2D_SCALE);
            b2Vec2 diff2 = b2Vec2(target2.x-pos2.x,target2.y-pos2.y);
            diff2.operator*=(2);
            paddles[1].body->SetLinearVelocity(diff2);
        
      
    //FOR WHEN THE BALL GETS TOO FAST OR SLOW
    for(int i=0; i<circles.size(); i++) {
         int maxSpeed = 50;
         int minSpeed = 10;
         b2Vec2 velocity = circles[i].body->GetLinearVelocity();
         float32 speed = velocity.Length();
          if (speed > maxSpeed) {
             circles[i].body->SetLinearDamping(0.5);
         } else if (speed < maxSpeed) {
             circles[i].body->SetLinearDamping(0.0);
         }
        if (speed < 10){
            b2Vec2 veloc = circles[i].body->GetLinearVelocity();
            veloc.operator*=(1.5);
           circles[i].body->SetLinearVelocity(veloc);
        }
     }
     for(int i=0; i<circles.size(); i++) {
        b2Vec2 p = circles[i].body->GetLocalCenter();
      //KEEP THE BALL GOING UP
        circles[i].body->ApplyForce(b2Vec2( 0, -3 ), p);
       //IF THE CIRCLE GOES OFFSCREEN, UP THE PLAYER SCORE 
        if(circles[i].getPosition().x > bounds.x + bounds.width - 40){
            score2 ++;
            box2d.getWorld()->DestroyBody(circles[i].body);
            circles.erase(circles.begin() + i);
            if (score1 < 5 && score2 < 5)
            newballbool = true;
        }
        if(circles[i].getPosition().x < bounds.x + 40){
            score1 ++;
             box2d.getWorld()->DestroyBody(circles[i].body);
            circles.erase(circles.begin() + i);
            if (score1 < 5 && score2 < 5)
            newballbool = true;
        }
	}
        if(newballbool){
            newballcount++;
            if (newballcount > 100){
                newBall();
                newballbool = false;
                newballcount = 0;
            }
            
        }
    for(int i=0; i<winningvideos.size(); i++) {
        Data * theData = (Data*)winningvideos[i].getData();
        if(theData->hit == true){
          //load the nominated videos & make their bodies
            for(int j=0; j< 4; j++){
                ofVideoPlayer * vid = new ofVideoPlayer();
                vid->loadMovie("movies/smingers" + ofToString(j) +".mov");
                vid->play();
                nvideos.push_back(vid);
                NominatedVideo v;
                v.setPhysics(1.0, 0.0, 0.5);
                v.setup(box2d.getWorld(), winningvideos[i].getPosition().x, winningvideos[i].getPosition().y, 30, 30, b2_dynamicBody);
                v.setupTheCustomData();
                v.setVelocity(int(ofRandom(0, 5)), int(ofRandom(0, 5)));
                v.movie = vid;
                nomvideos.push_back(v);
            }
              //delete winning videos after they've been hit 
            theData->hit = false;  
            winningvideos[i].movie->stop();
            delete winningvideos[i].movie;
            box2d.getWorld()->DestroyBody(winningvideos[i].body);
            wvideos.erase(wvideos.begin()+i);
            winningvideos.erase(winningvideos.begin()+i);   //HOW TO DELETE FROM VECTOR LIST?
            
        }
  	}
    
    //delete nominated videos after their lifespan ends
    for(int i=0; i<nomvideos.size(); i++) {
        Data * theData = (Data*)nomvideos[i].getData();
        if(theData->lifespan < 0){
        box2d.world->DestroyBody(nomvideos[i].body);
        nomvideos[i].movie->stop();
        delete nomvideos[i].movie;
        nvideos.erase(nvideos.begin()+i);
        nomvideos.erase(nomvideos.begin()+i);
        }
    }

    //GAME OVER- RESET

    if (counter == 199) {
        score1 = 0;
        score2 = 0;
        counter = 0;
       
        for(int i=0; i<nomvideos.size(); i++) {
            box2d.world->DestroyBody(nomvideos[i].body);
            delete nomvideos[i].movie;
        }  
        for(int i=0; i<winningvideos.size(); i++) {
            box2d.world->DestroyBody(winningvideos[i].body);
            delete winningvideos[i].movie;
            
        }  
            nomvideos.clear();
            winningvideos.clear();
            wvideos.clear();
            nvideos.clear();
        
        vector <ofxBox2dCircle>::iterator iter3 = circles.begin();  
        while (iter3 != circles.end()) {  
            iter3->draw();  
            box2d.world->DestroyBody(iter3->body);  
            iter3 = circles.erase(iter3);  
        }  
        startGameBool = false;
        startScreen = true;    
        drawusers = false;
        }   
    }
}

//--------------------------------------------------------------
void testApp::draw() {
    ofBackground(0); 
    ofSetColor(255);
    ofSetRectMode(OF_RECTMODE_CORNER);
    backgroundimg.draw(0,0, ofGetWidth(), ofGetHeight());
    ofSetColor(0, 173, 238, 125);
               
    if(startScreen){
        ofSetColor(236, 28, 36);
        visitor82.drawString("VFA PONG", bounds.x + bounds.width/2 - visitor82.stringWidth("VFA PONG")/2, bounds.y + bounds.height/2);
        visitor82.drawString("Press button to start a new game",  bounds.x + bounds.width/2 - 775,bounds.y + bounds.height/2 + 100);        printf("startscreen \n");
    }
   
    if(!user1load && players && !startScreen){
        //    printf("alpha: %d: \n", alpha);
        ofSetColor(0, 173, 238, alpha);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofRect(bounds.x,bounds.y + 78, bounds.width/2, bounds.height - 78);
        ofSetColor(255);
        visitor82.drawString("Left Player Tap!", bounds.x + 80, bounds.y + bounds.height/2);
        visitor42.drawString("Press button to play as guest!", bounds.x + 30, bounds.y + bounds.height/2 + 100);
    }
    
    if(user1load && !user2load && players){
        ofSetColor(0, 173, 238, alpha);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofRect(bounds.x + bounds.width/2, bounds.y + 78, bounds.width/2, bounds.height - 78);
        ofSetColor(255);
        user1.draw(bounds.x + bounds.width/4 - 50, bounds.height/2, 100, 100);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        ofSetColor(255);
        visitor82.drawString("Right Player Tap!", bounds.x + bounds.width/2 + 80, bounds.y + bounds.height/2);
        visitor42.drawString("Press button to play as guest", bounds.x + 30 + bounds.width/2, bounds.y + bounds.height/2 + 100);
        
    }
    
    if(user1load &&  user2load){
        countdownnumbool = true;
        ofSetColor(255);
        user1.draw(bounds.x + bounds.width/4 - 50, bounds.height/2, 100, 100);
        user2.draw(bounds.x + bounds.width/4 - 50 + bounds.width/2, bounds.height/2, 100, 100);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        visitor82.drawString(username2, bounds.x + bounds.width/4 - visitor82.stringWidth(username2)/2 + bounds.width/2, bounds.y + bounds.height/2 + 120);
        if(countdownnum > 300)
            visitor82.drawString("3", bounds.x + bounds.width/2, bounds.y + bounds.height/2);
        if(countdownnum > 200 &&countdownnum < 300 )
            visitor82.drawString("2", bounds.x + bounds.width/2, bounds.y + bounds.height/2);
        if(countdownnum > 100 && countdownnum < 200)
            visitor82.drawString("1", bounds.x + bounds.width/2, bounds.y + bounds.height/2);
        if(countdownnum > 0 && countdownnum < 100)
            visitor82.drawString("GO!", bounds.x + bounds.width/2, bounds.y + bounds.height/2);
        if (countdownnum == 0 && countdownnumbool){
            countdownnumbool = false;
            countdownnum = 400;
            startGame();
            drawusers = true;
            players = false;
            user1load = false;
            user2load = false;
            newBall();
        }
        
    }
    if(startGameBool){
	
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
        ofSetColor(236, 28, 36);
		paddles[i].draw();
	}
    
    for(int i=0; i<winningvideos.size(); i++) {
        winningvideos[i].draw();
       	}
    
    for(int i=0; i<nomvideos.size(); i++) {
        nomvideos[i].draw();
	}
    
    if (score1 > 4 && counter < 200) {
        ofSetColor(255,221,21);
        visitor82.drawString("YOU WIN!", bounds.x + bounds.width/4 - visitor82.stringWidth("YOU WIN!")/2, bounds.y + bounds.height/2 - 120);
        visitor82.drawString("YOU LOSE!", bounds.x + bounds.width/4 - visitor82.stringWidth("YOU LOSE!")/2 + bounds.width/2, bounds.y + bounds.height/2 - 120);        
        ofSetColor(255);
        ofSetRectMode(OF_RECTMODE_CORNER);
        user1.draw(bounds.x + bounds.width/4 - 50,  bounds.height/2 + 20, 100, 100);
        user2.draw(bounds.x + bounds.width/4 - 50 + bounds.width/2,  bounds.height/2 + 20, 100, 100);
        ofSetColor(255,221,21);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        visitor82.drawString(username2, bounds.x + bounds.width/4 - visitor82.stringWidth(username2)/2 + bounds.width/2, bounds.y + bounds.height/2 + 120);
        
        counter++;
        printf("counter: %d\n", counter);

    }
    if (score2 > 4 && counter < 200) {
        ofSetColor(255,221,21);
        visitor82.drawString("YOU LOSE!", bounds.x + bounds.width/4 - visitor82.stringWidth("YOU LOSE!")/2, bounds.y + bounds.height/2 - 120);
        visitor82.drawString("YOU WIN!", bounds.x + bounds.width/4 - visitor82.stringWidth("YOU WIN!")/2 + bounds.width/2, bounds.y + bounds.height/2 - 120);        
        ofSetColor(255);
        ofSetRectMode(OF_RECTMODE_CORNER);
        user1.draw(bounds.x + bounds.width/4 - 50,  bounds.height/2 + 20, 100, 100);
        user2.draw(bounds.x + bounds.width/4 - 50 + bounds.width/2,  bounds.height/2 + 20, 100, 100);
        ofSetColor(255,221,21);
        visitor82.drawString(username1, bounds.x + bounds.width/4 - visitor82.stringWidth(username1)/2, bounds.y + bounds.height/2 + 120);
        visitor82.drawString(username2, bounds.x + bounds.width/4 - visitor82.stringWidth(username2)/2 + bounds.width/2, bounds.y + bounds.height/2 + 120);
        
        counter++;
        printf("counter: %d\n", counter);

    }

   	ofSetColor(236, 28, 36);
    
    if (drawusers){
    visitor82.drawString("SCORE " + ofToString(score1, 1), bounds.x+100, bounds.y +65);
    visitor82.drawString("SCORE " + ofToString(score2, 1), bounds.x + bounds.width/2 + 100, bounds.y +65);
    ofSetColor(255);
    user1.draw(bounds.x+ 30, bounds.y+25, 40,40);
    user2.draw(bounds.x+ 30 + bounds.width/2, bounds.y+25, 40,40);
    }
    }
    

}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	if(key == 't') ofToggleFullscreen();
         
    if(key == 's' && startScreen == true){
        startScreen = false;
        players = true;
        
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

void testApp::startGame(){
    printf("starting new game");
    startGameBool = true;
        
    // load the 8 sfx soundfile
	for (int i=0; i<N_SOUNDS; i++) {
		sound[i].loadSound("sfx/"+ofToString(i)+".mp3");
		sound[i].setMultiPlay(true);
		sound[i].setLoop(false);
	}
    for(int i=0; i< 13; i++){
        ofVideoPlayer * v = new ofVideoPlayer();
        v->loadMovie("movies/fingers" + ofToString(i) +".mov");
        v->play();
        wvideos.push_back(v);
    }
    
    
    
    for(int i=0; i < 13; i++){
        WinningVideo v;
        v.setPhysics(1.0, 0.5, 0.3);
        // v.body->SetUserData(v);
        v.setup(box2d.getWorld(), float(bounds.x +i*(bounds.width-240)/13 + 120), float(ofRandom(bounds.y + 140, bounds.height - 180)), 60,60, b2_staticBody);
        v.setupTheCustomData();
        v.movie = wvideos[i];
        winningvideos.push_back(v);
        printf("right position \n", winningvideos[i].getPosition().x);
        
    }
    

}

void testApp::newBall(){
    ofxBox2dCircle  c;
    c.setPhysics(0.1, 1.0, 0.0);
    c.setup(box2d.getWorld(), bounds.x + bounds.width/2, bounds.y + bounds.height/2, 30);
    float sgn = ofRandom(-1, 1);
    float vx = copysign(20,sgn);
    c.setVelocity(vx, 0);
    c.setData(new Data());
    Data * sd = (Data*)c.getData();
    sd->soundID = ofRandom(0, N_SOUNDS);
    sd->hit	= false;		
    sd->type = 0;
    circles.push_back(c);	
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
	
	
    int w = xmlReader.getValue("settings:dimensions:width", 640, 0);
	int h = xmlReader.getValue("settings:dimensions:height", 480, 0);
	
    ofSetWindowShape(w, h);
    
	
	if(xmlReader.getValue("settings:go_fullscreen", "false", 0).compare("true") == 0) {
		fullscreen = true;
		ofSetFullscreen(true);
	}
}

