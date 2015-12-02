/**
* This file is part of a demo that shows how to use RT2D, a 2D OpenGL framework.
*
* - Copyright 2015 Rik Teerling <rik@onandoffables.com>
*     - Initial commit
* - Copyright 2015 Your Name <you@yourhost.com>
*     - What you did
*/

#include "scene00.h"

Scene00::Scene00() : SuperScene()
{
	// Start Timer t
	t.start();

	delay = 10;
	counter = 0;

	clicked = false;
	bulletCounter = -1;

	text[0]->message("Scene00: shoot the enemies before they shoot you!");

	Sprite* background_spr = new Sprite();
	background_spr->setupSprite("assets/tiles_1.tga", 0.5f, 0.5f, 1.0f, 1.0f, 1, 2);
	background_entity = new BasicEntity();
	background_entity->position = Point2(400, 400);
	background_entity->addSprite(background_spr);
	delete background_spr;

	Sprite* player_spr = new Sprite();
	player_spr->setupSprite("assets/player.tga", 0.5f, 0.5f, 1.0f, 1.0f, 1, 2);
	player_spr->color = GREEN;
	player_entity = new BasicEntity();
	player_entity->position = Point2(SWIDTH / 2, SHEIGHT / 2);
	player_entity->addSprite(player_spr);
	player_entity->scale = Point2(2,2);
	delete player_spr;

	for (int e = 0; e < 5; e++) {
		BasicEntity* enemie = new BasicEntity();
		enemie->addSprite("assets/player.tga");

		enemie->sprite()->color = MAGENTA;
		enemie->scale = Point2(2, 2);

		enemies.push_back(enemie);

		layers[2]->addChild(enemies[e]);

		
		BasicEntity* gun_enemie = new BasicEntity();
		gun_enemie->addSprite("assets/gun.tga");
		gun_enemie->sprite()->color = YELLOW;
		gun_enemie->position = Point2(15, 12);

		guns_enemies.push_back(gun_enemie);

		enemies[e]->addChild(gun_enemie);
	}
	
	enemies[0]->position = Point(100, 100);
	enemies[1]->position = Point(700, 600);
	enemies[2]->position = Point(750, 100);
	enemies[3]->position = Point(400, 400);
	enemies[4]->position = Point(200, 650);

	gun_player_entity = new BasicEntity();
	gun_player_entity->addSprite("assets/gun.tga");
	gun_player_entity->sprite()->color = YELLOW;
	gun_player_entity->position = Point2(15, 12);
	
	int amount = 10;

	for (int i = 0; i<amount; i++) {
		BasicEntity* b = new BasicEntity();
		b->addSprite("assets/block.tga");

		b->sprite()->color = BLUE;
		b->position.x = 250;
		b->position.y = rand() % 300 + 150;
		blocks.push_back(b);

		layers[2]->addChild(blocks[i]);
	}

	layers[1]->addChild(background_entity);
	layers[2]->addChild(player_entity);
	player_entity->addChild(gun_player_entity);
}


Scene00::~Scene00()
{

	int s = blocks.size();
	for (int i = 0; i<s; i++) {
		layers[2]->removeChild(blocks[i]);
		delete blocks[i];
	}
	blocks.clear();

	int p_b = player_bullets.size();
	for (int j = 0; j<p_b; j++) {
		layers[1]->removeChild(player_bullets[j]);
		delete player_bullets[j];
	}
	player_bullets.clear();

	int es = enemies.size();
	for (int k = 0; k < es; k++) {
		enemies[k]->removeChild(guns_enemies[k]);
		layers[1]->removeChild(enemies[k]);
		delete guns_enemies[k];
		delete enemies[k];
	}
	enemies.clear();

	layers[1]->removeChild(background_entity);
	layers[2]->removeChild(player_entity);
	player_entity->removeChild(gun_player_entity);
	

	delete background_entity;
	delete player_entity;
	delete gun_player_entity;
}

