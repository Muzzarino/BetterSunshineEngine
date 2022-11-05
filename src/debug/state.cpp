#include <Dolphin/MTX.h>
#include <Dolphin/types.h>

#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/J2D/J2DPane.hxx>
#include <JSystem/J2D/J2DTextBox.hxx>
#include <SMS/Enemy/EnemyMario.hxx>
#include <SMS/Player/Mario.hxx>

#include <SMS/npc/BaseNPC.hxx>
#include <SMS/raw_fn.hxx>
#include <SMS/sound/MSound.hxx>
#include <SMS/sound/MSoundSESystem.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/collision/MapCollisionData.hxx>

#include "debug.hxx"
#include "libs/cheathandler.hxx"
#include "libs/constmath.hxx"
#include "logging.hxx"
#include "module.hxx"

using namespace BetterSMS;

static s16 gPlayerMonitorX = 10, gPlayerMonitorY = 190;
static s16 gWorldMonitorX = 10, gWorldMonitorY = 310;
static s16 gCollisionMonitorX = 350, gCollisionMonitorY = 190;
static s16 gFontWidth = 11;

static J2DTextBox *gpPlayerStateStringW    = nullptr;
static J2DTextBox *gpPlayerStateStringB    = nullptr;
static J2DTextBox *gpWorldStateStringW     = nullptr;
static J2DTextBox *gpWorldStateStringB     = nullptr;
static J2DTextBox *gpCollisionStateStringW = nullptr;
static J2DTextBox *gpCollisionStateStringB = nullptr;

static char sPlayerStringBuffer[300]{};
static char sWorldStringBuffer[300]{};
static char sCollisionStringBuffer[300]{};

static size_t sHitObjCount = 0;
static bool sIsInitialized = false;

void addToHitActorCount(JDrama::TViewObj *obj, u32 flags, JDrama::TGraphics *graphics) {
    testPerform__Q26JDrama8TViewObjFUlPQ26JDrama9TGraphics(obj, flags, graphics);
    ++sHitObjCount;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802a0450, 0, 0, 0), addToHitActorCount);

void initStateMonitor(TApplication *app) {
    gpPlayerStateStringW                  = new J2DTextBox(gpSystemFont->mFont, "");
    gpPlayerStateStringB                  = new J2DTextBox(gpSystemFont->mFont, "");
    gpPlayerStateStringW->mStrPtr         = sPlayerStringBuffer;
    gpPlayerStateStringB->mStrPtr         = sPlayerStringBuffer;
    gpPlayerStateStringW->mNewlineSize    = 14;
    gpPlayerStateStringW->mCharSizeX      = gFontWidth;
    gpPlayerStateStringW->mCharSizeY      = 14;
    gpPlayerStateStringB->mNewlineSize    = 14;
    gpPlayerStateStringB->mCharSizeX      = gFontWidth;
    gpPlayerStateStringB->mCharSizeY      = 14;
    gpPlayerStateStringW->mGradientTop    = {255, 255, 255, 255};
    gpPlayerStateStringW->mGradientBottom = {255, 255, 255, 255};
    gpPlayerStateStringB->mGradientTop    = {0, 0, 0, 255};
    gpPlayerStateStringB->mGradientBottom = {0, 0, 0, 255};

    gpWorldStateStringW                  = new J2DTextBox(gpSystemFont->mFont, "");
    gpWorldStateStringB                  = new J2DTextBox(gpSystemFont->mFont, "");
    gpWorldStateStringW->mStrPtr         = sWorldStringBuffer;
    gpWorldStateStringB->mStrPtr         = sWorldStringBuffer;
    gpWorldStateStringW->mNewlineSize    = 14;
    gpWorldStateStringW->mCharSizeX      = gFontWidth;
    gpWorldStateStringW->mCharSizeY      = 14;
    gpWorldStateStringB->mNewlineSize    = 14;
    gpWorldStateStringB->mCharSizeX      = gFontWidth;
    gpWorldStateStringB->mCharSizeY      = 14;
    gpWorldStateStringW->mGradientTop    = {255, 255, 255, 255};
    gpWorldStateStringW->mGradientBottom = {255, 255, 255, 255};
    gpWorldStateStringB->mGradientTop    = {0, 0, 0, 255};
    gpWorldStateStringB->mGradientBottom = {0, 0, 0, 255};

    gpCollisionStateStringW                  = new J2DTextBox(gpSystemFont->mFont, "");
    gpCollisionStateStringB                  = new J2DTextBox(gpSystemFont->mFont, "");
    gpCollisionStateStringW->mStrPtr         = sCollisionStringBuffer;
    gpCollisionStateStringB->mStrPtr         = sCollisionStringBuffer;
    gpCollisionStateStringW->mNewlineSize    = 14;
    gpCollisionStateStringW->mCharSizeX      = gFontWidth;
    gpCollisionStateStringW->mCharSizeY      = 14;
    gpCollisionStateStringB->mNewlineSize    = 14;
    gpCollisionStateStringB->mCharSizeX      = gFontWidth;
    gpCollisionStateStringB->mCharSizeY      = 14;
    gpCollisionStateStringW->mGradientTop    = {255, 255, 255, 255};
    gpCollisionStateStringW->mGradientBottom = {255, 255, 255, 255};
    gpCollisionStateStringB->mGradientTop    = {0, 0, 0, 255};
    gpCollisionStateStringB->mGradientBottom = {0, 0, 0, 255};

    sIsInitialized = true;
}

