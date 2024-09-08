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

void readImage(const char *fileName, Vec3i *imageBuffer)
{
    FILE *file;
    int r,g,b, w,h, maxColor;
    char buffer[64];

    file = fopen(fileName, "r");

    if(file == NULL)
    {
        printf("Unable to open ppm file\n");
        fclose(file);
        exit(0);
    }
    else
    {
        rewind(file);

        buffer[0] = fgetc(file);
        buffer[1] = fgetc(file);

        if((buffer[0] != 'P') || (buffer[1] != '3'))
        {
            printf("not a P3 ppm file\n");
            exit(0);
        }

        while(isdigit(buffer[0] == false))
        {
            buffer[0] = fgetc(file);
        }

        ungetc(buffer[0], file);

        if(fscanf(file, "%d %d", w, h) != 2)
        {
            printf("cannot read correct width and height\n");
            exit(0);
        }

        if(fscanf(file, "%d", maxColor) != 1)
        {
            printf("cannot read correct color format\n");
            exit(0);
        }

        for(int j = 0; j < h; j++)
        {
            for(int i = 0; i < w; i++)
            {
                fscanf(file, "%d", r);
                fscanf(file, "%d", g);
                fscanf(file, "%d", b);

                imageBuffer[w * j + i] = Vec3i(r,g,b);
            }
        }

        fclose(file);
    }
}