#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#ifdef _WIN32
#define SEPARATOR '\\'
#else
#define SEPARATOR '/'
#endif

using namespace std;

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touch�.
Sound*	Chasseur::_wall_hit;	// on a tap� un mur.
Sound*	Gardien::_Guard_hit;	// cri du chasseur touch�.
Sound*	Gardien::_wall_hit;	// on a tap� un mur.
Sound*	Gardien::_Guard_fire;	// bruit de l'arme du chasseur.
Sound* Gardien::_Guard_death;

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}


/**
 * @brief va chercher les fichiers nécessaires pour les textures
 *
 * @param l le labyrinthe
 * @param file le fichier utilisé pour charger le labyrinthe
 * @warning va arrêter la lecture aau premier '+' rencontré dans
 * le fichier donné
 */
void scan_test(Labyrinthe* l, ifstream& file, map<char,int>& table_texture)
{

	//string ligne;
	string cible(l->texture_dir);
	cible.push_back(SEPARATOR);
	char input = file.get();
	char nom = '\n';
	//int cpt = 0;
	while (input != '+')
	{
		if (isalpha(input) && nom == '\n') // la première lettre qu'on croise est le nom de l'affiche
		{
			nom = input; //on stocke le nom
		}else if (input != '\n' && input != ' ' && input != '	' && input != '#'){
			cible.push_back(input);
		}

		//si on termine la ligne
		if(input == '\n' || input == '#')
		{
			//si on a trouvé un fichier on rempli la table_texture
			if (nom != '\n')
			{
				cible.shrink_to_fit();
				//cout<<nom<<":"<<cible<<"|"<<endl;
				char data[cible.size()];
				strcpy(data,cible.c_str());
				int ntex = l->wall_texture(data);
				//cout<<ntex<<endl;
				table_texture[nom] = ntex;
				//cout<<"result"<<table_texture[nom]<<endl;
			}
			//on reinitialise le tout
			nom = '\n';
			//cible.clear();
			cible.assign(l->texture_dir);
			cible.push_back(SEPARATOR);
			//si il s'agissait d'un # on avance le curseur jusqu'au début de la ligne suivante
			if(input == '#')
			{
				while (input != '\n')
					input = file.get();
			}
		}
			input = file.get();

	}
	file.unget();
	return;
}

