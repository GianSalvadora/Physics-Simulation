#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <random>

float movespeed = 50;
const sf::Vector2f gravity = sf::Vector2f(0, 9.8);

class Ball {
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    sf::Color color;

    std::default_random_engine gen;
    std::uniform_real_distribution<> dist;

    float getMagnitude(sf::Vector2f velocity) {
        return (float) (sqrt(pow(velocity.x, 2) + pow(velocity.y, 2)));
    }

    
    sf::Vector2f rotateVector(sf::Vector2f vector, float angle) {
        float radAngle = angle * M_PI / 180.0;
        float cosAngle = cos(radAngle);
        float sinAngle = sin(radAngle);
        return sf::Vector2f(vector.x * cosAngle - vector.y * sinAngle, vector.x * sinAngle + vector.y * cosAngle);
    }

    // Get a small random angle between -5 and 5 degrees
    float getRandomAngle() {
        return dist(gen) * 10 - 5; // Random angle between -5 and 5 degrees
    }

public:
    Ball(int radius, sf::Vector2f position)
        : dist(0.0, 1.0) { // initialize the distribution range here
        shape.setRadius(radius);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Blue);
    }

    void setVelocity(sf::Vector2f velocity) {
        this->velocity = velocity;
    }

    sf::Vector2f getVelocity() {
        return velocity;
    }

    sf::CircleShape getShape() {
        return shape;
    }

    void updateBall(float dt) {
        // V = V0 + aT
        setVelocity(getVelocity() + gravity * dt);
        shape.move(velocity * dt * movespeed);
    }

    void checkCollision(sf::RenderWindow &window) {
        sf::Vector2f position = getShape().getPosition();
        float radius = getShape().getRadius();

        if (position.x < 0) {
            velocity.x = -velocity.x;
            shape.setPosition(0, position.y);  // Correct position
            velocity = rotateVector(velocity, getRandomAngle());
        }
        else if (position.x + radius * 2 > window.getSize().x) {
            velocity.x = -velocity.x;
            shape.setPosition(window.getSize().x - radius * 2, position.y);  // Correct position
            velocity = rotateVector(velocity, getRandomAngle());
        }
        if (position.y < 0) {
            velocity.y = -velocity.y;
            shape.setPosition(position.x, 0);  // Correct position
            velocity = rotateVector(velocity, getRandomAngle());
        }
        else if (position.y + radius * 2 > window.getSize().y) {
            velocity.y = -velocity.y;
            shape.setPosition(position.x, window.getSize().y - radius * 2);  // Correct position
            velocity = rotateVector(velocity, getRandomAngle());
        }
    }

    void checkCollision(std::vector<Ball>& balls) {
        for (int i = 0; i < balls.size(); i++) {
            Ball& b = balls[i];

            if (this == &b) {
                continue;
            }

            if (getDistance(*this, b) < (this->getShape().getRadius() + b.getShape().getRadius())) {
                sf::Vector2f v1 = getVelocity();
                sf::Vector2f v2 = b.getVelocity();

                setVelocity(rotateVector(sf::Vector2f(-v1.x, -v1.y), getRandomAngle()));
                b.setVelocity(rotateVector(sf::Vector2f(-v2.x, -v2.y), getRandomAngle()));
            }
        }
    }

    float getDistance(Ball& a, Ball& b) {
        return sqrt(pow((b.getShape().getPosition().x - a.getShape().getPosition().x), 2) + pow((b.getShape().getPosition().y - a.getShape().getPosition().y), 2));
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kinematic ball simulation");
    window.setFramerateLimit(120);
    std::vector<Ball> balls;

    Ball ball(10, sf::Vector2f(400, 300));
    ball.setVelocity(sf::Vector2f(0, 5.0f));

    Ball ball2(10, sf::Vector2f(400, 500));
    ball2.setVelocity(sf::Vector2f(0, 5.0f));

    Ball ball3(10, sf::Vector2f(30, 100));
    ball3.setVelocity(sf::Vector2f(0, 5.0f));

    Ball ball4(10, sf::Vector2f(100, 0));
    ball4.setVelocity(sf::Vector2f(0, 5.0f));

    balls.push_back(ball);
    balls.push_back(ball2);
    balls.push_back(ball3);
    balls.push_back(ball4);

    sf::Clock time;
    float old = time.getElapsedTime().asSeconds();
    while (window.isOpen()) {
        float deltaTime = time.getElapsedTime().asSeconds() - old;
        time.restart();
        old = time.getElapsedTime().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        for (Ball &b : balls) {
            b.updateBall(deltaTime);
            b.checkCollision(window);
            b.checkCollision(balls);
            window.draw(b.getShape());
        }
        window.display();
    }

    return 0;
}
