#include <Dolphin/mem.h>
#include <JSystem/JGeometry.hxx>
#include <JSystem/JKernel/JKRHeap.hxx>

#include <SMS/game/Application.hxx>
#include <SMS/manager/MarioParticleManager.hxx>
#include <SMS/screen/SMSFader.hxx>
#include <SMS/sound/MSoundSESystem.hxx>

#include "collision/warp.hxx"
#include "globals.hxx"
#include "libs/triangle.hxx"
#include "logging.hxx"
#include "player.hxx"

void boostPadCol(TMario *player, const TBGCheckData *data, u32 flags) {
    auto playerData = Player::getData(player);

    if (!(flags & Player::InteractionFlags::GROUNDED))
        return;

    const f32 newSpeed    = data->mValue4;
    const f32 scale       = newSpeed / player->mForwardSpeed;
    player->mForwardSpeed = newSpeed;
    player->mPrevSpeed.scale(scale);

    u32 targetState = (player->mState == static_cast<u32>(TMario::STATE_DIVESLIDE) ||
                       player->mState == static_cast<u32>(TMario::STATE_GOOPSLIDE))
                          ? player->mState
                          : static_cast<u32>(TMario::STATE_RUNNING);
    if ((flags & Player::InteractionFlags::ON_ENTER) || (flags & Player::InteractionFlags::ON_CONTACT)) {
        player->changePlayerStatus(targetState, 0, false);
        player->startVoice(TMario::VOICE_JUMP);
    }
}