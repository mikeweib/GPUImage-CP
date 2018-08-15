//
// Created by Mike Pro on 2018/8/14.
//

#ifndef GPUIMAGE_CP_GPUIMAGEFRAMEBUFFER_H
#define GPUIMAGE_CP_GPUIMAGEFRAMEBUFFER_H

#include "GPCP-Common.h"

NS_GPCP_BEGIN

    typedef struct GPUTextureOptions {
        GLenum minFilter;
        GLenum magFilter;
        GLenum wrapS;
        GLenum wrapT;
        GLenum internalFormat;
        GLenum format;
        GLenum type;
    } GPUTextureOptions;

    class GPUImageFramebufferCache;

    class GPUImageFramebuffer {
        friend class GPUImageFramebufferCache;

    public:
        GPUImageFramebuffer();

        virtual ~GPUImageFramebuffer();

        static GPUImageFramebuffer *create(float w, float h);

        static GPUImageFramebuffer *create(float w, float h, GPUTextureOptions fboTextureOptions, bool onlyGenerateTexture);

        static GPUImageFramebuffer *create(float w, float h, GLuint inputTexture);

        // Usage
        void activateFramebuffer();

        // Reference counting
        void lock();

        void unlock();

        void clearAllLocks();

        void disableReferenceCounting();

        void enableReferenceCounting();

        // Image capture
#if TARGET_OS_IPHONE
        CGImageRef newCGImageFromFramebufferContents();
#else
#endif

        void restoreRenderTarget();

        // Raw data bytes
        void lockForReading();

        void unlockAfterReading();

        int bytesPerRow();

        GLubyte *byteBuffer();

    private:
        float _width, _height;
        GPUTextureOptions _textureOptions;
        GLuint _texture;
        bool _missingFramebuffer;

        GLuint _framebuffer;
#if TARGET_OS_IPHONE
        CVPixelBufferRef _renderTarget;
        CVOpenGLESTextureRef _renderTexture;
        NSUInteger _readLockCount;
#else
#endif
        unsigned int _framebufferReferenceCount;
        bool _referenceCountingDisabled;

        void initWithSize(float w, float h);

        void initWithSize(float w, float h, GPUTextureOptions fboTextureOptions, bool onlyGenerateTexture);

        void initWithSize(float w, float h, GLuint inputTexture);

        void generateFramebuffer();

        void generateTexture();

        void destroyFramebuffer();
    };

NS_GPCP_END

#endif //GPUIMAGE_CP_GPUIMAGEFRAMEBUFFER_H
