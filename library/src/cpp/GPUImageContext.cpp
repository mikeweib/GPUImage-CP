//
// Created by Mike Pro on 2018/8/14.
//

#include "GPUImageContext.h"
#include "GLProgram.h"
#include "GPUImageFramebufferCache.h"

USING_NS_GPCP;

NS_GPCP_BEGIN

    GPUImageContext::GPUImageContext() {

    }

    GPUImageContext::~GPUImageContext() {

    }

#pragma mark -
#pragma mark Static

    void GPUImageContext::useImageProcessingContext() {
        GPUImageContext::sharedImageProcessingContext()->useAsCurrentContext();
    }

    GPUImageFramebufferCache *GPUImageContext::sharedFramebufferCache() {
        static GPUImageFramebufferCache *sharedImageFramebufferCache = nullptr;
        if (sharedImageFramebufferCache == nullptr) {
            sharedImageFramebufferCache = new GPUImageFramebufferCache();
        }
        return sharedImageFramebufferCache;
    }

    GPUImageContext *GPUImageContext::sharedImageProcessingContext() {
        static GPUImageContext *sharedImageProcessingContext = nullptr;
        if (sharedImageProcessingContext == nullptr) {
            sharedImageProcessingContext = new GPUImageContext();
        }
        return sharedImageProcessingContext;
    }

    bool GPUImageContext::supportsFastTextureUpload() {
#if __ANDROID__
        return false;

#elif TARGET_OF_IOS
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
        return (CVOpenGLESTextureCacheCreate != NULL);
#pragma clang diagnostic pop

#else
        return false;
#endif
    }

    void GPUImageContext::setActiveShaderProgram(GLProgram *shaderProgram) {
        GPUImageContext::sharedImageProcessingContext()->setContextShaderProgram(shaderProgram);
    }

    GLint GPUImageContext::maximumTextureSizeForThisDevice() {
        static GLint maxTextureSize = -1;
        if (maxTextureSize == -1) {
            useImageProcessingContext();
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        }
        return maxTextureSize;
    }

    GLint GPUImageContext::maximumTextureUnitsForThisDevice() {
        static GLint maxTextureUnits = -1;
        if (maxTextureUnits == -1) {
            useImageProcessingContext();
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
        }
        return maxTextureUnits;
    }

    GLint GPUImageContext::maximumVaryingVectorsForThisDevice() {
        static GLint maxVaryingVectors = -1;
        if (maxVaryingVectors == -1) {
            useImageProcessingContext();
            glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVaryingVectors);
        }
        return maxVaryingVectors;
    }


#pragma mark -
#pragma mark Public

    void GPUImageContext::useAsCurrentContext() {
#if TARGET_OF_IOS
        EAGLContext *imageProcessingContext = [self context];
        if ([EAGLContext currentContext] != imageProcessingContext) {
            [EAGLContext setCurrentContext:imageProcessingContext];
        }
#endif
    }

    void GPUImageContext::setContextShaderProgram(GLProgram *shaderProgram) {
#if TARGET_OF_IOS
        useAsCurrentContext();
#endif

        if (_currentShaderProgram != shaderProgram) {
            _currentShaderProgram = shaderProgram;

            if (shaderProgram) {
                shaderProgram->use();
            }
        }
    }


#pragma mark -
#pragma mark Private

#if TARGET_OF_IOS
    EAGLContext* GPUImageContext::getContext() {
        if (_context == nil) {
            _context = createContext();
            [EAGLContext setCurrentContext:_context];

            // Set up a few global settings for the image processing pipeline
            glDisable(GL_DEPTH_TEST);
        }

        return _context;
    }
#endif

NS_GPCP_END