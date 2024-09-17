#include <vec2.h>
#include <vec3.h>
#include <stdio.h>
#include <matrix.h>

class Camera{
    public:
        Camera(int w, int h): imageWidth(w), imageHeight(h)
        {
            setProjectionMatrix();
        }

        float inchToMm = 25.4;

        float focalLength = 35;
        float filmWidth = 0.98;
        float filmHeight = 0.735;
        float nearClippingPlane = 0.1;
        float farClippingPlane = 100;
        float filmAspectRatio = filmWidth / filmHeight;

        int imageWidth, imageHeight;
        float deviceAspectRatio = imageWidth / imageHeight;
        float overscanScale = deviceAspectRatio / filmAspectRatio;
        float top = ((filmWidth * inchToMm / 2) / focalLength) * nearClippingPlane;
        float bottom = -top;
        float right = top * filmAspectRatio * overscanScale;
        float left = -right;

        Matrix4f worldToCameraM;
        Matrix4f projectionMatrix;

        void setProjectionMatrix()
        {
            projectionMatrix[0][0] = 2 * nearClippingPlane / (right - left);
            projectionMatrix[0][1] = 0;
            projectionMatrix[0][2] = 0;
            projectionMatrix[0][3] = 0;
            
            projectionMatrix[1][0] = 0;
            projectionMatrix[1][1] = 2 * nearClippingPlane / (top - bottom);
            projectionMatrix[1][2] = 0;
            projectionMatrix[1][3] = 0;
            
            projectionMatrix[2][0] = 0;
            projectionMatrix[2][1] = 0;
            projectionMatrix[2][2] = -((farClippingPlane + nearClippingPlane) / (farClippingPlane - nearClippingPlane));
            projectionMatrix[2][3] = -1.f;
            
            projectionMatrix[3][0] = 0;
            projectionMatrix[3][1] = 0;
            projectionMatrix[3][2] = -((2.f * farClippingPlane * nearClippingPlane) / (farClippingPlane - nearClippingPlane));
            projectionMatrix[3][3] = 0;
        }

        /// @brief Projection to camera space
        /// @param pWorld point in world space
        /// @return point in camera space
        Vec3f worldToCamera(const Vec3f &pWorld) const
        {
            Vec3f pCamera;
            worldToCameraM.multVecMatrix(pWorld, pCamera);
            return pCamera;
        }

        /// @brief Projection to NDC via projection matrix
        /// @param pWorld point in world space
        /// @return NDC point
        Vec3f worldToNDC(const Vec3f &pWorld) const
        {
            //worldspace to cameraSpace
            Vec3f pCamera = worldToCamera(pWorld);
            Vec3f pProj;
            projectionMatrix.multVecMatrix(pCamera, pProj);

            return pProj;
        }

        /// @brief Projection to raster space
        /// @param pWorld point in world space
        /// @return point int raster space
        Vec3f worldToScreen(const Vec3f &pWorld) const
        {
            Vec3f pNDC = worldToNDC(pWorld);

            Vec3f pRaster;
            pRaster.x = (pNDC.x + 1) / 2 * imageWidth;
            //flip from screen space(origin bottom left) to raster space(origin top left)
            pRaster.y = (1 - pNDC.y) / 2 * imageHeight;
            pRaster.z = pNDC.z;

            return pRaster;
        }
};