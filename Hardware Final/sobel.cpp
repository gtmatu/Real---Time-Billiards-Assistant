/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2011 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/*
 * Sobel demo code
 */

#include <stdio.h>
#include "ap_video.h"
#include "video_demo.h"
#include "xil_cache.h"
#include "xil_io.h"
#define ABS(x)          ((x>0)? x : -x)

void RgbToHsv(u8 r, u8 b, u8 g, u8& h, u8& s, u8& v)
{
    u8 rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    v = rgbMax;

    if (v == 0)
    {
        h = 0;
        s = 0;
        return;
    }

    s = 255 * long(rgbMax - rgbMin) / v;
    if (s == 0)
    {
        h = 0;
        return;
    }

    if (rgbMax == r)
        h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        h = 171 + 43 * (r - g) / (rgbMax - rgbMin);
}

void CalibrateSw(u16 *srcFrame, u16 *destFrame, u16 *cornersYcoiTop, u16 *cornersYcoiBottom, u16 *cornersXcoiLeft, u16 *cornersXcoiRight)
{
    u16 xcoi, ycoi;
    u16 maxSumTop = 0;
    u16 maxSumBottom = 0;
    u16 maxSumYcoiTop = 0;
    u16 maxSumYcoiBottom = 0;
    u16 maxSumLeft = 0;
    u16 maxSumRight = 0;
    u16 maxSumXcoiLeft = 0;
    u16 maxSumXcoiRight = 0;

    for(ycoi = 0; ycoi < DEMO_HEIGHT * 0.4; ycoi++)
    {
        u16 sumTop = 0;
        u16 sumBottom = 0;
        for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++)
        {
            u8 r, g, b, h, s, v;
            u16 pxlInTop, pxlInBottom;
            pxlInTop = srcFrame[xcoi + ycoi * DEMO_WIDTH];
            pxlInBottom = srcFrame[xcoi + (DEMO_HEIGHT-ycoi) * DEMO_WIDTH];
            r = ((pxlInTop & 0xF800) >> (11-3));
            b = ((pxlInTop & 0x07C0) >> (6-3));
            g = ((pxlInTop & 0x003F) << 2);

            RgbToHsv(r, b, g, h, s, v);

            bool colorRange = (90 < r && r < 125 && 35 < b && b < 75 && 60 < g && g < 105);
            if (colorRange) {
                sumTop++;
            }
            r = ((pxlInBottom & 0xF800) >> (11-3));
            b = ((pxlInBottom & 0x07C0) >> (6-3));
            g = ((pxlInBottom & 0x003F) << 2);
            colorRange = (90 < r && r < 125 && 35 < b && b < 75 && 60 < g && g < 105);
            if(colorRange){
                sumBottom++;
            }
        }
        if (sumTop > maxSumTop) {
            maxSumTop = sumTop;
            maxSumYcoiTop = ycoi;
        }
        if (sumBottom > maxSumBottom) {
            maxSumBottom = sumBottom;
            maxSumYcoiBottom = DEMO_HEIGHT - ycoi;
        }
    }

    for(xcoi = 0; xcoi < DEMO_WIDTH * 0.4; xcoi++)
    {
        u16 sumLeft = 0;
        u16 sumRight = 0;
        for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
        {
            u16 r, g, b;
            u16 pxlInLeft, pxlInRight;
            pxlInLeft = srcFrame[xcoi + ycoi * DEMO_WIDTH];
            pxlInRight = srcFrame[(DEMO_WIDTH - xcoi) + ycoi * DEMO_WIDTH];
            r = ((pxlInLeft & 0xF800) >> (11-3));
            b = ((pxlInLeft & 0x07C0) >> (6-3));
            g = ((pxlInLeft & 0x003F) << 2);
            bool colorRange = (90 < r && r < 125 && 35 < b && b < 75 && 60 < g && g < 105);
            if (colorRange) {
                sumLeft++;
            }
            r = ((pxlInRight & 0xF800) >> (11-3));
            b = ((pxlInRight & 0x07C0) >> (6-3));
            g = ((pxlInRight & 0x003F) << 2);
            colorRange = (90 < r && r < 125 && 35 < b && b < 75 && 60 < g && g < 105);
            if(colorRange){
                sumRight++;
            }
        }
        if (sumLeft > maxSumLeft) {
            maxSumLeft = sumLeft;
            maxSumXcoiLeft = xcoi;
        }
        if (sumRight > maxSumRight) {
            maxSumRight = sumRight;
            maxSumXcoiRight = DEMO_WIDTH - xcoi;
        }
    }

    *cornersYcoiTop= maxSumYcoiTop;
    *cornersYcoiBottom = maxSumYcoiBottom;
    *cornersXcoiLeft = maxSumXcoiLeft;
    *cornersXcoiRight = maxSumXcoiRight;



    // Flush the framebuffer memory range to ensure changes are written to the
    // actual memory, and therefore accessible by the VDMA.

    //Xil_DCacheFlushRange((unsigned int) destFrame, DEMO_MAX_FRAME);
}

