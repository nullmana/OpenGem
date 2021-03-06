#include "entity/monster_node.h"

#include <cstdlib>

const int PathWeight::dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int PathWeight::dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

int MonsterNode::pickDirection(const PathWeight& path)
{
    if (path.sum == 0)
        return -1;

    int r = rand() % path.sum;
    for (int i = 0; i < 8; ++i)
    {
        if (r < path.weight[i])
            return i;

        r -= path.weight[i];
    }
    return -1;
}

#ifdef DEBUG
#include "ingame/ingame_core.h"
#include "wrapfbg.h"

void MonsterNode::debugDrawPathWeights(IngameCore& core)
{
    struct _fbg* pFbg = core.fbg();
    Window* pWindow = core.renderer.getMapWindow();

    float scale = pWindow->width / float(g_game.ingameMapWidth);

    for (int y = 0; y < g_game.ingameMapHeight; ++y)
    {
        for (int x = 0; x < g_game.ingameMapWidth; ++x)
        {
            const PathWeight& tile = tileDirection.at(y, x);
            if (tile.sum > 0)
            {
                int x1 = (x + 0.5f) * scale + pWindow->x;
                int y1 = (y + 0.5f) * scale + pWindow->y;

                for (int i = 0; i < 8; ++i)
                {
                    int w = tile.weight[i];
                    if (g_game.game == GC_LABYRINTH)
                        w *= 8;
                    if (w > 0)
                        fbg_line(pFbg, x1, y1, PathWeight::dx[i] * w + x1,
                            PathWeight::dy[i] * w + y1, 0x00, 0x00, 0xFF);
                }
            }
        }
    }
}
#endif
