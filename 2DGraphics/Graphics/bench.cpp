/**
 *  Copyright 2013 Mike Reed
 *
 *  COMP 590 -- Fall 2013
 */

#include "GContext.h"
#include "GBitmap.h"
#include "GPaint.h"
#include "GRect.h"
#include "GRandom.h"
#include "GTime.h"
#include "app_utils.h"

#include <string.h>
#include <stdlib.h>

static bool gVerbose;
static int gRepeatCount = 1;
static int gTargetIndex = -1;

#define INDEX_LOOP(code)    \
    do { code } while (index == gTargetIndex);

static double time_erase(GContext* ctx, const GColor& color) {
    GBitmap bm;
    ctx->getBitmap(&bm);

    int loop = 2 * 1000 * gRepeatCount;
    
    GMSec before = GTime::GetMSec();
    
    for (int i = 0; i < loop; ++i) {
        ctx->clear(color);
    }
    
    GMSec dur = GTime::GetMSec() - before;
    
    return dur * 1000.0 / (bm.fWidth * bm.fHeight) / gRepeatCount;
}

static int clear_bench(int index) {
    const int DIM = 1 << 8;
    static const struct {
        int fWidth;
        int fHeight;
    } gSizes[] = {
        { DIM * DIM, 1 },
        { 1, DIM * DIM },
        { DIM, DIM },
    };

    const GColor color = { 0.5, 1, 0.5, 0 };
    double total = 0;
    
    for (int i = 0; i < GARRAY_COUNT(gSizes); ++i) {
        const int w = gSizes[i].fWidth;
        const int h = gSizes[i].fHeight;
        
        GContext* ctx = GContext::Create(w, h);
        if (!ctx) {
            fprintf(stderr, "GContext::Create failed [%d %d]\n", w, h);
            exit(-1);
        }
        
        double dur;
        INDEX_LOOP(dur = time_erase(ctx, color);)
        if (gVerbose) {
            printf("[%2d] [%5d, %5d] %8.4f per-pixel\n", index, w, h, dur);
        }
        index += 1;
        delete ctx;
        
        total += dur;
    }
    printf("Clear time %8.4f per-pixel\n", total / GARRAY_COUNT(gSizes));
    return index;
}

///////////////////////////////////////////////////////////////////////////////

static GRect rand_rect_255(GRandom& rand) {
    float x = rand.nextF() * 15;
    float y = rand.nextF() * 15;
    float w = rand.nextF() * 255;
    float h = rand.nextF() * 255;
    return GRect::MakeXYWH(x, y, w, h);
}

static double time_rect(GContext* ctx, const GRect& rect, float alpha,
                        GRect (*proc)(GRandom&)) {
    int loop = 20 * 1000 * gRepeatCount;
    
    GMSec before = GTime::GetMSec();
    GColor color = { alpha, 0, 0, 0 };
    GRandom rand;
    GPaint paint;

    double area = 0;
    if (proc) {
        for (int i = 0; i < loop; ++i) {
            GRect r = proc(rand);
            color.fR = rand.nextF();
            paint.setColor(color);
            ctx->drawRect(r, paint);
            // this is not really accurage for 'area', since we should really
            // measure the intersected-area of r w/ the context's bitmap
            area += r.width() * r.height();
        }
    } else {
        for (int i = 0; i < loop; ++i) {
            color.fR = rand.nextF();
            paint.setColor(color);
            ctx->drawRect(rect, paint);
            area += rect.width() * rect.height();
        }
    }
    GMSec dur = GTime::GetMSec() - before;
    

    return dur * 1000 * 1000.0 / area;
}