/**
 * @brief parcours le labyrinthe dans le fichier transmis en paramètre et met à jour l'objet labyrinthe en fonction
 * 
 * @param l l'objet labyrinthe à initialiser
 * @param file le stream du fichier à utiliser dont le curseur est placé au début du labyrinthe
 * @param table_texture table de hachage contenant pour chaque indice de texture la texture chargée
 * 
 * @warning La lecture du fichier utilise le jeu de coordonnée (ligne colonne) et le jeu un repère mathématique classique.
 * la fonction s'occupe d'inverser toutes les coordonnées à la fin de son execution
*/
void scan_laby (Labyrinthe* l,ifstream& file, map<char,int> table_texture)
{

	vector<Wall> picts;
	l->_npicts = 0;
	vector<Wall> walls;
	l->_nwall = 0;
	vector<Box> caisses;
	l->_nboxes = 0;
	vector<Box> marques;
	l->_nmarks = 0;
	vector<Mover*> Guards;
	Guards.push_back(new Chasseur(l));//création du chasseur
	Guards.push_back(new DummyGuard(l));
	l->_nguards = 2;


	string line; //la ligne du fichier

	list<Coord> murs_vert;//liste des murs verticaux en cours de lecture

	list<Coord> picts_vu; // utilisé pour verifier si une marque n'est pas ajoutée 2 fois

	int x = 0;
	//on parcours le labyrinthe ligne par ligne
	while(getline(file,line))
	{
		picts_vu.clear();
		//cout<<"lecture ligne "<<x<<endl<<line<<endl;

		bool mur_en_cours = false;
		Coord debut = {0,0};
		//recherche de mur verticaux
		auto iter = murs_vert.begin();
		//cout<<"recherche des murs verticaux"<<endl;
		int cpt = 1;
		int taille = murs_vert.size();
		/*
		Pour chaque potentiel mur vertical on verifie dans cette ligne ce qui se trouve à cette position
		*/
		while(!murs_vert.empty() && iter != murs_vert.end())
		{
			//cout<<cpt<<"/"<<taille<<endl;
			cpt++;
			int x1 = (*iter).x;
			int y1 = (*iter).y;
			switch(line[(*iter).y])
			{
				default:
					//si il s'agit d'une affich
					if(isalpha(line[(*iter).y]) && line[(*iter).y] != 'G' && line[(*iter).y] != 'C' && line[(*iter).y] != 'X')
					{
						(l->_npicts)++;
						picts.push_back({x,y1,x+1,y1,table_texture[line[(*iter).y]]}); //on l'ajoute en prenant la texture associée dans la table de texture
						picts_vu.push_back({x,y1}); // on marque cette afffiche comme vu pour ne pas la recompter lors de la lecture de la ligne pour les murs horizontaux
					}
				case '|'://si il y a un | on ne fait rien
					//cout<<"mur potentiel en"<<x1<<","<<y1<<endl;
					iter ++;
				break;
				case '+': // Si on trouve un + en dessous d'un + déjà vu on crée le mur
					//cout<<"decouverte mur vertical en "<<x1<<","<<y1<<" ; "<<x<<","<<y1<<endl;
					l->_nwall++;
					walls.push_back({x1,y1,x,y1,0});
					iter = murs_vert.erase(iter); //on retire le +
				break;

				case ' '://s'il n'y a rien on retire ce + de la liste des coord à verifier il n'y a pas de mur vertical à cette pos
					iter = murs_vert.erase(iter);
				break;
			}
		}
		//analyse de la ligne en cours

		for(int i = 0; i < line.size(); i++)
		{
			char c = line[i];
			switch (c)
			{
				case 'G': //si on tombe sur un G on rajoute le chasseur en réalisant tout de suite l'inversion de coordonnées
					Guards.push_back(new Gardien(l,"Marvin"));
					Guards.back()->_y = ((float) x)*((float) l->scale);
					Guards.back()->_x = ((float)i) * ((float) l->scale);
					l->_nguards++;
				break;

				case 'X': case 'x': //de même pour les caisses
					l->_nboxes++;
					caisses.push_back({i,x,0}); //on réalise tout de suite l'inversion de coordonnées
				break;
				case 'M':// pour les marques
					l->_nmarks++;
					marques.push_back({i,x,table_texture['M']});
				break;

				case 'T':
					l->_treasor._x = i ;
					l->_treasor._y = x ;
				break;

				case 'C':// les caisses
					Guards[0]->_y = ((float) x) * ((float) l->scale);
					Guards[0]->_x = (float) (i) * ((float) l->scale);
				break;
				case '+':
					murs_vert.push_back({x,i});//on ajoute ce plus dans la liste des potentiel murs verticaux

					if(mur_en_cours)//si un mur est en cours de lecture sur cette ligne le mur est fini et on l'ajoute
					{
						//cout<<"ajout du mur "<<debut.x<<","<<debut.y<<" ; "<<x<<","<<i<<endl;
						walls.push_back({debut.x,debut.y,x,i,0});//on copie dans le tableau);
						l->_nwall ++;
						if(i < line.size() - 1 && (line[i+1] == '-' || line[i+1] == '+'))//si un mur peut continuer après
						{
							debut.x = x;
							debut.y = i;
						}else //sinon le mur est fini
						{
							mur_en_cours = false;
						}
					}else{
						//if(i < line.size() - 1)
						{
							//on initialise le début du mur
							//cout<<"début de mur trouvé en "<<x<<","<<i<<endl;
							debut.x = x;
							debut.y = i;
							mur_en_cours = true; //on inidique qu'on commence à lire un mur
						}
					}
				break;

				default:
					if(isalpha(c)) //si il s'agit d'une lettre on ajoute alors l'affiche correspondante
					{
						//si on a déjà visité cette affiche
						bool visite = false;
						for(auto iter = picts_vu.begin(); iter != picts_vu.end(); iter ++) //on verifie que l'affiche n'appartient pas à un mur horizontal
						{
							if(Coord{x,i} == *iter)
							{
								visite = true;
								break;
							}
						}
						if(!visite) //si on ne l'a pas déja visité, on l'ajoute
						{
							(l->_npicts)++;
							picts.push_back({x,i,x,i+1,table_texture[c]});
						}

					}
				break;
			}
		}
		//cout<<"fin analyse ligne"<<endl;
		x++;//on passe à la ligne suivante
	}
	//cout<<"fin de la recherche et affichage des résultats"<<endl;
	//cout<<"murs trouves: "<<l->_nwall<<endl;
	l->_walls = new Wall[l->_nwall];
	//on affiche la liste des murs créers
	for (int i = 0; i < l->_nwall; i++)
	{
		/*necessaire car le système de coordonnée
		utilisé dans le fichier est l'inverse de celui du prog*/
		int tmp = walls.at(i)._x1;
		walls.at(i)._x1 = walls.at(i)._y1;
		walls.at(i)._y1 = tmp;
		tmp = walls.at(i)._x2;
		walls.at(i)._x2 = walls.at(i)._y2;
		walls.at(i)._y2 = tmp;
		// On copie maintenant le mur corrigé dans l'objet labyrinthe
		(l->_walls)[i] = walls.at(i);
		//cout<<"Wall n°"<<i<<": "<<(l->_walls)[i]._x1<<","<<(l->_walls)[i]._y1<<" : "<<(l->_walls)[i]._x2<<","<<(l->_walls)[i]._y2<<":"<<(l->_walls)[i]._ntex<<endl;

	}

	l->_picts = new Wall[l->_npicts];
	//on affiche la liste des murs créers
	for (int i = 0; i < l->_npicts; i++)
	{
		/*necessaire car le système de coordonnée
		utilisé dans le fichier est l'inverse de celui du prog*/
		int tmp = picts.at(i)._x1;
		picts.at(i)._x1 = picts.at(i)._y1;
		picts.at(i)._y1 = tmp;
		tmp = picts.at(i)._x2;
		picts.at(i)._x2 = picts.at(i)._y2;
		picts.at(i)._y2 = tmp;
		// On copie maintenant le mur corrigé dans l'objet labyrinthe
		(l->_picts)[i] = picts.at(i);
		//cout<<"Wall n°"<<i<<": "<<(l->_walls)[i]._x1<<","<<(l->_walls)[i]._y1<<" : "<<(l->_walls)[i]._x2<<","<<(l->_walls)[i]._y2<<":"<<(l->_walls)[i]._ntex<<endl;

	}

	//cout<<"caisses trouvees: "<<l->_nboxes<<endl;
	//caisses->shrink_to_fit();
	//l->_boxes = caisses->data();
	l->_boxes = new Box[l->_nboxes];
	for (int i = 0; i < l->_nboxes; i++)
	{
		/*inversion des coords
		int tmp = caisses[i]._x;
		caisses[i]._x = caisses[i]._y;
		caisses[i]._y = tmp;
		////////////////////*/
		l->_boxes[i] = caisses[i];
		//cout<<"{"<<(caisses)[i]._x<<","<<(caisses)[i]._y<<"}"<<endl;
	}

	//cout<<"guardes trouves: "<<l->_nguards<<endl;
	//Guards->shrink_to_fit();
	//l->_guards = Guards->data();
	l->_guards = new Mover*[l->_nguards];
	for(int i = 0; i < l->_nguards; i++)
	{
		//inversion des coords
		/*
		float tmp = Guards[i]->_x;
		Guards[i]->_x = Guards[i]->_y;
		Guards[i]->_y = tmp;
		/////////////////////*/
		l->_guards[i] = Guards[i];
		//cout<<"Guard "<<l->_guards[i]<<": "<<(l->_guards[i])->_x<<","<<(l->_guards[i])->_y<<endl;
	}

	//cout<<"marques au sol trouvees: "<<l->_nmarks<<endl;
	l->_marks = new Box[l->_nmarks];
	for(int i = 0; i < l->_nmarks; i++)
	{
		//inversion des coordonnées:
		/*
		int temp = marques[i]._x;
		marques[i]._x = marques[i]._y;
		marques[i]._y = temp;
		////////////////////////////*/
		l->_marks[i] = marques[i];
	}


	return;
}


