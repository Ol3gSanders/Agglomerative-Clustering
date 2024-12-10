#include "Cluster.h"

Cluster::Cluster()
{
}

Cluster::Cluster(std::vector<std::vector<double> > points, int index)
	: points(points), index(index) {}

void Cluster::push_back( const vector<double> &vec ) {
	points.push_back( vec );
}

void Cluster::erase( size_t index ) {
	points.erase( points.begin() + index );
}

size_t Cluster::size() const {
	return points.size();
}

const vector<double> &Cluster::operator[]( size_t index ) const {
	return points[index];
}

vector<double> &Cluster::operator[]( size_t index ) {
	return points[index];
}
