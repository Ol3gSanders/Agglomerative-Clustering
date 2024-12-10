#include "AgglomerativeClustering.h"

#include "TimeMeasurer.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cmath>

using namespace std;

AgglomerativeClustering::AgglomerativeClustering( unsigned n_clusters, Metric metric, Linkage linkage, double distanceThreshold )
{
	this->num_clusters = n_clusters;
	this->metric = metric;
	this->linkage = linkage;
	this->distanceThreshold = distanceThreshold;
}

void AgglomerativeClustering::fit( vector<vector<double> > &data )
{
	metrics.clear();
	min_metrics.clear();
	clusters.clear();
	// Проверка входной метрики и линковки
	if ( linkage == WARD && metric != EUCLIDEAN ){
		ostringstream oss;
		string provided_metric = metricToStr( metric );
		oss << provided_metric << " was provided as metric. " << "WARD can only work with euclidean distances.";
		throw logic_error( oss.str());
	}

	for ( size_t i = 0; i < data.size(); ++i ){
		clusters.push_back( Cluster( { data[i] }, i ));
	}

	max_cluster_index = clusters.size() - 1;


	while ( clusters.size() > num_clusters && this->distanceThreshold >= 0 ){
		TimeMeasurer tm;
		// Находим два ближайших кластера
		auto nearestNeighbors = findNearestNeighbors( metric, linkage );

		// Объединяем ближайшие кластеры
		mergeClusters( nearestNeighbors.first, nearestNeighbors.second );

	}
}

pair<unsigned, unsigned> AgglomerativeClustering::findNearestNeighbors( Metric metric, Linkage linkage )
{
	double min_metric = std::numeric_limits<double>::infinity();
	int i_min = -1, j_min = -1;

	for ( size_t i = 0; i < clusters.size(); ++i ){
		for ( size_t j = i + 1; j < clusters.size(); ++j ){
			double distance;
			switch ( linkage ){
			case SINGLE:
				distance = singleLinkage( clusters[i], clusters[j], metric );
				break;
			case COMPLETE:
				distance = completeLinkage( clusters[i], clusters[j], metric );
				break;
			case AVERAGE:
				distance = averageLinkage( clusters[i], clusters[j], metric );
				break;
			case WARD:
				distance = wardLinkage( clusters[i], clusters[j] );
				break;
			}

			if ( distance < min_metric ){
				min_metric = distance;
				i_min = i;
				j_min = j;
			}
		}
	}

	min_metrics.push_back( min_metric );
	return { i_min, j_min };
}

double AgglomerativeClustering::eucledean( Cluster *c1, Cluster *c2 ) {
	if ( c1->ndim() == 2 ){
		c1 = c1->mean();
	}
	if ( c2->ndim() == 2 ){
		c2 = c2->mean();
	}

	double d = 0.0;
	for ( size_t k = 0; k < c1->points[0].size(); ++k ){
		d += std::pow( c1->points[0][k] - c2->points[0][k], 2 );
	}
	return std::sqrt( d );
}

double AgglomerativeClustering::wardLinkage( Cluster &c1, Cluster &c2 )
{
	unsigned n1 = c1.size();
	unsigned n2 = c2.size();

	int ind1 = std::min( c1.index, c2.index );
	int ind2 = std::max( c1.index, c2.index );

	if ( metrics.find( { ind1, ind2 } ) != metrics.end()){
		return metrics[{ ind1, ind2 }];
	}

	if ( n1 == 1 && n2 == 1 ){
		double d = eucledean( &c1, &c2 );
		metrics[{ ind1, ind2 }] = d;
		return d;
	}


	if ( n1 > 1 ){
		Cluster cc1 = c1.childrens[0];
		Cluster cc2 = c1.childrens[1];

		int v = ( c2.ndim() == 1 ) ? 1 : c2.shape().first;
		int s = cc1.size();
		int t = cc2.size();
		int T = v + s + t;

		vector<double> last_point_c1 = c1.points.back();

		double d = std::sqrt(( v + s ) / (double)T * std::pow( wardLinkage( c2, cc1 ), 2 ) +
					 ( v + t ) / (double)T * std::pow( wardLinkage( c2, cc2 ), 2 ) -
					 v / (double)T * std::pow( wardLinkage( cc1, cc2 ), 2 ));

		metrics[{ ind1, ind2 }] = d;
		return d;
	}

	if ( n2 > 1 ){
		Cluster cc1 = c2.childrens[0];
		Cluster cc2 = c2.childrens[1];
		vector<double> last_point_c2 = c2.points.back();

		int v = ( c1.ndim() == 1 ) ? 1 : c1.shape().first;
		int s = cc1.size();
		int t = cc2.size();
		int T = v + s + t;


		double d = std::sqrt(( v + s ) / (double)T * std::pow( wardLinkage( c1, cc1 ), 2 ) +
					 ( v + t ) / (double)T * std::pow( wardLinkage( c1, cc2 ), 2 ) -
					 v / (double)T * std::pow( wardLinkage( cc1, cc2 ), 2 ));

		metrics[{ ind1, ind2 }] = d;
		return d;
	}
	return -1;
}

double AgglomerativeClustering::distanceEuclidean( const vector<double> &p1, const vector<double> &p2 )
{
	double sum = 0;
	for ( size_t i = 0; i < p1.size(); ++i ){
		sum += pow( p1[i] - p2[i], 2 );
	}
	return sqrt( sum );
}

