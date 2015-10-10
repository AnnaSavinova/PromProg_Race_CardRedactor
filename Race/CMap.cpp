#include "stdafx.h"
#include "CMap.h"

CMap::CMap() : sizeX( 8 ), sizeY( 6 ), colorsNumber( 3 )
{}

CMap::~CMap()
{}

const std::vector< std::vector<int> >& CMap::GetNumbers()
{
    return numbers;
}

int CMap::GetX()
{
    return sizeX;
}

int CMap::GetY()
{
    return sizeY;
}

void CMap::ClickCell( int i, int j )
{
    numbers[i][j] = ( numbers[i][j] + 1 ) % colorsNumber;
}

void CMap::LoadMapFromFile( std::ifstream& fin )
{
    fin >> sizeX >> sizeY;
    numbers.resize( sizeY );
    for( int i = 0; i < sizeY; i++ ) {
        numbers[i].resize( sizeX );
        for( int j = 0; j < sizeX; j++ ) {
            fin >> numbers[i][j];
        }
    }
}

void CMap::SaveMapToFile( std::ofstream & fout )
{
    fout << sizeX << " " << sizeY;
    fout << std::endl;

    for( int i = 0; i < sizeY; i++ ) {
        for( int j = 0; j < sizeX; j++ ) {
            fout << numbers[i][j] << " ";
        }
        fout << std::endl;
    }
}

void CMap::RestartMap()
{
    numbers = std::vector< std::vector<int> >( sizeY );
    for( int i = 0; i < sizeY; i++ ) {
        numbers[i].resize( sizeX );
        for( int j = 0; j < sizeX; j++ ) {
            numbers[i][j] = 0;
        }
    }
}

void CMap::SetSize( int x, int y )
{
    sizeX = x;
    sizeY = y;
}