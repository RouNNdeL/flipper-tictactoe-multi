#include "../ttt_multi.h"

static void ttt_multi_scene_local_move_callback(void* context, const TttMultiGameMove* move) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;
    ttt_multi->last_move->player = move->player;
    ttt_multi->last_move->x = move->x;
    ttt_multi->last_move->y = move->y;

    view_dispatcher_send_custom_event(ttt_multi->view_dispatcher, TttMultiCustomEventGameMove);
}

static void ttt_multi_scene_local_finish_callback(void* context, TttMultiGameResult result) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;
    ttt_multi->result = result;
    view_dispatcher_send_custom_event(ttt_multi->view_dispatcher, TttMultiCustomEventGameFinish);
}

void ttt_multi_scene_local_on_enter(void* context) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;
    TttMultiGameView *view = ttt_multi->game_view;

    ttt_multi_game_view_set_move_callback(view, ttt_multi_scene_local_move_callback, ttt_multi);
    ttt_multi_game_view_set_finish_callback(view, ttt_multi_scene_local_finish_callback, ttt_multi);
    ttt_multi_game_view_set_local_play(view);
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
            case TttMultiCustomEventGameFinish:
                if(ttt_multi->result == TttMultiGameResultDraw) {
                    notification_message(ttt_multi->notifications, &sequence_error);
                } else {
                    notification_message(ttt_multi->notifications,&sequence_success);
                }
                return true;
            default:
                break;
        }
    }

    return false;
}