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
    int threshold = 100;
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

vector<vector<int>> hystRecur(int x, int y, vector<vector<int>> intensities, int width, int height)
{
    int val = 0;
    for (int x1 = x-1; x1 <= x+1; x1++)
    {
        for (int y1 = y-1; y1 <= y+1; y1++)
        {
            if (x1 >= 0 && y1 >= 0 && x1 < width && y1 < height && x1 != x && y1 != y)
            {
                val = intensities[y1][x1];
                if (val == 1)
                {
                    intensities[y][x] = 1;
                    // intensities = hystRecur(x1, y1, intensities, width, height);
                    for (int a = x1-1; a <= x1+1; a++)
                    {
                        for (int b = y1-1; b <= y1+1; b++)
                        {
                            if (a >= 0 && b >= 0 && a < width && b < height && a != x1 && b != y1)
                            {
                                val = intensities[b][a];
                                if (val == -1)
                                {
                                    intensities = hystRecur(a, b, intensities, width, height);
                                }
                           }
                        }
                    }
                }
            }
        }
    }
    if (intensities[y][x] != 1)
    {
        intensities[y][x] = 0;
    }
    return intensities;
}

vector<vector<int>> hysteresis (vector<vector<int>> intensities, int x, int y)
{
    int lower = 50;
    int upper = 100;
    vector<vector<int>> magnitudes;
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
            temp.push_back(magnitude);
        }
        magnitudes.push_back(temp);
    }

    vector<vector<int>> binary;
    int count;
    for (int i = 0; i < y; i++)
    {
        vector<int> binaryline;
        for (int j = 0; j < x; j++)
        {
            int val = magnitudes[i][j];
            if (val >= upper)
            {
                binaryline.push_back(1);
            }
            else if (val >= lower)
            {
                binaryline.push_back(-1);
                count += 1;
            }
            else
            {
                binaryline.push_back(0);
            }
        }
        binary.push_back(binaryline);
    }
    count = 0;
    vector<vector<int>> matrix;
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            if (binary[i][j] == -1)
            {
                // cout << i << " " << j << "\n";
                binary = hystRecur(j, i, binary, x, y);
                count += 1;
            }
        }
    }
    for (int i = 0; i < y; i++)
    {
        vector<int> line;
        for (int j = 0; j < x; j++)
        {
            line.push_back(binary[i][j]);
            line.push_back(binary[i][j]);
            line.push_back(binary[i][j]);
        }
        matrix.push_back(line);
    }
    return matrix;
}

void part2(char const* filename)
{
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
        for (int j = 0; j < x; j++)
        {
            int r = values[j*3];
            int g = values[j*3+1];
            int b = values[j*3+2];
            int greyval = (r+g+b)/3;
            lineintensities.push_back(greyval);
        }
        matrix.push_back(lineintensities);
    }
    vector<vector<int>> output = hysteresis(matrix, x, y);
    drawPPM(output, y, x, 1, "image1.ppm");
}

int main()
{
    // part1("image.ppm");
    part2("image.ppm");
}
