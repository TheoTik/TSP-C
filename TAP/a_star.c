#include "tools.h"
#include "heap.h" // il faut aussi votre code pour heap.c
#include <math.h>


// Une fonction de type "heuristic" est une fonction h() qui renvoie
// une distance (double) entre une position de départ et une position
// de fin de la grille. La fonction pourrait aussi dépendre de la
// grille, mais on ne l'utilisera pas forcément ce paramètre.
typedef double (*heuristic)(position,position,grid*);


// Heuristique "nulle" pour Dijkstra.
double h0(position s, position t, grid *G){
  return 0.0;
}


// Heuristique "vol d'oiseau" pour A*.
double hvo(position s, position t, grid *G){
  return fmax(abs(t.x-s.x),abs(t.y-s.y));
}



// Heuristique "alpha x vol_d'oiseau" pour A*.
static double alpha=0; // 0 = h0, 1 = hvo, 2 = approximation ...
double halpha(position s, position t, grid *G) {
  return alpha*hvo(s,t,G);
}

double alphaman(position s, position t, grid *G){
  return alpha * sqrt(pow(abs(t.x-s.x),2)  + pow(abs(t.y-s.y),2)) ;
}

// Structure "noeud" pour le tas min Q.
typedef struct node {
  position pos;        // position (.x,.y) d'un noeud u
  double cost;         // coût[u]
  double score;        // score[u] = coût[u] + h(u,end)
  struct node* parent; // parent[u] = pointeur vers le père, NULL pour start
  int source;
} *node;


// Les arêtes, connectant les cases voisines de la grille (on
// considère le 8-voisinage), sont valuées par seulement certaines
// valeurs possibles. Le poids de l'arête u->v, noté w(u,v) dans le
// cours, entre deux cases u et v voisines est déterminé par la valeur
// de la case finale v. Plus précisément, si la case v de la grille
// contient la valeur C, le poids de u->v vaudra weight[C] dont les
// valeurs numériques exactes sont définies ci-après. La liste des
// valeurs possibles d'une case est donnée dans "tools.h": V_FREE,
// V_WALL, V_WATER, ... Remarquer que weight[V_WALL]<0, ce n'est pas
// une valuation correcte. Ce n'est pas choquant puisque si en
// position (i,j) si G.value[i][j] = V_WALL, alors c'est que le sommet
// à cette position n'existe pas.

double weight[]={
    1.0,  // V_FREE
  -99.0,  // V_WALL
    3.0,  // V_SAND
    9.0,  // V_WATER
    2.3,  // V_MUD
    1.5,  // V_GRASS
    0.1,  // V_TUNNEL
};


// Votre fonction A_star(G,h) doit construire un chemin dans la grille
// G entre la position G.start et G.end selon l'heuristique h(). S'il
// n'y a pas de chemin, affichez un simple message d'erreur. Sinon,
// vous devez remplir le champs .mark de la grille pour que le chemin
// trouvé soit affiché plus tard par drawGrid(G). La convention est
// qu'en retour G.mark[i][j] = M_PATH ssi (i,j) appartient au chemin
// trouvé (cf. "tools.h").
//
// Pour gérer l'ensemble P, servez-vous du champs G.mark[i][j] (=
// M_USED ssi (i,j) est dans P). Par défaut, ce champs est initialisé
// partout à M_NULL par initGrid().
//
// Pour gérer l'nullensemble Q, vous devez utiliser un tas min de noeuds
// (type node) avec une fonction de comparaison qui dépend du champs
// .score des noeuds. Pensez que la taille du tas Q est au plus la
// somme des degrés des sommets dans la grille. Pour visualiser un
// noeud de coordonnées (i,j) qui passe dans le tas Q vous pourrez
// mettre G.mark[i][j] = M_FRONT au moment où vous l'ajoutez.



