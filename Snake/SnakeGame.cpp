#include "SnakeGame.h"
#include "qtimer.h"
#include "qdatetime.h"

SnakeGame::SnakeGame(QWidget *parent)
	: QGLWidget(parent)
{
	qsrand(QTime::currentTime().msec());

	height = 300;
	width = 300;

	this->setFixedSize(QSize(width, height));
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	this->move(100, 100);

	wall.top = 100;
	wall.bottom = height - 100;
	wall.left = 100;
	wall.right = width - 100;

	snake = new Snake((wall.left +(wall.right - wall.left)/2), (wall.top + (wall.bottom - wall.top)/2));

	createPoint();

	QTimer * timer = new QTimer(this);
	timer->start(second_in_msec / 2);
	connect(timer, SIGNAL(timeout()), snake, SLOT(move()));			//движение по таймеру
	connect(snake, SIGNAL(moved()), this, SLOT(updateGL()));		//обновление при движении
	connect(snake, SIGNAL(moved()), timer, SLOT(start()));			//обновление таймера при движении
	connect(snake, SIGNAL(moved()), this, SLOT(check()));			//проверка положения змейки при движении
	connect(snake, SIGNAL(destroyed()), this, SLOT(gameOver()));	//заканчивает игру, если змейка уничтожена
	connect(this, SIGNAL(pointEaten()), snake, SLOT(grow()));		//рост змейки, когда она кушает
	connect(this, SIGNAL(pointEaten()), this, SLOT(createPoint()));	//создание новой точки
	connect(this, SIGNAL(pointEaten()), this, SLOT(updateGL()));	//обновление, когда змейка кушает

	//QTimer * timer1 = new QTimer(this);		
	//timer1->start(second_in_msec);
	//connect(timer1, SIGNAL(timeout()), snake, SLOT(grow()));
}

void SnakeGame::initializeGL()
{
	glOrtho(0, width, height, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_SMOOTH);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_TEXTURE_2D);
}

void SnakeGame::resizeGL(int aW, int aH)
{
	glLoadIdentity();
	glViewport(0, 0, (GLint)aW, (GLint)aH);
	glOrtho(0, width, height, 0, 0, 1);
}

void SnakeGame::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglClearColor(Qt::white);

	processing();

	draw();
}

void SnakeGame::keyPressEvent(QKeyEvent * key)
{
	switch (key->key())
	{
	case Qt::Key_Up:
	{
		if (snake->direction() != eDirection::DOWN)
		{
			snake->setDir(eDirection::UP);
			snake->move();
		}
		break;
	}
	case Qt::Key_Down:
	{
		if (snake->direction() != eDirection::UP)
		{
			snake->setDir(eDirection::DOWN);
			snake->move();
		}
		break;
	}
	case Qt::Key_Left:
	{
		if (snake->direction() != eDirection::RIGHT)
		{
			snake->setDir(eDirection::LEFT);
			snake->move();
		}
		break;
	}
	case Qt::Key_Right:
	{
		if (snake->direction() != eDirection::LEFT)
		{
			snake->setDir(eDirection::RIGHT);
			snake->move();
		}
		break;
	}
	}
}

void SnakeGame::processing()
{
}

void SnakeGame::draw()
{
	drawWalls();
	drawSnake();
	drawPoint();
	drawStats();
}

void SnakeGame::drawWalls()
{
	glBegin(GL_QUADS);
	qglColor(Qt::black);
	drawThickQuad(	(GLfloat)wall.left,			(GLfloat)wall.top	 - 2.5,	
					(GLfloat)wall.left,			(GLfloat)wall.bottom + 2.5, 5);
	drawThickQuad(	(GLfloat)wall.left  - 2.5,	(GLfloat)wall.bottom, 
					(GLfloat)wall.right + 2.5,	(GLfloat)wall.bottom,		5);
	drawThickQuad(	(GLfloat)wall.right,		(GLfloat)wall.bottom + 2.5, 
					(GLfloat)wall.right,		(GLfloat)wall.top	 - 2.5,	5);
	drawThickQuad(	(GLfloat)wall.right + 2.5,	(GLfloat)wall.top,	
					(GLfloat)wall.left  - 2.5,	(GLfloat)wall.top,			5);
	glEnd();
}

