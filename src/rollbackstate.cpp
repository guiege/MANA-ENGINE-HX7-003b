#include "RollbackState.h"

GameState gs = { 0 };
Renderer* renderer = NULL;
GGPOSession * ggpo = NULL;

/* 
 * Simple checksum function stolen from wikipedia:
 *
 *   http://en.wikipedia.org/wiki/Fletcher%27s_checksum
 */

int
fletcher32_checksum(short *data, size_t len)
{
   int sum1 = 0xffff, sum2 = 0xffff;

   while (len) {
      size_t tlen = len > 360 ? 360 : len;
      len -= tlen;
      do {
         sum1 += *data++;
         sum2 += sum1;
      } while (--tlen);
      sum1 = (sum1 & 0xffff) + (sum1 >> 16);
      sum2 = (sum2 & 0xffff) + (sum2 >> 16);
   }

   /* Second reduction step to reduce sums to 16 bits */
   sum1 = (sum1 & 0xffff) + (sum1 >> 16);
   sum2 = (sum2 & 0xffff) + (sum2 >> 16);
   return sum2 << 16 | sum1;
}

bool __cdecl vw_begin_game_callback(const char *)
{
	return true;
}

bool __cdecl vw_on_event_callback(GGPOEvent *info)
{
   // int progress;
   // switch (info->code) {
   // case GGPO_EVENTCODE_CONNECTED_TO_PEER:
   //    ngs.SetConnectState(info->u.connected.player, Synchronizing);
   //    break;
   // case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
   //    progress = 100 * info->u.synchronizing.count / info->u.synchronizing.total;
   //    ngs.UpdateConnectProgress(info->u.synchronizing.player, progress);
   //    break;
   // case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
   //    ngs.UpdateConnectProgress(info->u.synchronized.player, 100);
   //    break;
   // case GGPO_EVENTCODE_RUNNING:
   //    ngs.SetConnectState(Running);
   //    renderer->SetStatusText("");
   //    break;
   // case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
   //    ngs.SetDisconnectTimeout(info->u.connection_interrupted.player,
   //                             timeGetTime(),
   //                             info->u.connection_interrupted.disconnect_timeout);
   //    break;
   // case GGPO_EVENTCODE_CONNECTION_RESUMED:
   //    ngs.SetConnectState(info->u.connection_resumed.player, Running);
   //    break;
   // case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
   //    ngs.SetConnectState(info->u.disconnected.player, Disconnected);
   //    break;
   // case GGPO_EVENTCODE_TIMESYNC:
   //    Sleep(1000 * info->u.timesync.frames_ahead / 60);
   //    break;
   // }
   return true;
}

bool __cdecl vw_advance_frame_callback(int)
{
   int inputs[2] = { 0 };
   int disconnect_flags;

   // Make sure we fetch new inputs from GGPO and use those to update
   // the game state instead of reading from the keyboard.
   ggpo_synchronize_input(ggpo, (void *)inputs, sizeof(int) * 2, &disconnect_flags);
   Rollback_AdvanceFrame(inputs, disconnect_flags);
   return true;
}

bool __cdecl vw_load_game_state_callback(unsigned char* buffer, int len)
{
	memcpy(&gs, buffer, len);
	return true;
}

bool __cdecl vw_save_game_state_callback(unsigned char **buffer, int *len, int *checksum, int)
{
	*len = sizeof(gs);
	*buffer = (unsigned char *)malloc(*len);
	if(!*buffer){
		return false;
	}
	memcpy(*buffer, &gs, *len);
	*checksum = fletcher32_checksum((short *)*buffer, *len / 2);
	return true;
}

bool __cdecl
vw_log_game_state(char *filename, unsigned char *buffer, int)
{
   // FILE* fp = nullptr;
   // fopen_s(&fp, filename, "w");
   // if (fp) {
   //    GameState *gamestate = (GameState *)buffer;
   //    fprintf(fp, "GameState object.\n");
   //    fprintf(fp, "  bounds: %d,%d x %d,%d.\n", gamestate->_bounds.left, gamestate->_bounds.top,
   //            gamestate->_bounds.right, gamestate->_bounds.bottom);
   //    fprintf(fp, "  num_ships: %d.\n", gamestate->_num_ships);
   //    for (int i = 0; i < gamestate->_num_ships; i++) {
   //       Ship *ship = gamestate->_ships + i;
   //       fprintf(fp, "  ship %d position:  %.4f, %.4f\n", i, ship->position.x, ship->position.y);
   //       fprintf(fp, "  ship %d velocity:  %.4f, %.4f\n", i, ship->velocity.dx, ship->velocity.dy);
   //       fprintf(fp, "  ship %d radius:    %d.\n", i, ship->radius);
   //       fprintf(fp, "  ship %d heading:   %d.\n", i, ship->heading);
   //       fprintf(fp, "  ship %d health:    %d.\n", i, ship->health);
   //       fprintf(fp, "  ship %d speed:     %d.\n", i, ship->speed);
   //       fprintf(fp, "  ship %d cooldown:  %d.\n", i, ship->cooldown);
   //       fprintf(fp, "  ship %d score:     %d.\n", i, ship->score);
   //       for (int j = 0; j < MAX_BULLETS; j++) {
   //          Bullet *bullet = ship->bullets + j;
   //          fprintf(fp, "  ship %d bullet %d: %.2f %.2f -> %.2f %.2f.\n", i, j,
   //                  bullet->position.x, bullet->position.y,
   //                  bullet->velocity.dx, bullet->velocity.dy);
   //       }
   //    }
   //    fclose(fp);
   // }
   return true;
}

void __cdecl vw_free_buffer(void *buffer)
{
	free(buffer);
}

void Rollback_Init(unsigned short localport, int num_players, GGPOPlayer *players, int num_spectators)
{
	GGPOErrorCode result;
	ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");
	renderer = new Renderer(ResourceManager::GetShader("batch"));

	gs.Init();

	GGPOSessionCallbacks cb = { 0 };
   cb.begin_game      = vw_begin_game_callback;
   cb.advance_frame	 = vw_advance_frame_callback;
   cb.load_game_state = vw_load_game_state_callback;
   cb.save_game_state = vw_save_game_state_callback;
   cb.free_buffer     = vw_free_buffer;
   cb.on_event        = vw_on_event_callback;
   cb.log_game_state  = vw_log_game_state;


    ggpo_set_disconnect_timeout(ggpo, 3000);
    ggpo_set_disconnect_notify_start(ggpo, 1000);

}

void Rollback_DrawCurrentFrame()
{
   if(renderer != nullptr){

   }

}

void Rollback_AdvanceFrame(int inputs[], int disconnect_flags)
{
   // gs.update(inputs, disconnect_flags);

   ggpo_advance_frame(ggpo);

}

void Rollback_RunFrame()
{

}

void Rollback_Idle(int time)
{

}

void Rollback_DisconnectPlayer(int player)
{

}

void Rollback_Exit()
{

}

bool RollbackState::enter()
{
   // Rollback_Init();

   return true;
}

bool RollbackState::exit()
{
   return true;
}

void RollbackState::update(float dt)
{

}

void RollbackState::render()
{
   
}