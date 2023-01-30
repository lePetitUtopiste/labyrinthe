#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

class Labyrinthe;

class Gardien : public Entity {
private:
bool tire;
public:

	static Sound*	_Guard_hit;	// cri du chasseur touch�.
	static Sound*	_wall_hit;	// on a tap� un mur.
	static Sound*	_Guard_fire;	// bruit de l'arme du chasseur.
	static Sound* _Guard_death;

	const int FOV = 3000; //le champ de vision du gardien (représenté par un cercle autour de ce dernier)
	int _angle_cible;
	const float _speed = 1; // la vitesse de déplacement du gardien (pour l'instant inutilisé)

	//pourcentage de l'écart actuel a parcourir à chaque update
	const float _vitesse_rotation = 0.15;

	//l'imprécision maximale du gardien (l'imprécision effective est calculé par rapport au nombre de point de vie restant)
	const int decalage_tir_max = 10;

	Gardien (Labyrinthe* l, const char* modele) : Entity(120, 80, l, modele,10),tire(false),_angle_cible(0)
	{}

	// mon gardien pense tr�s mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy);
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);
};

#endif