static int rect_bench(int index) {
    const int W = 256;
    const int H = 256;
    static const struct {
        float fWidth;
        float fHeight;
        float fAlpha;
        const char* fDesc;
        GRect (*fProc)(GRandom&);
    } gRec[] = {
        { 2, H,    1.0f,   "opaque narrow", NULL },
        { W, 2,    1.0f,   "opaque   wide", NULL },

        { 2, H,    0.5f,   " blend narrow", NULL },
        { W, 2,    0.5f,   " blend   wide", NULL },
        { W, H,    0.5f,   " blend random", rand_rect_255 },

        { W, H,    0.0f,   "  zero   full", NULL },
    };

    GContext* ctx = GContext::Create(W, H);
    ctx->clear(GColor::Make(1, 1, 1, 1));

    double total = 0;
    for (int i = 0; i < GARRAY_COUNT(gRec); ++i) {
        GRect r = GRect::MakeWH(gRec[i].fWidth, gRec[i].fHeight);
        double dur;
        INDEX_LOOP(dur = time_rect(ctx, r, gRec[i].fAlpha, gRec[i].fProc);)
        if (gVerbose) {
            printf("[%2d] Rect %s %8.4f per-pixel\n", index, gRec[i].fDesc, dur);
        }
        index += 1;
        total += dur;
    }
    printf("Rect  time %8.4f per-pixel\n", total / GARRAY_COUNT(gRec));
    delete ctx;
    return index;
}

///////////////////////////////////////////////////////////////////////////////

/**
 *  colors[] are for each corner's starting color [LT, RT, RB, LB]
 */
static void fill_ramp(const GBitmap& bm, const GColor colors[4]) {
    const float xscale = 1.0f / (bm.width() - 1);
    const float yscale = 1.0f / (bm.height() - 1);
    
    GPixel* row = bm.fPixels;
    for (int y = 0; y < bm.height(); ++y) {
        for (int x = 0; x < bm.width(); ++x) {
            GColor c = lerp4colors(colors, x * xscale, y * yscale);
            row[x] = color_to_pixel(c);
        }
        row = next_row(bm, row);
    }
}

static void init(GBitmap* bm, int W, int H) {
    bm->fWidth = W;
    bm->fHeight = H;
    bm->fRowBytes = W * sizeof(GPixel);
    bm->fPixels = (GPixel*)malloc(bm->fRowBytes * bm->fHeight);
}

static double time_bitmap(GContext* ctx, const GBitmap& bm, float alpha) {
    int loop = 1000 * gRepeatCount;
    double area = bm.width() * bm.height();

    GPaint paint;
    paint.setAlpha(alpha);

    GMSec before = GTime::GetMSec();
    for (int i = 0; i < loop; ++i) {
        ctx->drawBitmap(bm, 0, 0, paint);
    }
    GMSec dur = GTime::GetMSec() - before;
    return dur * 500 * 1000.0 / (loop * area);
}

static int bitmap_bench_worker(int index, bool doScale) {
    const int W = 256;
    const int H = 256;
    
    GColor corners[] = {
        GColor::Make(1, 1, 0, 0),   GColor::Make(1, 0, 1, 0),
        GColor::Make(1, 0, 0, 1),   GColor::Make(1, 0, 0, 0),
    };
    
    const struct {
        const char* fDesc;
        const float fCornerAlpha;
        const float fGlobalAlpha;
    } gRec[] = {
        { "bitmap_solid_opaque",    1.0f,  1.0f },
        { "bitmap_blend_opaque",    0.5f,  1.0f },
        { "bitmap_solid_alpha ",     1.0f,  0.5f },
        { "bitmap_blend_alpha ",     0.5f,  0.5f },
    };
    
    GBitmap bitmaps[GARRAY_COUNT(gRec)];
    for (int i = 0; i < GARRAY_COUNT(gRec); ++i) {
        init(&bitmaps[i], W, H);
        corners[1].fA = corners[2].fA = gRec[i].fCornerAlpha;
        fill_ramp(bitmaps[i], corners);
    }
    
    GContext* ctx = GContext::Create(W, H);
    ctx->clear(GColor::Make(1, 1, 1, 1));
    
    const char* name = doScale ? "Bitmap_scale" : "Bitmap";
    
    if (doScale) {
        ctx->scale(1.1f, 1.1f);
    }

    double total = 0;
    for (int i = 0; i < GARRAY_COUNT(gRec); ++i) {
        double dur;
        INDEX_LOOP(dur = time_bitmap(ctx, bitmaps[i], gRec[i].fGlobalAlpha);)
        if (gVerbose) {
            printf("[%2d] %s %s %8.4f per-pixel\n", index, name, gRec[i].fDesc, dur);
        }
        index += 1;
        total += dur;
    }
    printf("%s time %7.4f per-pixel\n", name, total / GARRAY_COUNT(gRec));
    
    for (int i = 0; i < GARRAY_COUNT(bitmaps); ++i) {
        free(bitmaps[i].fPixels);
    }
    delete ctx;
    return index;
}

