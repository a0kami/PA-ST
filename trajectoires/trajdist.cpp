#include "trajdist.h"
float distEucl(Json::Value p1, Json::Value p2) {
    return sqrt((p1["x"].asFloat() - p2["x"].asFloat()) * (p1["x"].asFloat() - p2["x"].asFloat())
         + (p1["y"].asFloat() - p2["y"].asFloat()) * (p1["y"].asFloat() - p2["y"].asFloat()));
}

float dtw_old(Json::Value t1, Json::Value t2) {
    //initialisation du tableau
    std::vector<std::vector<float>> dist(t1.size()+1, std::vector<float>(t2.size()+1, 0));
    //std::cout << "dimention du tableau : [" << dist.size() << "," << dist[0].size() << "]" << std::endl;

    for(uint x = 1 ; x < t1.size()+1; x++) {
        for(uint y = 1 ; y < t2.size()+1; y++) {
            float poidChemin = std::min(std::min(dist[x-1][y] , dist[x][y-1]), dist[x-1][y-1]);
            dist[x][y] = distEucl(t1[x-1], t2[y-1]) + poidChemin;
        }
    }

    return dist[t1.size()][t2.size()];
}

float dtw(Json::Value t1, Json::Value t2) {
    //initialisation du tableau
    std::vector<std::vector<float>> dist(t1.size()+1, std::vector<float>(t2.size()+1, 0));
    for(uint x = 1 ; x < t1.size()+1; x++) {
        dist[x][0] = MAXFLOAT;
    }
    for(uint y = 1 ; y < t2.size()+1; y++) {
        dist[0][y] = MAXFLOAT;
    }
    //std::cout << "dimention du tableau : [" << dist.size() << "," << dist[0].size() << "]" << std::endl;

    for(uint x = 1 ; x < t1.size()+1; x++) {
        for(uint y = 1 ; y < t2.size()+1; y++) {
            float poidChemin = std::min(std::min(dist[x-1][y] , dist[x][y-1]), dist[x-1][y-1]);
            dist[x][y] = distEucl(t1[x-1], t2[y-1]) + poidChemin;
        }
    }

    return dist[t1.size()][t2.size()];
}

