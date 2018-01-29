#include "input_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <system/debug_log.h>


InputApp::InputApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	input_manager_(NULL),
	active_touch_id_(-1)
{
}

void InputApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	input_manager_ = gef::InputManager::Create(platform_);

	// make sure if there is a panel to detect touch input, then activate it
	if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
		input_manager_->touch_manager()->EnablePanel(0);

	sprite.set_position(platform_.width()*0.5f, platform_.height()*0.5f, 0.0f);
	sprite.set_width(32.0f);
	sprite.set_height(32.0f);

	InitFont();
}

void InputApp::CleanUp()
{
	CleanUpFont();

	delete input_manager_;
	input_manager_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool InputApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	if (input_manager_)
	{
		input_manager_->Update();

		ProcessTouchInput();
	}

	return true;
}

void InputApp::Render()
{
	sprite_renderer_->Begin();
	DrawHUD();
	sprite_renderer_->DrawSprite(sprite);
	sprite_renderer_->End();
}
void InputApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void InputApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void InputApp::DrawHUD()
{
	if(font_)
	{
		// if a touch is active lets draw some text
		if (active_touch_id_ != -1)
		{
			font_->RenderText(
				sprite_renderer_,
				gef::Vector4(touch_position_.x, touch_position_.y, -0.9f),
				1.0f, 0xffffffff, gef::TJ_LEFT,
				"(%.1f, %.1f)",
				touch_position_.x, touch_position_.y);
		}

		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void InputApp::ProcessTouchInput()
{
	const gef::TouchInputManager* touch_input = input_manager_->touch_manager();
	if (touch_input && (touch_input->max_num_panels() > 0))
	{
		// get the active touches for this panel
		const gef::TouchContainer& panel_touches = touch_input->touches(0);

		// go through the touches
		for (gef::ConstTouchIterator touch = panel_touches.begin(); touch != panel_touches.end(); ++touch)
		{
			// if active touch id is -1, then we are not currently processing a touch
			if (active_touch_id_ == -1)
			{
				gef::DebugOut("Touch = -1\n");
				// check for the start of a new touch
				if (touch->type == gef::TT_NEW)
				{
					gef::DebugOut("New touch\n");
					active_touch_id_ = touch->id;

					// do any processing for a new touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;
				}
			}
			else if (active_touch_id_ == touch->id)
			{

				gef::DebugOut("Touch\n");
				// we are processing touch data with a matching id to the one we are looking for
				if (touch->type == gef::TT_ACTIVE)
				{
					// update an active touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;
					if (IsInside(sprite, touch->position))
					{
						float x = touch->position.x;
						float y = touch->position.y;
						sprite.set_position(x, y, 0);
					}
				}
				else if (touch->type == gef::TT_RELEASED)
				{
					// the touch we are tracking has been released
					// perform any actions that need to happen when a touch is released here
					// we're not doing anything here apart from resetting the active touch id
					active_touch_id_ = -1;
					gef::DebugOut("Released\n");
					//sprite.set_colour(0xFF000000);
				}
			}
		}
	}
}

bool InputApp::IsInside(const gef::Sprite& sprite, const gef::Vector2& point)
{
	float spriteX1 = sprite.position().x();
	float spriteX2 = spriteX1 + sprite.width();
	float spriteY1 = sprite.position().y();
	float spriteY2 = spriteY1 + sprite.height();

	float pointX = point.x;
	float pointY = point.y;

	if (pointX < spriteX2 && pointX > spriteX1 && pointY > spriteY1 && pointY < spriteY2)
	{
		return true;
	}
	return false;
}


