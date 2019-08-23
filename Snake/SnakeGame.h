#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#define second_in_msec 1000

#pragma once

#include <QtWidgets/QWidget>
#include "qgl.h"
#include "qevent.h"
#include "Snake.h"

class SnakeGame : public QGLWidget
{
	Q_OBJECT

public:
	SnakeGame(QWidget *parent = Q_NULLPTR);

	struct Wall {
		int top;
		int bottom;
		int left;
		int right;
	} wall;

	struct Point
	{
		Point(Wall wall);
		~Point();
		int x, y;
		QColor * color;
	} *point{ nullptr };

private:

	void initializeGL()				override;
	void resizeGL(int, int)			override;
	void paintGL()					override;
	void keyPressEvent(QKeyEvent *)	override;

	int width;
	int height;

	void processing();
	void draw();

	void drawWalls();
	void drawSnake();
	void drawPoint();
	void drawStats();

	static void drawThickQuad(GLfloat x, GLfloat y, GLfloat x2, GLfloat y2, GLfloat width);

	Snake * snake;
	unsigned long secs{ 0 };

public slots:
	void gameOver();
	void check();
	void createPoint();
	void addSec();

signals:
	void pointEaten();
};

#endif