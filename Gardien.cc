#include "Gardien.h"
#include "Chasseur.h"
#include "Labyrinthe.h"
//#include <math.h>
#include <cmath>
#include <iostream>
#include <list>
using namespace std;

/**
 * @brief structure de vecteur basique utilise pour les calculs d'angles
 *
 */
struct vector
{
    float x,y;

    void init_vector(float x1,float y1, float x2, float y2)
    {
        x = x2 - x1;
        y = y2 - y1;
    }

    /**
     * @brief calcul la norme du vecteur
     *
     * @return float
     */
    float norm()
    {
        return sqrt(x*x + y*y);
    }

    /**
     * @brief mesure l'angle orienté entre le vecteur actuel et le vecteur fourni
     *
     * @param v le vecteur par rapport auquel on veut calculer l'angle
     * @return int un angle en degré
     */
    int angle_to(vector v)
    {
        return 180*(atan2(v.y,v.x) - atan2(y,x))/M_PI;
    }

    /**
     * @brief renvoie l'angle entre l'axe Y et le vecteur
     *
     * @return int un angle en degré
     */
    int angle()
    {
        return 180*(atan2(y,x) - atan2(1,0))/M_PI;
    }
};

/**
 * @brief fonction temporaire pour débuger l'aglo Bresenham
 *
 * @param tab le tableau a afficher
 * @warning Tous les affichages se font dans la console
 */
void affiche_data(char tab[LAB_WIDTH][LAB_HEIGHT])
{
    for(int i = 0; i< LAB_HEIGHT; i++)
    {
        cout<<"|";
        for(int j = 0; j < LAB_WIDTH; j++)
        {
            cout<<tab[j][i];
        }
        cout<<"|"<<endl;
    }
}

/**
 * @brief verifie si il y a des obstacles entre le joueur et le gardien
 * via l'algorithme de bresenham
 *
 * @param l l'environnemeent
 * @param x1 la position x du gardien
 * @param y1 la position y du gardien
 * @param x2 la position x du joueur
 * @param y2 la position y du joueur
 * @return true si aucun mur n'intersecte la ligne entre le joueur et le gardien
 * @return false sinon
 */
bool check_collision(Environnement* l, int x1, int y1, int x2, int y2)
{

    // Mise en place d'outil de debug

    char debug[LAB_WIDTH][LAB_HEIGHT];

    for (int y = 0; y < LAB_HEIGHT; y++)
    {
        for(int x = 0; x < LAB_WIDTH; x++)
        {
            debug[x][y] = '-';
        }

    }

    // debug[(int)x1][(int)y1] = 'X';
    // debug[(int)x2][(int)y2] = 'X';

    //initialisation de l'algo de bresenham
    int dx , dy;
    int sx, sy;
    int err,e2;

    if (x2 >= x1 )
    {
        dx = x2 - x1;
    }else{
      dx = x1 - x2;
    }

    if(y2 >= y1)
    {
      dy = y1 - y2;
    }else{
      dy = y2 - y1;
    }

    if(x1 < x2)
    {
      sx = 1;
    }else{
      sx = -1;
    }

    if(y1 < y2)
    {
      sy = 1;
    }else{
      sy = -1;
    }

    err = dx + dy;
    int x = x1;
    int y = y1;

    while(true)
    {

        if (x == x2 && y == y2)
          break;

        e2 = 2*err;
        if(e2 >= dy)
        {
          err += dy;
          x += sx;
        }

        if (e2 <= dx)
        {
          err += dx;
          y += sy;
        }

        // if(x != x2 || y != y2)
        //   debug[(int)x][(int)y] = 'O';

        if(l->data(x,y) > EMPTY)
        {
            // cout<<"fin recherche collision"<<endl;
            // affiche_data(debug);
            // cout<<"fin debug"<<endl;
            return false;
        }

    }
    return true;
}


/**
 * @brief verifie si il n'y a pas d'obstacles au point d'arrivé du gardien
 *
 * @param dx la composante x du vecteur de mouvmement
 * @param dy la composante y du vecteur du mouvement
 * @return true si le point d'arriver contient un obstacle
 * @return false sinon
 * @warning ne verifie pas encore si il y a un mur entre le point de départ et
  le point d'arrivé
 */
