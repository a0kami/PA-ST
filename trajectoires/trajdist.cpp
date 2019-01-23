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
	return distST2(t1,t2,i,j);
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

float distS(Json::Value t1, Json::Value t2, int i, int j){
	const float omega= 1.f;
	const float a= std::atan2(t1[i+1]["y"].asFloat()-t1[i]["y"].asFloat(), t1[i+1]["x"].asFloat()-t1[i]["x"].asFloat()),
	            b= std::atan2(t2[j+1]["y"].asFloat()-t2[j]["y"].asFloat(), t2[j+1]["x"].asFloat()-t2[j]["x"].asFloat());
	const float theta= std::abs(a-b);
	auto f=[=](float t){
		return (distSMID(t1,t2,i,j)/distMAX(t1,t2))*(omega+t);
	};
	return f(theta)*distST2(t1,t2,i,j);
}

#include <limits>
#include <iostream>
float sdtw(Json::Value t1, Json::Value t2){
	//std::cout << t1.size() << ' ' << t2.size() << std::endl;
	if(t1.size()<=1 && t2.size()<=1) return 0;
	if(t1.size()<=1 || t2.size()<=1) return std::numeric_limits<float>::max();
	Json::Value restT1= t1;
	Json::Value restT2= t2;
	restT1.removeIndex(0,nullptr);
	restT2.removeIndex(0,nullptr);
	return distS(t1, t2, 0, 0) + std::min(std::min(
		sdtw(t1, restT2),
		sdtw(t2, restT1)),
		sdtw(restT1, restT2));
}
