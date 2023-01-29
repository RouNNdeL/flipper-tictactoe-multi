#include "ttt_multi_game_view.h"

struct TttMultiGameView {
    View* view;
};

typedef struct {
    TttMultiGame* game;
    uint8_t position_x;
    uint8_t position_y;

    void* context;
    TttMultiGameViewCallback callback;
} TttMultiGameViewModel;

void ttt_multi_game_view_draw_callback(Canvas* canvas, void* _model) {
    TttMultiGameViewModel* model = _model;

    canvas_clear(canvas);
    const uint8_t cell_size = 20;

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            canvas_draw_frame(canvas, i * cell_size, j * cell_size, cell_size, cell_size);
        }
    }

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            if(ttt_multi_game_player_at(model->game, i, j) == TttMultiGamePlayerX) {
                canvas_draw_line(
                    canvas, i * cell_size, j * cell_size, (i + 1) * cell_size, (j + 1) * cell_size);
                canvas_draw_line(
                    canvas, i * cell_size, (j + 1) * cell_size, (i + 1) * cell_size, j * cell_size);
            } else if(ttt_multi_game_player_at(model->game, i, j) == TttMultiGamePlayerO) {
                canvas_draw_circle(
                    canvas,
                    i * cell_size + cell_size / 2,
                    j * cell_size + cell_size / 2,
                    cell_size / 2);
            }
        }
    }
}

TttMultiGameView* ttt_multi_game_view_alloc() {
    TttMultiGameView* game_view = malloc(sizeof(TttMultiGameView));
    game_view->view = view_alloc();
    view_set_context(game_view->view, game_view);
    view_allocate_model(game_view->view, ViewModelTypeLocking, sizeof(TttMultiGameViewModel));
    view_set_draw_callback(game_view->view, (ViewDrawCallback)ttt_multi_game_view_draw_callback);

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
    TttMultiGameViewCallback callback,
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