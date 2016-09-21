#pragma once
#include<iostream>
#include<fstream>

//using namespace std;



class KDE
{

public:
	static void ComputeKDE(double *buffer, double *de, long n, long numBins, double sigma);
	
};
