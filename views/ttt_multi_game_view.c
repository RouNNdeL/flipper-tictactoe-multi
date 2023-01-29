#include "ttt_multi_game_view.h"

struct TttMultiGameView {
    View* view;
};

typedef struct {
    TttMultiGame* game;
    uint8_t position_x;
    uint8_t position_y;

    void* context;
    TttMultiGameViewMoveCallback callback;
} TttMultiGameViewModel;

void ttt_multi_game_view_draw_callback(Canvas* canvas, void* _model) {
    TttMultiGameViewModel* model = _model;

    canvas_clear(canvas);
    const uint8_t cell_size = 20;
    const uint8_t padding = 4;

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            canvas_draw_frame(canvas, i * cell_size, j * cell_size, cell_size, cell_size);
        }
    }

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            bool is_selected = model->position_x == i && model->position_y == j;
            bool draw_current_x = is_selected && ttt_multi_game_get_state(model->game) == TttMultiGameStateTurnX;
            bool draw_current_o = is_selected && ttt_multi_game_get_state(model->game) == TttMultiGameStateTurnO;

            if(draw_current_x || ttt_multi_game_player_at(model->game, i, j) == TttMultiGamePlayerX) {
                canvas_draw_line(
                    canvas,
                    i * cell_size + padding,
                    j * cell_size + padding,
                    i * cell_size + cell_size - padding,
                    j * cell_size + cell_size - padding);
                canvas_draw_line(
                    canvas,
                    i * cell_size + cell_size - padding,
                    j * cell_size + padding,
                    i * cell_size + padding,
                    j * cell_size + cell_size - padding);
            } else if(draw_current_o || ttt_multi_game_player_at(model->game, i, j) == TttMultiGamePlayerO) {
                canvas_draw_circle(
                    canvas,
                    i * cell_size + cell_size / 2,
                    j * cell_size + cell_size / 2,
                    cell_size / 2 - padding);
            }
        }
    }
}

static void ttt_multi_game_view_process_left(TttMultiGameView* view);
static void ttt_multi_game_view_process_right(TttMultiGameView* view);
static void ttt_multi_game_view_process_up(TttMultiGameView* view);
static void ttt_multi_game_view_process_down(TttMultiGameView* view);
static void ttt_multi_game_view_process_ok(TttMultiGameView* view);

static bool ttt_multi_game_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    TttMultiGameView* game_view = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyLeft:
            ttt_multi_game_view_process_left(game_view);
            consumed = true;
            break;
        case InputKeyRight:
            ttt_multi_game_view_process_right(game_view);
            consumed = true;
            break;
        case InputKeyUp:
            ttt_multi_game_view_process_up(game_view);
            consumed = true;
            break;
        case InputKeyDown:
            ttt_multi_game_view_process_down(game_view);
            consumed = true;
            break;
        case InputKeyOk:
            ttt_multi_game_view_process_ok(game_view);
            consumed = true;
            break;
        default:
            break;
        }
    }

    return consumed;
}

TttMultiGameView* ttt_multi_game_view_alloc() {
    TttMultiGameView* game_view = malloc(sizeof(TttMultiGameView));
    game_view->view = view_alloc();
    view_set_context(game_view->view, game_view);
    view_allocate_model(game_view->view, ViewModelTypeLocking, sizeof(TttMultiGameViewModel));
    view_set_draw_callback(game_view->view, (ViewDrawCallback)ttt_multi_game_view_draw_callback);
    view_set_input_callback(game_view->view, ttt_multi_game_view_input_callback);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            model->game = ttt_multi_game_alloc();
            model->position_x = 0;
            model->position_y = 0;
        },
        true);

    return game_view;
}

void ttt_multi_game_view_free(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        { ttt_multi_game_free(model->game); },
        true);

    view_free(game_view->view);
    free(game_view);
}

View* ttt_multi_game_get_view(TttMultiGameView* game_view) {
    furi_assert(game_view);

    return game_view->view;
}

void ttt_multi_game_view_process_up(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_y > 0) {
                model->position_y--;
            }
        },
        true);
}

void ttt_multi_game_view_process_down(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_y < 2) {
                model->position_y++;
            }
        },
        true);
}

void ttt_multi_game_view_process_left(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_x > 0) {
                model->position_x--;
            }
        },
        true);
}

void ttt_multi_game_view_process_right(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_x < 2) {
                model->position_x++;
            }
        },
        true);
}

void ttt_multi_game_view_process_ok(TttMultiGameView* game_view) {
    furi_assert(game_view);
    TttMultiGameMove move = {};
    TttMultiGameViewMoveCallback callback = NULL;
    void * context = NULL;

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            move.player = ttt_multi_game_current_player(model->game);
            move.x = model->position_x;
            move.y = model->position_y;

            if(ttt_multi_game_is_move_valid(model->game, &move)) {
                callback = model->callback;
                context = model->context;
            }
        },
        true);

    if(callback) {
        callback(context, &move);
    }
}

void ttt_multi_game_view_move(TttMultiGameView* game_view, TttMultiGameMove* move) {
    furi_assert(game_view);
    furi_assert(move);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        { ttt_multi_game_make_move(model->game, move); },
        true);
}

void ttt_multi_game_view_set_callback(
    TttMultiGameView* game_view,
    TttMultiGameViewMoveCallback callback,
    void* context) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            model->callback = callback;
            model->context = context;
        },
        true);
}

void ttt_multi_game_view_reset(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            ttt_multi_game_reset(model->game);
            model->position_x = 0;
            model->position_y = 0;
        },
        true);
}