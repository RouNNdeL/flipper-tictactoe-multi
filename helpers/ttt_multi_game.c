#include "ttt_multi_game.h"

struct TttMultiGame {
    TttMultiGamePlayer local_player;
    TttMultiGameState state;
    TttMultiGameResult result;

    uint8_t board[3][3];
};

void ttt_multi_game_reset(TttMultiGame* game) {
    furi_assert(game);

    game->state = TttMultiGameStateTurnX;
    game->result = TttMultiGameResultNone;

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            game->board[i][j] = TttMultiGamePlayerNone;
        }
    }
}

void ttt_multi_game_set_local_player(TttMultiGame* game, TttMultiGamePlayer player) {
    furi_assert(game);
    game->local_player = player;
}

TttMultiGamePlayer ttt_multi_game_get_local_player(TttMultiGame* game) {
    furi_assert(game);
    return game->local_player;
}

TttMultiGameState ttt_multi_game_get_state(TttMultiGame* game) {
    furi_assert(game);
    return game->state;
}

TttMultiGameResult ttt_multi_game_get_result(TttMultiGame* game) {
    furi_assert(game);
    if(game->state != TttMultiGameStateFinished) {
        return TttMultiGameResultNone;
    }

    return game->result;
}

bool ttt_multi_game_is_draw(TttMultiGame* game) {
    furi_assert(game);

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            if(game->board[i][j] == TttMultiGamePlayerNone) {
                return false;
            }
        }
    }

    return true;
}

TttMultiGamePlayer ttt_multi_game_get_winner(TttMultiGame* game) {
    furi_assert(game);

    // Check rows
    for(uint8_t i = 0; i < 3; i++) {
        if(game->board[i][0] == game->board[i][1] && game->board[i][1] == game->board[i][2]) {
            return (TttMultiGamePlayer)game->board[i][0];
        }
    }

    // Check columns
    for(uint8_t i = 0; i < 3; i++) {
        if(game->board[0][i] == game->board[1][i] && game->board[1][i] == game->board[2][i]) {
            return (TttMultiGamePlayer)game->board[0][i];
        }
    }

    // Check diagonals
    if(game->board[0][0] == game->board[1][1] && game->board[1][1] == game->board[2][2]) {
        return (TttMultiGamePlayer)game->board[0][0];
    }
    if(game->board[0][2] == game->board[1][1] && game->board[1][1] == game->board[2][0]) {
        return (TttMultiGamePlayer)game->board[0][2];
    }

    return TttMultiGamePlayerNone;
}

static void ttt_multi_game_update_result(TttMultiGame* game) {
    furi_assert(game);

    if(game->state == TttMultiGameStateFinished) {
        return;
    }

    if(ttt_multi_game_is_draw(game)) {
        game->result = TttMultiGameResultDraw;
        game->state = TttMultiGameStateFinished;
        return;
    }

    TttMultiGamePlayer winner = ttt_multi_game_get_winner(game);
    if(winner == TttMultiGamePlayerNone) {
        return;
    }
    game->state = TttMultiGameStateFinished;

    if(winner == TttMultiGamePlayerX) {
        game->result = TttMultiGameResultXWin;
    } else if(winner == TttMultiGamePlayerO) {
        game->result = TttMultiGameResultOWin;
    }
}

bool ttt_multi_game_is_move_valid(TttMultiGame* game, TttMultiGameMove* move) {
    furi_assert(game);
    furi_assert(move);

    if(move->x > 2 || move->y > 2) {
        return false;
    }

    if(game->board[move->x][move->y] != TttMultiGamePlayerNone) {
        return false;
    }

    return true;
}

void ttt_multi_game_make_move(TttMultiGame* game, TttMultiGameMove* move) {
    furi_assert(game);
    furi_assert(move);
    furi_assert(ttt_multi_game_is_move_valid(game, move));

    game->board[move->x][move->y] = (uint8_t)move->player;
    ttt_multi_game_update_result(game);
}

TttMultiGamePlayer ttt_multi_game_player_at(TttMultiGame* game, uint8_t x, uint8_t y) {
    furi_assert(game);
    furi_assert(x < 3);
    furi_assert(y < 3);

    return (TttMultiGamePlayer)game->board[x][y];
}

TttMultiGame* ttt_multi_game_alloc() {
    TttMultiGame* game = malloc(sizeof(TttMultiGame));
    ttt_multi_game_reset(game);

    return game;
}

void ttt_multi_game_free(TttMultiGame* game) {
    furi_assert(game);

    free(game);
}
