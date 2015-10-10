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

    //меняет состояние клетки с координатами (i, j)
    void ClickCell( int i, int j );
    //обнуляет карту
    void RestartMap();
    //устанавливает размеры поля
    void SetSize( int x, int y );

    void LoadMapFromFile( std::ifstream& );
    void SaveMapToFile( std::ofstream& );
private:
    std::vector< std::vector<int> > numbers;
    int sizeX;
    int sizeY;
    int colorsNumber;
};