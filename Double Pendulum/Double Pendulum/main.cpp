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

	void EraseTrail()
	{
		particles.clear();
	}
};

class Slider
{
protected:
	FloatRect _rect, _sliderRect;
	RectangleShape _bodyShape;
	CircleShape _sliderShape;
	float value, LeftBorder, RightBorder;

public:
	Slider(FloatRect sliderRect, float LeftValue, float RightValue)
	{
		value = LeftValue;

		LeftBorder = LeftValue;
		RightBorder = RightValue;

		_rect = FloatRect(sliderRect.left, sliderRect.top + sliderRect.height / 3, sliderRect.width, sliderRect.height / 6);
		_sliderRect = sliderRect;
		_sliderShape.setOrigin(sliderRect.height / 2, sliderRect.height / 2);

		_bodyShape.setPosition(_rect.left, _rect.top);
		_bodyShape.setSize(Vector2f(_rect.width, _rect.height));

		_sliderShape.setRadius(sliderRect.height / 2);
		_sliderShape.setPosition(sliderRect.left + sliderRect.width / 2, sliderRect.top + sliderRect.height / 2);
		_sliderShape.setOrigin(sliderRect.height / 2, sliderRect.height / 2);

		_sliderShape.setFillColor(Color::Black);
		_bodyShape.setFillColor(Color::Black);
	}

	void draw(RenderWindow &window)
	{
		window.draw(_bodyShape);
		window.draw(_sliderShape);
	}

	bool moveSlider(bool pressed, Vector2i mousePos)
	{
		if (_sliderRect.intersects(FloatRect(mousePos.x, mousePos.y, 5, 5))) _sliderShape.setFillColor(Color::Green);
		else _sliderShape.setFillColor(Color::Black);

		if (pressed && _sliderRect.intersects(FloatRect(mousePos.x, mousePos.y, 1, 1)))
		{
			_sliderShape.setPosition(mousePos.x, _sliderShape.getPosition().y);
			value = LeftBorder + (abs(LeftBorder) + abs(RightBorder)) * (abs(mousePos.x - _rect.left) / abs(_rect.width));
			return true;
		}
		else
			return false;
	}

	float getValue()
	{
		return value;
	}

	void setValue(float Value)
	{
		value = Value;
		if (value <= RightBorder && value >= LeftBorder)
			_sliderShape.setPosition(_rect.left + value - LeftBorder, _sliderShape.getPosition().y);
	}
};

class Button
{
	
};

