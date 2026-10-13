#ifndef PSYZ_VIDEO_H
#define PSYZ_VIDEO_H

/**
 * @file video.h
 * @brief Window, display timing and frame presentation endpoints.
 */

#include <psyz/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the title of the game window
 *
 * Can also be set with psyz_title(<target> <str>) on CMakeLists
 *
 * @param str Window title string, truncated past 255 characters
 */
void Psyz_SetTitle(const char* str);

typedef enum {
    /**
     * Auto-detect use of driver VSync (default)
     * - Tests monitor refresh rate on initialization (adds ~30ms startup time)
     * - Uses driver VSync if monitor is detected between 57Hz-63Hz
     *   (±5% tolerance). This covers both 59.94Hz (NTSC) and 60Hz
     * - Uses manual limiter otherwise (144Hz, 165Hz, etc.)
     * - Balances performance and accuracy automatically
     */
    PSYZ_VSYNC_AUTO,

    /**
     * Always use driver VSync
     * Pros: Zero CPU overhead and no frame pacing on matching refresh rates
     * Cons: Game will run faster on high refresh monitors
     * Cons: Framerate always matches monitor exactly.
     *   On 60Hz monitor: runs at 60fps instead of NTSC 59.94fps
     *   causing ~1 second drift every ~16.7 minutes of gameplay
     * Real NTSC PSX hardware runs at 59.94Hz (60/1.001)
     *   PAL PSX hardware runs at exactly 50Hz
     * Best when combined with driver control panel frame rate limit
     * IMPORTANT: PAL games (50fps) will run at 60fps on 60Hz monitors without
     *   driver-level limiting, which is incorrect
     */
    PSYZ_VSYNC_ON,

    /**
     * Always use internal manual frame limiter
     * Pros: Precise 59.94fps (NTSC) / 50fps (PAL) matching real hardware
     * Pros: Safe for VRR displays
     * Pros: Consistent timing across all monitor refresh rates
     * Cons: ~6% CPU usage on one core (1ms busy-wait per frame for precision)
     * Cons: May have minor frame pacing variance on non-VRR displays
     */
    PSYZ_VSYNC_OFF,

    /**
     * Disable both driver VSync and the internal frame limiter.
     * Most games will be unplayable with this option, but it's useful for:
     *   - Adapting game on variable framerate using a deltaTime.
     *   - Reproduce gameplay with simulatd input on CI at high speed.
     *   - Measure frame time end-to-end and if game is CPU or GPU bound.
     */
    PSYZ_VSYNC_LIMITLESS,
} PsyzVsyncMode;

typedef enum {
    PSYZ_DITHER_AUTO, /**< let the game decide whether to dither (default) */
    PSYZ_DITHER_OFF,  /**< force dithering always off */
} PsyzDitherMode;

typedef enum {
    PSYZ_ASPECT_DISPLAY, /**< aspect from PS1 H/V display ranges (default) */
    PSYZ_ASPECT_SQUARE,  /**< 1:1 from framebuffer (pixel-perfect) */
} PsyzAspectMode;

typedef enum {
    PSYZ_INTERP_OFF,  /**< present every game frame once (default) */
    /**
     * When the display refreshes at a whole multiple of the game's rate
     * (120 Hz for a 60 Hz game), sync to the display and draw in-between
     * frames: each primitive tagged with Psyz_GpuTagPrim moves part of the way
     * from where its group was in the previous frame. Untagged primitives
     * stay where the game drew them. Other displays behave as OFF.
     */
    PSYZ_INTERP_AUTO,
} PsyzInterpMode;

typedef enum {
    PSYZ_FILTER_NONE, /**< nearest-neighbour scaling (default) */
    /**
     * MMPX style-preserving pixel-art magnification (McGuire & Gagiu 2021):
     * the frame is rendered at 1x and doubled by MMPX up to the window size.
     * While it is on the internal resolution is 1x; the one set before comes
     * back when it is turned off. Metal only for now.
     */
    PSYZ_FILTER_MMPX,
    /**
     * Hyllian's xBR-lv2: like MMPX on a 1x picture, but it treats close
     * colours as equal and blends along edges, so shaded art gets rounder
     * and smoother. One pass straight to the window size. Metal only.
     */
    PSYZ_FILTER_XBR,
} PsyzFilterMode;