Labyrinthe::Labyrinthe (char* filename)
{

		//ouverture du fichier
		ifstream file;
		file.open(filename);
		string line;

		map <char,int> texture_table;
		scan_test(this, file, texture_table);
		//getchar();
		scan_laby(this,file,texture_table);
		//Initialisation du son des Gardiens
		Gardien::_Guard_fire = new Sound("sons/guard_fire.wav");
		Gardien::_Guard_death = new Sound("sons/guard_die.wav");
		Gardien::_Guard_hit = new Sound("sons/guard_hit.wav");
		////////////////////////////////////
		//initialisation de _data
		for(int i = 0; i < LAB_WIDTH; i++)
		{
			for (int j = 0; j < LAB_HEIGHT ; j++)
			{
				_data[i][j] = EMPTY;
			}
		}
		//mise a jour des data pour

		//les marques au sol
		for (int cpt = 0; cpt < _nmarks; cpt++)
		{
			_data[_marks[cpt]._x][_marks[cpt]._y] = -1;
		}
		//les murs:
		for (int cpt = 0; cpt < _nwall; cpt++)
		{
			Wall w = _walls[cpt];
			//cout<<"mur "<<cpt<<endl<<endl;
			if(w._x1 == w._x2)
			{
				for(int i = w._y1; i<=w._y2; i++)
				{
					_data[w._x1][i] = '1';
					//cout<<w._x1<<','<<i<<endl;
				}
			}else if (w._y1 == w._y2){
				for(int i = w._x1; i<=w._x2; i++)
				{
					_data[i][w._y1] = '1';
					//cout<<i<<','<<w._y1<<endl;
				}
			}
			//cout<<"fin mur "<<cpt<<endl;
		}
		//les boites
		for(int i = 0; i < _nboxes; i++)
		{
			Box b = _boxes[i];
			_data[b._x][b._y] = '1';
		}

		/*
		   On ne met pas de collision au trésor pour permettre au joueur de rentrer dedans afin de finir la partie
		   Pas non plus de collision pour les gardes pour eviter certains bugs dues à leur mouvment (texture entre deux cases mais collisions que dans une seule)
		*/

		//le trésor
		//_data[_treasor._x][_treasor._y] = '1';
		//guardes
		/*
		for(int i = 0; i < _nguards ; i++)
		{
			Mover* m = _guards[i];
			_data[(int)(m->_x/scale)][(int)(m->_y/scale)] = '1';

		}
		*/
		//cout<<"fin du test"<<endl;

		//AFFICHAGE POUR DU DEBUG
		/*
		file.close();
		cout<<endl<<endl<<"============================================================================================"<<endl;
		cout<<"Verif des données"<<endl<<endl;
		for(int i = 0; i < _nwall; i++)
		{
			cout<<"wall "<<i<<" "<<_walls[i]._x1<<","<<_walls[i]._y1<<" : "<<_walls[i]._x2<<","<<_walls[i]._y2<<endl;
		}

		for(int i = 0; i < _nboxes;i++)
		{
			cout<<"boite :"<<_boxes[i]._x<<","<<_boxes[i]._y<<endl;
		}

		for(int i = 0 ; i < _nmarks; i++)
		{
			cout<<"marque :"<<_marks[i]._x<<","<<_marks[i]._y<<endl;
		}
		for(int i = 0; i< _nguards; i++)
		{
			cout<<"garde "<<_guards[i]<<": "<<_guards[i]->_x<<","<<_guards[i]->_y<<endl;
		}
		for(int i = 0; i < _npicts; i++)
		{
			cout<<"pict "<<i<<"["<<_picts[i]._x1<<","<<_picts[i]._y1<<"] : "<<_picts[i]._ntex<<endl;
		}
		for(int j = 0; j < LAB_HEIGHT; j++)
		{
			for(int i = 0; i < LAB_WIDTH; i++)
			{
				cout<<data(i,j);
			}
			cout<<endl;
		}
		cout<<"fin verif des données"<<endl;
		//initialisation des données pour les murs
	////////////////////////////*/
	//cout<<"fin chargement labyrinthe"<<endl<<"appuie sur une touche"<<endl;
}
