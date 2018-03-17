#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include <windows.h>
#include <iostream>
#include <vector>

#define _WIN32_WINNT 0x0500 

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define PI 3.1415926535897  
#define G 9.8

using namespace sf;

class Trail
{
private:
	Clock latch;
	std::vector<CircleShape> particles;
	int shapeSize = 0, ParticleLimit = 0;
	Color trailColor;

public:
	Trail(int particleLimit, int size, Color color)
	{
		shapeSize = size;
		ParticleLimit = particleLimit;
		trailColor = color;
	}
	~Trail()
	{
		particles.clear();
	}

	void Update(RenderWindow &window, Vector2f circle_position)
	{
		if (latch.getElapsedTime().asMilliseconds() > 100)
		{
			latch.restart();

			if (particles.size() < ParticleLimit)
			{
				CircleShape temp_circle(shapeSize);
				temp_circle.setFillColor(trailColor);
				temp_circle.setOrigin(shapeSize, shapeSize);
				temp_circle.setPosition(circle_position.x, circle_position.y);
				particles.push_back(temp_circle);
			}
			else
			{
				particles.erase(particles.begin());
			}
		}

		for (int i = 0; i < particles.size(); i++)
			window.draw(particles[i]);
	}
};

class Pendulum
{
public:
	const float OriginX = WINDOW_WIDTH / 2, OriginY = WINDOW_HEIGHT / 12, SpeedCoef = 360 * PI;
	float length = 0, mass = 0, angle = 0;
	CircleShape circle;
	RectangleShape line;
	float x = 0, y = 0, velocity = 0, acceleration = 0, angle_in_radians = 0;

	Pendulum(float Length, float Mass, float StartAngle)
	{
		length = Length;
		mass = Mass;
		angle = StartAngle;

		circle.setRadius(mass);
		line.setSize(Vector2f(length, 5));

		circle.setFillColor(Color::Black);
		line.setFillColor(Color::Black);

		circle.setOrigin(mass, mass);
		circle.setPosition(OriginX, OriginY);

		line.setOrigin(0, 2.5);
		line.setPosition(OriginX, OriginY);
	}
	~Pendulum()
	{
	}
};

class PendulumNode : public Pendulum
{
protected:
	float NextPendulumAcceleration, NextPendulumVelocity, NextPendulumAngle, NextPendulumLength, NextPendulumMass, NextPendulumAngleInRadians;

public:

	PendulumNode(float Length, float Mass, float StartAngle) : Pendulum(Length, Mass, StartAngle)
	{}
	~PendulumNode()
	{}

	void Update(RenderWindow &window, float deltaTime, Pendulum &nextPendulum)
	{
		NextPendulumAcceleration = nextPendulum.acceleration;
		NextPendulumVelocity = nextPendulum.velocity;
		NextPendulumAngle = nextPendulum.angle;
		NextPendulumLength = nextPendulum.length;
		NextPendulumMass = nextPendulum.mass;

		NextPendulumAngleInRadians = NextPendulumAngle * PI / 180;

		this->angle_in_radians = this->angle * PI / 180;

		this->x = OriginX + this->length * std::sin(this->angle_in_radians);
		this->y = OriginY + this->length * std::cos(this->angle_in_radians);

		this->circle.setPosition(this->x, this->y);
		this->line.setRotation(90 - this->angle);

		this->velocity += this->acceleration * deltaTime;
		NextPendulumVelocity += NextPendulumAcceleration * deltaTime;

		this->angle += this->velocity * deltaTime * SpeedCoef;
		NextPendulumAngle += NextPendulumVelocity * deltaTime * SpeedCoef;

		this->acceleration = (-G * (2 * this->mass + NextPendulumMass) * std::sin(this->angle_in_radians) - NextPendulumMass * G * std::sin(this->angle_in_radians - 2 * NextPendulumAngleInRadians) - 2 * std::sin(this->angle_in_radians - NextPendulumAngleInRadians) * NextPendulumMass * (std::pow(NextPendulumVelocity, 2) * NextPendulumLength + std::pow(this->velocity, 2) * this->length * std::cos(this->angle_in_radians - NextPendulumAngleInRadians))) / (this->length * (2 * this->mass + NextPendulumMass - NextPendulumMass * std::cos(2 * this->angle_in_radians - 2 * NextPendulumAngleInRadians)));
		NextPendulumAcceleration = (2 * std::sin(this->angle_in_radians - NextPendulumAngleInRadians) * (std::pow(this->velocity, 2) * this->length * (this->mass + NextPendulumMass) + G * (this->mass + NextPendulumMass) * std::cos(this->angle_in_radians) + std::pow(NextPendulumVelocity, 2) * NextPendulumLength * NextPendulumMass * std::cos(this->angle_in_radians - NextPendulumAngleInRadians))) / (NextPendulumLength * (2 * this->mass + NextPendulumMass - NextPendulumMass * std::cos(2 * this->angle_in_radians - 2 * NextPendulumAngleInRadians)));

		nextPendulum.acceleration = NextPendulumAcceleration;
		nextPendulum.velocity = NextPendulumVelocity;
		nextPendulum.angle = NextPendulumAngle;

		this->circle.setFillColor(Color::Black);
		this->line.setFillColor(Color::Black);

		window.draw(this->line);
		window.draw(this->circle);
	}
};

