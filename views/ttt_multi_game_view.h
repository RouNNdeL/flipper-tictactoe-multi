#pragma once

#include <gui/view.h>
#include "../ttt_multi_custom_event.h"
#include "../helpers/ttt_multi_game.h"

typedef void (*TttMultiGameViewMoveCallback)(void* context, const TttMultiGameMove* move);
typedef struct TttMultiGameView TttMultiGameView;

TttMultiGameView* ttt_multi_game_view_alloc();

void ttt_multi_game_view_free(TttMultiGameView* game_view);

View* ttt_multi_game_get_view(TttMultiGameView* game_view);

void ttt_multi_game_view_move(TttMultiGameView* game_view, TttMultiGameMove* move);

void ttt_multi_game_view_set_callback(
    TttMultiGameView* game_view,
    TttMultiGameViewMoveCallback callback,
    void* context);

void ttt_multi_game_view_reset(TttMultiGameView* game_view);