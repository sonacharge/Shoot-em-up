#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), time(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

  const int number_of_walls = 10;
  int a = 0;
  for(int i=0; i<number_of_walls; i++) {

    auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    auto pos   = Point2(25, a); //position of walls placed on screen
    wall->SetPosition(pos);
    walls.push_back(wall);
    a = a + 50;
  }
  const int number_of_walls3 = 14;
  int c = 0;
  for(int i=0; i<number_of_walls3; i++) {

    auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    auto pos   = Point2(c, canvas_h/2+6 ); //position of walls placed on screen
    wall->SetPosition(pos);
    walls.push_back(wall);
    c = c + 50;
  }

  const int number_of_walls2 = 10;
  int b = 0;
  for(int i=0; i<number_of_walls2; i++) {

    auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    auto pos   = Point2(canvas_w-25, b); //position of walls placed on screen
    wall->SetPosition(pos);
    walls.push_back(wall);
    b = b + 50;
  }

  const int number_of_aliens = 8;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(25+(canvas_w/number_of_aliens) * i, 400.0f); //position of aliens placed on screen
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(25+(canvas_w/number_of_aliens) * i, 100.0f); //position of aliens placed on screen
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }


	  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
	  auto pos  = Point2((canvas_w/4), 100);						 //position of coins placed on the screen
	  coin->SetPosition(pos);
	  coins.push_back(coin);


	  auto background = make_shared<SFAsset>(SFASSET_BACKGROUND, sf_window);
	  auto background_pos = Point2(canvas_w/2, canvas_h/2);
	  background->SetPosition(background_pos);
	  backgrounds.push_back(background);


}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;

  case SFEVENT_PLAYER_UP:
  	player->GoNorthPlayer();
  	break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEastPlayer();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouthPlayer();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWestPlayer();
    break;

  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  for(auto c: coins) {
    c->GoNorth();
  }

// Update enemy positions
for(auto a : aliens) {
 {
   if (time <= 180)
    {
	   time += 1;
	   a->AlienMovementRight();
    }
   if(time > 180)
   {
	   time += 1;
	   a->AlienMovementLeft();
   }
   if (time == 360)
		   {
	   time = 0;
		   }
 }
}

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
      }
    }
  }

  int hp = 0;
  for(auto p : projectiles) {     //if a projectile hits a wall
    for(auto w : walls) {
      if(p->CollidesWith(w)) {
        p->HandleCollision();
        hp++;
        if (hp == 5) {
            w->HandleCollision();
          }
        }
      }
    }

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  for(auto b : backgrounds) {
    b->OnRender();
  }

  for(auto w: walls) {
    if(w->IsAlive()) {w->OnRender();}
  }

  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender();
  }




  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
