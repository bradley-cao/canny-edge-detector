#include<iostream>
#include<iomanip>
#include<vector>
#include<list>
#include<ctime>
#include<cmath>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<string>
#include<chrono>
#include<random>
#include<unordered_map>
// #include <bits/stdc++.h>
using namespace std;

void drawPPM(vector<vector<int>> matrix, int y, int x, int max, char const* filename)
{
    FILE *fp;
    fp = fopen(filename, "w");
    int height = y;
    int width = x;
    int max_ccv = max;
    fprintf(fp,"P3 %d %d %d\n", width, height, max_ccv);

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < 3 * x; j++)
        {
            fprintf(fp, "%d", int(matrix[i][j]));
            fprintf(fp, " ");
        }   
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void part1(char const* filename)
{
    //generation of greyscale
    ifstream input(filename);
    string line;
    getline(input, line);
    getline(input, line);
    if (line.substr(0, 1) == "#")
    {
        getline(input, line);
        getline(input, line);
    }
    vector<int> parameters;
    string delimiter = " ";
    string val;
    int pos = 0;
    int a = 0;
    while ((pos = line.find(delimiter)) != string::npos)
    {
        val = line.substr(0, pos);
        parameters.push_back(stoi(val));
        line.erase(0, pos + delimiter.length());
    }
    parameters.push_back(stoi(line));

    getline(input, line);
    int max_ccv = stoi(line);
    int x = parameters[0];
    int y = parameters[1];
    vector<vector<int>> matrix;
    vector<vector<int>> intensities;

    for (int i = 0; i < y; i++)
    {
        vector<int> lineintensities;
        vector<int> values;
        string currLine = "";
        while (values.size() < 3 * x)
        {
            getline(input, line);
            pos = 0;
            while ((pos = line.find(delimiter)) != string::npos)
            {
                val = line.substr(0, pos);
                values.push_back(stoi(val));
                line.erase(0, pos + delimiter.length());
            }
        }
        vector<int> temp;
        for (int j = 0; j < x; j++)
        {
            int r = values[j*3];
            int g = values[j*3+1];
            int b = values[j*3+2];
            int greyval = (r+g+b)/3;
            temp.push_back(greyval);
            temp.push_back(greyval);
            temp.push_back(greyval);
            lineintensities.push_back(greyval);
        }
        matrix.push_back(temp);
        intensities.push_back(lineintensities);
    }
    drawPPM(matrix, y, x, max_ccv, "imageg.ppm");
    matrix.clear();

    //application of sobel
    int threshold = 40;
    for (int i = 0; i < y; i++)
    {
        vector<int> temp;
        for (int j = 0; j < x; j++)
        {
            int gx = 0;
            int gy = 0;
            if (j-1 > 0)
            {
                gx += intensities[i][j-1] * 2;
            }
            if (j+1 < x)
            {
                gx -= intensities[i][j+1] * 2;
            }
            if (i-1> 0)
            {
                gy += intensities[i-1][j] * 2;
            }
            if (i+1 < y)
            {
                gy -= intensities[i+1][j] * 2;
            }
            if (j-1 > 0 && i-1 > 0)
            {
                gx += intensities[i-1][j-1];
                gy += intensities[i-1][j-1];
            }
            if (j-1 > 0 && i+1 < y)
            {
                gx += intensities[i+1][j-1];
                gy -= intensities[i+1][j-1];
            }
            if (j+1 < x && i-1 >0)
            {
                gx -= intensities[i-1][j+1];
                gy += intensities[i-1][j+1];
            }
            if (j+1 < x && i+1 < y)
            {
                gx -= intensities[i+1][j+1];
                gy -= intensities[i+1][j+1];
            }
            int magnitude = int(sqrt(gx*gx + gy*gy));
            if (magnitude > threshold)
            {
                temp.push_back(1);
                temp.push_back(1);
                temp.push_back(1);

            }
            else
            {
                temp.push_back(0);
                temp.push_back(0);
                temp.push_back(0);
            }
        }
        matrix.push_back(temp);
    }
    drawPPM(matrix, y, x, 1, "imagem.ppm");
}

int main()
{
    part1("image.ppm");
}