float dist(float x1, float y1, float x2, float y2){
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

float distPointSegment(float x1, float y1, float x2, float y2, float x3, float y3){ // {x2;y2 x3;y3} is the segment
	const float r= (x3-x2)*(x1-x2) + (y3-y2)*(y1-y2);
	if(r<=0)
		return dist(x1,x2,y1,y2);
	const float Lseg= dist(x2,x3,y2,y3);
	if(r>=Lseg)
		return dist(x1,x3,y1,y3);
	const float dx= x2+(x3-x2)*(r/Lseg);
	const float dy= y2+(y3-y2)*(r/Lseg);
	return dist(x1,dx,y2,dy);
}

float distPS(Json::Value t1, Json::Value t2, int i, int j){
	float mid1[2], mid2[2];
	if(j==0){
		mid1[0]=t2[j]["x"].asFloat(); // xmid1
		mid1[1]=t2[j]["y"].asFloat(); // ymid1
	}else {
		mid1[0]=(t2[j-1]["x"].asFloat()+t2[j]["x"].asFloat())/2; // xmid1
		mid1[1]=(t2[j-1]["y"].asFloat()+t2[j]["y"].asFloat())/2; // ymid1
	}
	if(j==t2.size()-1){
		mid2[0]=t2[j]["x"].asFloat(); // xmid2
		mid2[1]=t2[j]["y"].asFloat(); // ymid2
	}else {
		mid2[0]=(t2[j+1]["x"].asFloat()+t2[j]["x"].asFloat())/2; // xmid2
		mid2[1]=(t2[j+1]["y"].asFloat()+t2[j]["y"].asFloat())/2; // ymid2
	}
	return distPointSegment(t1[i]["x"].asFloat(), t1[i]["y"].asFloat(), mid1[0], mid1[1], mid2[0], mid2[1]);
}

float distP(Json::Value t1, Json::Value t2, int i, int j){
	return distPS(t1,t2,i,j)+distPS(t2,t1,j,i);
}

float distT(Json::Value t1, Json::Value t2, int i, int j){
	Json::Value A,B;
	if(t1[i]["time"].asFloat()<t2[j]["time"].asFloat()){
		A=t2[j]; B=t1[i];
	}
	else{
		A=t1[i]; B=t2[j];
	}
	const float delta_t= A["time"].asFloat()-B["time"].asFloat();
	const float vx= (t1[i]["x"].asFloat()-t1[i-1]["x"].asFloat())/delta_t;
	const float vy= (t1[i]["y"].asFloat()-t1[i-1]["y"].asFloat())/delta_t;
	const float Bx= t1[i-1]["x"].asFloat()+vx*(B["time"].asFloat()+delta_t-t1[i-1]["time"].asFloat());
	const float By= t1[i-1]["y"].asFloat()+vy*(B["time"].asFloat()+delta_t-t1[i-1]["time"].asFloat());
	return dist(A["x"].asFloat(),Bx,A["y"].asFloat(),By);
}

float distST(Json::Value t1, Json::Value t2, int i, int j){
	/** We do not consider time in the given data*/
	//const float t= 10.f;
	return distP(t1,t2,i,j);//+t*distT(t1,t2,i,j);
}

float distST2(Json::Value t1, Json::Value t2, int i, int j){ // {t1[i] ; t[i+1]} and {t2[j] ; t2[j+1]} are the segments
	return distST(t1, t2, i, j) + distST(t1, t2, i+1, j+1);
}

float distSMID(Json::Value t1, Json::Value t2, int i, int j){
    float mid1[2], mid2[2];
    if(i==t1.size()-1){
        mid1[0]=t1[i]["x"].asFloat()+t1[i+1]["x"].asFloat(); // xmid1
        mid1[1]=t1[i]["y"].asFloat()+t1[i+1]["y"].asFloat(); // ymid1
    }else {
        mid1[0]=t1[i]["x"].asFloat(); // xmid1
        mid1[1]=t1[i]["y"].asFloat(); // ymid1
    }
    if(j==t2.size()-1){
        mid2[0]=t2[j]["x"].asFloat()+t2[j+1]["x"].asFloat(); // xmid2
        mid2[1]=t2[j]["y"].asFloat()+t2[j+1]["y"].asFloat(); // ymid2
    }else {
        mid2[0]=t2[j]["x"].asFloat(); // xmid2
        mid2[1]=t2[j]["y"].asFloat(); // ymid2
    }
    return dist(mid1[0],mid1[1],mid2[0],mid2[1]);
}

float distMAX(Json::Value t1, Json::Value t2){
	float dmax=0;
	for(uint i=0; i<t1.size(); i++)
		for(uint j=0; j<t2.size(); j++){
			const float d= distEucl(t1[i],t2[j]);
			if(d>dmax) dmax=d;
		}
	return dmax;
}

float distS(Json::Value t1, Json::Value t2, int i, int j,float Max){
    const float omega= 1.f;
	const float a= std::atan2(t1[i+1]["y"].asFloat()-t1[i]["y"].asFloat(), t1[i+1]["x"].asFloat()-t1[i]["x"].asFloat()),
	            b= std::atan2(t2[j+1]["y"].asFloat()-t2[j]["y"].asFloat(), t2[j+1]["x"].asFloat()-t2[j]["x"].asFloat());
	const float theta= std::abs(a-b);
	auto f=[=](float t){
        //return (distSMID(t1,t2,i,j)/distMAX(t1,t2))*(omega+t);
        return (distSMID(t1,t2,i,j)/Max)*(omega+t);
	};
	return f(theta)*distST2(t1,t2,i,j);
}
float gauss_kernel(float D){

    float S = 666;
    return std::log(-D*D/(2*S*S));
}

#include <limits>
#include <iostream>
/*float sdtw(Json::Value t1, Json::Value t2){
    //std::cout << t1.size() << ' ' << t2.size() << std::endl;
    if(t1.size()<=1 && t2.size()<=1){ return 0; }
    if(t1.size()<=1 || t2.size()<=1){return std::numeric_limits<float>::max();}

	Json::Value restT1= t1;
	Json::Value restT2= t2;

	restT1.removeIndex(0,nullptr);

	restT2.removeIndex(0,nullptr);

    return distS(t1, t2, 0, 0,0) + std::min(std::min(
		sdtw(t1, restT2),
		sdtw(t2, restT1)),
		sdtw(restT1, restT2));
}*/

float sdtw(Json::Value t1,Json::Value t2){
    long n = t1.size(), m = t2.size();

    std::cout<<"starting distS"<<std::endl;
    std::vector<std::vector<float>>  dists;
    float Max = distMAX(t1,t2);
    for(int i=0; i<n-1;++i){
        std::vector<float> row;
        for(int j=0; j<m-1;++j){
            row.push_back(distS(t1,t2,i,j,Max));
        }
        dists.push_back(row);
    }
     std::cout<<"distS done"<<std::endl;
      std::cout<<"starting init mat"<<std::endl;
    std::vector<std::vector<float>>  mat;

    for(int i=0; i<n;++i){
        std::vector<float> row;
        for(int j=0; j<m;++j){
            row.push_back(0);
        }
        mat.push_back(row);
    }
std::cout<<"init mat done"<<std::endl;
  std::cout<<"starting  mat"<<std::endl;
    for(int i=1; i<n-1;++i){
        for(int j=1; j<m-1;++j){
            mat[i][j]= dists[i][j] +std::min(mat[i-1][j-1],std::min(mat[i-1][j],mat[i][j-1]));
        }
    }
std::cout<<" mat done"<<std::endl;
    return mat[n-2][m-2];

}

/**
 * @brief hausdorffComponent
 * @param a Ensemble 1
 * @param b Ensemble 2
 * @return La composant a vers b de la distance d'hausdorff
 */
float hausdorffComponent(Json::Value a, Json::Value b){

    // Stockage de la Distance (float), type de primitive la plus proche (char: 's' segment ou 'p' point), id de la primitive (uint id-ième point ou segment)
    // Pour chaque sommet
    std::vector<float> distanceA;
    std::vector<char> typeDistanceA;
    std::vector<uint> idDistanceA;
    // Pour un sommet donné, garde en mémoire pour chaque segment de B
    std::vector<float> distanceSommetA;
    std::vector<char> typeDistanceSommetA;
    std::vector<uint> idDistanceSommetA;

    // 1. Distances des sommets de a vers polyligne b
    // Pour chaque sommet de a
    for(uint sommetA = 0; sommetA < a.size(); ++sommetA){
        // Reset
        distanceSommetA.clear();
        typeDistanceSommetA.clear();
        idDistanceSommetA.clear();

        // Pour chaque segment de b
        for(uint sommetB = 1; sommetB < b.size(); ++sommetB){
            // Calcul des vecteurs
            std::vector<float> b1a1 = {a[sommetA]["x"].asFloat() - a[sommetB-1]["x"].asFloat(), a[sommetA]["y"].asFloat() - a[sommetB-1]["y"].asFloat()};
            std::vector<float> b2a1 = {a[sommetA]["x"].asFloat() - a[sommetB]["x"].asFloat(), a[sommetA]["y"].asFloat() - a[sommetB]["y"].asFloat()};
            std::vector<float> b1b2 = {a[sommetB]["x"].asFloat() - a[sommetB-1]["x"].asFloat(), a[sommetB]["y"].asFloat() - a[sommetB-1]["y"].asFloat()};
            // Normalisation ?

            // Vérification sommetA dans la bande orthogonale au segment sommetB-1, sommetB
            float produitScalaireBandeGauche = b1a1[0] * b1b2[0] + b1a1[1] * b1b2[1];
            float produitScalaireBandeDroite  = b2a1[0] * b1b2[0] + b2a1[1] * b1b2[1];
            //std::cout << bandeGauche << std::endl << bandeDroite << std::endl << std::endl;

            // Si les 2 produits scalaires sont supérieures à 0, distance = produit scalaire entre b1a1 et vecteur orthogonal unitaire
            if(produitScalaireBandeGauche >= 0 && produitScalaireBandeDroite >= 0) {
                std::vector<float> vecteurOrthoUnit = {-(b[sommetB]["y"].asFloat() - b[sommetB-1]["y"].asFloat()), b[sommetB]["x"].asFloat() - b[sommetB-1]["x"].asFloat()};
                float normeVecteurOrthoUnit = sqrt(pow(vecteurOrthoUnit[0], 2) + pow(vecteurOrthoUnit[1], 2));
                vecteurOrthoUnit[0] = vecteurOrthoUnit[0] / normeVecteurOrthoUnit;
                vecteurOrthoUnit[1] = vecteurOrthoUnit[1] / normeVecteurOrthoUnit;

                // Push dans sommetA
                distanceSommetA.push_back(b1a1[0] * vecteurOrthoUnit[0] + b1a1[1] * vecteurOrthoUnit[1]);
                typeDistanceSommetA.push_back('s');
                idDistanceSommetA.push_back(sommetB-1);
            }
            // Sinon distance = distance euclidienne du sommet de B le plus proche
            else {
                float distance1 = distEucl(a[sommetA], b[sommetB-1]);
                float distance2 = distEucl(a[sommetA], b[sommetB]);
                if(distance1 <= distance2) {
                    distanceSommetA.push_back(distance1);
                    idDistanceSommetA.push_back(sommetB-1);
                }
                else {
                    distanceSommetA.push_back(distance2);
                    idDistanceSommetA.push_back(sommetB);
                }
                typeDistanceSommetA.push_back('p');
            }
        }

        // La plus petite distance est la distance euclidienne de sommetA à la polyligne b
        distanceA.push_back(distanceSommetA[0]);
        typeDistanceA.push_back(typeDistanceSommetA[0]);
        idDistanceA.push_back(idDistanceSommetA[0]);
        for(uint i = 0; i < distanceSommetA.size(); ++i) {
            if(distanceSommetA[i] < distanceA[sommetA]) {
                distanceA[sommetA] = distanceSommetA[i];
                typeDistanceA[sommetA] = typeDistanceSommetA[i];
                idDistanceA[sommetA] = idDistanceSommetA[i];
            }
        }
    }

    // 2. Élimination d'analyse complémentaire pour chaque segment de a
    std::vector<bool> analyseSup;
    for(uint sommetA = 1; sommetA < a.size(); ++sommetA){
        analyseSup.push_back(
            ((typeDistanceA[sommetA-1] == typeDistanceA[sommetA])
             &&
             (idDistanceA[sommetA-1] == idDistanceA[sommetA]))
            ||
            (((typeDistanceA[sommetA-1] == 'p')
             &&
             (typeDistanceA[sommetA] == 'p'))
             &&
             (abs((int) idDistanceSommetA[sommetA-1] - (int)idDistanceA[sommetA]) == 0))
        );
    }

    // Valeur retournée : Distance max des distances min de chaque sommet
    float maxDistance = distanceA[0];
    for(uint i = 1; i < distanceA.size(); ++i) {
        if(distanceA[i] > maxDistance) {
            maxDistance = distanceA[i];
        }
    }

    return maxDistance;

    // TODO:
    // 3. Analyse complémentaire
    for(auto value: analyseSup) {
        std::cout << value << std::endl;
    }

}

/**
 * @brief hausdorff
 * @param q Trajectoire requête
 * @param t Trajectoire testée
 * @return Distance de hausdorff entre q et t
 */
float hausdorff(Json::Value q, Json::Value t){
    // Calcule les 2 composants de la distance d'hausdorff
    float c1 = hausdorffComponent(q, t);
    float c2 = hausdorffComponent(t, q);

    // Renvoie le maximum de des 2 composants D(q->t), D(t->q)
    return fmax(c1, c2);
}
