#include "../ttt_multi.h"

static void ttt_multi_scene_local_callback(TttMultiCustomEvent event, void* context) {
    furi_assert(context);

    TttMultiApp* ttt_multi = context;
    view_dispatcher_send_custom_event(ttt_multi->view_dispatcher, event);
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
}

bool ttt_multi_scene_local_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    UNUSED(event);
    return false;
}