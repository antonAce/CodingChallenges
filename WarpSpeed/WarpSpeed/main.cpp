#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include <windows.h>
#include <iostream>
#include <vector>

#define _WIN32_WINNT 0x0500 

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define OFFSETX 400
#define OFFSETY 400
#define PI 3.1415926535897

using namespace sf;

float map(float value, float istart, float istop, float ostart, float ostop) {
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

class Star
{
private:
	float x, y, z, sx, sy, px, py, pz, speed = 1600, starSize = 2;
	CircleShape starShape;
	RectangleShape tailShape;

public:
	Star()
	{
		VertexArray lines(LinesStrip, 2);
		lines[0].position = Vector2f(10, 0);
		lines[1].position = Vector2f(20, 0);

		x = (rand() % (2 * WINDOW_WIDTH)) - WINDOW_WIDTH;
		y = (rand() % (2 * WINDOW_HEIGHT)) - WINDOW_HEIGHT;
		z = rand() % WINDOW_WIDTH;
		starShape.setRadius(starSize);
		starShape.setOrigin(starSize, starSize);
		starShape.setFillColor(Color::White);
		starShape.setPosition(x + OFFSETX, y + OFFSETY);
		tailShape.setSize(Vector2f(12, 1));
		tailShape.setOrigin(0, 0.5);
		pz = z;
	}

	void ChangeSpeedByMouse(float MouseX)
	{
		speed = map(MouseX, 0, WINDOW_WIDTH, 0, 2000);
	}

	void Update(RenderWindow &window, float deltaTime)
	{
		VertexArray lines(LinesStrip, 2);
		z -= deltaTime * speed;

		sx = map(x / z, 0, 1, 0, WINDOW_WIDTH);
		sy = map(y / z, 0, 1, 0, WINDOW_HEIGHT);
		starSize = map(z, 0, WINDOW_WIDTH, 5, 0);

		starShape.setPosition(sx + OFFSETX, sy + OFFSETY);
		starShape.setRadius(starSize);

		if (z < 0.1)
		{
			x = (rand() % (2 * WINDOW_WIDTH)) - WINDOW_WIDTH;
			y = (rand() % (2 * WINDOW_HEIGHT)) - WINDOW_HEIGHT;
			z = rand() % WINDOW_WIDTH;
			pz = z;
		}
		else
		{
			lines[0].position = Vector2f(px + OFFSETX, py + OFFSETY);
			lines[1].position = Vector2f(sx + OFFSETX, sy + OFFSETY);
		}

		px = map(x / pz, 0, 1, 0, WINDOW_WIDTH);
		py = map(y / pz, 0, 1, 0, WINDOW_HEIGHT);

		window.draw(lines);
		window.draw(starShape);

		pz = z;
	}

	~Star()
	{

	}
};

int main()
{
	Texture spaceCraftTexture;
	spaceCraftTexture.loadFromFile("resources/spaceCraft.png");

	Sprite spaceCraft;
	spaceCraft.setTexture(spaceCraftTexture);
	spaceCraft.setOrigin(105, 83);
	spaceCraft.setScale(0.4, 0.4);
	spaceCraft.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 83);

	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Warp Speed");
	Clock clock;
	Vector2i mousePosition;

	const int starsAmmount = 400;
	float time_unit = 0;
	std::vector<Star> stars;

	srand(time(0));

	for (int i = 0; i < starsAmmount; i++)
		stars.push_back(Star());

	while (window.isOpen())
	{
		time_unit = clock.getElapsedTime().asSeconds();
		mousePosition = Mouse::getPosition(window);

		clock.restart();

		spaceCraft.setPosition(mousePosition.x, spaceCraft.getPosition().y);

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear(Color::Black);

		for (int i = 0; i < stars.size(); i++)
		{
			//stars[i].ChangeSpeedByMouse(Mouse::getPosition(window).x);
			stars[i].Update(window, time_unit);
		}
		window.draw(spaceCraft);
		window.display();
	}
	return 0;
}