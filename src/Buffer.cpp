#include "Game.h"

game_offscreen_buffer::game_offscreen_buffer(int TotalWidth, int TotalHeight)
{
	this->TotalWidth = TotalWidth;
	this->TotalHeight = TotalHeight;
	this->MapBorderThickness = 5;
	this->MapBorderColor = MyRGB(1.0, 0.0, 1.0);
	//this->MapRegionTotal.x = 200;
	//this->MapRegionTotal.y = 50;
	//this->MapRegionTotal.Width = TotalWidth - 100;
	//this->MapRegionTotal.Height = TotalHeight - 100;
}

void game_offscreen_buffer::ClearBuffer()
{
	int32_t *Pixel = (int32_t*)this->Memory;

	for (int i = 0; i < this->TotalWidth * this->TotalHeight; i++)
	{
		Pixel[i] = 0;
	}
}

void game_offscreen_buffer::DrawRectangle(int Left, int Top, int Width, int Height, int32_t Color)
{
	int32_t* Pixel = (int32_t*)this->Memory;

	for (int y = MAX(Top, 0); y < Top + Height && y < this->TotalHeight; y++)
	{
		for (int x = MAX(Left, 0); x < Left + Width && x < this->TotalWidth; x++)
		{
			Pixel[y*this->TotalWidth + x] = Color;
		}
	}
}

void game_offscreen_buffer::DrawRectangle(float Left, float Top, float Width, float Height, int32_t Color)
{
	this->DrawRectangle((int)Left, (int)Top, (int)Width, (int)Height, Color);
}

void game_offscreen_buffer::DrawRectangle(intrectangle Rect, int32_t Color)
{
	this->DrawRectangle(Rect.x, Rect.y, Rect.Width, Rect.Height, Color);
}

//
//void DrawCircle(game_offscreen_buffer *Buffer, int Left, int Top, int Width, int Height, int32_t Color)
//{
//	float CenterX = (float)Left + Width / 2.0;
//	float CenterY = (float)Top + Height / 2.0;
//
//	int32_t* Pixel = (int32_t*)Buffer->Memory;
//
//	for (int y = MAX(Top, 0); y < Top + Height && y < Buffer->TotalHeight; y++)
//	{
//		for (int x = MAX(Left, 0); x < Left + Width && x < Buffer->TotalWidth; x++)
//		{
//			Pixel[y*Buffer->TotalWidth + x] = Color;
//		}
//	}
//}

//Map Coordinates go from:      0 to GameState->GameMap->Width
//                    and:      0 to GameState->GameMap->Height
//Display Coordinates go from:  ActionRegionInUse.x to ActionRegionInUse.x + ActionRegionInUse.Width
//                        and:  ActionRegionInUse.y to ActionRegionInUse.y + ActionRegionInUse.Height

vec2 game_offscreen_buffer::MapToDisplayCoordinates(float x, float y)
{
	intrectangle ActionRegionInUse = this->GameState->WindowRegions.ActionRegionInUse;
	game_map* GameMap = this->GameState->GameMap;
	float NewX = x / GameMap->Width * ActionRegionInUse.Width + ActionRegionInUse.x;
	float NewY = y / GameMap->Height * ActionRegionInUse.Height + ActionRegionInUse.y;
	return vec2(NewX, NewY);
}

rectangle game_offscreen_buffer::MapToDisplayRectangle(float x, float y, float Width, float Height)
{
	float Right = x + Width;
	float Bottom = y + Height;
	vec2 NewV1 = this->MapToDisplayCoordinates(x, y);
	vec2 NewV2 = this->MapToDisplayCoordinates(Right, Bottom);
	rectangle output = { NewV1.X, NewV1.Y, NewV2.X - NewV1.X, NewV2.Y - NewV1.Y };
	return output;
}



void game_offscreen_buffer::DrawBorder()
{
	intrectangle Inner = this->GameState->WindowRegions.ActionRegionInUse;  //this->MapRegionInUse;
	intrectangle Outer = {};
	int BorderWidth = this->MapBorderThickness;
	int Color = this->MapBorderColor;
	Outer.x = Inner.x - BorderWidth;
	Outer.y = Inner.y - BorderWidth;
	Outer.Width = Inner.Width + 2 * BorderWidth;
	Outer.Height = Inner.Height + 2 * BorderWidth;

	this->DrawRectangle(Outer, Color);
	this->DrawRectangle(Inner, MyRGB(0.0, 0.0, 0.0));
}


void game_offscreen_buffer::DrawSnake()
{
	snake* Snake = this->GameState->CurrentRound.Snake;
	game_map* GameMap = this->GameState->GameMap;
	std::list<snake_segment>& Segments = Snake->Segments;

	// Iterate through the segmeents in reverse order
	for (std::list<snake_segment>::reverse_iterator it = Segments.rbegin(); it != Segments.rend(); it++)
	{
		vec2 DrawLocation = it->Location + it->Direction * Snake->Timer;

		// For debugging the Intangibility timer.
		int Color = it->Color;
		if (it->IntangibleTimer > 0) Color = MyRGB(1, 1, 0);
		
		rectangle Rec = this->MapToDisplayRectangle(DrawLocation.X, DrawLocation.Y, 1, 1);
		this->DrawRectangle(Rec.x, Rec.y, Rec.Width, Rec.Height, Color);
	}

}

void game_offscreen_buffer::DrawMap()
{
	game_map *GameMap = this->GameState->GameMap;
	for (int y = 0; y < GameMap->Height; y++)
	{
		for (int x = 0; x < GameMap->Width; x++)
		{
			vec2 UpperLeftCorner = this->MapToDisplayCoordinates((float)x, (float)y);
			this->DrawRectangle(UpperLeftCorner.X, UpperLeftCorner.Y, 2.0, 2.0, MyRGB(.5, .5, .5));
		}
	}
}

void game_offscreen_buffer::DrawScore()
{
	for (int i = 0; i < GameState->CurrentRound.Score; ++i)
	{
		this->DrawRectangle(10 + 20*i, 10, 10, 10, MyRGB(1, 1, 1));
	}
}

void game_offscreen_buffer::RenderBuffer()
{
	this->ClearBuffer();
	this->DrawBorder();
	this->DrawMap();
	this->DrawScore();

	if (this->GameState->DebugBufferMode)
	{
		this->DrawDebugOverLay();
	}

	// Draw the pellets.
	for (std::list<pellet>::iterator it = this->GameState->CurrentRound.Pellets.begin(); it != this->GameState->CurrentRound.Pellets.end(); it++)
	{
		rectangle Rec = this->MapToDisplayRectangle(it->Location.X, it->Location.Y, 1.0, 1.0);
		this->DrawRectangle(Rec.x, Rec.y, Rec.Width, Rec.Height, it->Color);
	}

	this->DrawSnake();
}

void game_offscreen_buffer::DrawDebugOverLay()
{
	this->DrawRectangle(this->GameState->WindowRegions.ActionRegion, MyRGB(.7, 0, 0));
	this->DrawRectangle(this->GameState->WindowRegions.ActionRegionInUse, MyRGB(.9, 0, 0));
	this->DrawRectangle(this->GameState->WindowRegions.ScoreRegion, MyRGB(0, 0, 1));
	this->DrawRectangle(this->GameState->WindowRegions.StatsRegion, MyRGB(0, .2, .8));
	
}