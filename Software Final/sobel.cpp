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

void CalibrateSw(u16 *srcFrame, struct Corners *corners)
{
	u32 xcoi, ycoi;
	u32 maxSumTop = 0;
	u32 maxSumBottom = 0;
	u32 maxSumYcoiTop = 0;
	u32 maxSumYcoiBottom = 0;
	u32 maxSumLeft = 0;
	u32 maxSumRight = 0;
	u32 maxSumXcoiLeft = 0;
	u32 maxSumXcoiRight = 0;

	for(ycoi = 0; ycoi < DEMO_HEIGHT * 0.4; ycoi++)
	{
		u32 sumTop = 0;
		u32 sumBottom = 0;
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
		u32 sumLeft = 0;
		u32 sumRight = 0;
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

	corners->ycoiTop= maxSumYcoiTop;
	corners->ycoiBottom = maxSumYcoiBottom;
	corners->xcoiLeft = maxSumXcoiLeft;
	corners->xcoiRight = maxSumXcoiRight;
}

void DrawSw(u16 *srcFrame, u16 *destFrame, struct Corners corners)
{
	u32 xcoi, ycoi;
	for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
	{
		for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++) {
			u16 pxlIn;
			pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
			if (ycoi == corners.ycoiTop) {
				destFrame[(xcoi) + corners.ycoiTop * DEMO_WIDTH] = 0xFFFF;
			} else if (ycoi == corners.ycoiBottom){
				destFrame[(xcoi) + corners.ycoiBottom * DEMO_WIDTH] = 0xFFFF;
			} else if (xcoi == corners.xcoiLeft) {
				destFrame[(corners.xcoiLeft) + ycoi * DEMO_WIDTH] = 0xFFFF;
			} else if (xcoi == corners.xcoiRight){
				destFrame[(corners.xcoiRight) + ycoi * DEMO_WIDTH] = 0xFFFF;
			} else {
				destFrame[(xcoi) + ycoi * DEMO_WIDTH] = pxlIn;
			}
		}
	}
	Xil_DCacheFlushRange((unsigned int) destFrame, DEMO_MAX_FRAME);
}

void BallSw(u16 *srcFrame, struct Corners corners, struct Points *ball)
{
	u32 xcoi, ycoi;
	u32 sum = 0;
	u32 minWidth = DEMO_WIDTH * 0.0156;
	u32 maxWidth = DEMO_WIDTH * 0.018;
	bool overMaxWidth = false;
	bool withinRange = false;
	const u8 arraySize = 255;
	u32 ballX[arraySize] = {0};
	u32 ballY[arraySize] = {0};
	u32 i = 0;

	for(ycoi = corners.ycoiTop; ycoi < corners.ycoiBottom; ycoi++)
	{
		for(xcoi = corners.xcoiLeft; xcoi < corners.xcoiRight; xcoi++)
		{
			u8 r, g, b;
			u16 pxlIn;
			pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
			r = ((pxlIn & 0xF800) >> (11-3));
			b = ((pxlIn & 0x07C0) >> (6-3));
			g = ((pxlIn & 0x003F) << 2);

			bool colorRange = (130 < r && r < 255 && 90 < b && b < 255 && 150 < g && g < 255);
			if (colorRange && !overMaxWidth) {
				sum++;
			} else if (!colorRange && withinRange) {
				ballX[i] = xcoi - sum/2;
				ballY[i] = ycoi;
				if (i < arraySize) {
				i++;
				}
				withinRange = false;
				sum = 0;
			} else if (!colorRange) {
				sum = 0;
				overMaxWidth = false;
			}
			if (sum == minWidth) {
				withinRange = true;
			} else if (sum == maxWidth) {
				overMaxWidth = true;
				sum = 0;
				withinRange = false;
			}
		}
	}

	u32 ballNum = 0;
	for (int x = 0; x < arraySize; x++) {
		u32 ycoiTop = ballY[x] - maxWidth/2;
		u32 ycoiBottom = ballY[x] + maxWidth/2;
		sum = 0;
		for (u32 ycoi = ycoiTop; ycoi < ycoiBottom; ycoi++) {
			u8 r, g, b;
			u16 pxlIn;
			pxlIn = srcFrame[ballX[x] + ycoi * DEMO_WIDTH];
			r = ((pxlIn & 0xF800) >> (11-3));
			b = ((pxlIn & 0x07C0) >> (6-3));
			g = ((pxlIn & 0x003F) << 2);
			bool colorRange = (130 < r && r < 255 && 90 < b && b < 255 && 150 < g && g < 255);
			if (colorRange) {
				sum++;
			}
		}
		if (!(sum < maxWidth && sum > minWidth)) {
			ballX[x] = 0;
			ballY[x] = 0;
		} else {
			ballNum++;
		}
	}

	u16 xSum = 0;
	u16 ySum = 0;
	for (u32 i = 0; i < arraySize; i++) {
		xSum += ballX[i];
		ySum += ballY[i];
	}
	ball->xcoi = xSum / ballNum;
	ball->ycoi = ySum / ballNum;


	xil_printf("%d\n\r", ball->ycoi);
	xil_printf("%d\n\r", ball->xcoi);

	xil_printf("Ball Num Points");
	xil_printf("%d\n\r", i);
}



