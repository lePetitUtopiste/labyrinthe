#include "Gardien.h"
#include "Chasseur.h"
#include "Labyrinthe.h"
//#include <math.h>
#include <cmath>
#include <iostream>
#include <list>
using namespace std;

struct vector
{
    float x,y;
    
    void init_vector(float x1,float y1, float x2, float y2)
    {
        x = x2 - x1;
        y = y2 - y1;
    }

    float norm()
    {
        return sqrt(x*x + y*y);
    }

    int angle_to(vector v)
    {
        return 180*(atan2(v.y,v.x) - atan2(y,x))/M_PI;
    }

    int angle()
    {
        return 180*(atan2(y,x))/M_PI;
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
        for(int j = 0; j < LAB_WIDTH; j++)
        {
            cout<<tab[j][i];
        }
        cout<<endl;
    }
}

bool check_collision(Environnement* l, int x1, int y1, int x2, int y2)
{
    char debug[LAB_WIDTH][LAB_HEIGHT];
    for (int y = 0; y < LAB_HEIGHT; y++)
    {
        for(int x = 0; x < LAB_WIDTH; x++)
        {
            debug[x][y] = ' ';
        }
    }

    debug[(int)x1][(int)y1] = 'X';
    debug[(int)x2][(int)x2] = 'X';
    
    float dx = abs(x2 - x1);
    float sx;
    if(x1 < x2)
    {
        sx = 1;
    }else{
        sx = -1;
    }

    float dy = -abs(y2 - y1);
    float sy;

    float error = dx + dy;

    if(y1 < y2)
    {
        sy = -1;
    }else{
        sy = 1;
    }

    while(true)
    {
        if(x1 == x2 && y1 == y2)
        {
            //affiche_data(debug);
            return true;
        }
        float e2 = 2*error;
        if(e2 >= dy)
        {
            if(x1 == x2)
            {
                //affiche_data(debug);
                return true;
            }
            error = error + dy;
            x1 = x1 + sx;
        }
        if (e2 <= dx)
        {
            if (y1 == y2)
            {
                //affiche_data(debug);
                return true;
            }
            error = error + dx;
            y1 = y1 + sy;
        }

        debug[(int)x1][(int)y1] = 'O';
        //si on tombe sur une case occupée
        // on arrête la recherche et on renvoie faux
        if(l->data((int)x1,(int)y1) != EMPTY)
        {
            //affiche_data(debug);
            return false;
        }
    }
}



bool Gardien::move(double dx, double dy)
{
    if(((Labyrinthe*)_l)->data((_x+dx)/Environnement::scale,(_y+dy)/Environnement::scale) != EMPTY)
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
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		//message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	//_wall_hit -> play (1. - dist2/dmax2);
	//on verifie si la boule a touché le joueur
    int px = _l->_guards[0]->_x/Environnement::scale;
    int py = _l->_guards[0]->_y/Environnement::scale;
    //cout<<"Verif: "<<x<<";"<<y<<endl;
    if((int)((_fb->get_x())/Environnement::scale) == px && (int)((_fb->get_y())/Environnement::scale) == py)
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
				 /* angles de vis�e */ angle_vertical, _angle);
}

void Gardien::update()
{
    if(est_mort())
    {
        return;
    }

    float p_x = (_l->_guards[0])->_x;///Environnement::scale;
    float p_y = (_l->_guards[0])->_y;///Environnement::scale;

    float x = _x/Environnement::scale;
    float y = _y/Environnement::scale;

    float _angle_rad = 180*(_angle_cible+90)/M_PI;

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

    bool coll = !move(cos(_angle_rad),sin(_angle_rad));
    //bool coll = true;
    if(dist.norm() < FOV )//&& check_collision(_l,x,y,p_x,p_y))
    {
        _angle_cible = dist.angle() - 90; 
        if(!tire)
            fire(0);
    }   

    else if(coll)
    {
        int offset = rand()%45;
        cout<<offset<<endl; 
        _angle_cible = (_angle_cible+(offset)+90)%360;
    }

    if(_angle_cible != _angle)
        _angle =  (1-_vitesse_rotation) * _angle + _vitesse_rotation * _angle_cible;
    
    char test[100];
    sprintf(test,"Guard: angle:%d|cible:%d\nDist: %f angle:%d| %d",_angle,_angle_cible,dist.norm(),dist.angle(),coll);
    message(test);

}