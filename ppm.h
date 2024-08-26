#include <iostream>
#include <fstream>
#include <vector>

class PpmWriter
{
    public:
        PpmWriter(int width, int height) : w(width), h(height){}
        int w,h;

        void print(std::vector<std::vector<Vec3f>> image) const
        {
            std::ofstream file;
            file.open("output.ppm");

            file << "P3\n" << w << ' ' << h << "\n255\n";

            for (int j = 0; j < h; j++)
            {
                for(int i = 0; i < w; i++)
                {
                    file << image[i][j].x << ' ' << image[i][j].y << ' ' << image[i][j].z << '\n';
                }
            }
            
            file.close();
        }
};