void CueSw(u16 *srcFrame, struct Corners corners, struct Points *cue)
{
	u32 xcoi, ycoi;
	u32 sum = 0;
	bool overMaxWidth = false;
	bool withinRange = false;
	const u8 arraySize = 255;
	u32 CueX[arraySize] = {0};
	u32 CueY[arraySize] = {0};
	u32 i = 0;
	u32 minWidth = DEMO_WIDTH * 0.003;
	u32 maxWidth = DEMO_WIDTH * 0.011;

	for(ycoi = corners.ycoiTop; ycoi < corners.ycoiBottom; ycoi++)
		{
		for(xcoi = corners.xcoiLeft; xcoi < corners.xcoiRight; xcoi++)
		{
			u8 r, g, b, h, s, v;
			u16 pxlIn;
			pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
			r = ((pxlIn & 0xF800) >> (11-3));
			b = ((pxlIn & 0x07C0) >> (6-3));
			g = ((pxlIn & 0x003F) << 2);

			RgbToHsv(r, b, g, h, s, v);

			bool colorRange = (h > 134 && h < 149 && v > 76 && s > 102);
			if (colorRange && !overMaxWidth) {
				sum++;
			} else if (!colorRange && withinRange) {
				CueX[i] = xcoi - sum/2;
				CueY[i] = ycoi;
				if (i < arraySize) {
				i++;
				}
				withinRange = false;
				sum = 0;
			} else if (!colorRange) {
				sum = 0;
				overMaxWidth = false;
			}
			if (sum == minWidth) {
				withinRange = true;
			} else if (sum == maxWidth) {
				overMaxWidth = true;
				sum = 0;
				withinRange = false;
			}
		}
	}

	u32 ballNum = 0;
	for (int x = 0; x < arraySize; x++) {
		u32 ycoiTop = CueY[x] - maxWidth/2;
		u32 ycoiBottom = CueY[x] + maxWidth/2;
		sum = 0;
		for (u32 ycoi = ycoiTop; ycoi < ycoiBottom; ycoi++) {
			u8 r, g, b, h, s, v;
			u16 pxlIn;
			pxlIn = srcFrame[CueX[x] + ycoi * DEMO_WIDTH];
			r = ((pxlIn & 0xF800) >> (11-3));
			b = ((pxlIn & 0x07C0) >> (6-3));
			g = ((pxlIn & 0x003F) << 2);

			RgbToHsv(r, b, g, h, s, v);

			bool colorRange = (h > 134 && h < 149 && v > 76 && s > 102);
			if (colorRange) {
				sum++;
			}
		}
		if (!(sum < maxWidth && sum > minWidth)) {
			CueX[x] = 0;
			CueY[x] = 0;
		} else {
			ballNum++;
		}
	}

	u32 xSum = 0;
	u32 ySum = 0;
	for (u32 i = 0; i < arraySize; i++) {
		xSum += CueX[i];
		ySum += CueY[i];
	}
	cue->xcoi = xSum / ballNum;
	cue->ycoi = ySum / ballNum;

	//xil_printf("%s\n\r", finalXcoi);
	//xil_printf("%s\n\r", finalYcoi);
	xil_printf("Stick Num Points");
	xil_printf("%d\n\r", i);
}