typedef struct {
    double last_frame_time_us;       /**< duration of last frame */
    double last_draw_time_us;        /**< render time excluding vsync wait */
    double target_frame_time_us;     /**< target frame time */
    unsigned long long total_frames; /**< total frames rendered */
    int using_driver_vsync;          /**< 1 for VSync, 0 for limiter */
} PsyzVideoStats;

/**
 * @brief Get the current VSync mode
 *
 * @return current VSync mode
 */
PsyzVsyncMode Psyz_VideoGetVsyncMode(void);

/**
 * @brief Set VSync mode (default: AUTO)
 *
 * @param mode VSync mode to set
 * @return 0 on success, -1 if invalid mode
 */
int Psyz_VideoSetVsyncMode(PsyzVsyncMode mode);

/**
 * @brief Set the output filter (default: NONE); F5 cycles NONE, MMPX, XBR
 *
 * @return 0 on success, -1 if the backend does not support it
 */
int Psyz_VideoSetFilter(PsyzFilterMode mode);

/**
 * @brief Get the current output filter
 */
PsyzFilterMode Psyz_VideoGetFilter(void);

/**
 * @brief Get the last frame the filter worked on, for tests: pass 0 is its
 * 1x input, pass n the result of the n-th pass (MMPX doubles each time, xBR
 * has one pass at the window size)
 *
 * @return NULL if there is no such pass, or an RGB888 buffer to free()
 */
unsigned char* Psyz_VideoAllocCapturedFilterPass(int pass, int* w, int* h);

/**
 * @brief Present only the rows of the display area the game draws in
 * (default: off)
 *
 * The rows are the newest full-width clear inside the display area, which a
 * DRAWENV with isbg makes every frame; the black bars above and below are
 * left out and the picture keeps its proportions, so it gets wider. A
 * cropped picture within 5% of the window's shape is stretched to fill it.
 */
int Psyz_VideoSetCrop(int enabled);

/**
 * @brief Whether the display area is cropped (Psyz_VideoSetCrop)
 */
int Psyz_VideoGetCrop(void);

/**
 * @brief Set the frame interpolation mode (default: OFF)
 *
 * @param mode Interpolation mode to set
 * @return 0 on success, -1 if invalid or unsupported by the backend
 */
int Psyz_VideoSetInterpolation(PsyzInterpMode mode);

/**
 * @brief Get the current frame interpolation mode
 */
PsyzInterpMode Psyz_VideoGetInterpolation(void);

/**
 * @brief How many displays refreshes the current game frame lasts
 *
 * Interpolation spreads the in-between frames over this many
 * Psyz_VideoVSync(0) calls (a game running at 30 Hz calls it twice per frame).
 * Call it before the first of them; it applies to the next game frame only.
 */
void Psyz_VideoSetFrameSpan(int vsyncs);

/**
 * @brief Get the last interpolated frame as RGB888, for tests
 *
 * @return NULL when no in-between frame was drawn, or a buffer to free()
 */
unsigned char* Psyz_VideoAllocCapturedInterpFrame(int* w, int* h);

/**
 * @brief Get the current dithering mode
 *
 * @return current dithering mode
 */
PsyzDitherMode Psyz_VideoGetDitheringMode(void);

/**
 * @brief Set dithering mode (default: AUTO)
 *
 * @param mode Dithering mode to set
 * @return 0 on success, -1 if invalid mode
 */
int Psyz_VideoSetDitheringMode(PsyzDitherMode mode);

/**
 * @brief Get the current aspect mode
 *
 * @return current aspect mode
 */
PsyzAspectMode Psyz_VideoGetAspectMode(void);

/**
 * @brief Select how the presented aspect ratio is determined
 *
 * DISPLAY: use the game horizontal/vertical display (display sync) ranges,
 *   so different game resolutions present at the intended physical proportions.
 *   This is the accurate and default behaviour, originally intended to control
 *   the beam range of a CRT display. This is not pixel-perfect.
 * SQUARE: present framebuffer pixels 1:1, ignoring sync ranges. This presents
 *   the game pixel-perfect to the display at the cost of intent-accuracy.
 *
 * @param mode Aspect mode to set
 * @return 0 on success, -1 if invalid mode
 */
