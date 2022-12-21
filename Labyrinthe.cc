#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std;

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touch�.
Sound*	Chasseur::_wall_hit;	// on a tap� un mur.

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}


void get_murs (ifstream& file)
{
	struct Coord
	{
		int x,y;
		bool operator==(Coord c)
		{
			return (c.x == x) && (c.y == y);
		}
	};
	string line;

	vector<Wall> walls;
	list<Coord> murs_vert;
	int x = 0;
	int nb_mur = 0;
	while(getline(file,line))
	{
		cout<<"lecture ligne "<<x<<endl<<line<<endl;
		bool mur_en_cours = false;
		//Wall mur = {0,0,0,0,0};
		Coord debut = {0,0};
		//recherche de mur verticaux
		auto iter = murs_vert.begin();
		cout<<"recherche des murs verticaux"<<endl;
		int cpt = 1;
		int taille = murs_vert.size();
		while(!murs_vert.empty() && iter != murs_vert.end())
		{
			cout<<cpt<<"/"<<taille<<endl;
			cpt++;
			int x1 = (*iter).x;
			int y1 = (*iter).y;
			switch(line[(*iter).y])
			{
				case '|'://si il y a un | on ne fait rien
					cout<<"mur potentiel en"<<x1<<","<<y1<<endl;
					iter ++;
				break;
				case '+': // Si on trouve un + en dessous d'un + déjà vu on crée le mur
					cout<<"decouverte mur vertical en "<<x1<<","<<y1<<" ; "<<x<<","<<y1<<endl;
					nb_mur++;
					walls.push_back({x1,x,y1,y1,0});
					iter = murs_vert.erase(iter); //on retire le +
				break;

				default:
					iter = murs_vert.erase(iter);
				break;
			}
		}
		//analyse de la ligne en cours
		for(int i = 0; i < line.size(); i++)
		{
			//cout<<"char "<<i<<endl;
			char c = line[i];
			switch (c)
			{
				case '+':
					murs_vert.push_back({x,i});
					if(mur_en_cours)//si un mur est en cours de lecture sur cette ligne le mur est fini et on l'ajoute
					{
						//mur._x2 = x;
						//mur._y2 = i;
						cout<<"ajout du mur "<<debut.x<<","<<debut.y<<" ; "<<x<<","<<i<<endl;
						walls.push_back({debut.x,x,debut.y,i,0});//on copie dans le tableau);
						//mur = {0,0,0,0,0}; //on réinitialise le mur en cours
						nb_mur ++;
						if(i < line.size() - 1 && line[i+1] == '-')//si un mur continu après
						{
							debut.x = x;
							debut.y = i;
						}else //sinon le mur est fini
						{
							mur_en_cours = false;
						}
					}else{
						if(i < line.size() - 1 && line[i+1] == '-')
						{
							//on initialise le début du mur
							cout<<"début de mur trouvé en "<<x<<","<<i<<endl;
							//mur._x1 = x;
							//mur._y1 = i;
							debut.x = x;
							debut.y = i;
							mur_en_cours = true; //on inidique qu'on commence à lire un mur
						}
					}
				break;		
			}
		}
		cout<<"fin analyse ligne"<<endl;
		x++;//on passe à la ligne suivante
	}
	cout<<"fin de la recherche et affichage des résultats"<<endl;
	cout<<"murs trouve: "<<nb_mur<<endl;
	//on affiche la liste des murs créers
	for (int i = 0; i < walls.size(); i++)
	{
		cout<<"Wall: "<<walls[i]._x1<<","<<walls[i]._y1<<" : "<<walls[i]._x2<<","<<walls[i]._y2<<endl;
	}
	return;
}


Labyrinthe::Labyrinthe (char* filename)
{
	//test de la fonction de scan

	ifstream file;
	file.open(filename);
	string line;
	for(int i = 0; i<4; i++)
	{
		getline(file,line);
		cout<<line<<endl;
	}
	cout<<"et maintenant on cherche les murs"<<endl;
	get_murs(file);
	cout<<"fin du test"<<endl;
	file.close();
	/////////////////////////////
	// les 4 murs.
	static Wall walls [] = {
		{ 0, 0, LAB_WIDTH-1, 0, 0 },
		{ LAB_WIDTH-1, 0, LAB_WIDTH-1, LAB_HEIGHT-1, 0 },
		{ LAB_WIDTH-1, LAB_HEIGHT-1, 0, LAB_HEIGHT-1, 0 },
		{ 0, LAB_HEIGHT-1, 0, 0, 0 },
	};
	// une affiche.
	//  (attention: pour des raisons de rapport d'aspect, les affiches doivent faire 2 de long)
	static Wall affiche [] = {
		{ 4, 0, 6, 0, 0 },		// premi�re affiche.
		{ 8, 0, 10, 0, 0 },		// autre affiche.
	};
	// 3 caisses.
	static Box	caisses [] = {
		{ 70, 12, 0 },
		{ 10, 5, 0 },
		{ 65, 22, 0 },
	};

/* DEB - NOUVEAU */
	// 2 marques au sol.
	static Box	marques [] = {
		{ 50, 14, 0 },
		{ 20, 15, 0 },
	};
/* FIN - NOUVEAU */

	// juste un mur autour et les 3 caisses et le tr�sor dedans.
	for (int i = 0; i < LAB_WIDTH; ++i)
		for (int j = 0; j < LAB_HEIGHT; ++j) {
			if (i == 0 || i == LAB_WIDTH-1 || j == 0 || j == LAB_HEIGHT-1)
				_data [i][j] = 1;
			else
				_data [i][j] = EMPTY;
		}
	// indiquer qu'on ne marche pas sur une caisse.
	_data [caisses [0]._x][caisses [0]._y] = 1;
	_data [caisses [1]._x][caisses [1]._y] = 1;
	_data [caisses [2]._x][caisses [2]._y] = 1;
	// les 4 murs.
	_nwall = 4;
	_walls = walls;
	// deux affiches.
	_npicts = 2;
	_picts = affiche;
	// la deuxi�me � une texture diff�rente.
	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	_picts [1]._ntex = wall_texture (tmp);
	// 3 caisses.
	_nboxes = 3;
	_boxes = caisses;

/* DEB - NOUVEAU */
	// mettre une autre texture � la deuxi�me caisse.
	sprintf (tmp, "%s/%s", texture_dir, "boite.jpg");
	caisses [1]._ntex = wall_texture (tmp);

	// mettre les marques au sol.
	sprintf (tmp, "%s/%s", texture_dir, "p1.gif");
	marques [0]._ntex = wall_texture (tmp);
	sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
	marques [1]._ntex = wall_texture (tmp);
	_nmarks = 2;
	_marks = marques;
/* FIN - NOUVEAU */

	// le tr�sor.
	_treasor._x = 10;
	_treasor._y = 10;
	_data [_treasor._x][_treasor._y] = 1;	// indiquer qu'on ne marche pas sur le tr�sor.
	// le chasseur et les 4 gardiens.
	_nguards = 5;
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this);
	_guards [1] = new Gardien (this, "Lezard");
	_guards [2] = new Gardien (this, "Blade"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	_guards [3] = new Gardien (this, "Serpent"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	_guards [4] = new Gardien (this, "Samourai"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;
	// indiquer qu'on ne marche pas sur les gardiens.
	_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale)] = 1;
	_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
}