int main()
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	const float SpeedCoef = 1000 * PI;
	const int OriginX = 300, OriginY = 50;
	float r1 = 150, r2 = 150, m1 = 15, m2 = 15, a1 = 0, a2 = 90; // Length, mass and angle
	float x1 = 0, y1 = 0, x2 = 0, y2 = 0, a1_velocity = 0, a2_velocity = 0, a1_acceleration = 0, a2_acceleration = 0, time_unit = 0;
	float a1_radians = 0, a2_radians = 0;
	bool isSimulation = true, showTrail = true;

	Vector2i mousePosition(0, 0);

	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "DOUBLE PENDULUM!!!");
	CircleShape shape1(m1);
	CircleShape shape2(m2);
	CircleShape pointer(1);
	RectangleShape line1(Vector2f(r1, 5));
	RectangleShape line2(Vector2f(r2, 5));

	Slider firstAngleSlider(FloatRect(25, 500, 200, 30), -90, 90);
	Slider secondAngleSlider(FloatRect(25, 550, 200, 30), -90, 90);

	Clock clock, keys_latch;

	Trail TailTrail(200, 3, Color::Yellow);
	Trail HeadTrail(200, 3, Color::Cyan);


	Font font;
	font.loadFromFile("resources/tt0282m_.ttf");
	Text text("P - pause simulation, \n S - start simulation \n T - show / hide trail", font, 14);
	text.setFillColor(Color::Black);
	text.setStyle(sf::Text::Bold);
	text.setPosition(400, 550);


	shape1.setFillColor(Color::Black);
	shape2.setFillColor(Color::Black);
	line1.setFillColor(Color::Black);
	line2.setFillColor(Color::Black);
	pointer.setFillColor(Color::Black);

	shape1.setOrigin(m1, m1);
	shape1.setPosition(OriginX, OriginY);

	shape2.setOrigin(m2, m2);
	shape2.setPosition(OriginX, OriginY);

	pointer.setOrigin(1, 1);
	pointer.setPosition(OriginX, OriginY);

	line1.setOrigin(0, 2.5);
	line1.setPosition(OriginX, OriginY);

	line2.setOrigin(0, 2.5);
	line2.setPosition(OriginX, OriginY);

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

		if (Keyboard::isKeyPressed(Keyboard::Key::P)) isSimulation = false;
		else if (Keyboard::isKeyPressed(Keyboard::Key::S)) isSimulation = true;

		if (Keyboard::isKeyPressed(Keyboard::Key::T) && keys_latch.getElapsedTime().asMilliseconds() > 500)
		{
			TailTrail.EraseTrail();
			HeadTrail.EraseTrail();
			keys_latch.restart();
			showTrail = !showTrail;
		}

		if (isSimulation)
		{
			// Angle acceleration

			a1_radians = a1 * PI / 180;
			a2_radians = a2 * PI / 180;

			a1_acceleration = (-G * (2 * m1 + m2) * std::sin(a1_radians) - m2 * G * std::sin(a1_radians - 2 * a2_radians) - 2 * std::sin(a1_radians - a2_radians) * m2 * (std::pow(a2_velocity, 2) * r2 + std::pow(a1_velocity, 2) * r1 * std::cos(a1_radians - a2_radians))) / (r1 * (2 * m1 + m2 - m2 * std::cos(2 * a1_radians - 2 * a2_radians)));
			a2_acceleration = (2 * std::sin(a1_radians - a2_radians) * (std::pow(a1_velocity, 2) * r1 * (m1 + m2) + G * (m1 + m2) * std::cos(a1_radians) + std::pow(a2_velocity, 2) * r2 * m2 * std::cos(a1_radians - a2_radians))) / (r2 * (2 * m1 + m2 - m2 * std::cos(2 * a1_radians - 2 * a2_radians)));

			//

			x1 = OriginX + r1 * std::sin(a1_radians);
			y1 = OriginY + r1 * std::cos(a1_radians);

			x2 = x1 + r2 * std::sin(a2_radians);
			y2 = y1 + r2 * std::cos(a2_radians);


			firstAngleSlider.setValue(a1);
			secondAngleSlider.setValue(a2);
		}
		else
		{
			a1_radians = a1 * PI / 180;
			a2_radians = a2 * PI / 180;

			a1_acceleration = 0;
			a2_acceleration = 0;

			a1_velocity = 0;
			a2_velocity = 0;

			x1 = OriginX + r1 * std::sin(a1_radians);
			y1 = OriginY + r1 * std::cos(a1_radians);

			x2 = x1 + r2 * std::sin(a2_radians);
			y2 = y1 + r2 * std::cos(a2_radians);

			a1 = firstAngleSlider.getValue();
			a2 = secondAngleSlider.getValue();
		}

		shape1.setPosition(x1, y1);
		line1.setRotation(90 - a1);

		shape2.setPosition(x2, y2);
		line2.setPosition(x1, y1);
		line2.setRotation(90 - a2);

		a1_velocity += a1_acceleration * time_unit;
		a2_velocity += a2_acceleration * time_unit;

		a1 += a1_velocity * time_unit * SpeedCoef;
		a2 += a2_velocity * time_unit * SpeedCoef;

		mousePosition = Mouse::getPosition(window);
		pointer.setPosition(mousePosition.x, mousePosition.y); 

		firstAngleSlider.moveSlider(Mouse::isButtonPressed(Mouse::Left), mousePosition);
		secondAngleSlider.moveSlider(Mouse::isButtonPressed(Mouse::Left), mousePosition);

		window.clear(Color::White);
		window.draw(line1);
		window.draw(shape1);
		window.draw(line2);
		window.draw(shape2);

		if (showTrail)
		{
			TailTrail.Update(window, Vector2f(x2, y2));
			HeadTrail.Update(window, Vector2f(x1, y1));
		}
		firstAngleSlider.draw(window);
		secondAngleSlider.draw(window);
		window.draw(pointer);
		window.draw(text);
		window.display();
	}

	return 0;
}