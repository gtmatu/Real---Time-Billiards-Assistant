/*  video_demo.h    --  ZYBO Video demonstration                        */

#ifndef VIDEO_DEMO_H_
#define VIDEO_DEMO_H_
#ifdef __cplusplus
extern "C" {
#endif
    /* ------------------------------------------------------------ */
    /*              Include File Definitions                        */
    /* ------------------------------------------------------------ */

#include "xil_types.h"

    /* ------------------------------------------------------------ */
    /*                  Miscellaneous Declarations                  */
    /* ------------------------------------------------------------ */

#define DEMO_PATTERN_0 0
#define DEMO_PATTERN_1 1

#define DEMO_MAX_FRAME (1920*1080*2)
#define DEMO_STRIDE (1920 * 2)
#define DEMO_HEIGHT (1080)
#define DEMO_WIDTH (1920)
#define DEMO_PIXELS (1920 * 1080)
    /*#define BALL_MIN_WIDTH (1920 * 0.0156)
     #define BALL_MAX_WIDTH (1920 * 0.018)
     #define CUE_MIN_WIDTH (1920 * 0.003)
     #define CUE_MAX_WIDTH (1920 * 0.011)*/

    /*
     * Configure the Video capture driver to start streaming on signal
     * detection
     */
#define DEMO_START_ON_DET 1

    /* ------------------------------------------------------------ */
    /*                  Procedure Declarations                      */
    /* ------------------------------------------------------------ */

    void DemoInitialize();
    void DemoRun();
    void DemoPrintMenu();
    void DemoChangeRes();
    void DemoCRMenu();
    void DemoStream();

    //void DemoInvertStream(u8 fHw);
    //void DemoInvertFrameSw(u16 *srcFrame, u16 *destFrame);
    //#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
    //#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
    //void DemoInvertFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]);

    //void DemoGrayStream(u8 fHw);
    void DemoGrayFrameSw(u16 *srcFrame, u16 *destFrame);
    //#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
    //#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
    //void DemoGrayFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]);

    //void DemoSobelStream(u8 fHw);
    void DemoSobelFrameSw(u16 *srcFrame, u16 *destFrame);
#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
    void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp);
    void ColourSw(u16 *srcFrame, u16 *destFrame);
    void CalibrateSw(u16 *srcFrame, u16 *destFrame, u16 *cornersYcoiTop, u16 *cornersYcoiBottom, u16 *cornersXcoiLeft, u16 *cornersXcoiRight);
    void DrawSw(u16 *srcFrame, u16 *destFrame, u16 *cornersYcoiTop, u16 *cornersYcoiBottom, u16 *cornersXcoiLeft, u16 *cornersXcoiRight);
    void BallHw(u16 srcFrame2[DEMO_PIXELS], u16 cornersYcoiTop, u16 cornersYcoiBottom, u16 cornersXcoiLeft, u16 cornersXcoiRight, u16 *sumX, u16 *sumY, u16 *i, u16 minWidth, u16 maxWidth);
    void BallSw(u16 srcFrame[DEMO_PIXELS], u16 *ballXcoi, u16 *ballYcoi, u16 ballX[255], u16 ballY[255]);
    void CueHw(u16 srcFrame[DEMO_PIXELS], u16 ballXcoi, u16 ballYcoi, u16 *sumX, u16 *sumY, u16 *i, u16 minWidth, u16 maxWidth, u32 ycoiLower, u32 ycoiUpper, u32 xcoiLower, u32 xcoiUpper);
    void CueSw(u16 srcFrame[DEMO_PIXELS], u16 *cueXcoi, u16 *cueYcoi, u16 cueX[255], u16 cueY[255]);
    void gradientAndIntercpetsSw(u8 *yIntercept2Init, u8 *yIntercept3Init, float *gradient, float *gradient2, float *yIntercept, float *yIntercept2, float *yIntercept3, u16 cornersYcoiTop, u16 cornersYcoiBottom, u16 cornersXcoiLeft, u16 cornersXcoiRight, u16 ballXcoi, u16 ballYcoi, u16 cueXcoi, u16 cueYcoi, u32 Track);
    void DrawTrackingHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u8 yIntercept2Init, u8 yIntercept3Init, float gradient, float gradient2, float yIntercept, float yIntercept2, float yIntercept3, u16 cornersYcoiTop, u16 cornersYcoiBottom, u16 cornersXcoiLeft, u16 cornersXcoiRight, u16 ballXcoi, u16 ballYcoi, u16 cueXcoi, u16 cueYcoi, u32 Track, u16 BALL_MAX_WIDTH, u16 CUE_MAX_WIDTH);


    //void DemoScaleStream(u8 fHw);
    //void DemoScaleFrameSw(u16 *srcFrame, u16 *destFrame, u32 srcWidth, u32 srcHeight, u32 destWidth, u32 destHeight, u32 stride);

    void DemoISR(void *callBackRef, void *pVideo);

    //void DemoPrintTest(u16 *frame, u32 width, u32 height, u32 stride, int pattern);
    /* ------------------------------------------------------------ */

    /************************************************************************/

#ifdef __cplusplus
};
#endif
#endif /* VIDEO_DEMO_H_ */
