#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include "Mover.h"

#define	LAB_WIDTH	94
#define	LAB_HEIGHT	61

struct Coord
{
	int x,y;
	bool operator==(Coord c)
	{
		return (c.x == x) && (c.y == y);
	}
	inline bool operator!= (Coord c)
	{
		return !(c == *this );
	}
};

class Labyrinthe : public Environnement {
private:
	char	_data [LAB_WIDTH][LAB_HEIGHT];
	DummyGuard update_player;

public:
	Labyrinthe (char*);
	int width () { return LAB_WIDTH;}	// retourne la largeur du labyrinthe.
	int height () { return LAB_HEIGHT;}	// retourne la longueur du labyrinthe.
	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j).
};

#endif