bool Gardien::move(double dx, double dy)
{
    int x = (_x)/Environnement::scale;
    int y = (_y)/Environnement::scale;
    int dest_x = (_x+dx)/Environnement::scale;
    int dest_y = (_y+dy)/Environnement::scale;
    //cout<<x<<";"<<y<<"|"<<_l->data(x,y)<<endl;
    if(!check_collision(_l,x,y,dest_x,dest_y))
    {
        return false;

    }else{
        _x = (_x+dx);
        _y = (_y+dy);
        return true;
    }
}

bool Gardien::process_fireball(float dx, float dy)
{
// calculer la distance entre le chasseur et le lieu de l'explosion.
	int px = _l->_guards[0]->_x/Environnement::scale;
    int py = _l->_guards[0]->_y/Environnement::scale;

    int x = _fb->get_x()/Environnement::scale;
    int y = _fb->get_y()/Environnement::scale;

    float dist = (vector{px - x, py - y}).norm();

    //message("dist:%f",dist);

	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale))
                             && dist > 1)

	{
		//message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	//float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	//_wall_hit -> play (1. - dist2/dmax2);
	//on verifie si la boule a touché le joueur
    //cout<<"Verif: "<<x<<";"<<y<<endl;
    if( dist <= 1)
    {
        cout<<"touché"<<endl;
        Chasseur::_hunter_hit->play();
        ((Entity*)(_l->_guards[0]))->degat(10);
    }
    tire = false;
	//message ("Booom...");
	return false;

}

void Gardien::fire(int angle_vertical)
{
    //message ("Woooshh...");
    tire = true;
	_Guard_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, (-_angle_cible));
}

void Gardien::update()
{
    if(est_mort() == 1)
    {
        //si il vient de mourir, on le met au sol et on le détruit
        rester_au_sol();
        despawn();
        return;
    }else if(est_traite())//si on a déjà réalisé l'appel à despawn
                          //on se contente d'arrêter update
    {
        return;
    }

    float p_x = (_l->_guards[0])->_x;///Environnement::scale;
    float p_y = (_l->_guards[0])->_y;///Environnement::scale;

    float x = _x/Environnement::scale;
    float y = _y/Environnement::scale;

    float _angle_rad = 180*(_angle_cible)/M_PI;

    vector dist{};
    dist.init_vector(_x,_y,p_x,p_y);

    // cout<<"------------------------------------------------------------------------"
    //     <<endl;

    //     cout<<"scale = "<<_l->scale<<endl;
    //     cout<<"pos monstre = ("<<x<<","<<y<<") | pos joueur = ("<<p_x<<","<<p_y<<")"<<endl;
    //     cout<<"dist: "<<dist.x<<";"<<dist.y<<" | angle = "<<dist.angle()<<endl;
    //     cout<<"look: "<<look.x<<";"<<look.y<<" | angle = "<<look.angle()<<endl;



    // cout<<"angle actuel: "<<_angle
    //     <<" | angle futur: "<<dist.angle()<<endl;
    // cout<<"------------------------------------------------------------------------"
    //     <<endl;

    //bool coll = !move(-sin(_angle_rad)*_speed,cos(_angle_rad)*_speed);
    bool coll = true;
    bool vu_debug = false;
    if(dist.norm() < FOV && check_collision(_l,x,y,p_x/Environnement::scale
                                                  ,p_y/Environnement::scale))
    {
        vu_debug = true;
        _angle_cible = dist.angle();
        if(!tire)
        {
            fire(0);
        }
    }

    if(!coll)
    {
        int offset = rand()%45;
        //cout<<offset<<endl;
        //_angle = (_angle+(offset))%360;
        //_angle_cible = _angle;
    }

    if(_angle_cible != _angle)
        _angle =  (1-_vitesse_rotation) * _angle + _vitesse_rotation * _angle_cible; //interpolation linéaire


     char test[100];
     sprintf(test,"Guard: angle:%d|cible:%d\nDist: %f angle:%d| %d",_angle,
             _angle_cible,dist.norm(),dist.angle(),vu_debug);
     message(test);

}
