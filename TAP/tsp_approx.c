//
//  TSP - APPROXIMATION / HEURISTIQUES
//

// void reverse(int *T, int p, int q) {
//   // renverse la partie T[p]...T[q] du tableau T avec p<q
//   // si T={0,1,2,3,4,5,6} et p=2 et q=5, alors le nouveau
//   // tableau sera {0,1, 5,4,3,2, 6}
//   for(int k = 0; k < (p+q)/2; k++){
//     int temp = T[p+k];
//     T[p+k] = T[q-k];
//     T[q-k] = temp;
//   }
//   return;
// }
void reverse(int *T, int p, int q){
    int i = p;
    int j = q;
    while (i <j){
        int temp = T[i];
        T[i] = T[j];
        T[j] = temp;
        i = i+1;
        j = j-1;
    }
    return;
}

double first_flip(point *V, int n, int *P) {
  // renvoie le gain>0 du premier flip réalisable, tout en réalisant
  // le flip, et 0 s'il n'y en a pas
  for(int j = 2; j< n-1; j++){
    for(int i = 0; i < j-1; i++){
      double gain = dist(V[P[i]],V[P[i + 1]]) + dist(V[P[j]],V[P[j + 1]])
                    -(dist(V[P[i]],V[P[j]]) + dist(V[P[i+1]],V[P[j+1]]));
      if(gain > 0){
        reverse(P, i+1, j);
        return gain;
      }
    }
  }

  for(int i = 0; i< n-2 ; i++){
    double gain = dist(V[P[i]],V[P[i + 1]]) + dist(V[P[n-1]],V[P[0]])
                  -(dist(V[P[i]],V[P[n-1]]) + dist(V[P[i+1]],V[P[0]]));
    if(gain>0){
      reverse(P, i+1, n-1);
      return gain;
    }
  }
  return 0.0;
}

double tsp_flip(point *V, int n, int *P) {
  // la fonction doit renvoyer la valeur de la tournée obtenue. Pensez
  // à initialiser P, par exemple à P[i]=i. Pensez aussi faire
  // drawTour() pour visualiser chaque flip
  for(int i = 0; i< n ; i++){
    P[i] = i;
  }
  while(first_flip(V,n,P) > 0){
    drawTour(V,n,P);
  };
  return (value(V,n,P));
}

double tsp_greedy(point *V, int n, int *P) {
  for(int i = 0 ; i < n; i++ ){
    P[i] = i;
  }
  for(int i = 0; i < n-1 ; i++){
    int closest;
    double distClosest = DBL_MAX;
    for(int j = i+1; j < n; j++){
        double d = dist(V[P[i]],V[P[j]]);
        if(d < distClosest){
          distClosest = d;
          closest = j;
        }
      }
      int temp = P[i+1];
      P[i+1] = P[closest];
      P[closest] = temp;
    }
  return value(V,n,P);
}
