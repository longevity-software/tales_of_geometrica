#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TalesOfGeometrica : public olc::PixelGameEngine
{
public:

    olc::Renderable _mathildaDialogueSprite;

public:
	TalesOfGeometrica()
	{
		sAppName = "Tales of Geometrica";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
        _mathildaDialogueSprite.Load("assets/gfx/MathildaBaseDialogue.png");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
        Clear(olc::BLACK);
        
        DrawDecal(GetMousePos(), _mathildaDialogueSprite.Decal());

		return true;
	}
};

int main()
{
	TalesOfGeometrica game;
	if (game.Construct(240, 160, 1, 1))
		game.Start();

	return 0;
}