void DrawSw(u16 *srcFrame, u16 *destFrame, u16 *cornersYcoiTop, u16 *cornersYcoiBottom, u16 *cornersXcoiLeft, u16 *cornersXcoiRight)
{
    u16 xcoi, ycoi;
    for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
    {
        for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++) {
            u16 pxlIn;
            pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
            if (ycoi == *cornersYcoiTop) {
                destFrame[(xcoi) + *cornersYcoiTop * DEMO_WIDTH] = 0xFFFF;
            } else if (ycoi == *cornersYcoiBottom){
                destFrame[(xcoi) + *cornersYcoiBottom * DEMO_WIDTH] = 0xFFFF;
            } else if (xcoi == *cornersXcoiLeft) {
                destFrame[(*cornersXcoiLeft) + ycoi * DEMO_WIDTH] = 0xFFFF;
            } else if (xcoi == *cornersXcoiRight){
                destFrame[(*cornersXcoiRight) + ycoi * DEMO_WIDTH] = 0xFFFF;
            } else {
                destFrame[(xcoi) + ycoi * DEMO_WIDTH] = pxlIn;
            }
        }
    }
    //Xil_DCacheFlushRange((unsigned int) destFrame, DEMO_MAX_FRAME);
}


void BallHw(u16 srcFrame2[DEMO_PIXELS], u16 cornersYcoiTop, u16 cornersYcoiBottom, u16 cornersXcoiLeft, u16 cornersXcoiRight, u16 *sumX, u16 *sumY, u16 *i, u16 minWidth, u16 maxWidth)
{
#pragma HLS INTERFACE ap_fifo port=srcFrame2
    u16 xcoi, ycoi;
    u16 sum = 0;
    bool withinRange = false;
    u16 icopy = 0;
    u16 sumXcopy = 0;
    u16 sumYcopy = 0;

    for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
    {
        for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++)
        {
#pragma HLS PIPELINE //II = 3
            u16 pxlIn;
            pxlIn = srcFrame2[xcoi + ycoi * DEMO_WIDTH];
            if (ycoi > cornersYcoiTop && ycoi < cornersYcoiBottom && xcoi > cornersXcoiLeft && xcoi < cornersXcoiRight) {
                u8 r, g, b;
                r = ((pxlIn & 0xF800) >> (11-3));
                b = ((pxlIn & 0x07C0) >> (6-3));
                g = ((pxlIn & 0x003F) << 2);

                bool colorRange = (r > 190 && g > 200 && b > 185);
                if (colorRange && sum < maxWidth) {
                    sum++;
                }

                if (!colorRange) {
                    if (withinRange) {
                        withinRange = false;
                        sumXcopy = sumXcopy + (xcoi - (sum >> 1));
                        sumYcopy = sumYcopy + ycoi;
                        icopy++;
                    }
                    sum = 0;
                }

                if (sum == minWidth) {
                    withinRange = true;
                }
                if (sum == maxWidth) {
                    withinRange = false;
                }
            }
        }
    }
    *sumX = sumXcopy;
    *sumY = sumYcopy;
    *i = icopy;
}

void CueHw(u16 srcFrame[DEMO_PIXELS], u16 ballXcoi, u16 ballYcoi, u16 *sumX, u16 *sumY, u16 *i, u16 minWidth, u16 maxWidth, u32 ycoiLower, u32 ycoiUpper, u32 xcoiLower, u32 xcoiUpper)
{
#pragma HLS INTERFACE ap_fifo port=srcFrame
    u16 xcoi, ycoi;
    u16 sum = 0;
    u16 sumXcopy = 0;
    u16 sumYcopy = 0;
    u16 icopy = 0;
    ;
    bool withinRange = false;

    for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
    {
        for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++)
        {
#pragma HLS PIPELINE
            u16 pxlIn;
            pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
             if (ycoi > ycoiLower && ycoi < ycoiUpper && xcoi > xcoiLower && xcoi < xcoiUpper) {

                u8 r, g, b, h, s, v;
                r = ((pxlIn & 0xF800) >> (11-3));
                b = ((pxlIn & 0x07C0) >> (6-3));
                g = ((pxlIn & 0x003F) << 2);

                RgbToHsv(r, b, g, h, s, v);

                bool colorRange = (r > 150 && r < 215 && g > 115 && g < 150 && b > 80 && b < 135);
                if (colorRange && sum < maxWidth) {
                    sum++;
                }

                if (!colorRange) {
                    if (withinRange) {
                        withinRange = false;
                        sumXcopy = sumXcopy + (xcoi - (sum >> 1));
                        sumYcopy = sumYcopy + ycoi;
                        icopy++;
                    }
                    sum = 0;
                }

                if (sum == minWidth) {
                    withinRange = true;
                }
                if (sum == maxWidth) {
                    withinRange = false;
                }
            }
        }
    }
    *sumX = sumXcopy;
    *sumY = sumYcopy;
    *i = icopy;
}

