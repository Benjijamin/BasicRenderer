#include <ppm.h>

void PpmWriter::print(const Vec3i *imageBuffer) const
        {
            std::ofstream file;
            file.open("output.ppm");

            file << "P3\n" << w << ' ' << h << "\n255\n";

            for (int j = 0; j < h; j++)
            {
                for(int i = 0; i < w; i++)
                {
                    file << imageBuffer[j * w + i].x << 
                    ' ' << imageBuffer[j * w + i].y << 
                    ' ' << imageBuffer[j * w + i].z << '\n';
                }
            }

            file.close();
        }