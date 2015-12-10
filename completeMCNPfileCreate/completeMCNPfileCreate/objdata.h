#pragma once

class Face
{
private:
	unsigned int pointAindex_;
	unsigned int pointBindex_;
	unsigned int pointCindex_;
public:
	//unsigned int count;
	Face();
	unsigned int getpointAindex();
	unsigned int getpointBindex();
	unsigned int getpointCindex();
	void setpointAindex(unsigned int pointAindex);
	void setpointBindex(unsigned int pointBindex);
	void setpointCindex(unsigned int pointCindex);
};
class Point
{
private:
	float coorX_;
	float coorY_;
	float coorZ_;
public:
	//unsigned int count;
	Point();
	float getcoorX();
	float getcoorY();
	float getcoorZ();
	void setcoorX(float coorX);
	void setcoorY(float coorY);
	void setcoorZ(float coorZ);
};
class ObjGeo
{
public:
	Face* face;
	Point* pointnormal;
	Point* point;
	unsigned int facecount;
	unsigned int pointcount;
	unsigned int vncount;
	ObjGeo();
};

int storeGeo(char* filepath, ObjGeo& objgeo);
void freeGeo(ObjGeo& objgeo);
float calGeoVol(Point geocenter, ObjGeo objgeo);
Point calGeoCenter(ObjGeo objgeo);
float mixedProduct(Point A, Point B, Point C);