class PendulumBody : public Pendulum
{
protected:
	float PreviousPendulumAcceleration, PreviousPendulumVelocity, PreviousPendulumAngle, PreviousPendulumLength, PreviousPendulumMass, PreviousPendulumAngleInRadians, PreviousX, PreviousY;

public:
	PendulumBody(float Length, float Mass, float StartAngle) : Pendulum(Length, Mass, StartAngle)
	{}
	~PendulumBody()
	{}

	void Update(RenderWindow &window, float deltaTime, Pendulum &previousPendulum)
	{
		PreviousPendulumAcceleration = previousPendulum.acceleration;
		PreviousPendulumVelocity = previousPendulum.velocity;
		PreviousPendulumAngle = previousPendulum.angle;
		PreviousPendulumLength = previousPendulum.length;
		PreviousPendulumMass = previousPendulum.mass;
		PreviousX = previousPendulum.x;
		PreviousY = previousPendulum.y;


		PreviousPendulumAngleInRadians = PreviousPendulumAngle * PI / 180;

		this->angle_in_radians = this->angle * PI / 180;

		this->x = PreviousX + this->length * std::sin(this->angle_in_radians);
		this->y = PreviousY + this->length * std::cos(this->angle_in_radians);

		this->circle.setPosition(this->x, this->y);
		this->line.setPosition(PreviousX, PreviousY);
		this->line.setRotation(90 - this->angle);

		this->velocity += this->acceleration * deltaTime;
		PreviousPendulumVelocity += PreviousPendulumAcceleration * deltaTime;

		this->angle += this->velocity * deltaTime * SpeedCoef;
		PreviousPendulumAngle += PreviousPendulumVelocity * deltaTime * SpeedCoef;

		PreviousPendulumAcceleration = (-G * (2 * PreviousPendulumMass + this->mass) * std::sin(PreviousPendulumAngleInRadians) - this->mass * G * std::sin(PreviousPendulumAngleInRadians - 2 * this->angle_in_radians) - 2 * std::sin(PreviousPendulumAngleInRadians - this->angle_in_radians) * this->mass * (std::pow(this->velocity, 2) * this->length + std::pow(PreviousPendulumVelocity, 2) * PreviousPendulumLength * std::cos(PreviousPendulumAngleInRadians - this->angle_in_radians))) / (PreviousPendulumLength * (2 * PreviousPendulumMass + this->mass - this->mass * std::cos(2 * PreviousPendulumAngleInRadians - 2 * this->angle_in_radians)));
		this->acceleration = (2 * std::sin(PreviousPendulumAngleInRadians - this->angle_in_radians) * (std::pow(PreviousPendulumVelocity, 2) * PreviousPendulumLength * (PreviousPendulumMass + this->mass) + G * (PreviousPendulumMass + this->mass) * std::cos(PreviousPendulumAngleInRadians) + std::pow(this->velocity, 2) * this->length * this->mass * std::cos(PreviousPendulumAngleInRadians - this->angle_in_radians))) / (this->length * (2 * PreviousPendulumMass + this->mass - this->mass * std::cos(2 * PreviousPendulumAngleInRadians - 2 * this->angle_in_radians)));

		previousPendulum.acceleration = PreviousPendulumAcceleration;
		previousPendulum.velocity = PreviousPendulumVelocity;
		previousPendulum.angle = PreviousPendulumAngle;

		this->circle.setFillColor(Color::Black);
		this->line.setFillColor(Color::Black);

		window.draw(this->line);
		window.draw(this->circle);
	}
};

int main()
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
	float time_unit = 0;

	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "DOUBLE PENDULUM!!!");
	Clock clock;

	Trail trail(200, 3, Color::Yellow);

	PendulumNode node(50, 20, 15);
	PendulumBody body0(50, 20, 15);
	PendulumBody body1(50, 20, 15);
	PendulumBody body2(50, 20, 15);
	PendulumBody body3(50, 20, 15);


	while (window.isOpen())
	{
		time_unit = clock.getElapsedTime().asSeconds();
		clock.restart();

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}



		window.clear(Color::White);

		node.Update(window, time_unit, body0);
		body0.Update(window, time_unit, node);
		body1.Update(window, time_unit, body0);
		body2.Update(window, time_unit, body1);
		body3.Update(window, time_unit, body2);

		trail.Update(window, Vector2f(body3.x, body3.y));
		window.display();
	}

	return 0;
}