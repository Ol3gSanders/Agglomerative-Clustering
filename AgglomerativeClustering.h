#ifndef AGGLOMERATIVECLUSTERING_H
#define AGGLOMERATIVECLUSTERING_H

#include "Cluster.h"

#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <map>
#include <cmath>

using namespace std;

// Метрики
enum Metric {
	EUCLIDEAN = 0,
	MANHATTAN,
	COSINE
};

// Параметр linkage
enum Linkage {
	SINGLE = 0,
	COMPLETE,
	WARD,
	AVERAGE
};

class AgglomerativeClustering
{
public:
	AgglomerativeClustering( unsigned n_clusters = 2,
				 Metric metric = EUCLIDEAN,
				 Linkage linkage = WARD,
				 double distanceThreshold = 1000 );

	void fit( std::vector<std::vector<double> >& data );
	void printDistances();
	void printClusters();
	void printReport();
	void printPoints( std::vector<std::vector<double> > data );

private:
	std::string metricToStr( Metric metric );
	std::string linkageToStr( Linkage linkage );
	std::pair<unsigned, unsigned> findNearestNeighbors( Metric metricCalc, Linkage linkage );

	double eucledean( Cluster* c1, Cluster* c2 );

	// Функция для вычисления расстояния между двумя точками
	double distanceEuclidean( const std::vector<double>& p1,
				  const std::vector<double>& p2 );

	// Функция вычисления расстояния между двумя точками в зависимости от выбранной метрики
	double distanceCalc( const std::vector<double>& p1,
				 const std::vector<double>& p2,
				 Metric metric );

	// Функция объединения ближайших кластеров
	void mergeClusters( unsigned i,unsigned j );

	// ward
	double wardLinkage(  Cluster& c1, Cluster& c2 );

	// complete
	double completeLinkage( const Cluster& clusterA,
				const Cluster& clusterB,
				Metric metric );
	// average
	double averageLinkage( const Cluster& clusterA,
				   const Cluster& clusterB,
				   Metric metric );
	// single
	double singleLinkage( const Cluster& clusterA,
				  const Cluster& clusterB,
				  Metric metric );

private:
	// data
	std::map<std::pair<int, int>, double> metrics;
	int max_cluster_index = -1;
	unsigned num_clusters;
	vector<Cluster> clusters;
	Metric metric;
	Linkage linkage;
	double distanceThreshold;
	vector<double> min_metrics;
};

#endif // AGGLOMERATIVECLUSTERING_H
