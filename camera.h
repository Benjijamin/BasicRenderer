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

        
        Vec3f worldToCanvas( const Vec3f &pWorld ) const
        {
            //worldspace to cameraSpace
            Matrix4f worldToCamera;
            Vec3f pCamera;
            worldToCamera.multVecMatrix(pWorld, pCamera);

            //projection
            Vec3f pCanvas;
            pCanvas.x = pCamera.x / -pCamera.z * nearClippingPlane;
            pCanvas.y = pCamera.y / -pCamera.z * nearClippingPlane;
            pCanvas.z = -pCamera.z;

            return pCanvas;
        }

        /// @brief Projection to screenspace before converting to exact pixel
        /// @param pWorld point in world space
        /// @return point in screen space
        Vec3f worldToScreen( const Vec3f &pWorld) const
        {
            Vec3f pCanvas = worldToCanvas(pWorld);

            Vec3f pNormalized;
            pNormalized.x = 2 * pCanvas.x / (right - left) - (right + left) / (right - left);
            pNormalized.y = 2 * pCanvas.y / (top - bottom) - (top + bottom) / (top - bottom);
            pNormalized.z = pCanvas.z;

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