void Scene00::update(float deltaTime)
{
	// ###############################################################
	// Make SuperScene do what it needs to do (Escape key stops Scene)
	// ###############################################################
	SuperScene::update(deltaTime);

	PlayerStayInScreen();

	Vector2 playerRight = Vector2(1, 0);
	Vector2 playerLeft = Vector2(1, 0);
	Vector2 playerUp = Vector2(0, 1);
	Vector2 playerDown = Vector2(0, 1);

	// ###############################################################
	// Rotate player_entity to the position of the mouse
	// ###############################################################
	Point2 mousepos = Point2(input()->getMouseX(), input()->getMouseY());
	Vector2 delta = Vector2(player_entity->position, mousepos);
	float angle = delta.getAngle();
	player_entity->rotation = angle;

	for (int a = 0; a < enemies.size(); a++) {
		Point2 mousepos1 = Point2(player_entity->position.x, player_entity->position.y);
		Vector2 delta1 = Vector2(enemies[a]->position, mousepos1);
		float angle1 = delta1.getAngle();
		enemies[a]->rotation = angle1;
	}

	Player* player_p = new Player();
	for (int i = 0; i < blocks.size(); i++) {
		player_p->playerCollidWithBlock(player_entity, blocks[i], 32, 27);
	}

	// ###############################################################
	// Move player_entity to and from the mouse
	// ###############################################################
	float speed = 150.0f; // 150 units / second

	// Move up
	if (input()->getKey(GLFW_KEY_W)) {
		player_entity->position -= playerUp * deltaTime * speed;
	}
	// Move down
	if (input()->getKey(GLFW_KEY_S)) {
		player_entity->position += playerDown * deltaTime * speed;
	}
	// move left
	if (input()->getKey(GLFW_KEY_A)) {
		player_entity->position -= playerLeft * deltaTime * speed;
	}
	// move right
	if (input()->getKey(GLFW_KEY_D)) {
		player_entity->position += playerRight * deltaTime * speed;
	}

	
	if (counter < delay) {
		counter += 1 * deltaTime;
	}

	if (input()->getMouseDown(GLFW_MOUSE_BUTTON_1)){ //&& counter >= delay) {
		BasicEntity* bullet_player_entity = new BasicEntity();
		bullet_player_entity->addSprite("assets/bullet.tga");
		bullet_player_entity->sprite()->color = ORANGE;
		bullet_player_entity->position = Point2(player_entity->position.x, player_entity->position.y);

		layers[1]->addChild(bullet_player_entity);

		player_bullets.push_back(bullet_player_entity);

		Point2 mouseposbull = Point2(input()->getMouseX(), input()->getMouseY());
		Vector2 deltabull = Vector2(bullet_player_entity->position, mouseposbull);
		float anglebull = deltabull.getAngle();
		bullet_player_entity->rotation = anglebull;

		counter = 0;
		clicked = true;
		bulletCounter++;
	}
	if (clicked) {
		for (int k = bulletCounter; k <= bulletCounter; k++) {
			MouseX = input()->getMouseX();
			MouseY = input()->getMouseY();

			BulletX = player_bullets[bulletCounter]->position.x;
			BulletY = player_bullets[bulletCounter]->position.y;

			AngleX = MouseX - BulletX;
			AngleY = MouseY - BulletY;

			vectorLength = sqrt(AngleX*AngleX + AngleY*AngleY);

			DirectionX = AngleX / vectorLength;
			DirectionY = AngleY / vectorLength;

			VelocityX = DirectionX * 400 * deltaTime;
			VelocityY = DirectionY * 400 * deltaTime;
		}
		clicked = false;
	}
	if (bulletCounter >= 0) {
		for (int bb = 0; bb < bulletCounter; bb++) {
			player_bullets[bb]->position += Point2(VelocityX, VelocityY);
		}
	}
}

void Scene00::PlayerStayInScreen() {
	int playerRaduis = 24;

	if (player_entity->position.x < 0 + playerRaduis) {
		player_entity->position.x = 0 + playerRaduis;
	}
	if (player_entity->position.x > 800 - playerRaduis) {
		player_entity->position.x = 800 - playerRaduis;
	}
	if (player_entity->position.y < 0 + playerRaduis) {
		player_entity->position.y = 0 + playerRaduis;
	}
	if (player_entity->position.y > 720 - playerRaduis) {
		player_entity->position.y = 720 - playerRaduis;
	}
}