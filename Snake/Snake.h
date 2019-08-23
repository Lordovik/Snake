#ifndef SNAKE_H
#define SNAKE_H

#pragma once

#include <QObject>
#include <vector>

enum class eDirection
{
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
};

class Snake : public QObject
{
	Q_OBJECT


public:
	Snake(int _x = 100, int _y = 100, QObject *parent = Q_NULLPTR);
	//Snake(const Snake & _snake, QObject *parent = Q_NULLPTR);
	~Snake();

	void setDir(eDirection _dir);

	//<x, y>
	std::pair<int, int> getTailCoord(int i) const;
	int					getSize()			const;
	eDirection			direction()			const;

private:
	struct TailPart {
		TailPart(int _x = 100, int _y = 100, eDirection _dir = eDirection::UP);

		int x;
		int y;
		eDirection dir;

		void move();

		void up();
		void down();
		void left();
		void right();
	};


	std::vector<TailPart> tail;

	eDirection dir{ eDirection::UP };

	int size;

public slots:
	void move();
	void grow();

signals:
	void moved();
	void destroyed();
};

#endif