void updateStateMonitor(TApplication *app) {
    TMarDirector *director = reinterpret_cast<TMarDirector *>(app->mDirector);

    if (!director || !gpMarioAddress || !sIsInitialized)
        return;

    if (director->mCurState != TMarDirector::Status::STATE_NORMAL &&
        director->mCurState != TMarDirector::Status::STATE_PAUSE_MENU)
        return;

    u16 floorColType =
        gpMarioAddress->mFloorTriangle ? gpMarioAddress->mFloorTriangle->mCollisionType : 0xFFFF;
    u16 floorColValue     = gpMarioAddress->mFloorTriangle ? gpMarioAddress->mFloorTriangle->mValue4
                                                           : 0xFFFF;
    TVec3f floorColNormal = gpMarioAddress->mFloorTriangle ? gpMarioAddress->mFloorTriangle->mNormal
                                                           : TVec3f(0.0f, 0.0f, 0.0f);
    u16 wallColType  = gpMarioAddress->mWallTriangle ? gpMarioAddress->mWallTriangle->mCollisionType
                                                     : 0xFFFF;
    u16 wallColValue = gpMarioAddress->mWallTriangle ? gpMarioAddress->mWallTriangle->mValue4
                                                     : 0xFFFF;
    TVec3f wallColNormal = gpMarioAddress->mWallTriangle ? gpMarioAddress->mWallTriangle->mNormal
                                                         : TVec3f(0.0f, 0.0f, 0.0f);
    u16 roofColType  = gpMarioAddress->mRoofTriangle ? gpMarioAddress->mRoofTriangle->mCollisionType
                                                     : 0xFFFF;
    u16 roofColValue = gpMarioAddress->mRoofTriangle ? gpMarioAddress->mRoofTriangle->mValue4
                                                     : 0xFFFF;
    TVec3f roofColNormal = gpMarioAddress->mRoofTriangle ? gpMarioAddress->mRoofTriangle->mNormal
                                                         : TVec3f(0.0f, 0.0f, 0.0f);

    snprintf(sPlayerStringBuffer, 300,
             "Player Stats:\n"
             "  Position:   %.02f, %.02f, %.02f\n"
             "  Rotation:   %.02f, %.02f, %.02f\n"
             "  Movement: %.02f, %.02f, %.02f\n"
             "  Speed:     %.02f\n"
             "  Status:     0x%lX\n"
             "  State:      0x%lX\n"
             "  Flags:      0x%lX",
             gpMarioAddress->mPosition.x, gpMarioAddress->mPosition.y, gpMarioAddress->mPosition.z,
             gpMarioAddress->mRotation.x, gpMarioAddress->mRotation.y, gpMarioAddress->mRotation.z,
             gpMarioAddress->mSpeed.x, gpMarioAddress->mSpeed.y, gpMarioAddress->mSpeed.z,
             gpMarioAddress->mForwardSpeed, gpMarioAddress->mState, gpMarioAddress->mActionState,
             *reinterpret_cast<u32 *>(&gpMarioAddress->mAttributes));

    snprintf(sWorldStringBuffer, 300,
             "World Stats:\n"
             "  Area ID:        %d\n"
             "  Episode ID:     %d\n"
             "  Warp ID:        0x%X\n"
             "  Perform Objs:  %lu\n",
             director->mAreaID, director->mEpisodeID,
             ((director->mAreaID + 1) << 8) | director->mEpisodeID, sHitObjCount);

    snprintf(sCollisionStringBuffer, 300,
             "Collision Stats:\n"
             "  Collision Tris:   %lu\n"
             "  Floor Normal:   %.02f, %.02f, %.02f\n"
             "  Floor Type:     0x%hX\n"
             "  Floor Value:    0x%hX\n"
             "  Wall Normal:    %.02f, %.02f, %.02f\n"
             "  Wall Type:      0x%hX\n"
             "  Wall Value:     0x%hX\n"
             "  Roof Normal:   %.02f, %.02f, %.02f\n"
             "  Roof Type:     0x%hX\n"
             "  Roof Value:    0x%hX\n",
             gpMapCollisionData->mCheckDataLength, floorColNormal.x, floorColNormal.y,
             floorColNormal.z, floorColType, floorColValue, wallColNormal.x, wallColNormal.y,
             wallColNormal.z, wallColType, wallColValue, roofColNormal.x, roofColNormal.y,
             roofColNormal.z, roofColType, roofColValue);

    sHitObjCount = 0;
}

void drawStateMonitor(TApplication *app, const J2DOrthoGraph *ortho) {
    if (!sIsInitialized)
        return;

    {
        s16 adjust = getScreenRatioAdjustX();
        gpPlayerStateStringB->draw(gPlayerMonitorX - adjust + 1, gPlayerMonitorY + 1);
        gpPlayerStateStringW->draw(gPlayerMonitorX - adjust, gPlayerMonitorY);
        gpWorldStateStringB->draw(gWorldMonitorX - adjust + 1, gWorldMonitorY + 1);
        gpWorldStateStringW->draw(gWorldMonitorX - adjust, gWorldMonitorY);
        gpCollisionStateStringB->draw(gCollisionMonitorX + adjust + 1, gCollisionMonitorY + 1);
        gpCollisionStateStringW->draw(gCollisionMonitorX + adjust, gCollisionMonitorY);
    }
}