static int bitmap_bench(int index) {
    return bitmap_bench_worker(index, false);
}

static int bitmap_scale_bench(int index) {
    return bitmap_bench_worker(index, true);
}

static double time_poly(GContext* ctx, const GPoint pts[], int ptCount,
                        int loopN, const GPaint& paint) {
    int loop = 20000 * gRepeatCount;
    
    GMSec before = GTime::GetMSec();
    for (int outer = 0; outer < loopN; ++outer) {
        for (int i = 0; i < loop; ++i) {
            if (3 == ptCount) {
                ctx->drawTriangle(pts, paint);
            } else {
                ctx->drawConvexPolygon(pts, ptCount, paint);
            }
        }
    }
    GMSec dur = GTime::GetMSec() - before;
    return dur * 100.0 / loop;
}

static int triangle_bench(int index) {
    const int W = 256;
    const int H = 256;
    
    const struct {
        const char* fDesc;
        int         fN;
        GPoint      fTriangle[3];
    } gRec[] = {
        { "triangle_tiny   ", 200, {{ 1.25f, 1 }, { 1, 1.25f }, { 2, 2 }} },
        { "triangle_nodraw ", 10,  {{ 0.1f, 0 }, { 255.0f, 255 }, { 128.4f, 128.4f }} },
        { "triangle_big    ", 1,   {{ 128, 0 }, { 0, 256 }, { 256, 256 }} },
        { "triangle_clipped", 5,   {{ -100, 0 }, { 0, -100 }, { 100, 100 }} },
    };
    
    GPaint paint;
    GAutoDelete<GContext> ctx(GContext::Create(W, H));
    ctx->clear(GColor::Make(1, 1, 1, 1));
    
    double total = 0;
    for (int i = 0; i < GARRAY_COUNT(gRec); ++i) {
        double dur;
        INDEX_LOOP(dur = time_poly(ctx, gRec[i].fTriangle, 3, gRec[i].fN, paint);)
        if (gVerbose) {
            printf("[%2d] %s %8.4f per-pixel\n", index, gRec[i].fDesc, dur);
        }
        total += dur;
        index += 1;
    }
    printf("%s time %7.4f\n", "triangles", total / GARRAY_COUNT(gRec));
    return index;
}

static int poly_bench(int index) {
    const int W = 256;
    const int H = 256;
    const int N = 30;
    
    GPoint pts[N];
    
    GPaint paint;
    GAutoDelete<GContext> ctx(GContext::Create(W, H));
    ctx->clear(GColor::Make(1, 1, 1, 1));
    
    ctx->scale(W, H);
    
    double total = 0;
    int loop_count = 0;
    for (int n = 5; n < N; n += 5) {
        app_make_regular_poly(pts, n);
        double dur;
        INDEX_LOOP(dur = time_poly(ctx, pts, n, 1, paint);)
        if (gVerbose) {
            printf("[%2d] polygon_%d %8.4f\n", index, n, dur);
        }
        total += dur;
        index += 1;
        loop_count += 1;
    }
    printf("%s time %7.4f\n", "polygons", total / loop_count);
    return index;
}

typedef void (*LoopProc)(GContext*, const void*, const GPaint&, int N);

static void loop_rect(GContext* ctx, const void* obj, const GPaint& paint, int N) {
    const GRect* rect = (const GRect*)obj;
    for (int i = 0; i < N; ++i) {
        ctx->drawRect(*rect, paint);
    }
}