void SnakeGame::drawSnake()
{
	for (int i = 0; i < snake->getSize(); i++)
	{
		auto coord = snake->getTailCoord(i);
		
		glBegin(GL_QUADS);
		qglColor(Qt::blue);
		drawThickQuad(	(GLfloat)coord.first, (GLfloat)coord.second + 2.5,
						(GLfloat)coord.first, (GLfloat)coord.second - 2.5, 5);
		glEnd();
	}
}

void SnakeGame::drawPoint()
{
	if (point)
	{
		std::pair<int, int> coord = { point->x, point->y };

		glBegin(GL_QUADS);
		qglColor(*point->color);
		drawThickQuad((GLfloat)coord.first, (GLfloat)coord.second + 2.5,
			(GLfloat)coord.first, (GLfloat)coord.second - 2.5, 5);
		glEnd();
	}
}

void SnakeGame::drawStats()
{
	static QTimer * qTimer = new QTimer;
	if (!qTimer->isActive())
	{
		qTimer->start(second_in_msec);
		connect(qTimer, SIGNAL(timeout()), this, SLOT(addSec()));
	}

	QFont font("TimesNewRoman", 10);
	QString stat = u8"Score: " + QString::number(snake->getSize() - 2);
	QString time = u8"Time: " + QString::number(secs);

	qglColor(Qt::black);
	renderText(20, 20, stat, font);
	renderText(20, 30, time, font);
}

//todo через sin/cos
/*только для вертикальных/горизонтальных*/
void SnakeGame::drawThickQuad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat width)
{
	if (x1 == x2)
	{
		GLfloat x = x1;

		glVertex2f((GLfloat)x - width / 2.0, (GLfloat)y1);
		glVertex2f((GLfloat)x + width / 2.0, (GLfloat)y1);

		glVertex2f((GLfloat)x + width / 2.0, (GLfloat)y2);
		glVertex2f((GLfloat)x - width / 2.0, (GLfloat)y2);
	}
	else if (y1 == y2)
	{
		GLfloat y = y1;

		glVertex2f((GLfloat)x1, (GLfloat)y + width / 2.0);
		glVertex2f((GLfloat)x1, (GLfloat)y - width / 2.0);

		glVertex2f((GLfloat)x2, (GLfloat)y - width / 2.0);
		glVertex2f((GLfloat)x2, (GLfloat)y + width / 2.0);
	}
}

void SnakeGame::check()
{
	auto coord = snake->getTailCoord(0);

	if (coord.first >= wall.right || coord.first <= wall.left
		|| coord.second <= wall.top || coord.second >= wall.bottom)
		gameOver();

	if (point)
		if (coord.first == point->x && coord.second == point->y)
			emit pointEaten();
}

void SnakeGame::createPoint()
{
	bool b;
	do {
		b = false;
		point = new Point(wall);
		for (int i = 0; i < snake->getSize() && !b; i++)
		{
			auto coord = snake->getTailCoord(i);
			if (point->x == coord.first && point->y == coord.second)
				b = true;
		}
	} while (b);
}

void SnakeGame::addSec()
{
	secs++;
}

void SnakeGame::gameOver()
{
	close();
}

SnakeGame::Point::Point(Wall wall)
{
	x = (wall.left + 5) + (qrand() % ((wall.right - wall.left - 5) / 5)) * 5;
	y = (wall.top + 5) + (qrand() % ((wall.bottom - wall.top - 5) / 5)) * 5;
	
	switch (qrand() % 3)
	{
	case 0:
	{
		color = new QColor(Qt::red);
		break;
	}
	case 1:
	{
		color = new QColor(Qt::green);
		break;
	}
	case 2:
	{
		color = new QColor(Qt::magenta);
		break;
	}
	}
}

SnakeGame::Point::~Point()
{
	color = nullptr;
}
