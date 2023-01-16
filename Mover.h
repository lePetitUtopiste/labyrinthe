#ifndef MOVER_H
#define MOVER_H

class Labyrinthe;	// la (future) v�tre

#include "FireBall.h"
#include "Environnement.h"
#include "Sound.h"

class Mover {
private:
	static void*	init (const char*);	// initialise le mod�le md2.
public:
	Environnement*	_l;		// le labyrinthe dans lequel il �volue.
	FireBall*	_fb;		// sa boule de feu.
	float		_x, _y;		// position.
	int			_angle;		// angle de d�placement/tir.
	void*		_model;		// le mod�le graphique.

	Mover (int x, int y, Labyrinthe* l, const char* modele) :
		_l ((Environnement*)l), _fb (0), _x ((float)x), _y ((float)y),
		_angle (0)
	{ _model = init (modele); }
	virtual ~Mover () {}
	void tomber ();			// fait tomber un personnage (gardien) et se relever
	void rester_au_sol ();	// fait tomber un personnage (gardien) et le laisse au sol.
	virtual void update (void) =0;	// fait 'penser' le personnage (gardien).
	// fait bouger la boule de feu du personnage.
	virtual bool process_fireball (float dx, float dy) =0;
	// tente de d�placer le personnage de <dx,dy>.
	virtual bool move (double dx, double dy) =0;
	// fait tirer le personnage sur un ennemi (vous pouvez ignorer l'angle vertical).
	virtual void fire (int angle_vertical) =0;
	// appel�e pour le gardien 0 (chasseur) quand l'utilisateur fait un clic droit;
	// shift (control) est vrai si la touche shift (control) est appuy�e.
	virtual void right_click (bool shift, bool control) {}
};

class Entity : public Mover
{
	private:

	bool mort;

	public:

	static Sound* _hunter_fire;	// bruit de l'arme du chasseur.
	static Sound* _hunter_hit; // cri du chasseur touch�.
	int vie;

	void degat(int deg){
		if(mort)
		{
			return;
		}
		vie -= deg;
		if(vie <= 0)
		{
			//rester_au_sol();
			mort = true;
		}
	}
	Entity(int x, int y, Labyrinthe* l, 
			const char* modele,int v)
	:Mover(x,y,l,modele),mort(false),vie(v)
	{}

	/**
	 * @brief initialise les sons de toutes les entités
	 * @warning ne doit être appelé qu'une seule fois 
	 * sous risque de fuite de mémoire.
	 */
	
	bool est_mort()
	{
		return mort;
	}

};

#endif
