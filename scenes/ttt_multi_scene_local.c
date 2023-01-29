#include "../ttt_multi.h"

static void ttt_multi_scene_local_callback(void* context, const TttMultiGameMove* move) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;
    ttt_multi->last_move->player = move->player;
    ttt_multi->last_move->x = move->x;
    ttt_multi->last_move->y = move->y;

    view_dispatcher_send_custom_event(ttt_multi->view_dispatcher, TttMultiCustomEventGameMove);
}

void ttt_multi_scene_local_on_enter(void* context) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;
    TttMultiGameView *view = ttt_multi->game_view;

    ttt_multi_game_view_set_callback(view, ttt_multi_scene_local_callback, ttt_multi);
    view_dispatcher_switch_to_view(ttt_multi->view_dispatcher, TttMultiViewGame);
}

void ttt_multi_scene_local_on_exit(void* context) {
    furi_assert(context);
    TttMultiApp *ttt_multi = context;

    ttt_multi_game_view_reset(ttt_multi->game_view);
}

bool ttt_multi_scene_local_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    TttMultiApp* ttt_multi = context;

    if(event.type == SceneManagerEventTypeCustom) {
        TttMultiCustomEvent custom_event = event.event;
        switch(custom_event) {
            case TttMultiCustomEventGameMove:
                ttt_multi_game_view_move(ttt_multi->game_view, ttt_multi->last_move);
                return true;
            default:
                break;
        }
    }

    return false;
}