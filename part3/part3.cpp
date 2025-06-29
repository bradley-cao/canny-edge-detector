//Bradley Cao Period 5
#include<iostream>
#include<vector>
#include<cmath>
#include<fstream>
#include<string>
#include<unordered_map>
using namespace std;
//round down if angle is exactly halfway between multiples of 45 (i.e. 22.5 degrees)

void drawPPM(vector<vector<int>> matrix, int y, int x, int max, char const* filename)
{
    FILE* fp;
    fp = fopen(filename, "w");
    int height = y;
    int width = x;
    int max_ccv = max;
    fprintf(fp, "P3 %d %d %d\n", width, height, max_ccv);

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {            
            fprintf(fp, "%d", int(matrix[i][j]));
            fprintf(fp, " ");
            fprintf(fp, "%d", int(matrix[i][j]));
            fprintf(fp, " ");
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
            int r = values[j * 3];
            int g = values[j * 3 + 1];
            int b = values[j * 3 + 2];
            int greyval = (r + g + b) / 3;
            temp.push_back(greyval);
            // temp.push_back(greyval);
            // temp.push_back(greyval);
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
            if (j - 1 > 0)
            {
                gx += intensities[i][j - 1] * 2;
            }
            if (j + 1 < x)
            {
                gx -= intensities[i][j + 1] * 2;
            }
            if (i - 1 > 0)
            {
                gy += intensities[i - 1][j] * 2;
            }
            if (i + 1 < y)
            {
                gy -= intensities[i + 1][j] * 2;
            }
            if (j - 1 > 0 && i - 1 > 0)
            {
                gx += intensities[i - 1][j - 1];
                gy += intensities[i - 1][j - 1];
            }
            if (j - 1 > 0 && i + 1 < y)
            {
                gx += intensities[i + 1][j - 1];
                gy -= intensities[i + 1][j - 1];
            }
            if (j + 1 < x && i - 1 > 0)
            {
                gx -= intensities[i - 1][j + 1];
                gy += intensities[i - 1][j + 1];
            }
            if (j + 1 < x && i + 1 < y)
            {
                gx -= intensities[i + 1][j + 1];
                gy -= intensities[i + 1][j + 1];
            }
            int magnitude = int(sqrt(gx * gx + gy * gy));
            if (magnitude > threshold)
            {
                temp.push_back(1);
            }
            else
            {
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
    for (int x1 = x - 1; x1 <= x + 1; x1++)
    {
        for (int y1 = y - 1; y1 <= y + 1; y1++)
        {
            if (x1 >= 0 && y1 >= 0 && x1 < width && y1 < height && x1 != x && y1 != y)
            {
                val = intensities[y1][x1];
                if (val == 1)
                {
                    intensities[y][x] = 1;
                    for (int a = x1 - 1; a <= x1 + 1; a++)
                    {
                        for (int b = y1 - 1; b <= y1 + 1; b++)
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

vector<vector<int>> getmagnitudes(vector<vector<int>> intensities, int x, int y)
{
    vector<vector<int>> magnitudes;
    for (int i = 0; i < y; i++)
    {
        vector<int> temp;
        for (int j = 0; j < x; j++)
        {
            int gx = 0;
            int gy = 0;
            if (j - 1 >= 0)
            {
                gx += intensities[i][j - 1] * 2;
            }
            if (j + 1 < x)
            {
                gx -= intensities[i][j + 1] * 2;
            }
            if (i - 1 >= 0)
            {
                gy += intensities[i - 1][j] * 2;
            }
            if (i + 1 < y)
            {
                gy -= intensities[i + 1][j] * 2;
            }
            if (j - 1 >= 0 && i - 1 >= 0)
            {
                gx += intensities[i - 1][j - 1];
                gy += intensities[i - 1][j - 1];
            }
            if (j - 1 >= 0 && i + 1 < y)
            {
                gx += intensities[i + 1][j - 1];
                gy -= intensities[i + 1][j - 1];
            }
            if (j + 1 < x && i - 1 >= 0)
            {
                gx -= intensities[i - 1][j + 1];
                gy += intensities[i - 1][j + 1];
            }
            if (j + 1 < x && i + 1 < y)
            {
                gx -= intensities[i + 1][j + 1];
                gy -= intensities[i + 1][j + 1];
            }
            int magnitude = int(sqrt(gx * gx + gy * gy));
            temp.push_back(magnitude);

        }
        magnitudes.push_back(temp);
    }
    return magnitudes;
}

vector<vector<int>> getangles(vector<vector<int>> intensities, int x, int y)
{
    const double pi = 3.14159265358979323846;
    vector<vector<int>> angles;
    for (int i = 0; i < y; i++)
    {
        vector<int> temp;
        for (int j = 0; j < x; j++)
        {
            int gx = 0;
            int gy = 0;
            if (j - 1 > 0)
            {
                gx += intensities[i][j - 1] * 2;
            }
            if (j + 1 < x)
            {
                gx -= intensities[i][j + 1] * 2;
            }
            if (i - 1 > 0)
            {
                gy += intensities[i - 1][j] * 2;
            }
            if (i + 1 < y)
            {
                gy -= intensities[i + 1][j] * 2;
            }
            if (j - 1 > 0 && i - 1 > 0)
            {
                gx += intensities[i - 1][j - 1];
                gy += intensities[i - 1][j - 1];
            }
            if (j - 1 > 0 && i + 1 < y)
            {
                gx += intensities[i + 1][j - 1];
                gy -= intensities[i + 1][j - 1];
            }
            if (j + 1 < x && i - 1 > 0)
            {
                gx -= intensities[i - 1][j + 1];
                gy += intensities[i - 1][j + 1];
            }
            if (j + 1 < x && i + 1 < y)
            {
                gx -= intensities[i + 1][j + 1];
                gy -= intensities[i + 1][j + 1];
            }
            double angle = atan2(gy, gx) * 180 / pi;
            int intangle;
            //round angle to proper int angle that is multiple of 45 degrees
            if (-180 < angle < -135)
            {
                if (angle == -157.5)
                {
                    intangle = -180;
                }
                else
                {
                    if (abs(angle - (-180)) < abs(angle-(-135)))
                    {
                        intangle = -180;
                    }
                    else
                    {
                        intangle = -135;
                    }
                }
            }
            else if (-135 < angle < -90)
            {
                if (angle == -112.5)
                {
                    intangle = -135;
                }
                else
                {
                    if (abs(angle - (-135)) < abs(angle-(-90)))
                    {
                        intangle = -135;
                    }
                    else
                    {
                        intangle = -90;
                    }
                }
            }
            else if (-90 < angle < -45)
            {
                if (angle == -67.5)
                {
                    intangle = -90;
                }
                else
                {
                    if (abs(angle - (-90)) < abs(angle-(-45)))
                    {
                        intangle = -90;
                    }
                    else
                    {
                        intangle = -45;
                    }
                }
            }
            else if (-45 < angle < 0)
            {
                if (angle == -22.5)
                {
                    intangle = -45;
                }
                else
                {
                    if (abs(angle - (-45)) < abs(angle-(0)))
                    {
                        intangle = -45;
                    }
                    else
                    {
                        intangle = 0;
                    }
                }
            }
            else if (0 < angle < 45)
            {
                if (angle == 22.5)
                {
                    intangle = 0;
                }
                else
                {
                    if (abs(angle - (0)) < abs(angle-(45)))
                    {
                        intangle = 0;
                    }
                    else
                    {
                        intangle = 45;
                    }
                }
            }
            else if (45 < angle < 90)
            {
                if (angle == 67.5)
                {
                    intangle = 45;
                }
                else
                {
                    if (abs(angle - (45)) < abs(angle-(90)))
                    {
                        intangle = 45;
                    }
                    else
                    {
                        intangle = 90;
                    }
                }
            }
            else if (90 < angle < 135)
            {
                if (angle == 112.5)
                {
                    intangle = 90;
                }
                else
                {
                    if (abs(angle - (90)) < abs(angle-(135)))
                    {
                        intangle = 90;
                    }
                    else
                    {
                        intangle = 135;
                    }
                }
            }
            else if (135 < angle < 180)
            {
                if (angle == 157.5)
                {
                    intangle = 135;
                }
                else
                {
                    if (abs(angle - (135)) < abs(angle-(180)))
                    {
                        intangle = 135;
                    }
                    else
                    {
                        intangle = 180;
                    }
                }
            }
            else
            {
                intangle = (int) angle;
            }
            if (intangle < 0)
            {
                intangle += 180;
            }
            temp.push_back(intangle);
        }
        angles.push_back(temp);
    }
    return angles;
}

vector<vector<int>> hysteresis(vector<vector<int>> intensities, int x, int y, int lower, int upper)
{
    vector<vector<int>> magnitudes = getmagnitudes(intensities, x, y);
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
                if (j == 0 || j == x-1 || i == 0 | i == y-1)
                {
                    binaryline.push_back(0);
                }
                else
                {
                    binaryline.push_back(1);
                }
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
        }
        matrix.push_back(line);
    }
    return matrix;
}

void part2(char const* filename, int lower, int upper)
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
            int r = values[j * 3];
            int g = values[j * 3 + 1];
            int b = values[j * 3 + 2];
            int greyval = (r + g + b) / 3;
            lineintensities.push_back(greyval);
        }
        matrix.push_back(lineintensities);
    }
    vector<vector<int>> output = hysteresis(matrix, x, y, lower, upper);
    drawPPM(output, y, x, 1, "image1.ppm");
}

void part3(char const* filename, int lower, int upper)
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
        vector<int> temp;
        for (int j = 0; j < x; j++)
        {
            int r = values[j * 3];
            int g = values[j * 3 + 1];
            int b = values[j * 3 + 2];
            int greyval = (r + g + b) / 3;
            temp.push_back(greyval);
            lineintensities.push_back(greyval);
        }
        matrix.push_back(temp);
        intensities.push_back(lineintensities);
    }
    drawPPM(matrix, y, x, max_ccv, "imageg.ppm");
    vector<vector<int>> output = hysteresis(intensities, x, y, lower, upper);
    drawPPM(output, y, x, 1, "image1.ppm");
    vector<vector<int>> magnitudes = getmagnitudes(intensities, x, y);
    vector<vector<int>> angles = getangles(magnitudes, x, y);

    vector<vector<int>> nonmax;
    for (int i = 0; i < y; i++)
    {
        vector<int> temp;
        for (int j = 0; j < x; j++)
        {
            if (j <= 1 || j >= x-2 || i <= 1 || i >= y-2)
            {
                temp.push_back(1);
            }
            else
            {
                int q;
                int r;
                if (angles[i][j] == 0 || angles[i][j] == 180)
                {
                    q = magnitudes[i][j+1];
                    r = magnitudes[i][j-1];
                }
                if (angles[i][j] == 45)
                {
                    q = magnitudes[i+1][j-1];
                    r = magnitudes[i-1][j+1];
                }
                if (angles[i][j] == 90)
                {
                    q = magnitudes[i+1][j];
                    r = magnitudes[i-1][j];
                }
                if (angles[i][j] == 135)
                {
                    q = magnitudes[i-1][j-1];
                    r = magnitudes[i+1][j+1];
                }
                if (magnitudes[i][j] >= q && magnitudes[i][j] >= r)
                {
                    temp.push_back(1);
                }
                else
                {
                    temp.push_back(0);
                }
            }
        }
        nonmax.push_back(temp);
    }
    drawPPM(nonmax, y, x, 1, "image2.ppm");

    vector<vector<int>> output3;
    for (int i = 0; i < y; i++)
    {
        vector<int> templine;
        for (int j = 0; j < x; j++)
        {
            if (output[i][j] == 1 && nonmax[i][j] == 1)
            {
                templine.push_back(1);
            }
            else
            {
                templine.push_back(0);
            }
        }
        output3.push_back(templine);
    }
    drawPPM(output3, y, x, 1, "imagef.ppm");
}

int main(int argc, char* argv[])
{
    int lower = 125;
    int upper = 250;
    const char* file = "image.ppm";
    if (argc != 1)
    {
        lower = atoi(argv[2]);
        upper = atoi(argv[4]);
        file = argv[6];
    }
    // part1("image.ppm");
    // part2("image.ppm");
    part3(file, lower, upper);
}