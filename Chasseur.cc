#include "Chasseur.h"
#include "Gardien.h"
#include "labyrinthe.h"
#include <math.h>
#include <sstream>
#include <iostream>

using namespace std;


/*
 *	Tente un deplacement.
 */
bool Chasseur::move_aux (double dx, double dy)
{
	//Debug coord
	//char test[100];
	//sprintf(test,"coord: %f,%f | coord2: %d,%d",_x,_y,(int)_x/Environnement::scale,(int)_y/Environnement::scale);
	//message(test);
	static auto derniere_teleportation = chrono::high_resolution_clock::now();

	//on verifie que le joueur ne marche pas sur le trésor
	if ((int)_x/Environnement::scale == _l->_treasor._x && (int) _y/Environnement::scale == _l->_treasor._y)
		partie_terminee(true);
	else if (!collision || EMPTY >= _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		//si l'emplacement du joueur est une marque au sol (représenté par un -1 dans
		//	data)

		//on verifie que le temps écoulé depuis la dernière téléportation est supérieur au cooldown
		chrono::duration<double> temps_derniere_teleport = chrono::high_resolution_clock::now() - derniere_teleportation;
		if (_l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)) == -1
				&& temps_derniere_teleport.count() >= cooldown_telep) //on verifie que le joueur ne vient pas juste de se téléporter
		{
			int cpt = rand()%(_l-> _nmarks);

			cout<<cpt<<endl;
			_x = (_l->_marks[cpt]._x + 0.5) * Environnement::scale;
			_y = (_l->_marks[cpt]._y + 0.5) * Environnement::scale;

			derniere_teleportation = chrono::high_resolution_clock::now();
		}
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */
Chasseur::Chasseur (Labyrinthe* l) : Entity(100, 80, l, 0,10),collision(true)
{
	_hunter_fire = new Sound ("sons/hunter_fire.wav");	// bruit de l'arme du chasseur.
	_hunter_hit = new Sound ("sons/hunter_hit.wav");	// cri du chasseur touch�.
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_fb -> get_x ()) / Environnement::scale;
	float	y = (_fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

	//pour chaque garde
	for(int i = 1; i < _l->_nguards; i++)
	{
		Gardien* g = (Gardien*)_l->_guards[i];
		int g_x = ( g->_x) / Environnement::scale;
		int g_y = ( g->_y) / Environnement::scale;

		float dist = sqrt((g_x - x)*(g_x - x) + (g_y - y)*(g_y - y));
		if(dist <= 1) // si la boule est à distance plus basse que 1
		{
			//on joue le son et on inflige des dégats
			g->_Guard_death->play();
			g->degat(10);
			return false;//on retourne 0 pour détruire la fb
		}
	}
	//si on a pas touché de garde, on verifie que la fb ne touche pas de mur
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Chasseur::right_click (bool shift, bool control)
{
	collision =!collision;
	//char test[20];
	//sprintf(test,"collision:%d",collision);
	//message(test);
	if (shift)
		_l -> _guards [1] -> rester_au_sol ();
	else
		_l -> _guards [1] -> tomber ();
}

void Chasseur::update()
{
	if(est_mort())
		partie_terminee(false);
}
