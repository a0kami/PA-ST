#ifndef TRAJDIST_H
#define TRAJDIST_H

#include <dist/json/json.h>
#include <math.h>

float distEucl(Json::Value p1, Json::Value p2);
float dtw_old(Json::Value t1, Json::Value t2);
float dtw(Json::Value t1, Json::Value t2);
float sdtw(Json::Value t1, Json::Value t2);
//float sdtw2(Json::Value t1, Json::Value t2);
float hausdorff(Json::Value q, Json::Value r);

#endif // TRAJDIST_H
