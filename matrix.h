template<typename T>
class Matrix44
{
    public:
        //[00][01][02][03] x
        //[10][11][12][13] y
        //[20][21][22][23] z
        //[30][31][32][33] translate

        //scale
        //[x][0][0]
        //[0][x][0]
        //[0][0][x]

        Matrix44() {}
        Matrix44(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p)
        {
            m[0][0] = a; m[0][1] = b; m[0][2] = c; m[0][3] = d;
            m[1][0] = e; m[1][1] = f; m[1][2] = g; m[1][3] = h;
            m[2][0] = i; m[2][1] = j; m[2][2] = k; m[2][3] = l;
            m[3][0] = m; m[3][1] = n; m[3][2] = o; m[3][3] = p;
        }
        const T* operator [] (uint8_t i) const { return m[i]; }
        T* operator [] (uint8_t i) { return m[i]; }

        Matrix44 operator * (const Matrix44& mm) const
        {
            Matrix44 mult;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    mult[i][j] = m[i][0] * mm[0][j] + 
                                 m[i][1] * mm[1][j] + 
                                 m[i][2] * mm[2][j] + 
                                 m[i][3] * mm[3][j];
                }
            }
            return mult;
        }

        void multVecMatrix(const Vec3<T> &src, Vec3<T> &dst) const
        {
            dst.x = src.x*m[0][0] + src.y*m[1][0] + src.z*m[2][0] + m[3][0];
            dst.y = src.x*m[0][1] + src.y*m[1][1] + src.z*m[2][1] + m[3][1];
            dst.z = src.x*m[0][2] + src.y*m[1][2] + src.z*m[2][2] + m[3][2];
            T w = src.x*m[0][3] + src.y*m[1][3] + src.z*m[2][3] + m[3][3];
            if(w != 1 && w != 0) 
            {
                dst.x /= w;
                dst.y /= w;
                dst.z /= w;
            }
        }

        void multDirMatrix(const Vec3<T> &src, Vec3<T> &dst) const
        {
            dst.x = src.x*m[0][0] + src.y*m[1][0] + src.z*m[2][0];
            dst.y = src.x*m[0][1] + src.y*m[1][1] + src.z*m[2][1];
            dst.z = src.x*m[0][2] + src.y*m[1][2] + src.z*m[2][2];
        }

        //Black magic
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

        T m[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
};

typedef Matrix44<float> Matrix4f;