void DrawTrackingSw(u16 *srcFrame, u16 *destFrame, struct Corners corners, struct Points ball, struct Points cue, u32 Track)
{
	u32 xcoi, ycoi;
	u32 BALL_MAX_WIDTH = DEMO_WIDTH * 0.018;
	u32 CUE_MAX_WIDTH = DEMO_WIDTH * 0.011;

	float gradient = (ball.ycoi - cue.ycoi)*1.0f / (ball.xcoi - cue.xcoi);
	float gradient2 = -1 * gradient;
	float yIntercept = ( ball.ycoi - gradient * ball.xcoi);

	bool cueLeft = cue.xcoi < ball.xcoi;
	float reflectionXcoi, reflectionYcoi, yIntercept2, yIntercept3;
	bool bounce1Within = false;
	bool yIntercept2Init = false, yIntercept3Init = false;

	if (Track > 4) {
		yIntercept2Init = true;
		if (cueLeft) {
			float ycoiRightBoundary = gradient * corners.xcoiRight + yIntercept;
			if (ycoiRightBoundary < corners.ycoiTop) {
				reflectionXcoi = (corners.ycoiTop - yIntercept) / gradient;
				yIntercept2 = corners.ycoiTop + gradient * reflectionXcoi;
			} else if (ycoiRightBoundary > corners.ycoiBottom) {
				reflectionXcoi = (corners.ycoiBottom - yIntercept) / gradient;
				yIntercept2 = corners.ycoiBottom + gradient * reflectionXcoi;
			} else {
				bounce1Within = true;
				reflectionYcoi = gradient * corners.xcoiRight + yIntercept;
				yIntercept2 = reflectionYcoi + gradient * corners.xcoiRight;
			}
		} else {
			float ycoiLeftBoundary = gradient * corners.xcoiLeft + yIntercept;
			if (ycoiLeftBoundary < corners.ycoiTop) {
				reflectionXcoi = (corners.ycoiTop - yIntercept) / gradient;
				yIntercept2 = corners.ycoiTop + gradient * reflectionXcoi;
			} else if (ycoiLeftBoundary > corners.ycoiBottom) {
				reflectionXcoi = (corners.ycoiBottom - yIntercept) / gradient;
				yIntercept2 = corners.ycoiBottom + gradient * reflectionXcoi;
			} else {
				bounce1Within = true;
				reflectionYcoi = gradient * corners.xcoiLeft + yIntercept;
				yIntercept2 = reflectionYcoi + gradient * corners.xcoiLeft;
			}
		}
		if (Track > 8) {
			yIntercept3Init = true;
			if ((cueLeft && bounce1Within) || (!cueLeft && !bounce1Within)) {
				float ycoiLeftBoundary = gradient2 * corners.xcoiLeft + yIntercept2;
				if (ycoiLeftBoundary < corners.ycoiTop) {
					reflectionXcoi = (corners.ycoiTop - yIntercept2) / gradient2;
					yIntercept3 = corners.ycoiTop + gradient2 * reflectionXcoi;
				} else if (ycoiLeftBoundary > corners.ycoiBottom) {
					reflectionXcoi = (corners.ycoiBottom - yIntercept2) / gradient2;
					yIntercept3 = corners.ycoiBottom + gradient2 * reflectionXcoi;
				} else {
					reflectionYcoi = gradient2 * corners.xcoiLeft + yIntercept2;
					yIntercept3 = reflectionYcoi + gradient2 * corners.xcoiLeft;
				}
			} else {
				float ycoiRightBoundary = gradient2 * corners.xcoiRight + yIntercept2;
				if (ycoiRightBoundary < corners.ycoiTop) {
					reflectionXcoi = (corners.ycoiTop - yIntercept2) / gradient2;
					yIntercept3 = corners.ycoiTop + gradient2 * reflectionXcoi;
				} else if (ycoiRightBoundary > corners.ycoiBottom) {
					reflectionXcoi = (corners.ycoiBottom - yIntercept2) / gradient2;
					yIntercept3 = corners.ycoiBottom + gradient2 * reflectionXcoi;
				} else {
					reflectionYcoi = gradient2 * corners.xcoiRight + yIntercept2;
					yIntercept3 = reflectionYcoi + gradient2 * corners.xcoiRight;
				}
			}
		}
	}

	for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
		{
		for (xcoi = 0; xcoi < DEMO_WIDTH; xcoi++) {
			bool line = false, line2 = false, line3 = false;
			if (Track > 2) {
			line = (((gradient * xcoi + yIntercept) >= ycoi -1) && ((gradient * xcoi + yIntercept) <= ycoi +1) &&
				((ycoi - yIntercept) / gradient >= xcoi -1) && ((ycoi - yIntercept) / gradient <= xcoi +1));
			}
			if (yIntercept2Init) {
			line2 = (((gradient2 * xcoi + yIntercept2) >= ycoi -1) && ((gradient2 * xcoi + yIntercept2) <= ycoi +1) &&
				((ycoi - yIntercept2) / gradient2 >= xcoi -1) && ((ycoi - yIntercept2) / gradient2 <= xcoi +1));
			}
			if (yIntercept3Init) {
			line3 = (((gradient * xcoi + yIntercept3) >= ycoi -1) && ((gradient * xcoi + yIntercept3) <= ycoi +1) &&
				((ycoi - yIntercept3) / gradient >= xcoi -1) && ((ycoi - yIntercept3) / gradient <= xcoi +1));
			}

			bool withinTable = corners.ycoiTop < ycoi && corners.ycoiBottom > ycoi && corners.xcoiLeft < xcoi && corners.xcoiRight > xcoi;
			bool ballBool = ((ycoi == (ball.ycoi + BALL_MAX_WIDTH/2) ||  ycoi == (ball.ycoi - BALL_MAX_WIDTH/2))
				&&  xcoi > (ball.xcoi - BALL_MAX_WIDTH/2) &&  xcoi < (ball.xcoi + BALL_MAX_WIDTH/2)) ||
				(ycoi < (ball.ycoi + BALL_MAX_WIDTH/2) &&  ycoi > (ball.ycoi - BALL_MAX_WIDTH/2)
				&&  (xcoi == (ball.xcoi - BALL_MAX_WIDTH/2) ||  xcoi == (ball.xcoi + BALL_MAX_WIDTH/2)));

			bool cueBool = ((ycoi == (cue.ycoi + CUE_MAX_WIDTH/2) ||  ycoi == (cue.ycoi - CUE_MAX_WIDTH/2))
				&&  xcoi > (cue.xcoi - CUE_MAX_WIDTH/2) &&  xcoi < (cue.xcoi + CUE_MAX_WIDTH/2)) ||
				(ycoi < (cue.ycoi + CUE_MAX_WIDTH/2) &&  ycoi > (cue.ycoi - CUE_MAX_WIDTH/2)
				&&  (xcoi == (cue.xcoi - CUE_MAX_WIDTH/2) ||  xcoi == (cue.xcoi + CUE_MAX_WIDTH/2)));

			u16 pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];

			if (ballBool || cueBool) {
				destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0xEFE4;
			}
			else if (line && (ball.ycoi != 0) && (cue.xcoi != 0) && withinTable) {
				bool cmpXcoiCueBall = cue.xcoi < ball.xcoi;

				if (cmpXcoiCueBall && xcoi > cue.xcoi) {
					destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
				} else if(cue.xcoi > ball.xcoi && xcoi < cue.xcoi) {
					destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
				} else if ((cue.xcoi == ball.xcoi) && (cue.ycoi < ball.ycoi) && (ycoi > cue.ycoi)) {
				    destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
				} else if (cue.xcoi == ball.xcoi && cue.ycoi > ball.ycoi && ycoi < cue.ycoi){
				    destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
				} else {
					destFrame[(xcoi) + ycoi * DEMO_WIDTH] = pxlIn;
				}
			} else if ((line2 || line3) && (ball.ycoi != 0) && (cue.xcoi != 0) && withinTable) {
				 destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
			} else if (line && (ball.ycoi != 0) && (cue.xcoi != 0) && withinTable) {
					destFrame[(xcoi) + ycoi * DEMO_WIDTH] = 0;
			} else {
				destFrame[(xcoi) + ycoi * DEMO_WIDTH] = pxlIn;
			}
		}
	}
}
