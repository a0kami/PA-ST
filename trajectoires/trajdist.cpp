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