int Psyz_VideoSetAspectMode(PsyzAspectMode mode);

/**
 * @brief Get the resolution a game should target to render pixel-perfect
 *
 * Fixed-display targets (PSP, and future NDS/Saturn) return a physical size
 * the ported game can adopt to fully use the screen:
 * - PSYZ_ASPECT_SQUARE: the full physical resolution (PSP: 480x272), 1:1.
 * - PSYZ_ASPECT_DISPLAY: the largest 4:3 area at full height (PSP: 362x272);
 *   adopting it keeps output pixel-perfect while preserving the intended
 *   aspect ratio.
 * PC targets return the current window size in pixels.
 *
 * @return display size in pixels
 */
PsyzSize Psyz_VideoGetDisplaySize(void);

/**
 * @brief Choose where on the screen the presented output lands
 *
 * Only meaningful on fixed-display targets (PSP); ignored on PC.
 * The default rect {0,0,0,0} centers the output according to the aspect
 * mode. A non-empty rect places the output's top-left at (x, y) and scales
 * it to fit w x h, overriding the aspect mode's own sizing. The result is
 * still clipped to the physical screen, so a rect that extends past the
 * edge is cropped.
 *
 * @param rect target area on the display, or {0,0,0,0} to center
 */
void Psyz_VideoSetDrawArea(PsyzRect rect);

/** Maximum accepted internal resolution multiplier */
#define PSYZ_INTERNAL_RES_MAX 8

#ifndef __PSP__
/**
 * Targets without this capability must:
 *   - Implement Psyz_VideoSetInternalResolution as no-op
 *   - Always return 1 for Psyz_VideoGetInternalResolution
 */
#define PSYZ_HAS_INTERNAL_RESOLUTION_SCALE
#endif

/**
 * @brief Set the internal rendering resolution multiplier (default: 1)
 *
 * Internally multiplies horizontal and vertical resolution, giving 3D
 * geometry more sub-pixel precision (smoother edges, less shimmer).
 * Safe to call at runtime; the change applies on the next presented frame
 * without needing to restart the game.
 *
 * @param multiplier Integer scale factor, between 1 to PSYZ_INTERNAL_RES_MAX
 * @return 0 on success, -1 if multiplier < 1
 */
int Psyz_VideoSetInternalResolution(unsigned multiplier);

/**
 * @brief Get the current internal resolution multiplier
 *
 * @return current multiplier (>= 1)
 */
unsigned Psyz_VideoGetInternalResolution(void);

/**
 * @brief Synchronize with vertical blank
 *
 * Synchronize with the refresh rate mode set in Psyz_VideoSetVsyncMode.
 * The interface is very similar to libetc VSync.
 *
 * @param mode Synchronization mode:
 *             - 0: Present and wait for the next vertical blank
 *             - 1: Return immediately (non-blocking)
 *             - Negative: Return immediately (non-blocking)
 * @return Simulates the SDK VSync return values as closely as possible.
 */
int Psyz_VideoVSync(int mode);

/**
 * @brief Get frame timing statistics
 *
 * @param stats Output structure to fill
 * @return 0 on success, -1 if stats is NULL or platform not initialized
 */
int Psyz_VideoStats(PsyzVideoStats* stats);

/**
 * @brief Get frame output as a byte array
 *
 * This function is very slow.
 *
 * @param w Output frame width
 * @param h Output frame height
 * @return NULL on failure, or RGB888 buffer to be destroyed with free(ptr)
 */
unsigned char* Psyz_VideoAllocCapturedFrame(int* w, int* h);

/**
 * @brief Dump the whole video memory plane as RGB888
 *
 * @param w Output VRAM width
 * @param h Output VRAM height
 * @return NULL on failure, or RGB888 buffer to be destroyed with free(ptr)
 */
unsigned char* Psyz_VideoAllocVramDump(int* w, int* h);

#ifdef __cplusplus
}
#endif

#endif
