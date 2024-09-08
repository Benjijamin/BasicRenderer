#ifndef MATRIX_H
#define MATRIX_H

template<typename T>
class Matrix44
{
    public:
        Matrix44() {}
        Matrix44(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p)
        {
            x[0][0] = a; x[0][1] = b; x[0][2] = c; x[0][3] = d;
            x[1][0] = e; x[1][1] = f; x[1][2] = g; x[1][3] = h;
            x[2][0] = i; x[2][1] = j; x[2][2] = k; x[2][3] = l;
            x[3][0] = m; x[3][1] = n; x[3][2] = o; x[3][3] = p;
        }

        void multVecMatrix(const Vec3<T> &src, Vec3<T> &dst) const
        {
            dst.x = src.x*x[0][0] + src.y*x[1][0] + src.z*x[2][0] + x[3][0];
            dst.y = src.x*x[0][1] + src.y*x[1][1] + src.z*x[2][1] + x[3][1];
            dst.z = src.x*x[0][2] + src.y*x[1][2] + src.z*x[2][2] + x[3][2];
            T w = src.x*x[0][3] + src.y*x[1][3] + src.z*x[2][3] + x[3][3];
            if(w != 1 && w != 0) 
            {
                dst.x /= w;
                dst.y /= w;
                dst.z /= w;
            }
        }

        void multDirMatrix(const Vec3<T> &src, Vec3<T> &dst) const
        {
            dst.x = src.x*x[0][0] + src.y*x[1][0] + src.z*x[2][0];
            dst.y = src.x*x[0][1] + src.y*x[1][1] + src.z*x[2][1];
            dst.z = src.x*x[0][2] + src.y*x[1][2] + src.z*x[2][2];
        }

        Matrix44 inverse() const
        {
            int i, j, k;
            Matrix44 s;
            Matrix44 t (*this);

            // Forward elimination
            for (i = 0; i < 3 ; i++) 
            {
                int pivot = i;
            
                T pivotsize = t[i][i];
            
                if (pivotsize < 0) 
                {
                    pivotsize = -pivotsize;
                }

                for (j = i + 1; j < 4; j++) 
                {
                    T tmp = t[j][i];
                
                    if (tmp < 0)
                    {
                        tmp = -tmp;
                    }                        
                    
                    if (tmp > pivotsize) 
                    {
                        pivot = j;
                        pivotsize = tmp;
                    }
                }
            
                if (pivotsize == 0) 
                {
                    // Cannot invert singular matrix
                    return Matrix44();
                }
            
                if (pivot != i) 
                {
                    for (j = 0; j < 4; j++) 
                    {
                        T tmp;
                    
                        tmp = t[i][j];
                        t[i][j] = t[pivot][j];
                        t[pivot][j] = tmp;
                    
                        tmp = s[i][j];
                        s[i][j] = s[pivot][j];
                        s[pivot][j] = tmp;
                    }
                }
            
                for (j = i + 1; j < 4; j++) 
                {
                    T f = t[j][i] / t[i][i];
                
                    for (k = 0; k < 4; k++) 
                    {
                        t[j][k] -= f * t[i][k];
                        s[j][k] -= f * s[i][k];
                    }
                }
            }

            // Backward substitution
            for (i = 3; i >= 0; --i) 
            {
                T f;
            
                if ((f = t[i][i]) == 0) 
                {
                    // Cannot invert singular matrix
                    return Matrix44();
                }
            
                for (j = 0; j < 4; j++) 
                {
                    t[i][j] /= f;
                    s[i][j] /= f;
                }
            
                for (j = 0; j < i; j++) 
                {
                    f = t[j][i];
                
                    for (k = 0; k < 4; k++) 
                    {
                        t[j][k] -= f * t[i][k];
                        s[j][k] -= f * s[i][k];
                    }
                }
            }

            return s;
        }

        const Matrix44<T>& invert()
        {
            *this = inverse();
            return *this;
        }

        const T* operator[] (int i) const { return x[i]; }
        T* operator [] (int i) { return x[i]; }
        Matrix44 operator * (const Matrix44& m) const
        {
            Matrix44 mult;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    mult[i][j] = x[i][0] * m[0][j] + 
                                 x[i][1] * m[1][j] + 
                                 x[i][2] * m[2][j] + 
                                 x[i][3] * m[3][j];
                }
            }
            return mult;
        }
    private:
        T x[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
};

typedef Matrix44<float> Matrix4f;

#endif