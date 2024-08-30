class Camera{
    public:
        Camera(){}

        float inchToMm = 25.4;

        float focalLength = 35;
        float filmApertureWidth = 0.98;
        float filmApertureHeight = 0.735;
        float nearClippingPlane = 0.1;
        float farClippingPlane = 1000;

        //screen
        float top = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPlane;
        float bottom = -top;
        float filmAspectRatio = filmApertureWidth / filmApertureHeight;
        float right = top * filmAspectRatio;
        float left = -right;

        //resolution
        int imageWidth = 512, imageHeight = 512;

        
        Vec3f worldToNDC( const Vec3f &pWorld ) const
        {
            //worldspace to cameraSpace
            Matrix4f worldToCamera;
            Vec3f pCamera;
            worldToCamera.multVecMatrix(pWorld, pCamera);

            //projection
            Vec3f pNDC;
            pNDC.x = pNDC.x / -pNDC.z * nearClippingPlane;
            pNDC.y = pNDC.y / -pNDC.z * nearClippingPlane;
            pNDC.z = -pNDC.z;

            return pNDC;
        }

        /// @brief Projection to screenspace before converting to exact pixel
        /// @param pWorld point in world space
        /// @return point in screen space
        Vec3f worldToScreen( const Vec3f &pWorld) const
        {
            Vec3f pNDC = worldToNDC(pWorld);

            //[-1,1]
            Vec3f pNormalized;
            pNormalized.x = 2 * pNDC.x / (right - left) - (right + left) / (right - left);
            pNormalized.y = 2 * pNDC.y / (top - bottom) - (top + bottom) / (top - bottom);
            pNormalized.z = pNDC.z;

            Vec3f pScreen;
            pScreen.x = pNormalized.x * imageWidth;
            //flip from screen space(origin bottom left) to raster space(origin top left)
            pScreen.y = (1 - pNormalized.y) * imageHeight;
            pScreen.z = pNormalized.z;

            return pScreen;
        }

        Vec3i worldToPixel( const Vec3f &pWorld ) const
        {
            Vec3f pScreen = worldToScreen(pWorld);

            Vec3i pRaster;
            pRaster.x = (int)pScreen.x;
            pRaster.y = (int)pScreen.y;
            pRaster.z = pScreen.z;

            return pRaster;
        }


        bool isInView( const Vec2i pScreen ) const
        {
            return (pScreen.x >= left && pScreen.x <= right && pScreen.y >= bottom && pScreen.y <= top);
        }
};