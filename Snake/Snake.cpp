#include "Snake.h"

Snake::Snake(int _x, int _y, QObject *parent)
	: QObject(parent)
{
	tail.push_back(TailPart(_x, _y, eDirection::UP));
	tail.push_back(TailPart(_x, _y + 5, eDirection::UP));
	size = 2;
}

//Snake::Snake(const Snake & _snake, QObject *parent)
//	: QObject(parent)
//{
//	std::copy(_snake.tail.begin(), _snake.tail.end(), tail.begin());
//	dir = _snake.dir;
//}

Snake::~Snake()
{
}

void Snake::move()
{
	for (auto it = tail.end() - 1; it > tail.begin(); it--)
	{
		it->dir = (it - 1)->dir;
	}
	tail[0].dir = dir;
	for (TailPart &part : tail)
	{
		part.move();
	}
	if (tail.size() >= 5)
	{
		for (auto it = tail.begin() + 4; it < tail.end(); it++)
		{
			if (tail[0].x == (*it).x && tail[0].y == (*it).y)
				emit destroyed();
		}
	}
	emit moved();
}

void Snake::setDir(eDirection _dir)
{
	dir = _dir;
}

std::pair<int, int> Snake::getTailCoord(int i) const
{
	if (i >= tail.size())
		return { -1, -1 };

	return std::pair<int, int>(tail[i].x, tail[i].y);
}

int Snake::getSize() const
{
	return size;
}

eDirection Snake::direction() const
{
	return dir;
}

void Snake::grow()
{
	TailPart & curr = tail[size - 1];
	int x = curr.x;
	int y = curr.y;

	switch (curr.dir)
	{
	case eDirection::UP:
	{
		y += 5;
		break;
	}
	case eDirection::DOWN:
	{
		y -= 5;
		break;
	}
	case eDirection::LEFT:
	{
		x += 5;
		break;
	}
	case eDirection::RIGHT:
	{
		x -= 5;
		break;
	}
	}

	tail.push_back(TailPart(x, y, curr.dir));
	size = tail.size();
}

//////////////////////////////////////////////////////////////////////
/////////////////////     TailPart     ///////////////////////////////
//////////////////////////////////////////////////////////////////////

Snake::TailPart::TailPart(int _x, int _y, eDirection _dir)
{
	x = _x;
	y = _y;
	dir = _dir;
}

void Snake::TailPart::move()
{
	switch (dir)
	{
	case eDirection::UP:
	{
		up();
		break;
	}
	case eDirection::DOWN:
	{
		down();
		break;
	}
	case eDirection::LEFT:
	{
		left();
		break;
	}
	case eDirection::RIGHT:
	{
		right();
		break;
	}
	}
}

void Snake::TailPart::up()
{
	y -= 5;
}

void Snake::TailPart::down()
{
	y += 5;
}

void Snake::TailPart::left()
{
	x -= 5;
}

void Snake::TailPart::right()
{
	x += 5;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////