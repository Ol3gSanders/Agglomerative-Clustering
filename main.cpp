#include "AgglomerativeClustering.h"

#include "TimeMeasurer.h"

#include <iostream>
#include <fstream>
#include <fstream>
#include <sstream>

using namespace std;


std::vector<std::vector<double> > readDataFromFile( const std::string& filePath ) {
	std::ifstream input( filePath );
	if ( !input.is_open()){
		throw std::runtime_error( "File not found: " + filePath );
	}

	std::vector<std::vector<double> > data;
	std::string line;
	while ( getline( input, line )){
		std::istringstream iss( line );
		std::vector<double> row;
		double value;
		while ( iss >> value ){
			row.push_back( value );
		}
		if ( !row.empty()){
			data.push_back( row );
		}
	}

	input.close();
	return data;
}

void printClusteringInfo( AgglomerativeClustering agc )
{
	agc.printReport();
	agc.printClusters();
	agc.printDistances();
}


int main()
{
	setlocale( LC_ALL, "" );

	const std::string fileName = "../Agglomerative-Clustering/data/points100_3.txt";

	vector<vector<double> > data = readDataFromFile( fileName );

	int nClusters = 2;
	Metric metric = Metric::EUCLIDEAN;
	Linkage linkage = Linkage::WARD;
	double distanceThreshold = 10000;

	TimeMeasurer tm;
	AgglomerativeClustering agc( nClusters, metric, linkage, distanceThreshold );
	agc.fit( data );
	wcout << L"Время выполнения кластеризации: " << tm.getSec() << L" секунд." << endl;

	// вывод отчета кластеризации
	printClusteringInfo(agc);

	return 0;
}