node nodify(grid* G , heuristic h, position pos, node parent){

    if(G->value[pos.x][pos.y] == V_WALL)
        return NULL;

    int source;
    if(parent == NULL && pos.x == G->start.x && pos.y == G->start.y)
        source = 1;
    else if(parent == NULL && pos.x == G->end.x && pos.y == G->end.y)
        source = 0;
    else
        source = parent->source;

    if((source == 1 && G->mark[pos.x][pos.y] == M_USED) || (source == 0 && G->mark[pos.x][pos.y] == M_USED2))
        return NULL;
    node n = malloc(sizeof(*n));
    n-> pos = pos;

    n->source = source;

    if (parent!=NULL)
        n-> cost = parent->cost + weight[G->value[pos.x][pos.y]];
    else
        n->cost = 0;

    if(n->source == 1)
        n->score = h(pos, G->end, G) + n->cost;
    else
        n->score = h(pos, G->start, G) + n->cost;

    if(parent != NULL)
        if ((n->pos.x != parent->pos.x )&&(n->pos.y != parent->pos.y)){
            n->score = n->score +0.1 ; //Penalite de diagonale
        }
    if(G->mark[n->pos.x][n->pos.y] != M_USED && G->mark[n->pos.x][n->pos.y] != M_USED2)
        G->mark[pos.x][pos.y] = M_FRONT;
    n->parent = parent;
    return n;
}

int comparator (const void* a, const void* b){
    node n1 = (node) a;
    node n2 = (node) b;
    double x1 = n1-> score;
    double x2 = n2 -> score;
    if(x1 == x2){
        return 0;
    }
    if(x1 > x2)
        return 1;
    return -1;
}

void A_star(grid G, heuristic h){
    int MAX_NODE = 8 * G.X * G.Y;
    node used[MAX_NODE];
    int nbUsed =0;
    heap Q = heap_create(MAX_NODE, comparator );
    heap_add(Q,nodify(&G,h,G.start,NULL));
    heap_add(Q,nodify(&G,h,G.end,NULL));
    bool trouve = false;
    while(!heap_empty(Q) && !trouve){
        node n = heap_pop(Q);
        if((n->source == 1 && G.mark[n->pos.x][n->pos.y] == M_USED2)
                ||(n->source == 0 && G.mark[n->pos.x][n->pos.y] == M_USED)
                   ||(n->source == 1 && n->pos.x == G.end.x && n->pos.y == G.end.y)){
            //Un sommet est ajouté à Ps et mais etait déjà dans Pt, c'est fini
             trouve = true;
             node x = n;
             double l = n->cost;;
             while(x != NULL){
                 G.mark[x->pos.x][x->pos.y] = M_PATH;
                 x = x->parent;
             }
             int i = 0;
             while(i < nbUsed){
                 if(used[i] ->pos.x == n->pos.x && used[i] ->pos.y == n->pos.y){
                     x = used[i];
                     l+= x->cost;
                     while(x != NULL){
                         G.mark[x->pos.x][x->pos.y] = M_PATH;
                         x = x->parent;
                     }
                     i = nbUsed;
                 }
                 i++;
             }
             printf("chemin trouve de cout %f\n", l);
        }

        else if(G.mark[n->pos.x][n->pos.y] == M_USED || G.mark[n->pos.x][n->pos.y] == M_USED2){
           //Le node est deja dans P
            free(n);
        }
        else{
            //Le node n'est pas encore dans Ps ni Pt
            if(n->source == 1)
                G.mark[n->pos.x][n->pos.y] = M_USED; //On ajoute n à Ps
            else
                G.mark[n->pos.x][n->pos.y] = M_USED2;//On ajoute n à Pt
            drawGrid(G);
            for(int i =-1; i <= 1;i++){
                for(int j = -1;j<=1;j++){
                    if(i!=0 || j!=0){
                        position p = {.x = n->pos.x+i, .y= n->pos.y + j};
                        node v = nodify(&G,h,p,n);
                        if (v!=NULL){
                            heap_add(Q,v);
                            drawGrid(G);
                        }
                    }
                }
            }
            used[nbUsed] = n; // On stock n pour créer le path et le supprimer à la fin.
            nbUsed = nbUsed+1;
        }

    }
    while(! heap_empty(Q)){
        free(heap_pop(Q));
    }
    heap_destroy(Q);
    for(int i =0; i< nbUsed;i++){
        free(used[i]);
    }
    if(!trouve){
        printf("Chemin non trouve\n");
    }
  ;;;
  // Pensez à dessiner la grille avec drawGrid(G) à chaque fois que
  // possible, par exemple, lorsque vous ajoutez un sommet à P mais
  // aussi lorsque vous reconstruisez le chemin à la fin de la
  // fonction. Lorsqu'un sommet passe dans Q vous pourrez le marquerREE,1); // grille uniforme
    // grid G = initGridPoints(32,24,V_
  // M_FRONT (dans son champs .mark) pour le distinguer des sommets de
  // P (couleur différente).
  ;;;
  // Après avoir extrait un noeud de Q, il ne faut pas le détruire,
  // sous peine de ne plus pouvoir reconstruire le chemin trouvé! Vous
  // pouvez réfléchir à une solution simple pour libérer tous les
  // noeuds devenus inutiles à la fin de la fonction. Une fonction
  // createNode() peut simplifier votre code.
  ;;;
  // Les bords de la grille sont toujours constitués de murs (V_WALL) ce
  // qui évite d'avoir à tester la validité des indices des positions
  // (sentinelle).
  ;;;
  ;;;
  // Améliorations quand vous aurez fini:
  //
  // (1) Une fois la cible atteinte, afficher son coût ainsi que le
  // nombre de sommets visités (somme des .mark != M_NULL). Cela
  // permettra de comparer facilement les différences d'heuristiques,
  // h0() vs. hvo().
  //
  // (2) Le chemin a tendance à zizaguer, c'est-à-dire à utiliser
  // aussi bien des arêtes horizontales que diagonales (qui ont le
  // même coût), même pour des chemins en ligne droite. Essayer de
  // rectifier ce problème d'esthétique en modifiant le calcul de
  // score[v] de sorte qu'à coût[v] égale les arêtes (u,v)
  // horizontales ou verticales soient favorisées.
  //
  // (3) Modifier votre implémentation du tas dans heap.c de façon à
  // utiliser un tableau de taille variable, en utilisant realloc() et
  // une stratégie "doublante": lorsqu'il n'y a pas plus assez de
  // place dans le tableau, on double sa taille. On peut imaginer que
  // l'ancien paramètre 'n' devienne non pas le nombre maximal
  // d'éléments, mais la taille initial du tableau (comme par exemple
  // n=4).
  //
  // (4) Gérer plus efficacement la mémoire en libérant les noeudshalpha
  // devenus inutiles.
  //
  ;;;
}