void gradientAndIntercpetsSw(u8 *yIntercept2Init, u8 *yIntercept3Init, float *gradient, float *gradient2, float *yIntercept, float *yIntercept2, float *yIntercept3, u16 cornersYcoiTop, u16 cornersYcoiBottom, u16 cornersXcoiLeft, u16 cornersXcoiRight, u16 ballXcoi, u16 ballYcoi, u16 cueXcoi, u16 cueYcoi, u32 Track)
{

    *gradient = (ballYcoi - cueYcoi)*1.0f / (ballXcoi - cueXcoi);
    *gradient2 = -1 * (*gradient);
    *yIntercept = (ballYcoi - (*gradient) * ballXcoi);

    u8 cueLeft = cueXcoi < ballXcoi;
    float reflectionXcoi, reflectionYcoi;
    u8 bounce1Within = false;
    *yIntercept2Init = false;
    *yIntercept3Init = false;

    if (Track > 4) {
        *yIntercept2Init = true;
        if (cueLeft) {
            float ycoiRightBoundary = (*gradient) * cornersXcoiRight + (*yIntercept);
            if (ycoiRightBoundary < cornersYcoiTop) {
                reflectionXcoi = (cornersYcoiTop - (*yIntercept)) / (*gradient);
                *yIntercept2 = cornersYcoiTop + (*gradient) * reflectionXcoi;
            } else if (ycoiRightBoundary > cornersYcoiBottom) {
                reflectionXcoi = (cornersYcoiBottom - (*yIntercept)) / (*gradient);
                *yIntercept2 = cornersYcoiBottom + (*gradient) * reflectionXcoi;
            } else {
                bounce1Within = true;
                reflectionYcoi = (*gradient) * cornersXcoiRight + (*yIntercept);
                *yIntercept2 = reflectionYcoi + (*gradient) * cornersXcoiRight;
            }
        } else {
            float ycoiLeftBoundary = (*gradient) * cornersXcoiLeft + (*yIntercept);
            if (ycoiLeftBoundary < cornersYcoiTop) {
                reflectionXcoi = (cornersYcoiTop - (*yIntercept)) / (*gradient);
                *yIntercept2 = cornersYcoiTop + (*gradient) * reflectionXcoi;
            } else if (ycoiLeftBoundary > cornersYcoiBottom) {
                reflectionXcoi = (cornersYcoiBottom - (*yIntercept)) / (*gradient);
                *yIntercept2 = cornersYcoiBottom + (*gradient) * reflectionXcoi;
            } else {
                bounce1Within = true;
                reflectionYcoi = (*gradient) * cornersXcoiLeft + (*yIntercept);
                *yIntercept2 = reflectionYcoi + (*gradient) * cornersXcoiLeft;
            }
        }
        if (Track > 8) {
            *yIntercept3Init = true;
            if ((cueLeft && bounce1Within) || (!cueLeft && !bounce1Within)) {
                float ycoiLeftBoundary = (*gradient2) * cornersXcoiLeft + (*yIntercept2);
                if (ycoiLeftBoundary < cornersYcoiTop) {
                    reflectionXcoi = (cornersYcoiTop - (*yIntercept2)) / (*gradient2);
                    *yIntercept3 = cornersYcoiTop + (*gradient2) * reflectionXcoi;
                } else if (ycoiLeftBoundary > cornersYcoiBottom) {
                    reflectionXcoi = (cornersYcoiBottom - (*yIntercept2)) / (*gradient2);
                    *yIntercept3 = cornersYcoiBottom + (*gradient2) * reflectionXcoi;
                } else {
                    reflectionYcoi = (*gradient2) * cornersXcoiLeft + (*yIntercept2);
                    *yIntercept3 = reflectionYcoi + (*gradient2) * cornersXcoiLeft;
                }
            } else {
                float ycoiRightBoundary = (*gradient2) * cornersXcoiRight + (*yIntercept2);
                if (ycoiRightBoundary < cornersYcoiTop) {
                    reflectionXcoi = (cornersYcoiTop - (*yIntercept2)) / (*gradient2);
                    *yIntercept3 = cornersYcoiTop + (*gradient2) * reflectionXcoi;
                } else if (ycoiRightBoundary > cornersYcoiBottom) {
                    reflectionXcoi = (cornersYcoiBottom - (*yIntercept2)) / (*gradient2);
                    *yIntercept3 = cornersYcoiBottom + (*gradient2) * reflectionXcoi;
                } else {
                    reflectionYcoi = (*gradient2) * cornersXcoiRight + (*yIntercept2);
                    *yIntercept3 = reflectionYcoi + (*gradient2) * cornersXcoiRight;
                }
            }
        }
    }
}

