//
// Created by Mike Pro on 2018/8/14.
//

#ifndef GPUIMAGE_CP_GPUIMAGECONTEXT_H
#define GPUIMAGE_CP_GPUIMAGECONTEXT_H

#include "GPCP-Common.h"

NS_GPCP_BEGIN


    class GLProgram;
    class GPUImageFramebufferCache;

    class GPUImageContext {
    public:
        GPUImageContext();

        virtual ~GPUImageContext();

        static void useImageProcessingContext();

        static GPUImageContext *sharedImageProcessingContext();

        static GPUImageFramebufferCache *sharedFramebufferCache();

        static bool supportsFastTextureUpload();

        static void setActiveShaderProgram(GLProgram *shaderProgram);

        static GLint maximumTextureSizeForThisDevice();
        static GLint maximumTextureUnitsForThisDevice();
        static GLint maximumVaryingVectorsForThisDevice();


        void useAsCurrentContext();
        void setContextShaderProgram(GLProgram *shaderProgram);


    private:
        GLProgram *_currentShaderProgram;

#if TARGET_OF_IOS
        dispatch_queue_t _contextQueue;
        EAGLContext _context;
        CVOpenGLESTextureCacheRef _coreVideoTextureCache;


        EAGLContext* getContext();
        EAGLContext* createContext();
#endif // TARGET_OF_IOS
    };

NS_GPCP_END

#endif //GPUIMAGE_CP_GPUIMAGECONTEXT_H
