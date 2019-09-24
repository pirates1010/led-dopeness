#include "KernelOperator.h"

KernelOperator::KernelOperator(byte * kernel, byte len){
    this->kernel = kernel;
    this->kernelLen = len;
    this->kernelBuffer = (CHSV*)malloc(len / 2 * sizeof(CHSV));
    this->kernelBufferLen = 0;
    this->kernelBufferStart = 0;
}

KernelOperator::~KernelOperator(){
    free(this->kernelBuffer);
}

void KernelOperator::addToKernelBuffer(CHSV color){
    int idx = kernelBufferStart + kernelBufferLen;
    if(idx >= kernelLen / 2){
        idx -= kernelLen / 2;
    }

    if(kernelBufferLen < kernelLen / 2){
        kernelBufferLen++;
    }
    else {
        kernelBufferStart++;
    }

    kernelBuffer[idx] = color;
}

CHSV KernelOperator::getFromKernelBuffer(int idx){
    int adjIdx = idx + kernelBufferStart;
    if(adjIdx >= kernelLen / 2){
        adjIdx -= kernelLen / 2;
    }
    return kernelBuffer[adjIdx];
}

void KernelOperator::operator()(CHSV * leds, short len){
    kernelBufferLen = 0; //reset kernel buffer
    kernelBufferStart = 0;
    // for each LED
    for(int i = kernelLen / 2; i < len - kernelLen / 2; i++){
        CHSV total;
        //sweep across nearby leds
        int k = 0; // keep track of kernel index seperately, trust me it's easier this way
        for(int j = -kernelLen / 2; j < kernelLen / 2; j++, k++){
            CHSV curLed;
            if(k < kernelBufferLen){
               curLed = getFromKernelBuffer(k);
            }
            else{
               curLed = leds[i+j];
            }
            total = blend(total, curLed, kernel[k]);
        }
        addToKernelBuffer(leds[i]);
        leds[i] = total;
    }
}