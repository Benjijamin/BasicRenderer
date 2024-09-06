#include "vec2.h"
#include "vec3.h"

class Camera{
    public:
        Camera(int w, int h): imageWidth(w), imageHeight(h)
        {
            setProjectionMatrix();
        }

        float nearClippingPlane = 0.1;
        float farClippingPlane = 100;
        float fov = 90;

        //resolution
        int imageWidth = 512, imageHeight = 512;
        float deviceAspectRatio = imageWidth / imageHeight;

        Matrix4f worldToCameraM;
        Matrix4f projectionMatrix;

        void setProjectionMatrix()
        {
            //Rescaling x and y coordinates according to fieldOfView, and rescaling z to [0(near),1(far)]
            float fovScale = 1 / tan(fov * 0.5 * M_PI / 180);
            float zRemapZ = -farClippingPlane / (farClippingPlane - nearClippingPlane);
            float zRemapW = -farClippingPlane * nearClippingPlane / (farClippingPlane - nearClippingPlane);
            projectionMatrix = Matrix4f( fovScale , 0, 0, 0,
                                         0, fovScale, 0, 0,
                                         0, 0, zRemapZ, -1,
                                         0, 0, zRemapW, 0);
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

            printf("p.x=%f, p.y=%f, p.z=%f\n", pProj.x, pProj.y, pProj.z);

            return pProj;
        }

        Vec3f worldToScreen(const Vec3f &pWorld) const
        {
            Vec3f pNDC = worldToNDC(pWorld);

            Vec3f pRaster;
            pRaster.x = (pNDC.x + 1) / 2 * imageWidth;
            //flip from screen space(origin bottom left) to raster space(origin top left)
            pRaster.y = (1 - pNDC.y) / 2 * imageHeight;
            pRaster.z = pNDC.z;

            printf("pR.x=%f, pR.y=%f, pR.z=%f\n", pRaster.x, pRaster.y, pRaster.z);

            return pRaster;
        }
};