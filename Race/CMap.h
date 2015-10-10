#pragma once
#include "stdafx.h"
#include <vector>

class CMap {
public:
    CMap();
    ~CMap();

    const std::vector< std::vector<int> >& GetNumbers();
    int GetX();
    int GetY();

    //������ ��������� ������ � ������������ (i, j)
    void ClickCell( int i, int j );
    //�������� �����
    void RestartMap();
    //������������� ������� ����
    void SetSize( int x, int y );

    void LoadMapFromFile( std::ifstream& );
    void SaveMapToFile( std::ofstream& );
private:
    std::vector< std::vector<int> > numbers;
    int sizeX;
    int sizeY;
    int colorsNumber;
};