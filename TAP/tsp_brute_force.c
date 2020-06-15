//
//  TSP - BRUTE-FORCE
//
// -> la structure "point" est définie dans tools.h

static double dist(point A, point B) {
    double dx = A.x - B.x;
    double dy = A.y - B.y;
  return hypot(dx,dy);
}

static double value(point *V, int n, int *P) {
  double s = 0;
  for (int i=0 ; i < n-1 ; i++){
      s= s + dist(V[P[i]],V[P[i+1]]);
  }
  s = s + dist(V[P[0]],V[P[n-1]]);
  return s;
}
static double tsp_brute_force(point *V, int n, int *Q) {
  double min = DBL_MAX;
  double val;
  int P[n];
  for (int i = 0; i < n ; i++)
      P[i]=i;
  do{
      val = value(V,n,P);
      if(val < min){
          min = val;
          for(int i =0; i< n; i++){
              Q[i] = P[i];
          }
      }

  }while(NextPermutation(P,n-1)&& running);
  return min;
}

void Reverse(int *T, int z){
    int i = 0;
    int j = z -1;
    while (i <j){
        int temp = T[i];
        T[i] = T[j];
        T[j] = temp;
        i = i+1;
        j = j-1;
    }
    return;
}

static void MaxPermutation(int *P, int n, int k) {
    Reverse(P+k,n-k);
  return;
}
static double value_opt(point *V, int n, int *P, double w) {
    double s =0, retour = 0;
    int i = 0;
    while(i< n-1 && s + retour < w){
        s = s +dist(V[P[i]],V[P[i+1]]);
        retour = dist(V[P[0]],V[P[i+1]]);
        i=i+1;
    }
    if(s+retour >= w){
        return -i - 1;
    }
    return s + retour;
}

static double tsp_brute_force_opt(point *V, int n, int *Q) {
    double min = DBL_MAX;
    double val;
    int P[n];
    for (int i = 0; i < n ; i++)
        P[i]=i;
    do{
        val = value_opt(V,n,P,min);
        if (val < 0){
            Reverse(P-(int)val , n+(int)val-1);
        }
        else if(val < min){
            min = val;
            for(int i =0; i< n; i++){
                Q[i] = P[i];
            }
        }
    }while(NextPermutation(P,n-1)&& running);
    return min;
}

static double value_opt_tab(point *V, int n, int *P, double w) {
    static double ** D = NULL ; // initialisé à la compilation
        if ( D == NULL ){
            // ne sera exécuté qu’une seule fois
            D = malloc ( n * sizeof ( double *));
            for ( int i =0; i < n ; i ++){
                D [ i ]= malloc ( n * sizeof ( double ));
            for (int j=0;j<n;j++)
                D[i][j] = -1;
        }
    }
    double s =0, retour = 0;
    int i = 0;
    while(i< n-1 && s + retour < w){
        if(D[P[i]][P[i+1]] < 0){
            D[P[i]][P[i+1]] = dist(V[P[i]],V[P[i+1]]);
        }
        s= s + D[P[i]][P[i+1]];
        if(D[P[0]][P[i+1]] < 0){
            D[P[0]][P[i+1]] = dist(V[P[0]],V[P[i+1]]);
        }
        retour = D[P[0]][P[i+1]];
        i=i+1;
    }
    if(s+retour >= w){
        return -i - 1;
    }
    return s + retour;
}

static double tsp_brute_force_opt_tab(point *V, int n, int *Q) {
    double min = DBL_MAX;
    double val;
    int P[n];
    for (int i = 0; i < n ; i++)
        P[i]=i;
    do{
        val = value_opt_tab(V,n,P,min);
        if (val < 0){
            Reverse(P-(int)val , n+(int)val-1);
        }
        else if(val < min){
            min = val;
            for(int i =0; i< n; i++){
                Q[i] = P[i];
            }
        }
    }while(NextPermutation(P,n-1)&& running);
    return min;
}