double AgglomerativeClustering::distanceCalc( const vector<double> &p1, const vector<double> &p2, Metric metric )
{
	switch ( metric ){
	case EUCLIDEAN:
		return distanceEuclidean( p1, p2 );
	case MANHATTAN: {
		double sum = 0;
		for ( size_t i = 0; i < p1.size(); ++i ){
			sum += abs( p1[i] - p2[i] );
		}
		return sum;
	}
	case COSINE: {
		double dotProduct = 0, normA = 0, normB = 0;
		for ( size_t i = 0; i < p1.size(); ++i ){
			dotProduct += p1[i] * p2[i];
			normA += pow( p1[i], 2 );
			normB += pow( p2[i], 2 );
		}
		return 1 - ( dotProduct / ( sqrt( normA ) * sqrt( normB )));
	}
	}
	return -1;
}


void AgglomerativeClustering::mergeClusters( unsigned i, unsigned j )
{
	max_cluster_index++;

	std::vector<std::vector<double> > concat_points( clusters[i].points );
	concat_points.insert( concat_points.end(), clusters[j].points.begin(), clusters[j].points.end());

	Cluster* new_cluster = new Cluster( concat_points, max_cluster_index );
	new_cluster->childrens.push_back( clusters[i] );
	new_cluster->childrens.push_back( clusters[j] );

	clusters.erase( clusters.begin() + j );
	clusters.erase( clusters.begin() + i );
	clusters.push_back( *new_cluster );

}

double AgglomerativeClustering::completeLinkage( const Cluster& clusterA, const Cluster &clusterB, Metric metric )
{
	double maxDistance = 0;
	for ( const auto& a : clusterA.points ){
		for ( const auto& b : clusterB.points ){
			double dist = distanceCalc( a, b, metric );
			if ( dist > maxDistance ){
				maxDistance = dist;
			}
		}
	}
	return maxDistance;
}

double AgglomerativeClustering::averageLinkage( const Cluster &clusterA, const Cluster &clusterB, Metric metric )
{
	double totalDistance = 0;
	int count = 0;
	for ( const auto& a : clusterA.points ){
		for ( const auto& b : clusterB.points ){
			double dist = distanceCalc( a, b, metric );
			totalDistance += dist;
			++count;
		}
	}
	return totalDistance / count;
}

double AgglomerativeClustering::singleLinkage( const Cluster &clusterA, const Cluster &clusterB, Metric metric )
{
	double minDistance = numeric_limits<double>::infinity();
	for ( const auto& a : clusterA.points ){
		for ( const auto& b : clusterB.points ){
			double dist = distanceCalc( a, b, metric );
			if ( dist < minDistance ){
				minDistance = dist;
			}
		}
	}
	return minDistance;
}

void AgglomerativeClustering::printDistances()
{
	setlocale( LC_ALL, "" );

	wcout << L"\nДистанции: \n" << "_____________" << endl;

	for ( auto dist: this->min_metrics ){
		cout << dist << endl;
	}
	cout << "_____________" << endl;
}

void AgglomerativeClustering::printClusters()
{
	int idx = 1;
	for ( auto cluster: clusters ){
		wcout << L"\nКластер " << idx << endl;
		cout << "_____________" << endl;
		for ( auto vec: cluster.points ){
			for( auto item: vec ){
				cout << setprecision( 8 )  << item << "\t";
			}
			cout << "\n";
		}
		cout << "_____________\n";
		idx++;
	}
}

void AgglomerativeClustering::printReport()
{
	setlocale( LC_ALL, "" );
	wcout << L"_____Параметры кластеризации_____" << endl;
	wcout << L"Количество кластеров: " << num_clusters << endl;
	wcout << L"Метрика: " << metricToStr( metric ).c_str() << endl;
	wcout << L"Линковка: " << linkageToStr( linkage ).c_str() << endl;
	wcout << L"Порог: " << distanceThreshold << endl;
	cout << "" << endl;

	wcout << L"Результаты кластеризации: ";
	cout << "( ";
	for( auto cluster: clusters ){
		cout << cluster.size() << " ";
	}
	cout << ")\n";
	cout << "_________________________________" << endl;
}

void AgglomerativeClustering::printPoints( vector<vector<double> > data )
{
	setlocale( LC_ALL, "" );
	wcout << L"_______Точки_______\n" << endl;
	int idx = 1;
	for ( auto points: data ){
		wcout << L"Точка " << idx << ": ";
		for ( auto item: points ){
			cout << setprecision( 8 ) << "\t" << item << "\t";
		}
		idx++;
		cout << "\n";
	}
	cout << "____________________\n" << endl;
}

string AgglomerativeClustering::metricToStr( Metric metric )
{
	switch ( metric ){
	case EUCLIDEAN: return "EUCLIDEAN";
	case MANHATTAN: return "MANHATTAN";
	case COSINE: return "COSINE";
	}
	return "";
}

string AgglomerativeClustering::linkageToStr( Linkage linkage )
{
	switch ( linkage ){
	case SINGLE: return "SINGLE";
	case COMPLETE: return "COMPLETE";
	case AVERAGE: return "AVERAGE";
	case WARD: return "WARD";
	}
	return "";
}
