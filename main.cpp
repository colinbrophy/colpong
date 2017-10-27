/* A quick pong clone written by Colin Brophy */

#include<SFML/Graphics.hpp>
#include<unordered_set>
#include<string>
#include<iostream>

using std::unordered_set;
using std::string;
using std::to_string;
using std::cerr;

const double WINDOW_SIZE[2] = { 700, 700 };

class Object  {
public:
	Object(sf::Transformable& movObj, sf::Vector2f velocity) : velocity(velocity), movObj(movObj) {}
	virtual void newFrame();
	virtual void update();
	virtual sf::Drawable& getDrawable() = 0;
protected:
   sf::Vector2f velocity;
private:
   sf::Transformable& movObj;
};

void Object::newFrame() {}

void Object::update() {
   	movObj.move(velocity);
}

const sf::Vector2f BAT_SIZE(WINDOW_SIZE[0]*0.05, WINDOW_SIZE[1]*0.15);
const sf::Vector2f STILL = sf::Vector2f(0,0);
const double BAT_SPEED = 3;

class Player : public Object , private sf::RectangleShape {
public:
	Player() : Object(*this, STILL), sf::RectangleShape(BAT_SIZE) {}
	void newFrame() {
		velocity.y = 0; // Bat has no inertia
	}
	void up() {
		velocity.y = -BAT_SPEED;
	}
	void down() {
		velocity.y = BAT_SPEED;
	}
	sf::Drawable& getDrawable();
	using sf::RectangleShape::setPosition;
	using sf::RectangleShape::getSize;
	using sf::RectangleShape::getPosition;
};
sf::Drawable& Player::getDrawable() {
	return *this;
}

class Score : public Object , private sf::Text {
public:
	Score() : Object(*this, STILL), _p1Score(0), _p2Score(0) {}
	void update();
	void p1Score() { _p1Score++; }
	void p2Score() { _p2Score++; }

	sf::Drawable& getDrawable();
	using sf::Text::setFont;
	using sf::Text::setPosition;
private:
	int _p1Score;
	int _p2Score;
};

void Score::update() {
	Object::update();
	setString(string(to_string(_p1Score) + " : " + to_string(_p2Score)));
}

sf::Drawable& Score::getDrawable() {
	return *this;
}

const sf::Vector2f BALL_VELOCITY(-0.001 * WINDOW_SIZE[0], 0.001*WINDOW_SIZE[1]);

class Ball : public Object {
public:
	Ball(Score& score);
	void testCollision(Player& bat);
	void update();
	sf::Drawable& getDrawable();
private:
	Score& score;
	sf::CircleShape circle;
};

void Ball::update() {
	Object::update();
	bool hitplayer1edge = circle.getPosition().x - circle.getRadius() <= 0;
	bool hitplayer2edge = circle.getPosition().x + circle.getRadius() >= WINDOW_SIZE[0];
	bool hittop = circle.getPosition().y - circle.getRadius() <= 0;
	bool hitbottom = circle.getPosition().y + circle.getRadius() >= WINDOW_SIZE[1];
	if (hitplayer1edge) {
		velocity.x = -velocity.x;
		score.p1Score();
	}
	if (hitplayer2edge) {
		velocity.x = -velocity.x;
		score.p2Score();
	}
	if (hittop || hitbottom)
		velocity.y = -velocity.y;
}
void Ball::testCollision(Player& bat) {
	bool testx = circle.getPosition().x - circle.getRadius() <= bat.getPosition().x + bat.getSize().x &&
			 circle.getPosition().x + circle.getRadius() >= bat.getPosition().x;
	bool testy = circle.getPosition().y - circle.getRadius() <= bat.getPosition().y + bat.getSize().y &&
			 circle.getPosition().y + circle.getRadius() >= bat.getPosition().y;
	if (testx && testy)
		velocity.x = -velocity.x; // Conservation of energy
}

Ball::Ball(Score& score) : Object(circle, BALL_VELOCITY), score(score), circle(WINDOW_SIZE[0]*0.02){
	circle.setPosition(WINDOW_SIZE[0] /  2, WINDOW_SIZE[1] / 2);
	circle.setOrigin(sf::Vector2f(circle.getRadius(), circle.getRadius()));
}

sf::Drawable& Ball::getDrawable() {
	return circle;
}

int main() {
	sf::RenderWindow win(sf::VideoMode(WINDOW_SIZE[0],WINDOW_SIZE[1]), "Colpong");

	Score score;
	Ball ball(score);
	Player player1;
	Player player2;
	sf::Font font;

	if(!font.loadFromFile("tahoma.ttf")) {
		cerr << "Unable to load font";
		return EXIT_FAILURE;
	}

	win.setFramerateLimit(60);

	player2.setPosition(WINDOW_SIZE[0] - BAT_SIZE.x, WINDOW_SIZE[1]  / 2);
	player1.setPosition(0, WINDOW_SIZE[1]  / 2);


	score.setFont(font);
	score.setPosition(WINDOW_SIZE[0] / 2, 0);

	unordered_set<Object*> objects = { &ball, &score, &player1, &player2 };

	while (win.isOpen()) {
		sf::Event ev;

		for (auto& obj : objects)
			obj->newFrame();

		while (win.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed || (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape))
				win.close();
			else if (ev.type == sf::Event::KeyPressed) {
				switch (ev.key.code) {
				case sf::Keyboard::Up:
					player1.up();
					break;
				case sf::Keyboard::Down:
					player1.down();
					break;
				case sf::Keyboard::W:
					player2.up();
					break;
				case sf::Keyboard::S:
					player2.down();
					break;
				default:
					break;
				}
			}
		}
		ball.testCollision(player1);
		ball.testCollision(player2);
		win.clear();
		for (auto& o : objects) {
			o->update();
			win.draw(o->getDrawable());
		}
		win.display();
	}
	return EXIT_SUCCESS;
}