#pragma SDS data access_pattern(srcFrame:SEQUENTIAL)
#pragma SDS data access_pattern(destFrame:SEQUENTIAL)

void DrawTrackingHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u8 yIntercept2Init, u8 yIntercept3Init, float gradient, float gradient2, float yIntercept, float yIntercept2, float yIntercept3, u16 cornersYcoiTop, u16 cornersYcoiBottom, u16 cornersXcoiLeft, u16 cornersXcoiRight, u16 ballXcoi, u16 ballYcoi, u16 cueXcoi, u16 cueYcoi, u32 Track, u16 BALL_MAX_WIDTH, u16 CUE_MAX_WIDTH)
{
    u16 xcoi, ycoi;
    for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
    {
        for (xcoi = 0; xcoi < DEMO_WIDTH; xcoi++) {
#pragma HLS PIPELINE
            bool line = false, line2 = false, line3 = false;
            if (Track > 2) {
                line = (((gradient*xcoi + yIntercept - ycoi) > -2) && ((gradient*xcoi + yIntercept - ycoi) < 2)) || (((gradient*(xcoi+2)+ yIntercept - ycoi) > 0) && ((gradient*(xcoi-2)+ yIntercept - ycoi) < 0));
            }
            if (yIntercept2Init) {
                line2 = (((gradient2*xcoi + yIntercept2 - ycoi) > -2) && ((gradient2*xcoi + yIntercept2 - ycoi) < 2)) || (((gradient2*(xcoi+2)+ yIntercept2 - ycoi) > 0) && ((gradient2*(xcoi-2)+ yIntercept2 - ycoi) < 0));
            }
            if (yIntercept3Init) {
                line3 = (((gradient*xcoi + yIntercept3 - ycoi) > -2) && ((gradient*xcoi + yIntercept3 - ycoi) < 2)) || (((gradient*(xcoi+2)+ yIntercept3 - ycoi) > 0) && ((gradient*(xcoi-2)+ yIntercept3 - ycoi) < 0));
            }

            bool withinTable = cornersYcoiTop < ycoi && cornersYcoiBottom > ycoi && cornersXcoiLeft < xcoi && cornersXcoiRight > xcoi;
            bool ballu8 = ((ycoi == (ballYcoi + BALL_MAX_WIDTH) ||  ycoi == (ballYcoi - BALL_MAX_WIDTH))
                           &&  xcoi > (ballXcoi - BALL_MAX_WIDTH) &&  xcoi < (ballXcoi + BALL_MAX_WIDTH)) ||
            (ycoi < (ballYcoi + BALL_MAX_WIDTH) &&  ycoi > (ballYcoi - BALL_MAX_WIDTH)
             &&  (xcoi == (ballXcoi - BALL_MAX_WIDTH) ||  xcoi == (ballXcoi + BALL_MAX_WIDTH)));

            bool cueu8 = ((ycoi == (cueYcoi + CUE_MAX_WIDTH) ||  ycoi == (cueYcoi - CUE_MAX_WIDTH))
                          &&  xcoi > (cueXcoi - CUE_MAX_WIDTH) &&  xcoi < (cueXcoi + CUE_MAX_WIDTH)) ||
            (ycoi < (cueYcoi + CUE_MAX_WIDTH) &&  ycoi > (cueYcoi - CUE_MAX_WIDTH)
             &&  (xcoi == (cueXcoi - CUE_MAX_WIDTH) ||  xcoi == (cueXcoi + CUE_MAX_WIDTH)));

            u16 pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
            if ((line && (ballYcoi != 0) && (cueXcoi != 0) && withinTable) &&
                ((cueXcoi < ballXcoi && xcoi > cueXcoi) ||
                 (cueXcoi > ballXcoi && xcoi < cueXcoi) ||
                 ((cueXcoi == ballXcoi) && (cueYcoi < ballYcoi) && (ycoi > cueYcoi)) ||
                 (cueXcoi == ballXcoi && cueYcoi > ballYcoi && ycoi < cueYcoi))) {
                destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
            } else {
                destFrame[(xcoi) + ycoi * DEMO_WIDTH] = pxlIn;
            }
            if (ballu8 || cueu8) {
                destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0xEFE4;
            }
            if ((line2 || line3) && (ballYcoi != 0) && (cueXcoi != 0) && withinTable) {
                destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
            }
        }
    }
}