int main(int argc, char *argv[]){

  unsigned seed=time(NULL)%1000;
  seed = 719;
  printf("seed: %u\n",seed); // pour rejouer la même grille au cas où
  srandom(seed);


  // tester les différentes grilles et positions s->t ...

  //grid G = initGridPoints(80,60,V_FREE,1); // grille uniforme
  //grid G = initGridPoints(32,24,V_WALL,0.2); // grille de points aléatoires
  //position s={G.X/4,G.Y/2}, t={G.X/2,G.Y/4}; G.start=s; G.end=t; // s->t
  //grid G = initGridLaby(64,48,4); // labyrinthe aléatoire
  grid G = initGridLaby(width/8,height/8,3); // labyrinthe aléatoire
  //position tmp; SWAP(G.start,G.end,tmp); // t->s (inverse source et cible)
  //grid G = initGridFile("mygrid.txt"); // grille à partir d'un fichier
 
  // pour ajouter à G des "régions" de différent types:

//  addRandomBlob(G, V_WALL,   (G.X+G.Y)/20);
//  addRandomBlob(G, V_SAND,   (G.X+G.Y)/15);
//  addRandomBlob(G, V_WATER,  (G.X+G.Y)/3);
//  addRandomBlob(G, V_MUD,    (G.X+G.Y)/3);
//  addRandomBlob(G, V_GRASS,  (G.X+G.Y)/15);
   //addRandomBlob(G, V_TUNNEL, (G.X+G.Y)/4);

  // constantes à initialiser avant init_SDL_OpenGL()
  scale = fmin((double)width/G.X,(double)height/G.Y); // zoom courant
  delay = 80; // délais pour l'affichage (voir tools.h)
  hover = false; // interdire déplacement de points
  init_SDL_OpenGL(); // à mettre avant le 1er "draw"
  drawGrid(G); // dessin de la grille avant l'algo
  update = false; // accélère les dessins répétitifs

  alpha=0;
  A_star(G,halpha); // heuristique: h0, hvo, alpha*hvo

  update = true; // force l'affichage de chaque dessin
  while (running) { // affiche le résultat et attend
    drawGrid(G); // dessine la grille
    handleEvent(true); // attend un évènement
  }

  freeGrid(G);
  cleaning_SDL_OpenGL();
  return 0;
}