static void loop_bitmap(GContext* ctx, const void* obj, const GPaint& paint, int N) {
    const GBitmap* bitmap = (const GBitmap*)obj;
    for (int i = 0; i < N; ++i) {
        ctx->drawBitmap(*bitmap, 0, 0, paint);
    }
}

static double time_loop(GContext* ctx, LoopProc proc, const void* obj,
                        int loopN, const GPaint& paint) {
    const int loop = 1000 * gRepeatCount;

    GMSec before = GTime::GetMSec();
    for (int outer = 0; outer < loop; ++outer) {
        proc(ctx, obj, paint, loopN);
    }
    GMSec dur = GTime::GetMSec() - before;
    return dur * 10.0 / loop;
}

static int rotate_bench(int index) {
    const int W = 256;
    const int H = 256;
    const int SIZE = 50;

    const GRect rect = GRect::MakeWH(SIZE, SIZE);

    const GColor corners[] = {
        GColor::Make(1, 1, 0, 0),   GColor::Make(0.5, 0, 1, 0),
        GColor::Make(0, 0, 0, 1),   GColor::Make(1, 0, 0, 0),
    };
    GBitmap bitmap;
    init(&bitmap, SIZE, SIZE);
    fill_ramp(bitmap, corners);

    const struct {
        const char* fDesc;
        LoopProc    fProc;
        const void* fObj;
        float       fAlpha;
        int         fN;
    } gRec[] = {
        { "rotate_rect_opaque   ", loop_rect,   &rect,   1.0, 200 },
        { "rotate_rect_blend    ", loop_rect,   &rect,   0.5, 200 },
        { "rotate_bitmap_opaque ", loop_bitmap, &bitmap, 1.0, 20 },
        { "rotate_bitmap_blend  ", loop_bitmap, &bitmap, 0.5, 20 },
    };
    
    GPaint paint;
    GAutoDelete<GContext> ctx(GContext::Create(W, H));
    ctx->clear(GColor::Make(1, 1, 1, 1));

    ctx->rotate(G_PI/32);

    double total = 0;
    for (int i = 0; i < GARRAY_COUNT(gRec); ++i) {
        paint.setAlpha(gRec[i].fAlpha);

        double dur;
        INDEX_LOOP(dur = time_loop(ctx, gRec[i].fProc, gRec[i].fObj, gRec[i].fN, paint);)
        if (gVerbose) {
            printf("[%2d] %s %8.4f\n", index, gRec[i].fDesc, dur);
        }
        total += dur;
        index += 1;
    }
    printf("%s time %7.4f\n", "rotate", total / GARRAY_COUNT(gRec));
    return index;
}

///////////////////////////////////////////////////////////////////////////////

typedef int (*BenchProc)(int index);

static const BenchProc gBenches[] = {
    clear_bench,
    rect_bench,
    bitmap_bench,
    bitmap_scale_bench,
    triangle_bench, poly_bench,
    rotate_bench,
};

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help")) {
            printf("Time drawing commands on GContext.\n"
                   "--verbose (or -v) for verbose/detailed output.\n"
                   "--repeat N to run the internal loops N times to reduce noise.\n");
            return 0;
        }
        if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
            gVerbose = true;
        } else if (!strcmp(argv[i], "--index") || !strcmp(argv[i], "-i")) {
            if (i == argc - 1) {
                fprintf(stderr, "%s needs a valid index\n", argv[i]);
                exit(-1);
            }
            gTargetIndex = (int)atol(argv[++i]);
        } else if (!strcmp(argv[i], "--repeat")) {
            if (i == argc - 1) {
                fprintf(stderr, "need valid repeat_count # after --repeat\n");
                exit(-1);
            }
            int n = (int)atol(argv[i + 1]);
            if (n > 0) {
                gRepeatCount = n;
            } else {
                fprintf(stderr, "repeat value needs to be > 0\n");
                exit(-1);
            }
        }
    }

    int index = 0;
    for (int i = 0; i < GARRAY_COUNT(gBenches); ++i) {
        index = gBenches[i](index);
    }
    return 0;
}

