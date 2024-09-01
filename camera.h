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
        float deviceAspectRatio = imageWidth / imageHeight;

        void rescaleAspectRatio()
        {
            float xS = 1;
            float yS = 1;

            if(filmAspectRatio > deviceAspectRatio)
            {
                xS = deviceAspectRatio / filmAspectRatio;
            } 
            else 
            {
                yS = filmAspectRatio / deviceAspectRatio;
            }

            right *= xS;
            left = -right;
            top *= yS;
            bottom = -top;
        }

        /// @brief Projection to screenspace
        /// @param pWorld point in world space
        /// @return point in screen space
        Vec3f worldToScreen( const Vec3f &pWorld) const
        {
            //worldspace to cameraSpace
            Matrix4f worldToCamera;
            Vec3f pCamera;
            worldToCamera.multVecMatrix(pWorld, pCamera);

            //projection
            Vec3f pScreen;
            pScreen.x = nearClippingPlane * pCamera.x / -pCamera.z;
            pScreen.y = nearClippingPlane * pCamera.y / -pCamera.z;

            //[-1,1]
            Vec3f pNDC;
            pNDC.x = 2 * pScreen.x / (right - left) - (right + left) / (right - left);
            pNDC.y = 2 * pScreen.y / (top - bottom) - (top + bottom) / (top - bottom);

            Vec3f pRaster;
            pRaster.x = (pNDC.x + 1) / 2 * imageWidth;
            //flip from screen space(origin bottom left) to raster space(origin top left)
            pRaster.y = (1 - pNDC.y) / 2 * imageHeight;
            pRaster.z = -pCamera.z;

            return pRaster;
        }

        bool isInView( const Vec2i pScreen ) const
        {
            return (pScreen.x >= left && pScreen.x <= right && pScreen.y >= bottom && pScreen.y <= top);
        }
};