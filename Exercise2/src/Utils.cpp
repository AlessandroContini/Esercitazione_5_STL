#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

namespace PolygonalLibrary
{
bool ImportMesh(PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(mesh))
        return false;

    if(!ImportCell1Ds(mesh))
        return false;

    if(!ImportCell2Ds(mesh))
        return false;

    return true;

}

bool ImportCell0Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell0Ds.csv");

    if(file.fail())
        return false;
	
	list<string> listLines;

    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();
	
	listLines.pop_front();

    mesh.NumCell0Ds = listLines.size();

    if (mesh.NumCell0Ds == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DsId.reserve(mesh.NumCell0Ds);
    mesh.Cell0DsCoordinates = Eigen::MatrixXd::Zero(3, mesh.NumCell0Ds);
	
	for (const string& line : listLines)
    {
        istringstream converter(line);
		string tmp;
		getline(converter, tmp, ';');
		unsigned int id = stoul(tmp);
		//cout << "ID: " << id << endl;
		getline(converter, tmp, ';');
        unsigned int marker = stoul(tmp);
		//cout << "marker: " << marker << endl;
		
		Vector2d coord;
		for (unsigned int i = 0; i < 2; ++i) {
			getline(converter, tmp, ';');
			coord(i) = stod(tmp);
			//cout << "Coordinate " << i << ": " << coord(i) << endl;
		}
		
		mesh.Cell0DsCoordinates(0, id) = coord(0);
		mesh.Cell0DsCoordinates(1, id) = coord(1);
		
        mesh.Cell0DsId.push_back(id);

        if(marker != 0)
        {
            const auto it = mesh.MarkerCell0Ds.find(marker);
            if(it == mesh.MarkerCell0Ds.end())
            {
                mesh.MarkerCell0Ds.insert({marker, {id}});
            }
            else
            {
                it->second.push_back(id);
            }
        }

    }

    return true;
}

bool ImportCell1Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell1Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumCell1Ds = listLines.size();

    if (mesh.NumCell1Ds == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    mesh.Cell1DsExtrema = Eigen::MatrixXi(2, mesh.NumCell1Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);
		string tmp;
		getline(converter, tmp, ';');
		unsigned int id = stoul(tmp);
		getline(converter, tmp, ';');
        unsigned int marker = stoul(tmp);
		
        Vector2i vertices;
		for (unsigned int i = 0; i < 2; ++i) {
			getline(converter, tmp, ';');
			vertices(i) = stoi(tmp);
		}
		
		mesh.Cell1DsExtrema(0, id) = vertices(0);
		mesh.Cell1DsExtrema(1, id) = vertices(1);
		
        mesh.Cell1DsId.push_back(id);

        if(marker != 0)
        {
            const auto it = mesh.MarkerCell1Ds.find(marker);
            if(it == mesh.MarkerCell1Ds.end())
            {
                mesh.MarkerCell1Ds.insert({marker, {id}});
            }
            else
            {             
                it->second.push_back(id);
            }
        }
    }

    return true;
}

bool ImportCell2Ds(PolygonalMesh& mesh)
{
    ifstream file;
    file.open("./Cell2Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumCell2Ds = listLines.size();

    if (mesh.NumCell2Ds == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DsId.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsVertices.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsEdges.reserve(mesh.NumCell2Ds);
	
	for (const string& line : listLines)
    {
        istringstream converter(line);

		string tmp;
		getline(converter, tmp, ';');
		unsigned int id = stoul(tmp);
		getline(converter, tmp, ';');
        unsigned int marker = stoul(tmp);
		getline(converter, tmp, ';');
		unsigned int NumVertices = stoul(tmp);
		
        VectorXi vertices;
		vertices.resize(NumVertices);
		for(unsigned int i = 0; i < NumVertices; i++){
			getline(converter, tmp, ';');
			vertices(i) = stoi(tmp);
		}
		
		getline(converter, tmp, ';');
		unsigned int NumEdges = stoul(tmp);
		
        VectorXi edges;
		edges.resize(NumEdges);
		for(unsigned int i = 0; i < NumEdges; i++){
			getline(converter, tmp, ';');
			edges(i) = stoi(tmp);
		}
		
        mesh.Cell2DsId.push_back(id);
		mesh.Cell2DsNumVertices.push_back(NumVertices);
        mesh.Cell2DsVertices.push_back(vertices);
		mesh.Cell2DsNumEdges.push_back(NumEdges);
        mesh.Cell2DsEdges.push_back(edges);
		
        if(marker != 0)
        {
            const auto it = mesh.MarkerCell2Ds.find(marker);
            if(it == mesh.MarkerCell2Ds.end())
            {
                mesh.MarkerCell1Ds.insert({marker, {id}});
            }
            else
            {
                it->second.push_back(id);
            }
        }
    }

    return true;
}

//TEST SUI DATI
bool TestMarkers(PolygonalMesh& mesh)
{
	for(const auto& marker_coppie : mesh.MarkerCell0Ds)
		for(unsigned int id : marker_coppie.second)
			if(id >= mesh.Cell0DsId.size() || mesh.Cell0DsId[id] != id)
				return false;
			
	for(const auto& marker_coppie : mesh.MarkerCell1Ds)
		for(unsigned int id : marker_coppie.second)
			if(id >= mesh.Cell1DsId.size() || mesh.Cell1DsId[id] != id)
				return false;
			
	for(const auto& marker_coppie : mesh.MarkerCell2Ds)
		for(unsigned int id : marker_coppie.second)
			if(id >= mesh.Cell2DsId.size() || mesh.Cell2DsId[id] != id)
				return false;
			
	return true;
}

bool TestEdgeLength(PolygonalMesh& mesh)
{
	for(unsigned int i = 0; i < mesh.NumCell1Ds; i++){
		double x0 = mesh.Cell0DsCoordinates(0, mesh.Cell1DsExtrema(0, i));
		double x1 = mesh.Cell0DsCoordinates(0, mesh.Cell1DsExtrema(1, i));
		double y0 = mesh.Cell0DsCoordinates(1, mesh.Cell1DsExtrema(0, i));
		double y1 = mesh.Cell0DsCoordinates(1, mesh.Cell1DsExtrema(1, i));
		
		double length = sqrt((x1-x0)*(x1-x0) + (y1-yo)*(y1-yo));
		
		if(length <= 1e-10)
			return false;
	}
	return true;
}

bool TestPolygonArea(PolygonalMesh& mesh)
{
	for(unsigned int i = 0; i < mesh.NumCell2Ds; i++){
		const VectorXi& vertices = mesh.Cell2DsVertices[i];
        double area = 0.0;
        int n = vertices.size();

        for (int i = 0; i < n; ++i)
        {
            int j = (i + 1) % n; //modulo n (quando arriva all'ultimo torna al primo)
            double xi = mesh.Cell0DsCoordinates(0, vertices(i));
            double yi = mesh.Cell0DsCoordinates(1, vertices(i));
            double xj = mesh.Cell0DsCoordinates(0, vertices(j));
            double yj = mesh.Cell0DsCoordinates(1, vertices(j));
            area += (xi * yj - xj * yi);
        }

        area = fabs(area) * 0.5;
        if (area <= 1e-10)
            return false;
    }
    return true;
}

}
