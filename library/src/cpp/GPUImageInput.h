//
// Created by Mike Pro on 2018/8/14.
//

#ifndef GPUIMAGE_CP_GPUIMAGEINPUT_H
#define GPUIMAGE_CP_GPUIMAGEINPUT_H

#include "GPCP-Common.h"

NS_GPCP_BEGIN

    typedef enum {
        kGPUImageNoRotation,
        kGPUImageRotateLeft,
        kGPUImageRotateRight,
        kGPUImageFlipVertical,
        kGPUImageFlipHorizonal,
        kGPUImageRotateRightFlipVertical,
        kGPUImageRotateRightFlipHorizontal,
        kGPUImageRotate180
    } GPUImageRotationMode;

    class GPUImageFramebuffer;

    class GPUImageInput {
    public:
        GPUImageInput() {}

        virtual ~GPUImageInput() {}
        virtual void newFrameReadyAtTime(int textureIndex) = 0;
        virtual void setInputFramebuffer(GPUImageFramebuffer *newInputFramebuffer, int textureIndex) = 0;
        virtual int nextAvailableTextureIndex() = 0;
        virtual void setInputSize(float w, float h, int textureIndex) = 0;
        virtual void setInputRotation(GPUImageRotationMode newInputRotation, int textureIndex) = 0;
        virtual void maximumOutputSize(float *w, float *h) = 0;
        virtual void endProcessing() = 0;
        virtual bool shouldIgnoreUpdatesToThisTarget() = 0;
        virtual bool enabled() = 0;
        virtual bool wantsMonochromeInput() = 0;
        virtual void setCurrentlyReceivingMonochromeInput(bool newValue) = 0;

    };

NS_GPCP_END

#endif //GPUIMAGE_CP_GPUIMAGEINPUT_H
